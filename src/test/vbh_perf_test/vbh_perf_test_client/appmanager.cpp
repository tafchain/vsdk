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
		m_nGroupCount = 1; //û�����ã�������һ��Ĭ��ֵ
	}

	if (m_nClientType == EN_TRADE_TYPE_TRADE) // ��ͨ����
	{
		CTradeService* pVbhPerfTestTradeClientService;

		DSC_NEW(pVbhPerfTestTradeClientService, CTradeService(m_nTotalTradeCount, m_nGroupCount, m_nChannelID, m_bCryptPath, m_nLimitTimes));

		CDscDispatcherCenterDemon::instance()->RegistDscTask(pVbhPerfTestTradeClientService, CTradeService::EN_SERVICE_TYPE, 1);
	}
	else if (m_nClientType == EN_QUERY_TYPE_QUERY_USER) // ��ѯ�û�
	{
		CQueryUserService* pCVbhPerfTestQueryClientService;
		DSC_NEW(pCVbhPerfTestQueryClientService, CQueryUserService(m_nGroupCount, m_nChannelID, m_bCryptPath, m_nLimitTimes));
		CDscDispatcherCenterDemon::instance()->RegistDscTask(pCVbhPerfTestQueryClientService, CQueryUserService::EN_SERVICE_TYPE, 1);
	}
	else if (m_nClientType == EN_QUERY_TYPE_QUERY_TRADE) // ��ѯ����
	{
		CQueryTradeService* pQueryTradeService;
		DSC_NEW(pQueryTradeService, CQueryTradeService(m_nGroupCount, m_nChannelID, m_bCryptPath, m_nLimitTimes, m_nPeerID));
		CDscDispatcherCenterDemon::instance()->RegistDscTask(pQueryTradeService, CQueryTradeService::EN_SERVICE_TYPE, 1);
	}
	else if (m_nClientType == EN_QUERY_TYPE_QUERY_BLOCK) // ��ѯ����
	{

	}

	return 0;
}

void CPerfTestAppManager::OnParam(const int argc, ACE_TCHAR* argv)
{
	if (argc == 'g' || argc == 'G') //��������£����뽻�׵������� ��ѯ����£�����Ĳ�ѯ������ѯ�����������ȷ����ף������û�����Ҫ�Ǹ�ֵ��2����
	{
		DSC::DscAtoi(argv, m_nGroupCount);
	}
	else if (argc == 'u' || argc == 'U') //���ڽ���ʱ��Ҫ����ѯ����Ҫ�˲��������뽻�׵��û��������뽻�׵��û�����/2�������channel�Ľ��׸���
	{
		DSC::DscAtoi(argv, m_nTotalTradeCount);
		m_nTotalTradeCount /= 2;
	}
	else if (argc == 'c' || argc == 'C') //ָ����channelID
	{
		DSC::DscAtoi(argv, m_nChannelID);
	}
	else if (argc == 'y' || argc == 'Y') //�Ƿ����, Ĭ�ϼ���
	{
		if (strcmp(argv, "no-crypt") == 0)
		{
			m_bCryptPath = false;
		}
	}
	else if (argc == 'q' || argc == 'Q') // �ͻ������ͣ�Ĭ�ϲ���Ϊ��ͨ����
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
	else if (argc == 'b' || argc == 'B') // ����ID����ѯ����ʱ����Ҫ����
	{
		DSC::DscAtoi(m_nBlockNum, argv, ::strlen(argv));
	}
	else if (argc == 'l' || argc == 'L') // ��ѡ ÿ��Session�Ĳ�ѯ������Ĭ��Ϊ0������
	{
		DSC::DscAtoi(argv, m_nLimitTimes);
	}
	else if (argc == 'p' || argc == 'P') // ��ѡ ��ѯ������ѡ���peerid
	{
		DSC::DscAtoi(argv, m_nPeerID);
	}
}

ACE_UINT32 CPerfTestAppManager::GetTestUserNum(void) const
{
	return m_nGroupCount;
}
