#include "ace/OS_NS_sys_stat.h"

#include "dsc/dsc_log.h"

#include "vbh_perf_test_client/query_user_service.h"
#include "vbh_comm/vbh_comm_error_code.h"
#include "vbh_comm/vbh_comm_func.h"
#include "vbh_comm/vbh_comm_macro_def.h"
#include "vbh_comm/vbh_key_codec.h"
#include "vbh_comm/test_def.h"
#include "vbh_perf_test_comm/vbh_perf_test_comm_msg_def.h"
#include "vbh_perf_test_client/load_user_key.h"


CQueryUserService::CQueryUserService(ACE_UINT32 nQueryGroupCount, ACE_UINT32 nChannelID, bool bCrypt, ACE_INT32 nLimitTimes)
	: m_nQueryGroupCount(nQueryGroupCount)
	, m_nChannelID(nChannelID)
	, m_nLimitTimes(nLimitTimes)
{
	if (bCrypt) //加密，走加密通道
	{
		m_nClientServiceType = VBH::EN_VBH_CRYPT_CLIENT_SERVICE_TYPE;
	}
	else //走不加密通道
	{
		m_nClientServiceType = VBH::EN_VBH_CLIENT_SERVICE_TYPE;
	}

	if (m_nLimitTimes > 0)
	{
		m_bLimitTimes = true;
	}
}

ACE_INT32 CQueryUserService::OnInit(void)
{
	if(CDscReactor::OnInit())
	{
		DSC_RUN_LOG_ERROR("no-encrypt-sdk-test-propose-client service init failed!");

		return -1;
	}

	ACE_OS::sleep(5);


	ACE_UINT32 nSessionID = 0;
	CQueryUserSession* pQueryUserSession;

	for (ACE_UINT32 idx = 0; idx < m_nQueryGroupCount; ++idx)
	{
		pQueryUserSession = DSC_THREAD_TYPE_NEW(CQueryUserSession) CQueryUserSession(*this);

		if (LoadUserKey(pQueryUserSession->m_strUserID, pQueryUserSession->m_strUserCryptKey, m_nChannelID, idx))
		{
			return -1;
		}

		pQueryUserSession->m_nLimitTimes = m_nLimitTimes;
		pQueryUserSession->m_nSessionID = ++nSessionID;
		Query(pQueryUserSession);
		this->SetDscTimer(pQueryUserSession, EN_SESSION_TIMEOUT_VALUE, true);

		m_mapQueryUserSession.DirectInsert(pQueryUserSession->m_nSessionID, pQueryUserSession);
	}

	this->SetDscTimer(this, 1, true);

	return 0;
}

ACE_INT32 CQueryUserService::OnExit( void )
{
	this->CancelDscTimer(this);

	return CDscReactor::OnExit();
}

void CQueryUserService::OnTimer(void)
{
	m_nSum += m_nQuerySum;
	++m_nSumCount;

	double fAvg = m_nSum;

	fAvg /= m_nSumCount;

	ACE_OS::printf("Cur Second Query User Sum: %d!\t", m_nQuerySum);
	ACE_OS::printf("Cur Second Query User Time Delay: %f ms!\t", 1000 / (double)m_nQuerySum);
	ACE_OS::printf("Success: %.2f%%\n", 100 * (1 - m_nTotalFailedRsp / (double)m_nTotalReq));
	
	if ((m_bLimitTimes) && (m_nTotalRsp >= (m_nQueryGroupCount * m_nLimitTimes)))
	{
#ifdef WIN32
		ACE_OS::system("pause");
#endif
		ACE_OS::exit(0);
	}
	m_nQuerySum = 0;
}

void CQueryUserService::OnTimeOut(CQueryUserSession* pSession)
{
	this->Query(pSession);
}

void CQueryUserService::OnDscMsg(VBH::CQueryCltCltRsp& rsp, const CDscMsg::CDscMsgAddr& rSrcMsgAddr)
{
	VBH_MESSAGE_ENTER_TRACE(CQueryUserService, VBH::CQueryCltCltRsp);
	
	++m_nTotalRsp;
	if (rsp.m_nReturnCode == VBH::EN_OK_TYPE)
	{
		++m_nQuerySum;
	}
	else
	{
		++m_nTotalFailedRsp;
		DSC_RUN_LOG_ERROR("CQueryUserInfoCltCltRsp, session-id:%d, error-code:%d", rsp.m_nSrcSessionID, rsp.m_nReturnCode);
	}
	
	CQueryUserSession* pSession = m_mapQueryUserSession.Find(rsp.m_nSrcSessionID);
	
	if (pSession)
	{
		if (m_bLimitTimes)
		{
			--pSession->m_nLimitTimes;
			if (pSession->m_nLimitTimes > 0)
			{
				Query(pSession);
				CDscReactor::ResetDscTimer(pSession, EN_SESSION_TIMEOUT_VALUE);
			}
		}
		else
		{
			Query(pSession);
			CDscReactor::ResetDscTimer(pSession, EN_SESSION_TIMEOUT_VALUE);
		}
	}
	else
	{
		DSC_RUN_LOG_ERROR("can not find session, session-id:%d", rsp.m_nSrcSessionID);
	}
	
	VBH_MESSAGE_LEAVE_TRACE(CQueryUserService, VBH::CQueryCltCltRsp);
}

void CQueryUserService::Query(CQueryUserSession* pSession)
{
	const CDscMsg::CDscMsgAddr address(CDscAppManager::m_nNodeType, CDscAppManager::m_nNodeID, m_nClientServiceType, DSC::EN_INVALID_ID);
	VBH::CQueryCltCltReq req;
	char* pParam;
	size_t nParamLen;
	TEST_CC::CQueryUserAction action;

	VBH::Assign(action.m_userID, pSession->m_strUserID);
	DSC::Encode(action, pParam, nParamLen);
	req.m_nActionID = TEST_CC::CQueryUserAction::EN_ACTION_ID;
	VBH::Assign(req.m_userCryptKey, pSession->m_strUserCryptKey);
	req.m_param.Set(pParam, nParamLen);
	req.m_nSrcSessionID = pSession->m_nSessionID;
	req.m_nDstPeerID = 1;

	this->SendDscMessage(req, address);
	++m_nTotalReq;

	DSC_THREAD_SIZE_FREE(pParam, nParamLen);
}


CQueryUserService::CQueryUserSession::CQueryUserSession(CQueryUserService& rService)
	:m_rService(rService)
{

}

void CQueryUserService::CQueryUserSession::OnTimer(void)
{
	m_rService.OnTimeOut(this);
}
