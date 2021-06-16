#ifndef VBH_PERF_TEST_TRADE_CLIENT_SERVICE_H_8799654654654654687651840327478978909
#define VBH_PERF_TEST_TRADE_CLIENT_SERVICE_H_8799654654654654687651840327478978909

#include "dsc/dsc_log.h"
#include "dsc/dsc_reactor.h"
#include "dsc/service_timer/dsc_service_timer_handler.h"
#include "dsc/container/bare_hash_map.h"

#include "vbh_comm/comm_msg_def/vbh_comm_msg_clt_clt_def.h"
#include "vbh_perf_test_comm/vbh_perf_test_comm_id_def.h"
#include "vbh_perf_test_client/load_user_key.h"
class CLoadUserKey;

class CTradeService : public CDscReactor, public CDscServiceTimerHandler, public CLoadUserKey
{
public:
	enum
	{
		EN_SERVICE_TYPE = VBH_TEST::VBH_PERF_TEST_TRADE_CLIENT_SERVICE_TYPE,
		EN_HASH_MAP_BITES = 20,
		EN_GROUP_TRADE_TIME_INTERVAL = 1 //每隔多久对1个分组进行一次交易，轮转一次，需要的时间是 组数*时间
	};

public:
	//nTotalTradeCount: 总的交易个数，这些交易个数按nGroupCount进行分组，一个交易由两个人参与
	//nGroupCount: 分组个数，//分组用来轮番发起交易，防止一个区块打包周期内，对同一个用户进行多次交易
	CTradeService(ACE_UINT32 nTotalTradeCount, ACE_UINT32 nGroupCount, ACE_UINT32 nChannelID, bool bCrypt, ACE_INT32 nLimitTimes);

private:
	//用于交易的用户session //先用该session进行注册，再用该session进行交易
	class CTradeGroupTimer : public CDscServiceTimerHandler
	{
	public:
		CTradeGroupTimer(CTradeService& rService);

	public:
		void OnTimer(void) override;

	public:
		CTradeService& m_rServiece;
	};

	//用于交易的用户session //先用该session进行注册，再用该session进行交易
	class CSessionUser
	{
	public:
		CDscString m_strUserID;
		CDscString m_strUserCryptKey;
	};

	class CTradeSession
	{
	public:
		ACE_UINT32 m_nSessionID = 1;
		CSessionUser m_user[2]; //用于交易的用户数
		bool m_bFrom = true;
		ACE_INT32 m_nLimitTimes = 0;
	};

public:
	ACE_INT32 OnInit(void);
	ACE_INT32 OnExit(void);
	void OnTimer(void);

	//对1组内的用户进行交易
	void StartTrade(ACE_UINT32 nGroupCount);

protected:
	//提案流程
	BIND_DSC_MESSAGE(VBH::CProposeCltCltRsp)

public:
	void OnDscMsg(VBH::CProposeCltCltRsp& rsp, const CDscMsg::CDscMsgAddr& rSrcMsgAddr);

private:
	//发起一次交易
	void Trade(CTradeSession* pSession);


private:
	ACE_INT32 m_nSum = 0;
	ACE_INT32 m_nSumCount = 0;
	ACE_UINT64 m_nTotalFailedRsp = 0;
	ACE_UINT64 m_nTotalReq = 0;

private:
	CTradeSession* m_pTradeSession; //长度是 m_nTotalTradeCount 

	ACE_UINT32 m_nChannelID; //channel
	ACE_UINT32 m_nTotalTradeCount; //总的交易个数

	ACE_UINT32 m_nGroupCount; //分组个数 //分组序号从idx从0开始 //
	ACE_UINT32 m_nGroupTradeCount; //一个分组内的交易个数
	
	ACE_UINT64 m_nTotalRsp = 0;
	ACE_INT32 m_nLimitTimes = 0;	// 每个Session的查询次数，默认为0则不限制
	bool m_bLimitTimes = false;		// 标志是否限制次数

	ACE_UINT16 m_nClientServiceType; //接收消息的service-type
	ACE_UINT32 m_nGroupNum; //分发的组数

	ACE_UINT32 m_nTradeSum = 0;
    ACE_UINT32 m_nCount = 0;
};

#endif
