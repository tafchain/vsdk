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
		EN_GROUP_TRADE_TIME_INTERVAL = 1 //ÿ����ö�1���������һ�ν��ף���תһ�Σ���Ҫ��ʱ���� ����*ʱ��
	};

public:
	//nTotalTradeCount: �ܵĽ��׸�������Щ���׸�����nGroupCount���з��飬һ�������������˲���
	//nGroupCount: ���������//���������ַ������ף���ֹһ�������������ڣ���ͬһ���û����ж�ν���
	CTradeService(ACE_UINT32 nTotalTradeCount, ACE_UINT32 nGroupCount, ACE_UINT32 nChannelID, bool bCrypt, ACE_INT32 nLimitTimes);

private:
	//���ڽ��׵��û�session //���ø�session����ע�ᣬ���ø�session���н���
	class CTradeGroupTimer : public CDscServiceTimerHandler
	{
	public:
		CTradeGroupTimer(CTradeService& rService);

	public:
		void OnTimer(void) override;

	public:
		CTradeService& m_rServiece;
	};

	//���ڽ��׵��û�session //���ø�session����ע�ᣬ���ø�session���н���
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
		CSessionUser m_user[2]; //���ڽ��׵��û���
		bool m_bFrom = true;
		ACE_INT32 m_nLimitTimes = 0;
	};

public:
	ACE_INT32 OnInit(void);
	ACE_INT32 OnExit(void);
	void OnTimer(void);

	//��1���ڵ��û����н���
	void StartTrade(ACE_UINT32 nGroupCount);

protected:
	//�᰸����
	BIND_DSC_MESSAGE(VBH::CProposeCltCltRsp)

public:
	void OnDscMsg(VBH::CProposeCltCltRsp& rsp, const CDscMsg::CDscMsgAddr& rSrcMsgAddr);

private:
	//����һ�ν���
	void Trade(CTradeSession* pSession);


private:
	ACE_INT32 m_nSum = 0;
	ACE_INT32 m_nSumCount = 0;
	ACE_UINT64 m_nTotalFailedRsp = 0;
	ACE_UINT64 m_nTotalReq = 0;

private:
	CTradeSession* m_pTradeSession; //������ m_nTotalTradeCount 

	ACE_UINT32 m_nChannelID; //channel
	ACE_UINT32 m_nTotalTradeCount; //�ܵĽ��׸���

	ACE_UINT32 m_nGroupCount; //������� //������Ŵ�idx��0��ʼ //
	ACE_UINT32 m_nGroupTradeCount; //һ�������ڵĽ��׸���
	
	ACE_UINT64 m_nTotalRsp = 0;
	ACE_INT32 m_nLimitTimes = 0;	// ÿ��Session�Ĳ�ѯ������Ĭ��Ϊ0������
	bool m_bLimitTimes = false;		// ��־�Ƿ����ƴ���

	ACE_UINT16 m_nClientServiceType; //������Ϣ��service-type
	ACE_UINT32 m_nGroupNum; //�ַ�������

	ACE_UINT32 m_nTradeSum = 0;
    ACE_UINT32 m_nCount = 0;
};

#endif
