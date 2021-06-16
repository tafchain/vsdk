//#ifndef VBH_PERF_TEST_QUERY_BLOCK_CLIENT_SERVICE_H_879965465465456456456434534544554234556
//#define VBH_PERF_TEST_QUERY_BLOCK_CLIENT_SERVICE_H_879965465465456456456434534544554234556
//
//#include "dsc/dsc_log.h"
//#include "dsc/dsc_reactor.h"
//#include "dsc/service_timer/dsc_service_timer_handler.h"
//#include "dsc/container/bare_hash_map.h"
//
//#include "vbh_comm/vbh_comm_msg_def.h"
//#include "vbh_perf_test_comm/vbh_perf_test_comm_id_def.h"
//
//class CVbhPerfTestQueryBlockClientService : public CDscReactor, public CDscServiceTimerHandler
//{
//public:
//	enum
//	{
//		EN_SERVICE_TYPE = VBH_TEST::VBH_PERF_TEST_QUERY_BLOCK_CLIENT_SERVICE_TYPE,
//		EN_HASH_MAP_BITES = 16,
//		EN_SESSION_TIMEOUT_VALUE = 60
//	};
//
//public:
//	CVbhPerfTestQueryBlockClientService(ACE_UINT64 nBlockNum, ACE_UINT32 nTradeGroupCount, ACE_UINT32 nChannelNum, bool bCrypt, ACE_INT32 nLimitTimes);
//
//private:
//	//���ڲ�ѯ���û�session
//	class CSessionUser
//	{
//	public:
//		CDscString m_strUserID;
//		CDscString m_strUserCryptKey;
//	};
//
//	class CQueryBlockSession : public CDscServiceTimerHandler
//	{
//	public:
//		CQueryBlockSession(CVbhPerfTestQueryBlockClientService& rService);
//
//	public:
//		virtual void OnTimer(void) override;
//
//	public:
//		ACE_UINT32 m_nChannelID = 1;
//		ACE_UINT32 m_nSessionID = 0;
//		CSessionUser m_user; //���ڽ��׵��û���
//		ACE_INT32 m_nLimitTimes = 0;//���Ʋ�ѯ�Ĵ���Ĭ��Ϊ0��������
//
//	public:
//		ACE_UINT32 m_nKey = 0;
//		CQueryBlockSession* m_pPrev = NULL;
//		CQueryBlockSession* m_pNext = NULL;
//
//	public:
//		CVbhPerfTestQueryBlockClientService& m_rService;
//	};
//
//public:
//	ACE_INT32 OnInit(void);
//	ACE_INT32 OnExit(void);
//	void OnTimer(void);
//
//	void OnTimeOut(CQueryBlockSession* pSession);
//
//	//������������û��Ŀͻ�����Կ
//	ACE_INT32 LoadClientCryptKey(CDscString strCryptKeyPath, CDscString& strClientCryptKey, const ACE_UINT32 nChannelID, const ACE_UINT32 nUserID);
//
//	//������������û����û�ID
//	ACE_INT32 LoadUserKey(CDscString strUserKeyPath, CDscString& strUserKey, const ACE_UINT32 nChannelID, const ACE_UINT32 nUserID);
//
//
//protected:
//	BIND_DSC_MESSAGE(VBH::CQueryBlockInfoCltCltRsp)
//
//public:
//	void OnDscMsg(VBH::CQueryBlockInfoCltCltRsp& rsp, const CDscMsg::CDscMsgAddr& rSrcMsgAddr);
//
//private:
//	//����һ�β�ѯ
//	void Query(CQueryBlockSession* pSession);
//
//private:
//	using query_block_map_session = CBareHashMap<ACE_UINT32, CQueryBlockSession, EN_HASH_MAP_BITES>;
//
//private:
//	ACE_INT32 m_nSum = 0;
//	ACE_INT32 m_nSumCount = 0;
//	ACE_UINT64 m_nTotalFailedRsp = 0;
//	ACE_UINT64 m_nTotalReq = 0;
//
//private:
//	query_block_map_session m_mapQueryBlockSession;
//
//	ACE_UINT32 m_nChannelNum = 1;		//�����ѯ��ͨ������
//	ACE_UINT32 m_nQueryGroupCount = 0; //�����ѯ������ //1��1���û�
//	ACE_UINT16 m_nClientServiceType; //������Ϣ��service-type
//
//	ACE_UINT64 m_nTotalRsp = 0;
//
//	ACE_INT32 m_nLimitTimes = 0;	// ÿ��Session�Ĳ�ѯ������Ĭ��Ϊ0������
//	bool m_bLimitTimes = false;		// ��־�Ƿ����ƴ���
//
//	ACE_UINT32 m_nQuerySum = 0;
//	// ACE_UINT32 m_nCount = 0;
//
//	ACE_UINT64 m_nBlockNum = 1;		// ��ѯ�������
//};
//
//#endif
