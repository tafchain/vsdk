#ifndef VBH_PERF_TEST_QUERY_CLIENT_SERVICE_H_8799654654654654687651840327478978909
#define VBH_PERF_TEST_QUERY_CLIENT_SERVICE_H_8799654654654654687651840327478978909

#include "dsc/dsc_log.h"
#include "dsc/dsc_reactor.h"
#include "dsc/service_timer/dsc_service_timer_handler.h"
#include "dsc/container/bare_hash_map.h"

#include "vbh_comm/comm_msg_def/vbh_comm_msg_clt_clt_def.h"
#include "vbh_perf_test_comm/vbh_perf_test_comm_id_def.h"
#include "vbh_perf_test_client/load_user_key.h"
class CLoadUserKey;

class CQueryUserService : public CDscReactor, public CDscServiceTimerHandler, public CLoadUserKey
{
public:
	enum
	{
		EN_SERVICE_TYPE = VBH_TEST::VBH_PERF_TEST_QUERY_USER_CLIENT_SERVICE_TYPE,
		EN_HASH_MAP_BITES = 16,
		EN_SESSION_TIMEOUT_VALUE = 60
	};

public:
	CQueryUserService(ACE_UINT32 nTradeGroupCount, ACE_UINT32 nChannelID, bool bCrypt, ACE_INT32 nLimitTimes);

private:
	class CQueryUserSession : public CDscServiceTimerHandler
	{
	public:
		CQueryUserSession(CQueryUserService& rService);

	public:
		virtual void OnTimer(void) override;

	public:
		ACE_UINT32 m_nSessionID = 0;
		CDscString m_strUserID;
		CDscString m_strUserCryptKey;
		ACE_INT32 m_nLimitTimes = 0;//限制查询的次数默认为0，不限制

	public:
		ACE_UINT32 m_nKey = 0;
		CQueryUserSession* m_pPrev = NULL;
		CQueryUserSession* m_pNext = NULL;

	public:
		CQueryUserService& m_rService;
	};

public:
	ACE_INT32 OnInit(void);
	ACE_INT32 OnExit(void);
	void OnTimer(void);

	void OnTimeOut(CQueryUserSession* pSession);

protected:
	BIND_DSC_MESSAGE(VBH::CQueryCltCltRsp)

public:
	void OnDscMsg(VBH::CQueryCltCltRsp& rsp, const CDscMsg::CDscMsgAddr& rSrcMsgAddr);

private:
	//发起一次查询
	void Query(CQueryUserSession* pSession);

private:
	using query_user_map_session = CBareHashMap<ACE_UINT32, CQueryUserSession, EN_HASH_MAP_BITES>;

private:
	ACE_INT32 m_nSum = 0;
	ACE_INT32 m_nSumCount = 0;
	ACE_UINT64 m_nTotalFailedRsp = 0;
	ACE_UINT64 m_nTotalReq = 0;

private:
	query_user_map_session m_mapQueryUserSession;

	ACE_UINT32 m_nChannelID = 0;
	ACE_UINT32 m_nQueryGroupCount = 0; //参与交易的组数 //1组1个用户
	ACE_UINT16 m_nClientServiceType; //接收消息的service-type

	ACE_UINT64 m_nTotalRsp = 0;
	ACE_INT32 m_nLimitTimes = 0;	// 每个Session的查询次数，默认为0则不限制
	bool m_bLimitTimes = false;		// 标志是否限制次数

	ACE_UINT32 m_nQuerySum = 0;
};

#endif
