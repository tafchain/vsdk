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
		EN_HASH_MAP_BITS = 16, //短流程的session-map规模，包括直接查询
		EN_SESSION_TIMEOUT_VALUE = 60,
		EN_SESSION_WAIT_NOTIFY_TIMEOUT_VALUE = 300
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
	class CVbhAdapterBaseHandler : public CMcpClientHandler
	{
	public:
		CVbhAdapterBaseHandler(CMcpClientService& rService, ACE_HANDLE handle, const ACE_INT32 nHandleID,
			PROT_COMM::CDscIpAddr& rRemoteAddr, PROT_COMM::CDscIpAddr& localAddr);

	public:
		ACE_UINT32 m_nIndex = 0; //使用 CDscTypeArray 容器必须具备的接口

	public:
		CDscTypeArray<IBaseSession> m_arrSession;
	};

	using CVbhAdapterBaseHandlerPtr = CVbhAdapterBaseHandler *;

	//一个peer的peer连接session
	class CVbhPeerConnectSession
	{
	public:
		//释放hander数组
		void FreeHandlerArray(void);

		//添加1个handler，如果数组空间不够时，扩充数组空间
		void AddHandler(CVbhAdapterBaseHandler* pHandler);

		//移除一个handler，移除指定的handler后，将其后的handler向前移动
		void RemoveHandler(CVbhAdapterBaseHandler* pHandler);

		//移动拷贝，拷贝后，删除原session中的数组指针，
		void MoveCopy(CVbhPeerConnectSession& srcSession);

	public:
		CVbhAdapterBaseHandlerPtr* m_ppVbhClientServiceHandler = nullptr;
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
		CVbhPeerConnectSession* FindSession(ACE_UINT32 nPeerID);

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
		IBaseSession(CVbhAdapterBase& rClientService);

	public:
		//在OnNetError中不解除勾连关系，直接从map中移除session，并释放session
		//此时勾连关系的解除由mcp-handler的错误处理承担
		virtual void OnNetError(CVbhAdapterBaseHandler* pVbhClientServiceHandler) = 0; //网络出错时，释放本session的函数

	public:
		ACE_UINT32 m_nCltSessionID; //客户端sessionID
		ACE_UINT32 m_nSrcSessionID; //发送业务请求方的sessionID

	public:
		ACE_UINT32 m_nIndex = 0; //使用 CDscTypeArray 容器必须具备的接口

	protected:
		CVbhAdapterBase& m_rClientService;
	};

	//注册用户的session
	class CRegistUserSession : public IBaseSession
	{
	public:
		CRegistUserSession(CVbhAdapterBase& rClientService);

	public:
		virtual void OnTimer(void) override;
		virtual void OnNetError(CVbhAdapterBaseHandler* pVbhClientServiceHandler) override;

	public:
		CDscTypeArray<CVbhAdapterBaseHandler> m_arrHandler; //客户端和多个背书节点的连接

	//为字节对齐，节约空间，特意这样排变量
	public:
		bool m_bWaitNotify = false; //用户是否等待通知
		ACE_UINT32 m_nSucceedRspNum = 0; //背书节点中成功应答的数目
		ACE_UINT32 m_nEsSessionID; //保存endorser service 的 session 

		CDscString m_strCltPubKey; // 客户端公钥
		CDscString m_strSvrPriKey; //服务端私钥
		CDscString m_strEnvelopeKey; //对称秘钥

	public:
		ACE_UINT32 m_nKey = 0;//hash map用到
		CRegistUserSession* m_pPrev = NULL;//hash map用到
		CRegistUserSession* m_pNext = NULL;//hash map用到

	public:
		CVbhAdapterBaseHandler* m_pSubmitEsHandler = NULL; //被选中的endorser service 的handle-session
		CDscString m_strCcGenerateUserInfo; //CC允许注册时，为用户生成的初始信息
	};

	//注册用户等待notify的session
	class CRegistUserWaitNotitySession : public IBaseSession
	{
	public:
		CRegistUserWaitNotitySession(CVbhAdapterBase& rClientService);

	public:
		virtual void OnTimer(void) override;
		virtual void OnNetError(CVbhAdapterBaseHandler* pVbhClientServiceHandler) override;

	public:
		CVbhAdapterBaseHandler* m_pHandler; //等待应答时所用连接

	public:
		ACE_UINT32 m_nKey = 0;
		CRegistUserWaitNotitySession* m_pPrev = NULL;
		CRegistUserWaitNotitySession* m_pNext = NULL;
	};

	//注册用户的session
	class CCreateInformationSession : public IBaseSession
	{
	public:
		CCreateInformationSession(CVbhAdapterBase& rClientService);

	public:
		virtual void OnTimer(void) override;
		virtual void OnNetError(CVbhAdapterBaseHandler* pVbhClientServiceHandler) override;

	public:
		CDscTypeArray<CVbhAdapterBaseHandler> m_arrHandler; //客户端和多个背书节点的连接

	//为字节对齐，节约空间，特意这样排变量
	public:
		bool m_bWaitNotify = false; //用户是否等待通知
		ACE_UINT32 m_nSucceedRspNum = 0; //背书节点中成功应答的数目
		ACE_UINT32 m_nEsSessionID; //保存endorser service 的 session 

	public:
		ACE_UINT32 m_nKey = 0;//hash map用到
		CCreateInformationSession* m_pPrev = NULL;//hash map用到
		CCreateInformationSession* m_pNext = NULL;//hash map用到

	public:
		CVbhAdapterBaseHandler* m_pSubmitEsHandler = NULL; //被选中的endorser service 的handle-session
		CDscString m_strCcGenerateValue; //CC允许创建时，为information生成的初始值
	};

	//创建information等待notify的session
	class CCreateInformationWaitNotitySession : public IBaseSession
	{
	public:
		CCreateInformationWaitNotitySession(CVbhAdapterBase& rClientService);

	public:
		virtual void OnTimer(void) override;
		virtual void OnNetError(CVbhAdapterBaseHandler* pVbhClientServiceHandler) override;

	public:
		CVbhAdapterBaseHandler* m_pHandler; //等待应答时所用连接

	public:
		ACE_UINT32 m_nKey = 0;
		CCreateInformationWaitNotitySession* m_pPrev = NULL;
		CCreateInformationWaitNotitySession* m_pNext = NULL;
	};

	//发起提案交易的session
	class CProposeSession : public IBaseSession
	{
	public:
		CProposeSession(CVbhAdapterBase& rClientService);

	public:
		virtual void OnTimer(void) override;
		virtual void OnNetError(CVbhAdapterBaseHandler* pVbhClientServiceHandler) override;

	public:
		CDscTypeArray<CVbhAdapterBaseHandler> m_arrHandler; //客户端和多个背书节点的连接

	public:
		bool m_bWaitNotify = false; //用户是否等待通知
		ACE_UINT32 m_nSucceedRspNum = 0; //背书节点中成功应答的数目
		CVbhAdapterBaseHandler* m_pSubmitEsHandler = NULL; //被选中的endorser service 的handle-session
		ACE_UINT32 m_nEsSessionID; //保存endorser service 的 session 
		CDscString m_strTransContent; //transaction - content

	public:
		ACE_UINT32 m_nKey = 0;
		CProposeSession* m_pPrev = NULL;
		CProposeSession* m_pNext = NULL;
	};

	//发起提案等待notify的session
	class CProposeWaitNotifySession : public IBaseSession
	{
	public:
		CProposeWaitNotifySession(CVbhAdapterBase& rClientService);

	public:
		virtual void OnTimer(void) override;
		virtual void OnNetError(CVbhAdapterBaseHandler* pVbhClientServiceHandler) override;

	public:
		CVbhAdapterBaseHandler* m_pHandler; //等待应答时所用连接

	public:
		ACE_UINT32 m_nKey = 0;
		CProposeWaitNotifySession* m_pPrev = NULL;
		CProposeWaitNotifySession* m_pNext = NULL;
	};

	//查询用户信息的session
	class CQueryUserSession : public IBaseSession
	{
	public:
		CQueryUserSession(CVbhAdapterBase& rRegistUserService);

	public:
		virtual void OnTimer(void) override;
		virtual void OnNetError(CVbhAdapterBaseHandler* pVbhClientServiceHandler) override;

	public:
		CVbhAdapterBaseHandler* m_pHandler; //查询时所用连接

	public:
		ACE_UINT32 m_nKey = 0;
		CQueryUserSession* m_pPrev = NULL;
		CQueryUserSession* m_pNext = NULL;
	};

	//查询事务的session
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
		/* 注册用户 */
		BIND_HTS_MESSAGE(VBH::CRegistUserEsCltRsp)
		BIND_HTS_MESSAGE(VBH::CSubmitRegistUserTransactionEsCltRsp)
		BIND_HTS_MESSAGE(VBH::CSubscribeRegistUserTransactionEsCltNotify)
		/* 创建information */
		BIND_HTS_MESSAGE(VBH::CCreateInformationEsCltRsp)
		BIND_HTS_MESSAGE(VBH::CSubmitCreateInformationTransactionEsCltRsp)
		BIND_HTS_MESSAGE(VBH::CSubscribeCreateInformationTransactionEsCltNotify)
		/* 发起提案 */
		BIND_HTS_MESSAGE(VBH::CProposeEsCltRsp)
		BIND_HTS_MESSAGE(VBH::CSubmitProposalTransactionEsCltRsp)
		BIND_HTS_MESSAGE(VBH::CSubscribeProposeTransactionEsCltNotify)
		/* 直接查询 */
		BIND_HTS_MESSAGE(VBH::CQueryUserInfoEsCltRsp)
		BIND_HTS_MESSAGE(VBH::CQueryTransInfoEsCltRsp)
		END_HTS_MESSAGE_BIND

public:
	/* 注册用户 */
	ACE_INT32 OnHtsMsg(VBH::CRegistUserEsCltRsp& rRegistUserRsp, CMcpHandler* pMcpHandler);
	ACE_INT32 OnHtsMsg(VBH::CSubmitRegistUserTransactionEsCltRsp& rSubmitTransRsp, CMcpHandler* pMcpHandler);
	ACE_INT32 OnHtsMsg(VBH::CSubscribeRegistUserTransactionEsCltNotify& rNotify, CMcpHandler* pMcpHandler);
	/* 创建information */
	ACE_INT32 OnHtsMsg(VBH::CCreateInformationEsCltRsp& rCreateInfoRsp, CMcpHandler* pMcpHandler);
	ACE_INT32 OnHtsMsg(VBH::CSubmitCreateInformationTransactionEsCltRsp& rSubmitTransRsp, CMcpHandler* pMcpHandler);
	ACE_INT32 OnHtsMsg(VBH::CSubscribeCreateInformationTransactionEsCltNotify& rNotify, CMcpHandler* pMcpHandler);
	/* 发起提案 */
	ACE_INT32 OnHtsMsg(VBH::CProposeEsCltRsp& rSubmitProposalRsp, CMcpHandler* pMcpHandler);
	ACE_INT32 OnHtsMsg(VBH::CSubmitProposalTransactionEsCltRsp& rSubmitTransRsp, CMcpHandler* pMcpHandler);
	ACE_INT32 OnHtsMsg(VBH::CSubscribeProposeTransactionEsCltNotify& rNotify, CMcpHandler* pMcpHandler);
	/* 直接查询 */
	ACE_INT32 OnHtsMsg(VBH::CQueryUserInfoEsCltRsp& rQueryUserRsp, CMcpHandler* pMcpHandler);
	ACE_INT32 OnHtsMsg(VBH::CQueryTransInfoEsCltRsp& rQueryTransRsp, CMcpHandler* pMcpHandler);

public:
	//释放有多个连接的session， (注册用户session， 提案session)
	//pIgnoreMcpHandler参数用来指明要忽略哪个mcp-handler不取消勾连；在发生网络断连时，当前正在断连的那个mcp-handler不需要取消勾连，最后会统一处理。
	template <typename SESSION_TYPE>
	void OnReleaseMultiMcpHandler(SESSION_TYPE* pSession, CVbhAdapterBaseHandler* pIgnoreMcpHandler);

	template <typename SESSION_TYPE>
	void OnReleaseMultiMcpHandler(SESSION_TYPE* pSession);

	//删除 本session 和 非提交节点连接session的勾连关系 (注册用户session， 提案session)
	template <typename SESSION_TYPE>
	void RemoveNonSubmitMcpHandler(SESSION_TYPE* pSession, CMcpHandler* pMcpHandler);

	//释放只有1个连接的session，(查询session， wait-notify-session)
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
	//根据McpHandler的句柄ID找到对应的peerID //句柄ID和peerID是在初始化时就建立好的
	ACE_INT32 GetPeerID(const ACE_UINT32 nHandleID);
	CVbhAdapterBaseHandler* GetMcpHandler(void);
	ACE_INT32 LoadPeerInfo(void);

	//同时向多个背书节点发送消息，发送消息的同时，建立业务session和背书节点连接handler的勾连关系
	template <typename REQ_TYPE, typename SESSION_TYPE>
	ACE_INT32 SendMultiEndorseMessage(REQ_TYPE& req, SESSION_TYPE* pSession);

	//向一个节点发送消息， 建立业务session和endorser连接handler的勾连关系
	template <typename REQ_TYPE, typename SESSION_TYPE>
	ACE_INT32 SendSingleEndoreMessage(REQ_TYPE& req, SESSION_TYPE* pSession);

private:
	//注册用户的session-map
	using regist_user_session_map_type = CBareHashMap<ACE_UINT32, CRegistUserSession, EN_HASH_MAP_BITS>;
	using regist_user_wait_notify_session_map_type = CBareHashMap<ACE_UINT32, CRegistUserWaitNotitySession, EN_HASH_MAP_BITS>;
	//创建information的session-map
	using create_information_session_map_type = CBareHashMap<ACE_UINT32, CCreateInformationSession, EN_HASH_MAP_BITS>;
	using create_information_wait_notify_session_map_type = CBareHashMap<ACE_UINT32, CCreateInformationWaitNotitySession, EN_HASH_MAP_BITS>;
	//提案/发起事务的session-map
	using propose_session_map_type = CBareHashMap<ACE_UINT32, CProposeSession, EN_HASH_MAP_BITS>;
	using propose_wait_notify_session_map_type = CBareHashMap<ACE_UINT32, CProposeWaitNotifySession, EN_HASH_MAP_BITS>;
	//直接查询的session-map
	using query_user_map_type = CBareHashMap<ACE_UINT32, CQueryUserSession, EN_HASH_MAP_BITS>;
	using query_transaction_map_type = CBareHashMap<ACE_UINT32, CQueryTransactionSession, EN_HASH_MAP_BITS>;

private:
	//注册用户的session-map
	regist_user_session_map_type m_mapRegistUserSession;
	regist_user_wait_notify_session_map_type m_mapRegistUserWaitNotifySession;
	//创建information的session-map
	create_information_session_map_type m_mapCreateInfoSession;
	create_information_wait_notify_session_map_type m_mapCreateInfoWaitNotifySession;
	//提案/发起事务的session-map
	propose_session_map_type m_mapProposeSession;
	propose_wait_notify_session_map_type m_mapProposeWaitNotifySession;
	//直接查询的session-map
	query_user_map_type m_mapQueryUserSession;
	query_transaction_map_type m_mapQueryTransSession;

	DSC::CDscList <CVbhPeer> m_lstVbhPeer;
	CVbhPeerConnectSessionSummaryInfo m_vbhPeerConnSumInfo; //和所有peer的连接信息汇总
	ACE_UINT32 m_seed = 0;
	ACE_UINT32 m_nSessionID = 0;//下一个分配的session id
	ACE_UINT32 m_nEndorsePeerCount = 0; //提交背书节点的个数，当前使用最简单的背书策略; TODO: 后期版本要调整。
};

#include "vbh_adapter_base/vbh_adapter_base.inl"

#endif

