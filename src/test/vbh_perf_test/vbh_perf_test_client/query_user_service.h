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
		ACE_INT32 m_nLimitTimes = 0;//���Ʋ�ѯ�Ĵ���Ĭ��Ϊ0��������

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
	//����һ�β�ѯ
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
	ACE_UINT32 m_nQueryGroupCount = 0; //���뽻�׵����� //1��1���û�
	ACE_UINT16 m_nClientServiceType; //������Ϣ��service-type

	ACE_UINT64 m_nTotalRsp = 0;
	ACE_INT32 m_nLimitTimes = 0;	// ÿ��Session�Ĳ�ѯ������Ĭ��Ϊ0������
	bool m_bLimitTimes = false;		// ��־�Ƿ����ƴ���

	ACE_UINT32 m_nQuerySum = 0;
};

#endif
