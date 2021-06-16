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
		EN_HASH_MAP_BITS = 16, //短流程的session-map规模，包括直接查询
		EN_SESSION_TIMEOUT_VALUE = 120
	};

private:
	//endorser连接地址信息
	class CEndorserAddr
	{
	public:
		CDscString m_strIpAddr;
		ACE_INT32 m_nPort = 0;
		ACE_UINT32 m_nHandleID = 0;
	};

	//peer连接地址信息
	class CVbhPeer
	{
	public:
		ACE_UINT16 m_nPeerID = 0;
		DSC::CDscList <CEndorserAddr> m_lstEndorserAddr;
	};

	//重载网络通信句柄
	class IBaseSession;
	class CVbhClientServiceHandler : public CMcpClientHandler
	{
	public:
		CVbhClientServiceHandler(CMcpClientService& rService, ACE_HANDLE handle, const ACE_INT32 nHandleID,
			PROT_COMM::CDscIpAddr& rRemoteAddr, PROT_COMM::CDscIpAddr& localAddr);

	public:
		ACE_UINT32 m_nIndex = 0; //使用 CDscTypeArray 容器必须具备的接口

	public:
		CDscTypeArray<IBaseSession> m_arrSession;
	};

	using CVbhClientServiceHandlerPtr = CVbhClientServiceHandler *;
	//一个peer的peer连接session
	class CVbhPeerConnectSession
	{
	public:
		//释放hander数组
		void FreeHandlerArray(void);

		//添加1个handler，如果数组空间不够时，扩充数组空间
		void AddHandler(CVbhClientServiceHandler* pHandler);

		//移除一个handler，移除指定的handler后，将其后的handler向前移动
		void RemoveHandler(CVbhClientServiceHandler* pHandler);

		//移动拷贝，拷贝后，删除原session中的数组指针，
		void MoveCopy(CVbhPeerConnectSession& srcSession);

	public:
		CVbhClientServiceHandlerPtr* m_ppVbhClientServiceHandler = nullptr;
		ACE_UINT16 m_nPeerID = 0;
		ACE_UINT32 m_nHandlerNum = 0; //m_ppVbhClientServiceHandler 数组中，实际元素的个数
		ACE_UINT32 m_nArrayLen = 0; //m_ppVbhClientServiceHandler 数据的实际长度，//空间不够时，每次增长8个
		ACE_UINT32 m_nIndex = 0; //需要在连接建立时为index赋一个随机初始值；以保证所有客户端能随机连接到某个peer的所有endorser.
	};

	//所有peer连接session的信息汇聚
	class CVbhPeerConnectSessionSummaryInfo
	{
	public:
		//释放session数组
		void FreeSessionArray(void);

		//查找指定peer-id 的session //找不到返回null指针
		CVbhPeerConnectSession* FindSession(const ACE_UINT16 nPeerID);

		//申请一个空的session //数组中没有空session则扩展数组，并返回一个空的session
		CVbhPeerConnectSession* AllocSession(void);

		//释放1个session //该session可以用于下一次分配
		void FreeSession(CVbhPeerConnectSession* pSession);

	public:
		CVbhPeerConnectSession* m_pArrPeerSession = nullptr;
		ACE_UINT32 m_nSessionNum = 0; //m_pArrPeerSession 数组中，实际元素的个数
		ACE_UINT32 m_nArrayLen = 0; //m_pArrPeerSession 数据的实际长度，//空间不够时，每次增长8个
		ACE_UINT32 m_nIndex = 0;
	};

private:
	//通用基础session 
	class IBaseSession : public CDscServiceTimerHandler
	{
	public:
		IBaseSession(CVbhCryptClientService& rClientService);

	public:
		//在OnNetError中不解除勾连关系，直接从map中移除session，并释放session
		//此时勾连关系的解除由mcp-handler的错误处理承担
		virtual void OnNetError(CVbhClientServiceHandler* pVbhClientServiceHandler) = 0; //网络出错时，释放本session的函数

	public:
		CDscMsg::CDscMsgAddr m_srcMsgAddr;
		ACE_UINT32 m_nSrcSessionID;
		ACE_UINT32 m_nCltSessionID; //客户端sessionID
		ACE_INT32 m_nNonce;  //通信随机数

	public:
		ACE_UINT32 m_nIndex = CDscTypeArray<IBaseSession>::EN_INVALID_INDEX_ID; //使用 CDscTypeArray 容器必须具备的接口
		CVbhClientServiceHandler* m_pHandler = nullptr; //endorser service 的handle-session //对于需要多节点背书的session，表示选中的endorser节点连接Handler

	protected:
		CVbhCryptClientService& m_rClientService;
	};

	//注册用户的session
	class CRegistUserSession : public IBaseSession
	{
	public:
		CRegistUserSession(CVbhCryptClientService& rClientService);

	public:
		virtual void OnTimer(void) override;
		//TODO: 所有的OnNetError不需要参数
		virtual void OnNetError(CVbhClientServiceHandler* pVbhClientServiceHandler) override;

	public:
		ACE_UINT32 m_nSucceedRspNum = 0; //背书节点中成功应答的数目
		ACE_UINT32 m_nEsSessionID = 0; //保存endorser service 的 session //0是VBH系统session的保留值

		CDscString m_strCltPubKey; // 客户端公钥

	public:
		ACE_UINT32 m_nKey = 0;//hash map用到
		CRegistUserSession* m_pPrev = NULL;//hash map用到
		CRegistUserSession* m_pNext = NULL;//hash map用到

	public:
		CDscString m_strCcGenerateUserInfo; //CC允许注册时，为用户生成的初始信息
	};


	//发起提案交易的session
	class CProposeSession : public IBaseSession
	{
	public:
		CProposeSession(CVbhCryptClientService& rClientService);

	public:
		virtual void OnTimer(void) override;
		virtual void OnNetError(CVbhClientServiceHandler* pVbhClientServiceHandler) override;

	public:
		ACE_UINT32 m_nSucceedRspNum = 0; //背书节点中成功应答的数目
		ACE_UINT32 m_nEsSessionID; //保存endorser service 的 session 

		CDscString m_strEnvelopeKey; //对称秘钥

	public:
		ACE_UINT32 m_nKey = 0;
		CProposeSession* m_pPrev = NULL;
		CProposeSession* m_pNext = NULL;

	public:
		CDscString m_strTransContent; //transaction - content
	};

	//查询信息的session
	class CQuerySession : public IBaseSession
	{
	public:
		CQuerySession(CVbhCryptClientService& rClientService);

	public:
		virtual void OnTimer(void) override;
		virtual void OnNetError(CVbhClientServiceHandler* pVbhClientServiceHandler) override;

	public:
		ACE_UINT32 m_nActionID;
		CDscString m_strEnvelopeKey; //对称秘钥

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
	/* 注册用户 */
	BIND_HTS_MESSAGE(VBH::CCryptRegistUserEsCltRsp)
	BIND_HTS_MESSAGE(VBH::CCryptSubmitRegistUserTransactionEsCltRsp)
	/* 发起提案 */
	BIND_HTS_MESSAGE(VBH::CCryptProposeEsCltRsp)
	BIND_HTS_MESSAGE(VBH::CCryptSubmitProposalTransactionEsCltRsp)
	/* 查询 */
	BIND_HTS_MESSAGE(VBH::CCryptQueryEsCltRsp)


	END_HTS_MESSAGE_BIND

public:
	/* 注册用户 */
	ACE_INT32 OnHtsMsg(VBH::CCryptRegistUserEsCltRsp& rRegistUserRsp, CMcpHandler* pMcpHandler);
	ACE_INT32 OnHtsMsg(VBH::CCryptSubmitRegistUserTransactionEsCltRsp& rSubmitTransRsp, CMcpHandler* pMcpHandler);
	/* 发起提案 */
	ACE_INT32 OnHtsMsg(VBH::CCryptProposeEsCltRsp& rProposeRsp, CMcpHandler* pMcpHandler);
	ACE_INT32 OnHtsMsg(VBH::CCryptSubmitProposalTransactionEsCltRsp& rSubmitTransRsp, CMcpHandler* pMcpHandler);
	/* 直接查询 */
	ACE_INT32 OnHtsMsg(VBH::CCryptQueryEsCltRsp& rQueryUserRsp, CMcpHandler* pMcpHandler);


protected:
	BIND_DSC_MESSAGE(
		/* 注册用户 */
		VBH::CRegistUserCltCltReq,
		/* 发起提案 */
		VBH::CProposeCltCltReq,
		/* 查询 */
		VBH::CQueryCltCltReq




	)

public:
	/* 注册用户 */
	void OnDscMsg(VBH::CRegistUserCltCltReq& rRegistUserReq, const CDscMsg::CDscMsgAddr& rSrcMsgAddr);
	/* 发起提案 */
	void OnDscMsg(VBH::CProposeCltCltReq& rProposeReq, const CDscMsg::CDscMsgAddr& rSrcMsgAddr);
	/* 查询 */
	void OnDscMsg(VBH::CQueryCltCltReq& rQueryReq, const CDscMsg::CDscMsgAddr& rSrcMsgAddr);

public:
	//释放只有1个连接的session，(查询session， wait-notify-session)
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
//根据McpHandler的句柄ID找到对应的peerID //句柄ID和peerID是在初始化时就建立好的
	ACE_INT32 GetPeerID(const ACE_UINT32 nHandleID);
	CVbhClientServiceHandler* GetMcpHandler(void);
	CVbhClientServiceHandler* GetMcpHandler(const ACE_UINT16 nPeerID);
	ACE_INT32 LoadPeerInfo(void);

	//同时向多个背书节点发送消息，消息全部成功发送时，返回和提交节点的连接mcp-hanlder
	//其中，req是发送给非选中节点的请求，req4SubmitNode 是发送给选中节点的请求
	template <typename REQ_TYPE>
	CVbhClientServiceHandler* SendMultiEndorseMessage(REQ_TYPE& req, REQ_TYPE& req4SubmitNode);

	ACE_UINT32 AllocSessionID(void);

private:
	//注册用户的session-map
	using regist_user_session_map_type = CBareHashMap<ACE_UINT32, CRegistUserSession, EN_HASH_MAP_BITS>;
	//提案/发起事务的session-map
	using propose_session_map_type = CBareHashMap<ACE_UINT32, CProposeSession, EN_HASH_MAP_BITS>;
	//直接查询的session-map
	using query_map_type = CBareHashMap<ACE_UINT32, CQuerySession, EN_HASH_MAP_BITS>;


private:
	//注册用户的session-map
	regist_user_session_map_type m_mapRegistUserSession;
	//提案/发起事务的session-map
	propose_session_map_type m_mapProposeSession;
	//查询的session-map
	query_map_type m_mapQuerySession;

	DSC::CDscList <CVbhPeer> m_lstVbhPeer;

	CVbhPeerConnectSessionSummaryInfo m_vbhPeerConnSumInfo; //和所有peer的连接信息汇总
	ACE_UINT32 m_nSeed;
	ACE_UINT32 m_nSessionID = 0;//下一个分配的session id
	CDscString m_peerPublicKey; //peer端的public key //TODO: 将来交换对称秘钥时可能有用
	CDscString m_peerEnvelopeKey; //SDK 和 endorser通信时使用的对称秘钥；TODO: 这个为临时增加的变量，待将来sdk和endorser的秘钥交换通道打通后就会扔掉
	ACE_UINT32 m_nEndorsePeerCount = 0; //提交背书节点的个数，当前使用最简单的背书策略; TODO: 后期版本要调整。

	EC_GROUP* m_pGroup = NULL; //创建秘钥所用的group
};

#include "vbh_crypt_client_service/vbh_crypt_client_service.inl"

#endif
