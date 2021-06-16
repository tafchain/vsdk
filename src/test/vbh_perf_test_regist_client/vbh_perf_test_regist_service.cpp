#include "ace/OS_NS_sys_stat.h"

#include "dsc/dsc_log.h"

#include "vbh_perf_test_regist_client/vbh_perf_test_regist_service.h"
#include "vbh_comm/comm_msg_def/vbh_comm_msg_clt_clt_def.h"
#include "vbh_comm/vbh_comm_error_code.h"
#include "vbh_comm/vbh_comm_func.h"
#include "vbh_comm/vbh_key_codec.h"
#include "vbh_comm/vbh_comm_macro_def.h"
#include "vbh_comm/test_def.h"

CVbhPerfTestRegistService::CRegistUserTimer::CRegistUserTimer(CVbhPerfTestRegistService& rService)
: m_rServiece(rService)
{
}

void CVbhPerfTestRegistService::CRegistUserTimer::OnTimer(void)
{
	m_rServiece.StartRegistUser();
}

CVbhPerfTestRegistService::CVbhPerfTestRegistService(ACE_UINT32 nNeedRegistUserNum, ACE_UINT32 nChannelID, ACE_UINT32 nStartNum,bool bCrypt)
	: m_nNeedRegistUserNum(nNeedRegistUserNum)
	, m_nChannelID(nChannelID)
	, m_nStartNum(nStartNum)
	, m_registUserTimer(*this)
{
	if (bCrypt) //加密，走加密通道
	{
		m_nClientServiceType = VBH::EN_VBH_CRYPT_CLIENT_SERVICE_TYPE;
	}
	else //走不加密通道
	{
		m_nClientServiceType = VBH::EN_VBH_CLIENT_SERVICE_TYPE;
	}
}

ACE_INT32 CVbhPerfTestRegistService::OnInit(void)
{
	if (CDscReactor::OnInit())
	{
		DSC_RUN_LOG_ERROR("vbh_perf_test_regist_client service init failed!");

		return -1;
	}

	m_strUserKeyPath = CDscAppManager::Instance()->GetWorkRoot();
	m_strUserKeyPath += DSC_FILE_PATH_SPLIT_CHAR;
	m_strUserKeyPath += "test";
	m_strUserKeyPath += DSC_FILE_PATH_SPLIT_CHAR;
	m_strUserKeyPath += "channel_";
	m_strUserKeyPath += m_nChannelID;
	m_strUserKeyPath += DSC_FILE_PATH_SPLIT_CHAR;
	m_strUserKeyPath += "user_key";
	m_strUserKeyPath += DSC_FILE_PATH_SPLIT_CHAR;

	if (DSC::DscRecurMkdir(m_strUserKeyPath.c_str(), m_strUserKeyPath.size()))
	{
		ACE_OS::printf("make-dir:%s failed.", m_strUserKeyPath.c_str());
		return -1;
	}

	ACE_OS::sleep(5); //为了比要拉起的插件晚启动
	this->SetDscTimer(this, 1, true);
	this->SetDscTimer(&m_registUserTimer, 1, true);

	return 0;
}

ACE_INT32 CVbhPerfTestRegistService::OnExit(void)
{
	CRegistUserSession* pRegistUserSession;
	for (auto it = m_mapTradeUserSession.begin(); it != m_mapTradeUserSession.end();)
	{
		pRegistUserSession = it.second;
		this->CancelDscTimer(pRegistUserSession);
		++it;
		DSC_THREAD_TYPE_DELETE(pRegistUserSession);
	}

	this->CancelDscTimer(&m_registUserTimer);
	this->CancelDscTimer(this);

	return CDscReactor::OnExit();
}

ACE_INT32 CVbhPerfTestRegistService::WriteFile(const CDscString& strFileName, const CDscString& strCryptKey)
{
	ACE_HANDLE handle = ACE_OS::open(strFileName.c_str(), O_RDWR | O_BINARY | O_CREAT | O_TRUNC);

	if (ACE_INVALID_HANDLE == handle)
	{
		int nLastError = ACE_OS::last_error();
		ACE_OS::printf("cann't open file:%s, last error code:%d, last error reason:%s.", strFileName.c_str(), nLastError, ACE_OS::strerror(nLastError));
		return -1;
	}

	if (-1 == ACE_OS::write(handle, strCryptKey.c_str(), strCryptKey.size()))
	{
		int nLastError = ACE_OS::last_error();
		ACE_OS::printf("write file failed, file name:%s, last error code:%d, last error reason:%s.", strFileName.c_str(), nLastError, ACE_OS::strerror(nLastError));
		ACE_OS::close(handle);

		return -1;
	}

	ACE_OS::close(handle);

	return 0;
}

void CVbhPerfTestRegistService::OnDscMsg(VBH::CRegistUserCltCltRsp& rsp, const CDscMsg::CDscMsgAddr& rSrcMsgAddr)
{
	VBH_MESSAGE_ENTER_TRACE(CVbhPerfTestRegistService, VBH::CRegistUserCltCltRsp);

	CRegistUserSession* pSession = m_mapTradeUserSession.Find(rsp.m_nSrcSessionID);

	if (pSession)
	{
		if (rsp.m_nReturnCode == VBH::EN_OK_TYPE)
		{
			CDscString strUserCryptKey;

			if (VBH::GenerateUserCryptKey(strUserCryptKey,
				pSession->m_strCltPriKey.c_str(), pSession->m_strCltPriKey.size(),
				rsp.m_vbhCryptKey.GetBuffer(), rsp.m_vbhCryptKey.GetSize(), m_nChannelID))
			{
				DSC_RUN_LOG_ERROR("GenerateBase64Uuid failed.");
			}
			else
			{
				CDscString strVbhUserID(rsp.m_vbhUserID.GetBuffer(), rsp.m_vbhUserID.GetSize());

				//接收到通知，注册成功，将user-key 写入文件
				if (SaveUserInfo(strUserCryptKey, strVbhUserID, pSession->m_nUserIndex+ m_nStartNum))
				{
					DSC_RUN_LOG_ERROR("save user-key info file failed, channel-id:%d user-id:%d", m_nChannelID, pSession->m_nUserIndex);
				}
				else
				{
					++m_nRegistRspCount;
				}
			}
		}
		else
		{
			DSC_RUN_LOG_ERROR("regist failed, session-id:%d, error code:%d, error string:%s", rsp.m_nSrcSessionID, rsp.m_nReturnCode, VBH::GetErrorString(rsp.m_nReturnCode));
			
			this->ResetDscTimer(pSession, 1);//延时两秒重新注册
		}

	}
	else
	{
		DSC_RUN_LOG_ERROR("can not find session, session-id:%d", rsp.m_nSrcSessionID);
	}

	VBH_MESSAGE_LEAVE_TRACE(CVbhPerfTestRegistService, VBH::CRegistUserCltCltRsp);
}

void CVbhPerfTestRegistService::RegistUser(CRegistUserSession* pRegistUserSession, DSC::CDscShortBlob& cltPubKey)
{
	const CDscMsg::CDscMsgAddr address(m_nClientServiceType, DSC::EN_INVALID_ID);

	VBH::CRegistUserCltCltReq req;
	TEST_CC::CCcCheckInfoOnUserRegist info;
	CDscString userName;
	CDscString idCardNo;
	char* pInfoBuf;
	size_t nInfoBuf;

	info.m_nAsset = 10000 * 1000;
	userName = "user_";
	userName += pRegistUserSession->m_nUserIndex;
	idCardNo = pRegistUserSession->m_nUserIndex;
	VBH::Assign(info.m_userName, userName);
	VBH::Assign(info.m_idCardNo, idCardNo);

	req.m_nChannelID = m_nChannelID;
	req.m_nSessionID = pRegistUserSession->m_nSessionID;
	req.m_cltPubKey = cltPubKey;

	DSC::Encode(info, pInfoBuf, nInfoBuf);
	req.m_userInfo.Set(pInfoBuf, nInfoBuf);

	if (this->SendDscMessage(req, address))
	{
		DSC_RUN_LOG_ERROR("SendDscMessage failed, channdel id:%d, user id:%d", m_nChannelID, pRegistUserSession->m_nUserIndex);
	}

	req.m_userInfo.FreeBuffer();
}

void CVbhPerfTestRegistService::StartRegistUser()
{
	const ACE_UINT32 nCurRegistCount = DSC_MIN(EN_REGIST_NUM, m_nNeedRegistUserNum - m_nCurUserID); //本次注册的用户个数
	CRegistUserSession* pSession;
	ACE_UINT32 count = 0;

	while (count < nCurRegistCount)
	{
		pSession = DSC_THREAD_TYPE_NEW(CRegistUserSession)CRegistUserSession(*this);

		pSession->m_nSessionID = m_nSessionID;
		pSession->m_nUserIndex = m_nCurUserID;

		if (VBH::CreateHexCryptKeyPair(pSession->m_strCltPubKey, pSession->m_strCltPriKey))
		{
			DSC_RUN_LOG_ERROR("CreateBinCryptKeyPair failed.");
		}
		m_mapTradeUserSession.DirectInsert(m_nSessionID, pSession);
		++m_nSessionID;

		DSC::CDscShortBlob base64PubKey(pSession->m_strCltPubKey.data(), pSession->m_strCltPubKey.size());

		this->RegistUser(pSession, base64PubKey);

		++m_nCurUserID;
		++count;
	}

	if (m_nCurUserID >= m_nNeedRegistUserNum)
	{
		this->CancelDscTimer(&m_registUserTimer);
	}
}

void CVbhPerfTestRegistService::OnTimer()
{
	//老的不会引发core的使用方法
	//if (m_nTimerCounter == 0XFFFFFFFF)
	//{
	//	CDscAppManager::Instance()->SetAppExited();
	//}
	//else
	//{
	//	ACE_OS::printf("[time:%6d] regist rsp:%d count.", m_nTimerCounter, m_nRegistRspCount);

	//	if (m_nRegistRspCount >= m_nNeedRegistUserNum)
	//	{
	//		ACE_OS::printf("| regist all user successed!\n");

	//		m_nTimerCounter = 0XFFFFFFFF;
	//	}
	//	else
	//	{
	//		ACE_OS::printf("\n");
	//		++m_nTimerCounter;
	//	}
	//}


	ACE_OS::printf("[time:%6d] regist rsp:%d count.", m_nTimerCounter, m_nRegistRspCount);

	if (m_nRegistRspCount >= m_nNeedRegistUserNum)
	{
		ACE_OS::printf("| regist all user successed!\n");

		CDscAppManager::Instance()->SetAppExited();
		this->CancelDscTimer(this);
	}
	else
	{
		ACE_OS::printf("\n");
		++m_nTimerCounter;
	}
}

ACE_INT32 CVbhPerfTestRegistService::SaveUserInfo(CDscString& strUserCryptKey, CDscString& strVbhUserID, const ACE_UINT32 nUserIdx)
{
	CDscString strCryptKeyFileName = m_strUserKeyPath;

	strCryptKeyFileName += nUserIdx;
	strCryptKeyFileName += ".ckey";

	if (WriteFile(strCryptKeyFileName, strUserCryptKey))
	{
		return -1;
	}

	CDscString strUserIDFileName = m_strUserKeyPath;

	strUserIDFileName += nUserIdx;
	strUserIDFileName += ".id";

	if (WriteFile(strUserIDFileName, strVbhUserID))
	{
		return -1;
	}

	return 0;
}

CVbhPerfTestRegistService::CRegistUserSession::CRegistUserSession(CVbhPerfTestRegistService& rService)
	: m_rServiece(rService)
{

}

void CVbhPerfTestRegistService::CRegistUserSession::OnTimer(void)
{
	DSC::CDscShortBlob pubKey(this->m_strCltPubKey.data(), this->m_strCltPubKey.size());

	m_rServiece.RegistUser(this, pubKey);
}
