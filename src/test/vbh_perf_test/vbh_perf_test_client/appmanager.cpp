#include "dsc/dsc_comm_func.h"
#include "dsc/dispatcher/dsc_dispatcher_center.h"

#include "vbh_comm/vbh_encrypt_lib.h"

#include "vbh_perf_test_client/appmanager.h"
#include "vbh_perf_test_client/trade_service.h"
#include "vbh_perf_test_client/query_user_service.h"
#include "vbh_perf_test_client/query_trade_service.h"
#include "vbh_perf_test_client/vbh_perf_test_query_block_client_service.h"


ACE_INT32 CPerfTestAppManager::OnInit()
{
	VBH::InitOpenSsl();

	DSC_FORWARD_CALL(CDscAppManager::OnInit() );

	if (m_nGroupCount < 1)
	{
		m_nGroupCount = 1; //没有设置，则设置一个默认值
	}

	if (m_nClientType == EN_TRADE_TYPE_TRADE) // 普通交易
	{
		CTradeService* pVbhPerfTestTradeClientService;

		DSC_NEW(pVbhPerfTestTradeClientService, CTradeService(m_nTotalTradeCount, m_nGroupCount, m_nChannelID, m_bCryptPath, m_nLimitTimes));

		CDscDispatcherCenterDemon::instance()->RegistDscTask(pVbhPerfTestTradeClientService, CTradeService::EN_SERVICE_TYPE, 1);
	}
	else if (m_nClientType == EN_QUERY_TYPE_QUERY_USER) // 查询用户
	{
		CQueryUserService* pCVbhPerfTestQueryClientService;
		DSC_NEW(pCVbhPerfTestQueryClientService, CQueryUserService(m_nGroupCount, m_nChannelID, m_bCryptPath, m_nLimitTimes));
		CDscDispatcherCenterDemon::instance()->RegistDscTask(pCVbhPerfTestQueryClientService, CQueryUserService::EN_SERVICE_TYPE, 1);
	}
	else if (m_nClientType == EN_QUERY_TYPE_QUERY_TRADE) // 查询交易
	{
		CQueryTradeService* pQueryTradeService;
		DSC_NEW(pQueryTradeService, CQueryTradeService(m_nGroupCount, m_nChannelID, m_bCryptPath, m_nLimitTimes, m_nPeerID));
		CDscDispatcherCenterDemon::instance()->RegistDscTask(pQueryTradeService, CQueryTradeService::EN_SERVICE_TYPE, 1);
	}
	else if (m_nClientType == EN_QUERY_TYPE_QUERY_BLOCK) // 查询区块
	{

	}

	return 0;
}

void CPerfTestAppManager::OnParam(const int argc, ACE_TCHAR* argv)
{
	if (argc == 'g' || argc == 'G') //交易情况下：参与交易的组数； 查询情况下：参与的查询数（查询交易由于是先发起交易，所以用户数需要是该值的2倍）
	{
		DSC::DscAtoi(argv, m_nGroupCount);
	}
	else if (argc == 'u' || argc == 'U') //仅在交易时需要，查询不需要此参数，参与交易的用户数，参与交易的用户个数/2就是这个channel的交易个数
	{
		DSC::DscAtoi(argv, m_nTotalTradeCount);
		m_nTotalTradeCount /= 2;
	}
	else if (argc == 'c' || argc == 'C') //指定的channelID
	{
		DSC::DscAtoi(argv, m_nChannelID);
	}
	else if (argc == 'y' || argc == 'Y') //是否加密, 默认加密
	{
		if (strcmp(argv, "no-crypt") == 0)
		{
			m_bCryptPath = false;
		}
	}
	else if (argc == 'q' || argc == 'Q') // 客户端类型，默认不传为普通交易
	{
		if (strcmp(argv, "user") == 0)
		{
			m_nClientType = EN_QUERY_TYPE_QUERY_USER;
		}
		else if (strcmp(argv, "trade") == 0)
		{
			m_nClientType = EN_QUERY_TYPE_QUERY_TRADE;
		}
		else if (strcmp(argv, "block") == 0)
		{
			m_nClientType = EN_QUERY_TYPE_QUERY_BLOCK;
		}
	}
	else if (argc == 'b' || argc == 'B') // 区块ID，查询区块时候需要传入
	{
		DSC::DscAtoi(m_nBlockNum, argv, ::strlen(argv));
	}
	else if (argc == 'l' || argc == 'L') // 可选 每个Session的查询次数，默认为0则不限制
	{
		DSC::DscAtoi(argv, m_nLimitTimes);
	}
	else if (argc == 'p' || argc == 'P') // 可选 查询交易所选择的peerid
	{
		DSC::DscAtoi(argv, m_nPeerID);
	}
}

ACE_UINT32 CPerfTestAppManager::GetTestUserNum(void) const
{
	return m_nGroupCount;
}
