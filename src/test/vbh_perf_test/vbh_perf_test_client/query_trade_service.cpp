#include "ace/OS_NS_sys_stat.h"

#include "dsc/dsc_log.h"

#include "vbh_perf_test_client/query_trade_service.h"
#include "vbh_comm/comm_msg_def/vbh_comm_msg_clt_clt_def.h"
#include "vbh_comm/vbh_comm_error_code.h"
#include "vbh_comm/vbh_comm_func.h"
#include "vbh_comm/vbh_comm_macro_def.h"
#include "vbh_comm/vbh_key_codec.h"
#include "vbh_comm/test_def.h"
#include "vbh_perf_test_comm/vbh_perf_test_comm_msg_def.h"

//#include "cc/cc_comm/cc_comm_def.h"
//#include "cc/cc_comm/cc_explorer_query_msg_def.h"
#include "vbh_perf_test_client/load_user_key.h"


CQueryTradeService::CQueryTradeService(ACE_UINT32 nQueryGroupCount, ACE_UINT32 nChannelID, bool bCrypt, ACE_INT32 nLimitTimes, ACE_UINT16 nPeerId)
	: m_nQueryGroupCount(nQueryGroupCount)
	, m_nChannelID(nChannelID)
	, m_nLimitTimes(nLimitTimes)
	, m_nPeerId(nPeerId)
	, m_queryTradeTimerHandler(*this)
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

ACE_INT32 CQueryTradeService::OnInit(void)
{
	ACE_OS::sleep(6);

	if(CDscReactor::OnInit())
	{
		DSC_RUN_LOG_ERROR("no-encrypt-sdk-test-propose-client service init failed!");

		return -1;
	}

	ACE_UINT32 nSessionID = 0;
	CQueryBlockSession* pQueryTradeSession;

	for (size_t nUser = 0; nUser <= m_nQueryGroupCount; ++nUser)
	{
		pQueryTradeSession = DSC_THREAD_TYPE_NEW(CQueryBlockSession) CQueryBlockSession;

		if (LoadUserKey(pQueryTradeSession->m_user[0].m_strUserID, pQueryTradeSession->m_user[0].m_strUserCryptKey, m_nChannelID, nUser * 2))
		{
			DSC_RUN_LOG_ERROR("load client key error, channel id:%d, user-id:%d", m_nChannelID, nUser * 2);
			return -1;
		}

		if (LoadUserKey(pQueryTradeSession->m_user[1].m_strUserID, pQueryTradeSession->m_user[1].m_strUserCryptKey, m_nChannelID, nUser * 2+1))
		{
			DSC_RUN_LOG_ERROR("load client key error, channel id:%d, user-id:%d", m_nChannelID, nUser * 2);
			return -1;
		}

		pQueryTradeSession->m_nLimitTimes = m_nLimitTimes;
		pQueryTradeSession->m_nChannelID = m_nChannelID;
		pQueryTradeSession->m_nSessionID = ++nSessionID;
		Trade(pQueryTradeSession);
		m_mapQueryTradeSession.DirectInsert(pQueryTradeSession->m_nSessionID, pQueryTradeSession);
		++m_nNeedProposalCount;
	}


	return 0;
}

ACE_INT32 CQueryTradeService::OnExit( void )
{
	this->CancelDscTimer(this);
	this->CancelDscTimer(&m_queryTradeTimerHandler);

	return CDscReactor::OnExit();
}

void CQueryTradeService::OnTimer(void)
{
	m_nSum += m_nQuerySum;
	++m_nSumCount;

	double fAvg = m_nSum;

	fAvg /= m_nSumCount;

	ACE_OS::printf("Cur Second Query Trade Sum: %d!\t", m_nQuerySum);
	ACE_OS::printf("Cur Second Query Trade Time Delay: %f ms!\t", 1000 / (double)m_nQuerySum);
	ACE_OS::printf("Success: %.2f%%\n", 100 * (1 - m_nTotalFailedRsp / (double)m_nTotalReq));

	if((m_bLimitTimes) && (m_nTotalRsp == (m_nChannelID * m_nQueryGroupCount * m_nLimitTimes)))
	{
#ifdef WIN32
		ACE_OS::system("pause");
#endif
		ACE_OS::exit(0);
	}
	m_nQuerySum = 0;
}

void CQueryTradeService::OnTimeOut(CQueryBlockSession* pSession)
{
	this->QueryTrade(pSession);
}

void CQueryTradeService::OnDscMsg(VBH::CQueryCltCltRsp& rsp, const CDscMsg::CDscMsgAddr& rSrcMsgAddr)
{
	VBH_MESSAGE_ENTER_TRACE(CVbhPerfTestQueryTradeClientService, VBH::CQueryCltCltRsp);
	
	++m_nTotalRsp;
	if (rsp.m_nReturnCode == VBH::EN_OK_TYPE)
	{
		++m_nQuerySum;
	}
	else
	{
		++m_nTotalFailedRsp;
		DSC_RUN_LOG_ERROR("CQueryTransInfoCltCltRsp, session-id:%d, error-code:%d", rsp.m_nSrcSessionID, rsp.m_nReturnCode);
	}
	
	CQueryBlockSession* pSession = m_mapQueryTradeSession.Find(rsp.m_nSrcSessionID);

	if (pSession)
	{
		if (m_bLimitTimes)
		{
			--pSession->m_nLimitTimes;
			if (pSession->m_nLimitTimes > 0)
			{
				QueryTrade(pSession);
			}
		}
		else
		{
			QueryTrade(pSession);
		}
	}
	else
	{
		DSC_RUN_LOG_ERROR("can not find session, session-id:%d", rsp.m_nSrcSessionID);
	}
	
	VBH_MESSAGE_LEAVE_TRACE(CVbhPerfTestQueryTradeClientService, VBH::CQueryCltCltRsp);
}

void CQueryTradeService::OnDscMsg(VBH::CProposeCltCltRsp& rsp, const CDscMsg::CDscMsgAddr& rSrcMsgAddr)
{
	VBH_MESSAGE_ENTER_TRACE(CVbhPerfTestQueryTradeClientService, VBH::CProposeCltCltRsp);

	if (rsp.m_nReturnCode == VBH::EN_OK_TYPE)
	{
		CQueryBlockSession* pSession = m_mapQueryTradeSession.Find(rsp.m_nSrcSessionID);

		if (pSession)
		{
			pSession->m_txid.assign(rsp.m_transKey.GetBuffer(), rsp.m_transKey.GetSize());
		}
		else
		{
			DSC_RUN_LOG_ERROR("can not find session, session-id:%d", rsp.m_nSrcSessionID);
		}
	}
	else
	{
		DSC_RUN_LOG_ERROR("CProposeCltCltRsp, session-id:%d, error-code:%d", rsp.m_nSrcSessionID, rsp.m_nReturnCode);
	}
	++m_nProposalRspCount;
	if (m_nProposalRspCount >= m_nNeedProposalCount)
	{
		this->SetDscTimer(&m_queryTradeTimerHandler, EN_SESSION_QUERY_WAIT_TIMEOUT_VALUE, false);
	}
	
	VBH_MESSAGE_LEAVE_TRACE(CVbhPerfTestQueryTradeClientService, VBH::CProposeCltCltRsp);
}

void CQueryTradeService::QueryTrade(CQueryBlockSession* pSession)
{
	const CDscMsg::CDscMsgAddr address(CDscAppManager::m_nNodeType, CDscAppManager::m_nNodeID, m_nClientServiceType, DSC::EN_INVALID_ID);
	VBH::CQueryCltCltReq req;


	char* m_pParam;
	size_t nParamLen;
	TEST_CC::CQueryTransAction action;

	action.m_transID.Set((char*)pSession->m_txid.c_str(), pSession->m_txid.size());

	DSC::Encode(action, m_pParam, nParamLen);

	req.m_nDstPeerID = m_nPeerId;
	req.m_nSrcSessionID = pSession->m_nSessionID;
	req.m_nActionID = TEST_CC::CQueryTransAction::EN_ACTION_ID;
	req.m_param.Set(m_pParam, nParamLen);

	VBH::Assign(req.m_userCryptKey, pSession->m_user[0].m_strUserCryptKey);

	req.m_nSrcSessionID = pSession->m_nSessionID;

	++m_nTotalReq;
	this->SendDscMessage(req, address);

	DSC_THREAD_SIZE_FREE(m_pParam, nParamLen);
}

void CQueryTradeService::Trade(CQueryBlockSession* pSession)
{
	const CDscMsg::CDscMsgAddr address(CDscAppManager::m_nNodeType, CDscAppManager::m_nNodeID, m_nClientServiceType, DSC::EN_INVALID_ID);
	VBH::CProposeCltCltReq req;
	TEST_CC::CTradeAction tradeInfo;
	char* pInfoBuf;
	size_t nInfoBuf;

	VBH::Assign(tradeInfo.m_fromUserKey, pSession->m_user[0].m_strUserID);
	VBH::Assign(tradeInfo.m_toUserKey, pSession->m_user[1].m_strUserID);
	VBH::Assign(req.m_userCryptKey, pSession->m_user[0].m_strUserCryptKey);

	tradeInfo.m_nAsset = 1;

	DEF_ENCODE(tradeInfo, pInfoBuf, nInfoBuf);

	req.m_nActionID = TEST_CC::CTradeAction::EN_ACTION_ID;
	req.m_nSrcSessionID = pSession->m_nSessionID;
	req.m_proposal.Set(pInfoBuf, nInfoBuf);

	this->SendDscMessage(req, address);
}

void CQueryTradeService::StartQueryTrade(void)
{
	CQueryBlockSession* pSession;
	for (auto it = m_mapQueryTradeSession.begin(); it != m_mapQueryTradeSession.end(); ++it)
	{
		pSession = it.second;
		QueryTrade(pSession);
	}

	this->SetDscTimer(this, 1, true);
}


CQueryTradeService::CQueryTradeTimerHandler::CQueryTradeTimerHandler(CQueryTradeService& rService)
	: m_rService(rService)
{
}

void CQueryTradeService::CQueryTradeTimerHandler::OnTimer(void)
{
	m_rService.StartQueryTrade();
}
