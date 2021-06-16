#ifndef VBH_ADAPTER_BASE_H_356893127368177848
#define VBH_ADAPTER_BASE_H_356893127368177848

#include "dsc/protocol/hts/dsc_hts_service.h"
#include "dsc/service_timer/dsc_service_timer_handler.h"
#include "dsc/container/bare_hash_map.h"
#include "dsc/protocol/mcp/mcp_client_handler.h"

#include "common/vbh_comm_msg_def.h"

class PLUGIN_EXPORT CVbhAdapterBase : public CDscHtsClientService
{
	///////////////////////////  PUBLIC INTERFACE BEGIN ////////////////////
public:
	ACE_INT32 RegistUser(ACE_UINT32 nRequestID, ACE_UINT32 nChannelID, bool bWaitNotify, const DSC::CDscShortBlob & userInfo,const DSC::CDscShortBlob& serverCryptKey);
	ACE_INT32 Propose(ACE_UINT32 nRequestID, ACE_UINT32 nChannelID, bool bWaitNotify, ACE_UINT32 nActionID, const DSC::CDscShortBlob& userKey, const DSC::CDscShortBlob &propose);
	ACE_INT32 QueryUserInfo(ACE_UINT32 nRequestID, ACE_UINT32 nChannelID, const DSC::CDscShortBlob& userKey);
	ACE_INT32 QueryTransactionInfo(ACE_UINT32 nRequestID, ACE_UINT32 nChannelID, const DSC::CDscShortBlob& transKey);
	ACE_INT32 CreateInformation(ACE_UINT32 nRequestID, ACE_UINT32 nChannelID, bool bWaitNotify,const DSC::CDscShortBlob &userCryptKey, const DSC::CDscShortBlob &proposal);
	///////////////////////////  PUBLIC INTERFACE END ////////////////////

	///////////////////////////  CALLBACK INTERFACE BEGIN ////////////////////
public:
	virtual void OnRegistUserResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, const DSC::CDscShortBlob& userKey);
	virtual void OnRegistUserResultNotify(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID);
	virtual void OnCreateInformationRespondse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, const DSC::CDscShortBlob& infoKey);
	virtual void OnCreateInformationResultNotify(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID);
	virtual void OnProposeResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, const DSC::CDscShortBlob &transKey);
	virtual void OnProposeResultNotify(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID);
	virtual void OnQueryUserInfoResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, const DSC::CDscShortBlob& userInfo);
	virtual void OnQueryTransactionInfoResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, const DSC::CDscBlob &tranInfo);
	///////////////////////////  CALLBACK INTERFACE END ////////////////////

private:
	enum
	{
		EN_HASH_MAP_BITS = 16, //�����̵�session-map��ģ������ֱ�Ӳ�ѯ
		EN_SESSION_TIMEOUT_VALUE = 60,
		EN_SESSION_WAIT_NOTIFY_TIMEOUT_VALUE = 300
	};

private:
	//endorser���ӵ�ַ��Ϣ
	class CEndorserAddr
	{
	public:
		CDscString m_strIpAddr;
		ACE_INT32 m_nPort = 0;
		ACE_UINT32 m_nHandleID = 0;
	};

	//peer���ӵ�ַ��Ϣ
	class CVbhPeer
	{
	public:
		ACE_UINT16 m_nPeerID = 0;
		DSC::CDscList <CEndorserAddr> m_lstEndorserAddr;
	};

	//��������ͨ�ž��
	class IBaseSession;
	class CVbhAdapterBaseHandler : public CMcpClientHandler
	{
	public:
		CVbhAdapterBaseHandler(CMcpClientService& rService, ACE_HANDLE handle, const ACE_INT32 nHandleID,
			PROT_COMM::CDscIpAddr& rRemoteAddr, PROT_COMM::CDscIpAddr& localAddr);

	public:
		ACE_UINT32 m_nIndex = 0; //ʹ�� CDscTypeArray ��������߱��Ľӿ�

	public:
		CDscTypeArray<IBaseSession> m_arrSession;
	};

	using CVbhAdapterBaseHandlerPtr = CVbhAdapterBaseHandler *;

	//һ��peer��peer����session
	class CVbhPeerConnectSession
	{
	public:
		//�ͷ�hander����
		void FreeHandlerArray(void);

		//���1��handler���������ռ䲻��ʱ����������ռ�
		void AddHandler(CVbhAdapterBaseHandler* pHandler);

		//�Ƴ�һ��handler���Ƴ�ָ����handler�󣬽�����handler��ǰ�ƶ�
		void RemoveHandler(CVbhAdapterBaseHandler* pHandler);

		//�ƶ�������������ɾ��ԭsession�е�����ָ�룬
		void MoveCopy(CVbhPeerConnectSession& srcSession);

	public:
		CVbhAdapterBaseHandlerPtr* m_ppVbhClientServiceHandler = nullptr;
		ACE_UINT16 m_nPeerID = 0;
		ACE_UINT32 m_nHandlerNum = 0; //m_ppVbhClientServiceHandler �����У�ʵ��Ԫ�صĸ���
		ACE_UINT32 m_nArrayLen = 0; //m_ppVbhClientServiceHandler ���ݵ�ʵ�ʳ��ȣ�//�ռ䲻��ʱ��ÿ������8��
		ACE_UINT32 m_nIndex = 0; //��Ҫ�����ӽ���ʱΪindex��һ�������ʼֵ���Ա�֤���пͻ�����������ӵ�ĳ��peer������endorser.
	};

	//����peer����session����Ϣ���
	class CVbhPeerConnectSessionSummaryInfo
	{
	public:
		//�ͷ�session����
		void FreeSessionArray(void);

		//����ָ��peer-id ��session //�Ҳ�������nullָ��
		CVbhPeerConnectSession* FindSession(ACE_UINT32 nPeerID);

		//����һ���յ�session //������û�п�session����չ���飬������һ���յ�session
		CVbhPeerConnectSession* AllocSession(void);

		//�ͷ�1��session //��session����������һ�η���
		void FreeSession(CVbhPeerConnectSession* pSession);

	public:
		CVbhPeerConnectSession* m_pArrPeerSession = nullptr;
		ACE_UINT32 m_nSessionNum = 0; //m_pArrPeerSession �����У�ʵ��Ԫ�صĸ���
		ACE_UINT32 m_nArrayLen = 0; //m_pArrPeerSession ���ݵ�ʵ�ʳ��ȣ�//�ռ䲻��ʱ��ÿ������8��
		ACE_UINT32 m_nIndex = 0;
	};

private:
	//ͨ�û���session 
	class IBaseSession : public CDscServiceTimerHandler
	{
	public:
		IBaseSession(CVbhAdapterBase& rClientService);

	public:
		//��OnNetError�в����������ϵ��ֱ�Ӵ�map���Ƴ�session�����ͷ�session
		//��ʱ������ϵ�Ľ����mcp-handler�Ĵ�����е�
		virtual void OnNetError(CVbhAdapterBaseHandler* pVbhClientServiceHandler) = 0; //�������ʱ���ͷű�session�ĺ���

	public:
		ACE_UINT32 m_nCltSessionID; //�ͻ���sessionID
		ACE_UINT32 m_nSrcSessionID; //����ҵ�����󷽵�sessionID

	public:
		ACE_UINT32 m_nIndex = 0; //ʹ�� CDscTypeArray ��������߱��Ľӿ�

	protected:
		CVbhAdapterBase& m_rClientService;
	};

	//ע���û���session
	class CRegistUserSession : public IBaseSession
	{
	public:
		CRegistUserSession(CVbhAdapterBase& rClientService);

	public:
		virtual void OnTimer(void) override;
		virtual void OnNetError(CVbhAdapterBaseHandler* pVbhClientServiceHandler) override;

	public:
		CDscTypeArray<CVbhAdapterBaseHandler> m_arrHandler; //�ͻ��˺Ͷ������ڵ������

	//Ϊ�ֽڶ��룬��Լ�ռ䣬���������ű���
	public:
		bool m_bWaitNotify = false; //�û��Ƿ�ȴ�֪ͨ
		ACE_UINT32 m_nSucceedRspNum = 0; //����ڵ��гɹ�Ӧ�����Ŀ
		ACE_UINT32 m_nEsSessionID; //����endorser service �� session 

		CDscString m_strCltPubKey; // �ͻ��˹�Կ
		CDscString m_strSvrPriKey; //�����˽Կ
		CDscString m_strEnvelopeKey; //�Գ���Կ

	public:
		ACE_UINT32 m_nKey = 0;//hash map�õ�
		CRegistUserSession* m_pPrev = NULL;//hash map�õ�
		CRegistUserSession* m_pNext = NULL;//hash map�õ�

	public:
		CVbhAdapterBaseHandler* m_pSubmitEsHandler = NULL; //��ѡ�е�endorser service ��handle-session
		CDscString m_strCcGenerateUserInfo; //CC����ע��ʱ��Ϊ�û����ɵĳ�ʼ��Ϣ
	};

	//ע���û��ȴ�notify��session
	class CRegistUserWaitNotitySession : public IBaseSession
	{
	public:
		CRegistUserWaitNotitySession(CVbhAdapterBase& rClientService);

	public:
		virtual void OnTimer(void) override;
		virtual void OnNetError(CVbhAdapterBaseHandler* pVbhClientServiceHandler) override;

	public:
		CVbhAdapterBaseHandler* m_pHandler; //�ȴ�Ӧ��ʱ��������

	public:
		ACE_UINT32 m_nKey = 0;
		CRegistUserWaitNotitySession* m_pPrev = NULL;
		CRegistUserWaitNotitySession* m_pNext = NULL;
	};

	//ע���û���session
	class CCreateInformationSession : public IBaseSession
	{
	public:
		CCreateInformationSession(CVbhAdapterBase& rClientService);

	public:
		virtual void OnTimer(void) override;
		virtual void OnNetError(CVbhAdapterBaseHandler* pVbhClientServiceHandler) override;

	public:
		CDscTypeArray<CVbhAdapterBaseHandler> m_arrHandler; //�ͻ��˺Ͷ������ڵ������

	//Ϊ�ֽڶ��룬��Լ�ռ䣬���������ű���
	public:
		bool m_bWaitNotify = false; //�û��Ƿ�ȴ�֪ͨ
		ACE_UINT32 m_nSucceedRspNum = 0; //����ڵ��гɹ�Ӧ�����Ŀ
		ACE_UINT32 m_nEsSessionID; //����endorser service �� session 

	public:
		ACE_UINT32 m_nKey = 0;//hash map�õ�
		CCreateInformationSession* m_pPrev = NULL;//hash map�õ�
		CCreateInformationSession* m_pNext = NULL;//hash map�õ�

	public:
		CVbhAdapterBaseHandler* m_pSubmitEsHandler = NULL; //��ѡ�е�endorser service ��handle-session
		CDscString m_strCcGenerateValue; //CC������ʱ��Ϊinformation���ɵĳ�ʼֵ
	};

	//����information�ȴ�notify��session
	class CCreateInformationWaitNotitySession : public IBaseSession
	{
	public:
		CCreateInformationWaitNotitySession(CVbhAdapterBase& rClientService);

	public:
		virtual void OnTimer(void) override;
		virtual void OnNetError(CVbhAdapterBaseHandler* pVbhClientServiceHandler) override;

	public:
		CVbhAdapterBaseHandler* m_pHandler; //�ȴ�Ӧ��ʱ��������

	public:
		ACE_UINT32 m_nKey = 0;
		CCreateInformationWaitNotitySession* m_pPrev = NULL;
		CCreateInformationWaitNotitySession* m_pNext = NULL;
	};

	//�����᰸���׵�session
	class CProposeSession : public IBaseSession
	{
	public:
		CProposeSession(CVbhAdapterBase& rClientService);

	public:
		virtual void OnTimer(void) override;
		virtual void OnNetError(CVbhAdapterBaseHandler* pVbhClientServiceHandler) override;

	public:
		CDscTypeArray<CVbhAdapterBaseHandler> m_arrHandler; //�ͻ��˺Ͷ������ڵ������

	public:
		bool m_bWaitNotify = false; //�û��Ƿ�ȴ�֪ͨ
		ACE_UINT32 m_nSucceedRspNum = 0; //����ڵ��гɹ�Ӧ�����Ŀ
		CVbhAdapterBaseHandler* m_pSubmitEsHandler = NULL; //��ѡ�е�endorser service ��handle-session
		ACE_UINT32 m_nEsSessionID; //����endorser service �� session 
		CDscString m_strTransContent; //transaction - content

	public:
		ACE_UINT32 m_nKey = 0;
		CProposeSession* m_pPrev = NULL;
		CProposeSession* m_pNext = NULL;
	};

	//�����᰸�ȴ�notify��session
	class CProposeWaitNotifySession : public IBaseSession
	{
	public:
		CProposeWaitNotifySession(CVbhAdapterBase& rClientService);

	public:
		virtual void OnTimer(void) override;
		virtual void OnNetError(CVbhAdapterBaseHandler* pVbhClientServiceHandler) override;

	public:
		CVbhAdapterBaseHandler* m_pHandler; //�ȴ�Ӧ��ʱ��������

	public:
		ACE_UINT32 m_nKey = 0;
		CProposeWaitNotifySession* m_pPrev = NULL;
		CProposeWaitNotifySession* m_pNext = NULL;
	};

	//��ѯ�û���Ϣ��session
	class CQueryUserSession : public IBaseSession
	{
	public:
		CQueryUserSession(CVbhAdapterBase& rRegistUserService);

	public:
		virtual void OnTimer(void) override;
		virtual void OnNetError(CVbhAdapterBaseHandler* pVbhClientServiceHandler) override;

	public:
		CVbhAdapterBaseHandler* m_pHandler; //��ѯʱ��������

	public:
		ACE_UINT32 m_nKey = 0;
		CQueryUserSession* m_pPrev = NULL;
		CQueryUserSession* m_pNext = NULL;
	};

	//��ѯ�����session
	class CQueryTransactionSession : public IBaseSession
	{
	public:
		CQueryTransactionSession(CVbhAdapterBase& rClientService);

	public:
		virtual void OnTimer(void) override;
		virtual void OnNetError(CVbhAdapterBaseHandler* pVbhClientServiceHandler) override;

	public:
		CVbhAdapterBaseHandler* m_pHandler;

	public:
		ACE_UINT32 m_nKey = 0;
		CQueryTransactionSession* m_pPrev = NULL;
		CQueryTransactionSession* m_pNext = NULL;
	};

public:
	enum
	{
		EN_SERVICE_TYPE = VBH::EN_VBH_CLIENT_SERVICE_TYPE
	};

public:
	ACE_INT32 OnInit(void);
	ACE_INT32 OnExit(void);

protected:
	BEGIN_HTS_MESSAGE_BIND
		/* ע���û� */
		BIND_HTS_MESSAGE(VBH::CRegistUserEsCltRsp)
		BIND_HTS_MESSAGE(VBH::CSubmitRegistUserTransactionEsCltRsp)
		BIND_HTS_MESSAGE(VBH::CSubscribeRegistUserTransactionEsCltNotify)
		/* ����information */
		BIND_HTS_MESSAGE(VBH::CCreateInformationEsCltRsp)
		BIND_HTS_MESSAGE(VBH::CSubmitCreateInformationTransactionEsCltRsp)
		BIND_HTS_MESSAGE(VBH::CSubscribeCreateInformationTransactionEsCltNotify)
		/* �����᰸ */
		BIND_HTS_MESSAGE(VBH::CProposeEsCltRsp)
		BIND_HTS_MESSAGE(VBH::CSubmitProposalTransactionEsCltRsp)
		BIND_HTS_MESSAGE(VBH::CSubscribeProposeTransactionEsCltNotify)
		/* ֱ�Ӳ�ѯ */
		BIND_HTS_MESSAGE(VBH::CQueryUserInfoEsCltRsp)
		BIND_HTS_MESSAGE(VBH::CQueryTransInfoEsCltRsp)
		END_HTS_MESSAGE_BIND

public:
	/* ע���û� */
	ACE_INT32 OnHtsMsg(VBH::CRegistUserEsCltRsp& rRegistUserRsp, CMcpHandler* pMcpHandler);
	ACE_INT32 OnHtsMsg(VBH::CSubmitRegistUserTransactionEsCltRsp& rSubmitTransRsp, CMcpHandler* pMcpHandler);
	ACE_INT32 OnHtsMsg(VBH::CSubscribeRegistUserTransactionEsCltNotify& rNotify, CMcpHandler* pMcpHandler);
	/* ����information */
	ACE_INT32 OnHtsMsg(VBH::CCreateInformationEsCltRsp& rCreateInfoRsp, CMcpHandler* pMcpHandler);
	ACE_INT32 OnHtsMsg(VBH::CSubmitCreateInformationTransactionEsCltRsp& rSubmitTransRsp, CMcpHandler* pMcpHandler);
	ACE_INT32 OnHtsMsg(VBH::CSubscribeCreateInformationTransactionEsCltNotify& rNotify, CMcpHandler* pMcpHandler);
	/* �����᰸ */
	ACE_INT32 OnHtsMsg(VBH::CProposeEsCltRsp& rSubmitProposalRsp, CMcpHandler* pMcpHandler);
	ACE_INT32 OnHtsMsg(VBH::CSubmitProposalTransactionEsCltRsp& rSubmitTransRsp, CMcpHandler* pMcpHandler);
	ACE_INT32 OnHtsMsg(VBH::CSubscribeProposeTransactionEsCltNotify& rNotify, CMcpHandler* pMcpHandler);
	/* ֱ�Ӳ�ѯ */
	ACE_INT32 OnHtsMsg(VBH::CQueryUserInfoEsCltRsp& rQueryUserRsp, CMcpHandler* pMcpHandler);
	ACE_INT32 OnHtsMsg(VBH::CQueryTransInfoEsCltRsp& rQueryTransRsp, CMcpHandler* pMcpHandler);

public:
	//�ͷ��ж�����ӵ�session�� (ע���û�session�� �᰸session)
	//pIgnoreMcpHandler��������ָ��Ҫ�����ĸ�mcp-handler��ȡ���������ڷ����������ʱ����ǰ���ڶ������Ǹ�mcp-handler����Ҫȡ������������ͳһ����
	template <typename SESSION_TYPE>
	void OnReleaseMultiMcpHandler(SESSION_TYPE* pSession, CVbhAdapterBaseHandler* pIgnoreMcpHandler);

	template <typename SESSION_TYPE>
	void OnReleaseMultiMcpHandler(SESSION_TYPE* pSession);

	//ɾ�� ��session �� ���ύ�ڵ�����session�Ĺ�����ϵ (ע���û�session�� �᰸session)
	template <typename SESSION_TYPE>
	void RemoveNonSubmitMcpHandler(SESSION_TYPE* pSession, CMcpHandler* pMcpHandler);

	//�ͷ�ֻ��1�����ӵ�session��(��ѯsession�� wait-notify-session)
	template <typename SESSION_TYPE>
	void OnReleaseSingleMcpHanler(SESSION_TYPE* pSession);

	void OnResponse(CRegistUserSession* pRegistUserSession, const ACE_INT32 nReturnCode,
		const DSC::CDscShortBlob& userKey = DSC::CDscShortBlob(nullptr, 0));
	void OnTimeOut(CRegistUserSession* pRegistUserSession);
	void OnNetError(CRegistUserSession* pRegistUserSession, CVbhAdapterBaseHandler* pVbhClientServiceHandler);

	void OnResponse(CRegistUserWaitNotitySession* pNotifySession, const ACE_INT32 nReturnCode);
	void OnTimeOut(CRegistUserWaitNotitySession* pNotifySession);
	void OnNetError(CRegistUserWaitNotitySession* pNotifySession);

	void OnResponse(CCreateInformationSession* pCreateInfoSession, const ACE_INT32 nReturnCode,
		const DSC::CDscShortBlob& infoKey = DSC::CDscShortBlob(nullptr, 0));
	void OnTimeOut(CCreateInformationSession* pCreateInfoSession);
	void OnNetError(CCreateInformationSession* pCreateInfoSession, CVbhAdapterBaseHandler* pVbhClientServiceHandler);

	void OnResponse(CCreateInformationWaitNotitySession* pNotifySession, const ACE_INT32 nRetrunCode);
	void OnTimeOut(CCreateInformationWaitNotitySession* pNotifySession);
	void OnNetError(CCreateInformationWaitNotitySession* pNotifySession);

	void OnResponse(CProposeSession* pProposeSession, const ACE_INT32 nRetrunCode,
		const DSC::CDscShortBlob& transKey = DSC::CDscShortBlob(nullptr, 0));
	void OnTimeOut(CProposeSession* pProposeSession);
	void OnNetError(CProposeSession* pProposeSession, CVbhAdapterBaseHandler* pVbhClientServiceHandler);

	void OnResponse(CProposeWaitNotifySession* pNotifySession, const ACE_INT32 nRetrunCode);
	void OnTimeOut(CProposeWaitNotifySession* pNotifySession);
	void OnNetError(CProposeWaitNotifySession* pNotifySession);

	void OnResponse(CQueryUserSession* pQueryUserSession, const ACE_INT32 nRetrunCode, DSC::CDscShortBlob& userInfo);
	void OnTimeOut(CQueryUserSession* pQueryUserSession);
	void OnNetError(CQueryUserSession* pQueryUserSession);

	void OnResponse(CQueryTransactionSession* pQueryTransSession, const ACE_INT32 nRetrunCode, DSC::CDscBlob& transInfo);
	void OnTimeOut(CQueryTransactionSession* pQueryTransSession);
	void OnNetError(CQueryTransactionSession* pQueryTransSession);

protected:
	virtual ACE_INT32 OnConnectedNodify(CMcpClientHandler* pMcpClientHandler) override;
	virtual void OnNetworkError(CMcpHandler* pMcpHandler) override;
	virtual CMcpClientHandler* AllocMcpHandler(ACE_HANDLE handle, const ACE_UINT32 nHandleID,
		PROT_COMM::CDscIpAddr& rRemoteAddr, PROT_COMM::CDscIpAddr& localAddr) override;

private:
	//����McpHandler�ľ��ID�ҵ���Ӧ��peerID //���ID��peerID���ڳ�ʼ��ʱ�ͽ����õ�
	ACE_INT32 GetPeerID(const ACE_UINT32 nHandleID);
	CVbhAdapterBaseHandler* GetMcpHandler(void);
	ACE_INT32 LoadPeerInfo(void);

	//ͬʱ��������ڵ㷢����Ϣ��������Ϣ��ͬʱ������ҵ��session�ͱ���ڵ�����handler�Ĺ�����ϵ
	template <typename REQ_TYPE, typename SESSION_TYPE>
	ACE_INT32 SendMultiEndorseMessage(REQ_TYPE& req, SESSION_TYPE* pSession);

	//��һ���ڵ㷢����Ϣ�� ����ҵ��session��endorser����handler�Ĺ�����ϵ
	template <typename REQ_TYPE, typename SESSION_TYPE>
	ACE_INT32 SendSingleEndoreMessage(REQ_TYPE& req, SESSION_TYPE* pSession);

private:
	//ע���û���session-map
	using regist_user_session_map_type = CBareHashMap<ACE_UINT32, CRegistUserSession, EN_HASH_MAP_BITS>;
	using regist_user_wait_notify_session_map_type = CBareHashMap<ACE_UINT32, CRegistUserWaitNotitySession, EN_HASH_MAP_BITS>;
	//����information��session-map
	using create_information_session_map_type = CBareHashMap<ACE_UINT32, CCreateInformationSession, EN_HASH_MAP_BITS>;
	using create_information_wait_notify_session_map_type = CBareHashMap<ACE_UINT32, CCreateInformationWaitNotitySession, EN_HASH_MAP_BITS>;
	//�᰸/���������session-map
	using propose_session_map_type = CBareHashMap<ACE_UINT32, CProposeSession, EN_HASH_MAP_BITS>;
	using propose_wait_notify_session_map_type = CBareHashMap<ACE_UINT32, CProposeWaitNotifySession, EN_HASH_MAP_BITS>;
	//ֱ�Ӳ�ѯ��session-map
	using query_user_map_type = CBareHashMap<ACE_UINT32, CQueryUserSession, EN_HASH_MAP_BITS>;
	using query_transaction_map_type = CBareHashMap<ACE_UINT32, CQueryTransactionSession, EN_HASH_MAP_BITS>;

private:
	//ע���û���session-map
	regist_user_session_map_type m_mapRegistUserSession;
	regist_user_wait_notify_session_map_type m_mapRegistUserWaitNotifySession;
	//����information��session-map
	create_information_session_map_type m_mapCreateInfoSession;
	create_information_wait_notify_session_map_type m_mapCreateInfoWaitNotifySession;
	//�᰸/���������session-map
	propose_session_map_type m_mapProposeSession;
	propose_wait_notify_session_map_type m_mapProposeWaitNotifySession;
	//ֱ�Ӳ�ѯ��session-map
	query_user_map_type m_mapQueryUserSession;
	query_transaction_map_type m_mapQueryTransSession;

	DSC::CDscList <CVbhPeer> m_lstVbhPeer;
	CVbhPeerConnectSessionSummaryInfo m_vbhPeerConnSumInfo; //������peer��������Ϣ����
	ACE_UINT32 m_seed = 0;
	ACE_UINT32 m_nSessionID = 0;//��һ�������session id
	ACE_UINT32 m_nEndorsePeerCount = 0; //�ύ����ڵ�ĸ�������ǰʹ����򵥵ı������; TODO: ���ڰ汾Ҫ������
};

#include "vbh_adapter_base/vbh_adapter_base.inl"

#endif

