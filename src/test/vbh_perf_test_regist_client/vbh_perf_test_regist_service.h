#ifndef VBH_PERF_TEST_REGIST_SERVICE_H_8979465465345649684635416548960
#define VBH_PERF_TEST_REGIST_SERVICE_H_8979465465345649684635416548960

#include "dsc/dsc_log.h"
#include "dsc/dsc_reactor.h"
#include "dsc/service_timer/dsc_service_timer_handler.h"
#include "dsc/container/bare_hash_map.h"

#include "vbh_comm/comm_msg_def/vbh_comm_msg_clt_clt_def.h"
#include "vbh_perf_test_comm/vbh_perf_test_comm_id_def.h"

class CVbhPerfTestRegistService : public CDscReactor,  public CDscServiceTimerHandler
{
public:
	enum
	{
		EN_SESSION_TIMEOUT_VALUE = 120,
		EN_HASH_MAP_BITES = 16,
		EN_SERVICE_TYPE = VBH_TEST::VBH_PERF_TEST_REGIST_CLIENT_SERVICE_TYPE,
		EN_TRADE_SESSION_TIME_OUT = 6,
		EN_REGIST_NUM = 1000
	};

public:
	CVbhPerfTestRegistService(ACE_UINT32 nNeedRegistUserNum, ACE_UINT32 nChannelID, ACE_UINT32 nStartNum, bool bCrypt);

private:

	//用于交易的用户session //先用该session进行注册，再用该session进行交易
	class CRegistUserTimer : public CDscServiceTimerHandler
	{
	public:
		CRegistUserTimer(CVbhPerfTestRegistService& rService);

	public:
		void OnTimer(void) override;

	public:
		CVbhPerfTestRegistService& m_rServiece;
	};

	class CRegistUserSession : public CDscServiceTimerHandler
	{
	public:
		CRegistUserSession(CVbhPerfTestRegistService& rService);

	public:
		ACE_UINT32 m_nSessionID = 0; //session-id+1 = user-id
		CDscString m_strCltPubKey; //base64格式的 client-public-key
		CDscString m_strCltPriKey; //base64格式的 client-private-key
		ACE_UINT32 m_nUserIndex;

	public:
		void OnTimer(void) override;

	public:
		ACE_UINT32 m_nKey = 0;
		CRegistUserSession* m_pPrev = NULL;
		CRegistUserSession* m_pNext = NULL;

	public:
		CVbhPerfTestRegistService& m_rServiece;
	};

public:
	ACE_INT32 OnInit(void);
	ACE_INT32 OnExit(void);
	void OnTimer(void) override;

	//将生成好的user-key+client-crypt-key保存到文件中
	ACE_INT32 SaveUserInfo(CDscString& strUserCryptKey, CDscString& strVbhUserID, const ACE_UINT32 nUserIdx);

	//写文件
	ACE_INT32 WriteFile(const CDscString& strFileName, const CDscString& strCryptKey);

public:
	void StartRegistUser(void);
	//注册一个用于交易的用户
	void RegistUser(CRegistUserSession* pRegistUserSession, DSC::CDscShortBlob& cltPubKey);

protected:
	/*注册用户流程相关*/
	BIND_DSC_MESSAGE(VBH::CRegistUserCltCltRsp)

public:
	void OnDscMsg(VBH::CRegistUserCltCltRsp& rsp, const CDscMsg::CDscMsgAddr& rSrcMsgAddr);

private:
	using trade_user_map_session = CBareHashMap<ACE_UINT32, CRegistUserSession, EN_HASH_MAP_BITES>;

private:
	trade_user_map_session m_mapTradeUserSession;
	CRegistUserTimer m_registUserTimer;

	ACE_UINT32 m_nSessionID = 1;
	ACE_UINT32 m_nChannelID = 0;
	ACE_UINT16 m_nClientServiceType; //接收消息的service-type
	ACE_UINT32 m_nNeedRegistUserNum = 0; //需要注册的用户数

	ACE_UINT32 m_nRegistRspCount = 0;
	ACE_UINT32 m_nTimerCounter = 0;

	CDscString m_strUserKeyPath;
	ACE_UINT32 m_nStartNum = 0;

private:
	ACE_UINT32 m_nCurUserID = 0;
};

#endif
