#include "ace/OS_NS_sys_stat.h"

#include "dsc/dsc_log.h"

#include "vbh_perf_test_client/trade_service.h"
#include "vbh_comm/vbh_comm_error_code.h"
#include "vbh_comm/vbh_comm_func.h"
#include "vbh_comm/vbh_comm_macro_def.h"
#include "vbh_comm/vbh_key_codec.h"
#include "vbh_comm/test_def.h"
#include "vbh_perf_test_comm/vbh_perf_test_comm_msg_def.h"
#include "vbh_perf_test_client/load_user_key.h"


CTradeService::CTradeService(ACE_UINT32 nTotalTradeCount, ACE_UINT32 nGroupCount, ACE_UINT32 nChannelID, bool bCrypt, ACE_INT32 nLimitTimes)
	: m_nTotalTradeCount(nTotalTradeCount)
	, m_nGroupCount(nGroupCount)
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
	m_nGroupNum = 30;
	//计算每个分组的交易个数
	m_nGroupTradeCount = m_nTotalTradeCount / m_nGroupCount;
	m_nTotalTradeCount = m_nGroupCount * m_nGroupTradeCount;

}

ACE_INT32 CTradeService::OnInit(void)
{
	if (CDscReactor::OnInit())
	{
		DSC_RUN_LOG_ERROR("no-encrypt-sdk-test-propose-client service init failed!");

		return -1;
	}

	ACE_OS::sleep(5);


	//加载所有user的秘钥
	DSC_THREAD_TYPE_ALLOCATE_ARRAY(m_pTradeSession, m_nTotalTradeCount);


	for (ACE_UINT32 nTradeIdx = 0; nTradeIdx < m_nTotalTradeCount; ++nTradeIdx)
	{
		m_pTradeSession[nTradeIdx].m_nSessionID = nTradeIdx;
		m_pTradeSession[nTradeIdx].m_nLimitTimes = m_nLimitTimes;

		if (LoadUserKey(m_pTradeSession[nTradeIdx].m_user[0].m_strUserID, m_pTradeSession[nTradeIdx].m_user[0].m_strUserCryptKey, m_nChannelID, nTradeIdx * 2))
		{
			DSC_RUN_LOG_ERROR("load client key error, channel id:%d, user-id:%d", m_nChannelID, nTradeIdx * 2);
			return -1;
		}

		if (LoadUserKey(m_pTradeSession[nTradeIdx].m_user[1].m_strUserID, m_pTradeSession[nTradeIdx].m_user[1].m_strUserCryptKey, m_nChannelID, nTradeIdx * 2 + 1))
		{
			DSC_RUN_LOG_ERROR("load client key error, channel id:%d, user-id:%d", m_nChannelID, nTradeIdx * 2 + 1);
			return -1;
		}
	}

	this->SetDscTimer(this, 1, true);
	StartTrade(0);

	return 0;
}

ACE_INT32 CTradeService::OnExit(void)
{
	this->CancelDscTimer(this);

	DSC_THREAD_TYPE_DEALLOCATE_ARRAY(m_pTradeSession, m_nTotalTradeCount);

	return CDscReactor::OnExit();
}

void CTradeService::OnTimer(void)
{
	m_nSum += m_nTradeSum;
	++m_nSumCount;

	ACE_OS::printf("Cur Second Trade Sum: %d!\t", m_nTradeSum);
	if (m_nTradeSum > 0)
	{
		ACE_OS::printf("Cur Second Trade Time Delay: %f ms!\t", 1000 / (double)m_nTradeSum);
	}

	if ((m_nTotalFailedRsp + m_nTradeSum) > 0)
	{
		ACE_OS::printf("Success : %.2f%%\n", 100 * (m_nTradeSum / (double)(m_nTotalFailedRsp + m_nTradeSum)));
	}
	else
	{
	}

	if ((m_bLimitTimes) && (m_nTotalRsp >= (m_nTotalTradeCount * m_nLimitTimes)))
	{
		CDscAppManager::Instance()->SetAppExited();
		this->CancelDscTimer(this);
	}
	for (int i = 0; i < m_nGroupNum/5; i++)
	{
		m_nCount++;
		if (m_nCount == m_nGroupCount)
		{
			m_nCount = 0;
		}
		StartTrade(m_nCount);
	}
	if (m_nGroupNum < 50)
	{
		m_nGroupNum++;
	}
	m_nTradeSum = 0;
	m_nTotalFailedRsp = 0;
}

void CTradeService::StartTrade(ACE_UINT32 nGroupCount)
{
	for (ACE_UINT32 nGroupTradeIdx = 0; nGroupTradeIdx < m_nGroupTradeCount; ++nGroupTradeIdx)
	{
		this->Trade(m_pTradeSession + nGroupTradeIdx+ nGroupCount* m_nGroupTradeCount);

		if (m_bLimitTimes)
		{
			--m_pTradeSession[nGroupTradeIdx].m_nLimitTimes;
		}
	}
}

void CTradeService::OnDscMsg(VBH::CProposeCltCltRsp& rsp, const CDscMsg::CDscMsgAddr& rSrcMsgAddr)
{
	VBH_MESSAGE_ENTER_TRACE(CTradeService, VBH::CProposeCltCltRsp);

	ACE_UINT32 nTradeIdx = rsp.m_nSrcSessionID;

	++m_nTotalRsp;
	if (rsp.m_nReturnCode == VBH::EN_OK_TYPE)
	{
		++m_nTradeSum;
		
		//如果成功，则发起下批中对应用户的交易，否则继续发起自己的交易
		nTradeIdx = (nTradeIdx + m_nGroupTradeCount) % m_nTotalTradeCount;
	}
	else
	{
		++m_nTotalFailedRsp;
		DSC_RUN_LOG_ERROR("CProposeCltCltRsp, session-id:%d, error-code:%d, error-string:%s",
			rsp.m_nSrcSessionID, rsp.m_nReturnCode, VBH::GetErrorString(rsp.m_nReturnCode));
	}

	CTradeSession* pTradeSession = m_pTradeSession + nTradeIdx;

	//if (m_bLimitTimes)
	//{
	//	if (pTradeSession->m_nLimitTimes > 0)
	//	{
	//		this->Trade(pTradeSession);
	//		--pTradeSession->m_nLimitTimes;
	//	}
	//}
	//else
	//{
	//	this->Trade(pTradeSession);
	//}

	VBH_MESSAGE_LEAVE_TRACE(CTradeService, VBH::CProposeCltCltRsp);
}

void CTradeService::Trade(CTradeSession* pSession)
{
	const CDscMsg::CDscMsgAddr address(CDscAppManager::m_nNodeType, CDscAppManager::m_nNodeID, m_nClientServiceType, DSC::EN_INVALID_ID);
	VBH::CProposeCltCltReq req;
	TEST_CC::CTradeAction tradeInfo;
	char* pInfoBuf;
	size_t nInfoBuf;

	if (pSession->m_bFrom)
	{
		VBH::Assign(tradeInfo.m_fromUserKey, pSession->m_user[0].m_strUserID);
		VBH::Assign(tradeInfo.m_toUserKey, pSession->m_user[1].m_strUserID);
		VBH::Assign(req.m_userCryptKey, pSession->m_user[0].m_strUserCryptKey);
	}
	else
	{
		VBH::Assign(tradeInfo.m_fromUserKey, pSession->m_user[1].m_strUserID);
		VBH::Assign(tradeInfo.m_toUserKey, pSession->m_user[0].m_strUserID);
		VBH::Assign(req.m_userCryptKey, pSession->m_user[1].m_strUserCryptKey);
	}

	tradeInfo.m_nAsset = 1;
	pSession->m_bFrom = !pSession->m_bFrom;

	DEF_ENCODE(tradeInfo, pInfoBuf, nInfoBuf);

	req.m_nActionID = TEST_CC::CTradeAction::EN_ACTION_ID;
	req.m_nSrcSessionID = pSession->m_nSessionID;
	req.m_proposal.Set(pInfoBuf, nInfoBuf);

	this->SendDscMessage(req, address);
	++m_nTotalReq;
}

CTradeService::CTradeGroupTimer::CTradeGroupTimer(CTradeService& rService)
	: m_rServiece(rService)
{
}

void CTradeService::CTradeGroupTimer::OnTimer(void)
{
	//m_rServiece.StartTrade();
}
