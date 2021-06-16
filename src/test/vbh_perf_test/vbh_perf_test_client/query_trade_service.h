#ifndef QUERY_TRADE_SERVICE_H_879965465465465468765184sdfgsdf3454
#define QUERY_TRADE_SERVICE_H_879965465465465468765184sdfgsdf3454

#include "dsc/dsc_log.h"
#include "dsc/dsc_reactor.h"
#include "dsc/service_timer/dsc_service_timer_handler.h"
#include "dsc/container/bare_hash_map.h"

#include "vbh_comm/comm_msg_def/vbh_comm_msg_clt_clt_def.h"
#include "vbh_perf_test_comm/vbh_perf_test_comm_id_def.h"
#include "vbh_perf_test_client/load_user_key.h"
class CLoadUserKey;


class CQueryTradeService : public CDscReactor, public CDscServiceTimerHandler,public CLoadUserKey
{
public:
	enum
	{
		EN_SERVICE_TYPE = VBH_TEST::VBH_PERF_TEST_QUERY_TRADE_CLIENT_SERVICE_TYPE,
		EN_HASH_MAP_BITES = 16,
		EN_SESSION_QUERY_WAIT_TIMEOUT_VALUE = 5,
		EN_SESSION_TIMEOUT_VALUE = 60
	};

public:
	CQueryTradeService(ACE_UINT32 nTradeGroupCount, ACE_UINT32 nChannelNum, bool bCrypt, ACE_INT32 nLimitTimes, ACE_UINT16 nPeerId);

private:
	//���ڲ�ѯ���û�session //���ø�session���н��ף����ø�session���в�ѯ
	class CSessionUser
	{
	public:
		CDscString m_strUserID;
		CDscString m_strUserCryptKey;
	};

	class CQueryBlockSession 
	{
	public:
		ACE_UINT32 m_nChannelID = 1;
		ACE_UINT32 m_nSessionID = 0;
		CSessionUser m_user[2]; //���ڽ��׵��û���
		CDscString m_txid;// ���ڲ�ѯ�Ľ���ID
		ACE_INT32 m_nLimitTimes = 0;//���Ʋ�ѯ�Ĵ���Ĭ��Ϊ0��������

	public:
		ACE_UINT32 m_nKey = 0;
		CQueryBlockSession* m_pPrev = NULL;
		CQueryBlockSession* m_pNext = NULL;
	};

	class CQueryTradeTimerHandler : public CDscServiceTimerHandler
	{
	public:
		CQueryTradeTimerHandler(CQueryTradeService& rService);

	public:
		virtual void OnTimer(void) override;

	public:
		CQueryTradeService& m_rService;
	};

public:
	ACE_INT32 OnInit(void);
	ACE_INT32 OnExit(void);
	void OnTimer(void);
	void OnTimeOut(CQueryBlockSession* pSession);


protected:
	BIND_DSC_MESSAGE(
		VBH::CQueryCltCltRsp,
		VBH::CProposeCltCltRsp
	)

public:
	void OnDscMsg(VBH::CQueryCltCltRsp& rsp, const CDscMsg::CDscMsgAddr& rSrcMsgAddr);
	void OnDscMsg(VBH::CProposeCltCltRsp& rsp, const CDscMsg::CDscMsgAddr& rSrcMsgAddr);

private:
	//����һ�β�ѯ
	void QueryTrade(CQueryBlockSession* pSession);
	// �Ƚ��ף����ѯ
	void Trade(CQueryBlockSession* pSession);

	void StartQueryTrade(void);

private:
	using query_user_map_session = CBareHashMap<ACE_UINT32, CQueryBlockSession, EN_HASH_MAP_BITES>;

private:
	ACE_INT32 m_nSum = 0;
	ACE_INT32 m_nSumCount = 0;
	ACE_UINT64 m_nTotalFailedRsp = 0;
	ACE_UINT64 m_nTotalReq = 0;

private:
	query_user_map_session m_mapQueryTradeSession;

	ACE_UINT32 m_nQueryGroupCount = 0; //���뽻�׵����� //1��2���û�
	ACE_UINT16 m_nClientServiceType; //������Ϣ��service-type
	ACE_UINT32 m_nChannelID = 0;

	ACE_UINT32 m_nQuerySum = 0;	
	ACE_UINT64 m_nTotalRsp = 0;

	ACE_INT32 m_nLimitTimes = 0;	// ÿ��Session�Ĳ�ѯ������Ĭ��Ϊ0������
	bool m_bLimitTimes = false;		// ��־�Ƿ����ƴ���
	ACE_UINT16 m_nPeerId = 1;
	
private:
	CQueryTradeTimerHandler m_queryTradeTimerHandler;
	ACE_UINT32 m_nNeedProposalCount = 0; //��Ҫ����Ľ��׸���
	ACE_UINT32 m_nProposalRspCount = 0;
};

#endif
