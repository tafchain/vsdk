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
//	//用于查询的用户session
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
//		CSessionUser m_user; //用于交易的用户数
//		ACE_INT32 m_nLimitTimes = 0;//限制查询的次数默认为0，不限制
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
//	//加载属于这个用户的客户端秘钥
//	ACE_INT32 LoadClientCryptKey(CDscString strCryptKeyPath, CDscString& strClientCryptKey, const ACE_UINT32 nChannelID, const ACE_UINT32 nUserID);
//
//	//加载属于这个用户的用户ID
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
//	//发起一次查询
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
//	ACE_UINT32 m_nChannelNum = 1;		//参与查询的通道个数
//	ACE_UINT32 m_nQueryGroupCount = 0; //参与查询的组数 //1组1个用户
//	ACE_UINT16 m_nClientServiceType; //接收消息的service-type
//
//	ACE_UINT64 m_nTotalRsp = 0;
//
//	ACE_INT32 m_nLimitTimes = 0;	// 每个Session的查询次数，默认为0则不限制
//	bool m_bLimitTimes = false;		// 标志是否限制次数
//
//	ACE_UINT32 m_nQuerySum = 0;
//	// ACE_UINT32 m_nCount = 0;
//
//	ACE_UINT64 m_nBlockNum = 1;		// 查询的区块号
//};
//
//#endif
