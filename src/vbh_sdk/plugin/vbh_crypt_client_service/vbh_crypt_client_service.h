#ifndef VBH_CRYPT_CLIENT_SERVICE_H_432988766832867432
#define VBH_CRYPT_CLIENT_SERVICE_H_432988766832867432

#include "openssl/ec.h"

#include "dsc/protocol/hts/dsc_hts_service.h"
#include "dsc/service_timer/dsc_service_timer_handler.h"
#include "dsc/container/bare_hash_map.h"
#include "dsc/protocol/mcp/mcp_client_handler.h"

#include "vbh_comm/comm_msg_def/vbh_comm_msg_es_def.h"
#include "vbh_comm/comm_msg_def/vbh_comm_msg_clt_clt_def.h"


#include "client_sdk/client_api.h"

class PLUGIN_EXPORT CVbhCryptClientService : public CDscHtsClientService
{
private:
	enum
	{
		EN_HASH_MAP_BITS = 16, //�����̵�session-map��ģ������ֱ�Ӳ�ѯ
		EN_SESSION_TIMEOUT_VALUE = 120
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
	class CVbhClientServiceHandler : public CMcpClientHandler
	{
	public:
		CVbhClientServiceHandler(CMcpClientService& rService, ACE_HANDLE handle, const ACE_INT32 nHandleID,
			PROT_COMM::CDscIpAddr& rRemoteAddr, PROT_COMM::CDscIpAddr& localAddr);

	public:
		ACE_UINT32 m_nIndex = 0; //ʹ�� CDscTypeArray ��������߱��Ľӿ�

	public:
		CDscTypeArray<IBaseSession> m_arrSession;
	};

	using CVbhClientServiceHandlerPtr = CVbhClientServiceHandler *;
	//һ��peer��peer����session
	class CVbhPeerConnectSession
	{
	public:
		//�ͷ�hander����
		void FreeHandlerArray(void);

		//���1��handler���������ռ䲻��ʱ����������ռ�
		void AddHandler(CVbhClientServiceHandler* pHandler);

		//�Ƴ�һ��handler���Ƴ�ָ����handler�󣬽�����handler��ǰ�ƶ�
		void RemoveHandler(CVbhClientServiceHandler* pHandler);

		//�ƶ�������������ɾ��ԭsession�е�����ָ�룬
		void MoveCopy(CVbhPeerConnectSession& srcSession);

	public:
		CVbhClientServiceHandlerPtr* m_ppVbhClientServiceHandler = nullptr;
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
		CVbhPeerConnectSession* FindSession(const ACE_UINT16 nPeerID);

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
		IBaseSession(CVbhCryptClientService& rClientService);

	public:
		//��OnNetError�в����������ϵ��ֱ�Ӵ�map���Ƴ�session�����ͷ�session
		//��ʱ������ϵ�Ľ����mcp-handler�Ĵ�����е�
		virtual void OnNetError(CVbhClientServiceHandler* pVbhClientServiceHandler) = 0; //�������ʱ���ͷű�session�ĺ���

	public:
		CDscMsg::CDscMsgAddr m_srcMsgAddr;
		ACE_UINT32 m_nSrcSessionID;
		ACE_UINT32 m_nCltSessionID; //�ͻ���sessionID
		ACE_INT32 m_nNonce;  //ͨ�������

	public:
		ACE_UINT32 m_nIndex = CDscTypeArray<IBaseSession>::EN_INVALID_INDEX_ID; //ʹ�� CDscTypeArray ��������߱��Ľӿ�
		CVbhClientServiceHandler* m_pHandler = nullptr; //endorser service ��handle-session //������Ҫ��ڵ㱳���session����ʾѡ�е�endorser�ڵ�����Handler

	protected:
		CVbhCryptClientService& m_rClientService;
	};

	//ע���û���session
	class CRegistUserSession : public IBaseSession
	{
	public:
		CRegistUserSession(CVbhCryptClientService& rClientService);

	public:
		virtual void OnTimer(void) override;
		//TODO: ���е�OnNetError����Ҫ����
		virtual void OnNetError(CVbhClientServiceHandler* pVbhClientServiceHandler) override;

	public:
		ACE_UINT32 m_nSucceedRspNum = 0; //����ڵ��гɹ�Ӧ�����Ŀ
		ACE_UINT32 m_nEsSessionID = 0; //����endorser service �� session //0��VBHϵͳsession�ı���ֵ

		CDscString m_strCltPubKey; // �ͻ��˹�Կ

	public:
		ACE_UINT32 m_nKey = 0;//hash map�õ�
		CRegistUserSession* m_pPrev = NULL;//hash map�õ�
		CRegistUserSession* m_pNext = NULL;//hash map�õ�

	public:
		CDscString m_strCcGenerateUserInfo; //CC����ע��ʱ��Ϊ�û����ɵĳ�ʼ��Ϣ
	};


	//�����᰸���׵�session
	class CProposeSession : public IBaseSession
	{
	public:
		CProposeSession(CVbhCryptClientService& rClientService);

	public:
		virtual void OnTimer(void) override;
		virtual void OnNetError(CVbhClientServiceHandler* pVbhClientServiceHandler) override;

	public:
		ACE_UINT32 m_nSucceedRspNum = 0; //����ڵ��гɹ�Ӧ�����Ŀ
		ACE_UINT32 m_nEsSessionID; //����endorser service �� session 

		CDscString m_strEnvelopeKey; //�Գ���Կ

	public:
		ACE_UINT32 m_nKey = 0;
		CProposeSession* m_pPrev = NULL;
		CProposeSession* m_pNext = NULL;

	public:
		CDscString m_strTransContent; //transaction - content
	};

	//��ѯ��Ϣ��session
	class CQuerySession : public IBaseSession
	{
	public:
		CQuerySession(CVbhCryptClientService& rClientService);

	public:
		virtual void OnTimer(void) override;
		virtual void OnNetError(CVbhClientServiceHandler* pVbhClientServiceHandler) override;

	public:
		ACE_UINT32 m_nActionID;
		CDscString m_strEnvelopeKey; //�Գ���Կ

	public:
		ACE_UINT32 m_nKey = 0;
		CQuerySession* m_pPrev = NULL;
		CQuerySession* m_pNext = NULL;
	};



public:
	enum
	{
		EN_SERVICE_TYPE = VBH::EN_VBH_CRYPT_CLIENT_SERVICE_TYPE
	};

public:
	ACE_INT32 OnInit(void);
	ACE_INT32 OnExit(void);

protected:
	BEGIN_HTS_MESSAGE_BIND
	/* ע���û� */
	BIND_HTS_MESSAGE(VBH::CCryptRegistUserEsCltRsp)
	BIND_HTS_MESSAGE(VBH::CCryptSubmitRegistUserTransactionEsCltRsp)
	/* �����᰸ */
	BIND_HTS_MESSAGE(VBH::CCryptProposeEsCltRsp)
	BIND_HTS_MESSAGE(VBH::CCryptSubmitProposalTransactionEsCltRsp)
	/* ��ѯ */
	BIND_HTS_MESSAGE(VBH::CCryptQueryEsCltRsp)


	END_HTS_MESSAGE_BIND

public:
	/* ע���û� */
	ACE_INT32 OnHtsMsg(VBH::CCryptRegistUserEsCltRsp& rRegistUserRsp, CMcpHandler* pMcpHandler);
	ACE_INT32 OnHtsMsg(VBH::CCryptSubmitRegistUserTransactionEsCltRsp& rSubmitTransRsp, CMcpHandler* pMcpHandler);
	/* �����᰸ */
	ACE_INT32 OnHtsMsg(VBH::CCryptProposeEsCltRsp& rProposeRsp, CMcpHandler* pMcpHandler);
	ACE_INT32 OnHtsMsg(VBH::CCryptSubmitProposalTransactionEsCltRsp& rSubmitTransRsp, CMcpHandler* pMcpHandler);
	/* ֱ�Ӳ�ѯ */
	ACE_INT32 OnHtsMsg(VBH::CCryptQueryEsCltRsp& rQueryUserRsp, CMcpHandler* pMcpHandler);


protected:
	BIND_DSC_MESSAGE(
		/* ע���û� */
		VBH::CRegistUserCltCltReq,
		/* �����᰸ */
		VBH::CProposeCltCltReq,
		/* ��ѯ */
		VBH::CQueryCltCltReq




	)

public:
	/* ע���û� */
	void OnDscMsg(VBH::CRegistUserCltCltReq& rRegistUserReq, const CDscMsg::CDscMsgAddr& rSrcMsgAddr);
	/* �����᰸ */
	void OnDscMsg(VBH::CProposeCltCltReq& rProposeReq, const CDscMsg::CDscMsgAddr& rSrcMsgAddr);
	/* ��ѯ */
	void OnDscMsg(VBH::CQueryCltCltReq& rQueryReq, const CDscMsg::CDscMsgAddr& rSrcMsgAddr);

public:
	//�ͷ�ֻ��1�����ӵ�session��(��ѯsession�� wait-notify-session)
	template <typename SESSION_TYPE>
	void OnReleaseSingleMcpHanler(SESSION_TYPE* pSession);

	void OnResponse(CRegistUserSession* pRegistUserSession, const ACE_INT32 nReturnCode,
		const DSC::CDscShortBlob& vbhCryptKey = DSC::CDscShortBlob(nullptr, 0), 
		const DSC::CDscShortBlob& vbhUserID = DSC::CDscShortBlob(nullptr, 0));
	void OnTimeOut(CRegistUserSession* pRegistUserSession);
	void OnNetError(CRegistUserSession* pRegistUserSession, CVbhClientServiceHandler* pVbhClientServiceHandler);

	void OnResponse(CProposeSession* pProposeSession, const ACE_INT32 nRetrunCode,ACE_UINT32 nActionID, 
		const DSC::CDscShortBlob& infokey = DSC::CDscShortBlob(nullptr, 0), 
		const DSC::CDscShortBlob& receipt = DSC::CDscShortBlob(nullptr, 0),
		const DSC::CDscShortBlob& transKey = DSC::CDscShortBlob(nullptr, 0));

	void OnTimeOut(CProposeSession* pProposeSession);
	void OnNetError(CProposeSession* pProposeSession, CVbhClientServiceHandler* pVbhClientServiceHandler);

	void OnResponse(CQuerySession* pQueryUserSession, const ACE_INT32 nRetrunCode, 
		const ACE_UINT32 nActionID, const DSC::CDscBlob& info = DSC::CDscBlob(nullptr, 0));
	void OnTimeOut(CQuerySession* pQueryUserSession);
	void OnNetError(CQuerySession* pQueryUserSession);



protected:
	virtual ACE_INT32 OnConnectedNodify(CMcpClientHandler* pMcpClientHandler) override;
	virtual void OnNetworkError(CMcpHandler* pMcpHandler) override;
	virtual CMcpClientHandler* AllocMcpHandler(ACE_HANDLE handle, const ACE_UINT32 nHandleID,
		PROT_COMM::CDscIpAddr& rRemoteAddr, PROT_COMM::CDscIpAddr& localAddr) override;

private:
//����McpHandler�ľ��ID�ҵ���Ӧ��peerID //���ID��peerID���ڳ�ʼ��ʱ�ͽ����õ�
	ACE_INT32 GetPeerID(const ACE_UINT32 nHandleID);
	CVbhClientServiceHandler* GetMcpHandler(void);
	CVbhClientServiceHandler* GetMcpHandler(const ACE_UINT16 nPeerID);
	ACE_INT32 LoadPeerInfo(void);

	//ͬʱ��������ڵ㷢����Ϣ����Ϣȫ���ɹ�����ʱ�����غ��ύ�ڵ������mcp-hanlder
	//���У�req�Ƿ��͸���ѡ�нڵ������req4SubmitNode �Ƿ��͸�ѡ�нڵ������
	template <typename REQ_TYPE>
	CVbhClientServiceHandler* SendMultiEndorseMessage(REQ_TYPE& req, REQ_TYPE& req4SubmitNode);

	ACE_UINT32 AllocSessionID(void);

private:
	//ע���û���session-map
	using regist_user_session_map_type = CBareHashMap<ACE_UINT32, CRegistUserSession, EN_HASH_MAP_BITS>;
	//�᰸/���������session-map
	using propose_session_map_type = CBareHashMap<ACE_UINT32, CProposeSession, EN_HASH_MAP_BITS>;
	//ֱ�Ӳ�ѯ��session-map
	using query_map_type = CBareHashMap<ACE_UINT32, CQuerySession, EN_HASH_MAP_BITS>;


private:
	//ע���û���session-map
	regist_user_session_map_type m_mapRegistUserSession;
	//�᰸/���������session-map
	propose_session_map_type m_mapProposeSession;
	//��ѯ��session-map
	query_map_type m_mapQuerySession;

	DSC::CDscList <CVbhPeer> m_lstVbhPeer;

	CVbhPeerConnectSessionSummaryInfo m_vbhPeerConnSumInfo; //������peer��������Ϣ����
	ACE_UINT32 m_nSeed;
	ACE_UINT32 m_nSessionID = 0;//��һ�������session id
	CDscString m_peerPublicKey; //peer�˵�public key //TODO: ���������Գ���Կʱ��������
	CDscString m_peerEnvelopeKey; //SDK �� endorserͨ��ʱʹ�õĶԳ���Կ��TODO: ���Ϊ��ʱ���ӵı�����������sdk��endorser����Կ����ͨ����ͨ��ͻ��ӵ�
	ACE_UINT32 m_nEndorsePeerCount = 0; //�ύ����ڵ�ĸ�������ǰʹ����򵥵ı������; TODO: ���ڰ汾Ҫ������

	EC_GROUP* m_pGroup = NULL; //������Կ���õ�group
};

#include "vbh_crypt_client_service/vbh_crypt_client_service.inl"

#endif
