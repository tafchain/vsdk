#include "sdk/vbh_adapter_base/vbh_adapter_base.h"

#include "dsc/mem_mng/dsc_allocator.h"
#include "dsc/configure/dsc_configure.h"

#include "common/vbh_comm_error_code.h"
#include "common/vbh_comm_macro_def.h"
#include "common/vbh_comm_msg_def.h"
#include "common/vbh_comm_wrap_msg_def.h"
#include "common/vbh_comm_func.h"
#include "common/vbh_encrypt_lib.h"
#include "common/crypt_key_codec.h"
#include "vbh/common/vbh_user_key_hexcodec.h"


void CVbhAdapterBase::OnRegistUserResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, const DSC::CDscShortBlob& userKey)
{
}

void CVbhAdapterBase::OnRegistUserResultNotify(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID)
{
}

void CVbhAdapterBase::OnCreateInformationRespondse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, const DSC::CDscShortBlob& infoKey)
{
}

void CVbhAdapterBase::OnCreateInformationResultNotify(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID)
{
}

void CVbhAdapterBase::OnProposeResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, const DSC::CDscShortBlob& tranKey)
{
}

void CVbhAdapterBase::OnProposeResultNotify(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID)
{
}

void CVbhAdapterBase::OnQueryUserInfoResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, const DSC::CDscShortBlob& userInfo)
{
}

void CVbhAdapterBase::OnQueryTransactionInfoResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, const DSC::CDscBlob& tranInfo)
{
}
CVbhAdapterBase::CVbhAdapterBaseHandler::CVbhAdapterBaseHandler(CMcpClientService& rService, ACE_HANDLE handle, const ACE_INT32 nHandleID,
	PROT_COMM::CDscIpAddr& rRemoteAddr, PROT_COMM::CDscIpAddr& localAddr)
	: CMcpClientHandler(rService, handle, nHandleID, rRemoteAddr, localAddr)
{
}

void CVbhAdapterBase::CVbhPeerConnectSession::FreeHandlerArray(void)
{
	if (m_ppVbhClientServiceHandler)
	{
		ACE_ASSERT(m_nArrayLen);
		DSC_THREAD_TYPE_DEALLOCATE_ARRAY(m_ppVbhClientServiceHandler, m_nArrayLen);

		m_ppVbhClientServiceHandler = nullptr;
		m_nArrayLen = 0;
		m_nHandlerNum = 0;
		m_nIndex = 0;
	}
}

void CVbhAdapterBase::CVbhPeerConnectSession::AddHandler(CVbhAdapterBaseHandler* pHandler)
{
	if (m_nHandlerNum == m_nArrayLen)//空间不够了，要重新开辟空间
	{
		//先开辟需要的空间
		ACE_UINT32 nArrayLen = m_nArrayLen + 8; //每次增长8个
		CVbhAdapterBaseHandlerPtr* ppMcpHandler;

		DSC_THREAD_TYPE_ALLOCATE_ARRAY(ppMcpHandler, nArrayLen); //开辟新数组
		if (m_ppVbhClientServiceHandler) // 如果原来有，则移动过来
		{
			ACE_ASSERT(m_nArrayLen);
			ACE_OS::memcpy(ppMcpHandler, m_ppVbhClientServiceHandler, sizeof(CVbhAdapterBaseHandlerPtr) * m_nHandlerNum);
			DSC_THREAD_TYPE_DEALLOCATE_ARRAY(m_ppVbhClientServiceHandler, m_nArrayLen);
		}

		m_ppVbhClientServiceHandler = ppMcpHandler;
		m_nArrayLen = nArrayLen;
	}

	//记录handler
	m_ppVbhClientServiceHandler[m_nHandlerNum] = pHandler;
	++m_nHandlerNum;

	//当连接数变动时，重新计算一遍随机值，保证session平均的发送到每个endorser
	ACE_UINT32 nSeed = ACE_OS::time(0);

	m_nIndex = abs(ACE_OS::rand_r(&nSeed)) % m_nHandlerNum;
}

void CVbhAdapterBase::CVbhPeerConnectSession::RemoveHandler(CVbhAdapterBaseHandler* pHandler)
{
	ACE_UINT32 nPos = 0; //用于找到指定handler的位置

	for (; nPos < m_nHandlerNum; ++nPos)
	{
		if (m_ppVbhClientServiceHandler[nPos] == pHandler)
		{
			break;//找到了，位置记录于nPos中
		}
	}

	if (nPos < m_nHandlerNum)//找到了，否则是没有找到
	{
		//将最后1个移动到这个空位置
		m_ppVbhClientServiceHandler[nPos] = m_ppVbhClientServiceHandler[m_nHandlerNum - 1];
		--m_nHandlerNum;

		//重新计算index值，保证session的平均发送
		if (m_nHandlerNum)
		{
			ACE_UINT32 nSeed = ACE_OS::time(0);

			m_nIndex = abs(ACE_OS::rand_r(&nSeed)) % m_nHandlerNum;
		}
	}
}

void CVbhAdapterBase::CVbhPeerConnectSession::MoveCopy(CVbhPeerConnectSession& srcSession)
{
	this->FreeHandlerArray(); //先释放自身

	this->m_ppVbhClientServiceHandler = srcSession.m_ppVbhClientServiceHandler;
	srcSession.m_ppVbhClientServiceHandler = nullptr;

	this->m_nArrayLen = srcSession.m_nArrayLen;
	srcSession.m_nArrayLen = 0;

	this->m_nHandlerNum = srcSession.m_nHandlerNum;
	srcSession.m_nHandlerNum = 0;

	this->m_nIndex = srcSession.m_nIndex;
	srcSession.m_nIndex = 0;

	this->m_nPeerID = srcSession.m_nPeerID;
	srcSession.m_nPeerID = 0;
}

void CVbhAdapterBase::CVbhPeerConnectSessionSummaryInfo::FreeSessionArray(void)
{
	if (m_pArrPeerSession)
	{
		ACE_ASSERT(m_nArrayLen);

		for (ACE_UINT32 idx = 0; idx < m_nSessionNum; ++idx)
		{
			m_pArrPeerSession[idx].FreeHandlerArray();
		}
		DSC_THREAD_TYPE_DEALLOCATE_ARRAY(m_pArrPeerSession, m_nArrayLen);

		m_pArrPeerSession = nullptr;
		m_nArrayLen = 0;
		m_nSessionNum = 0;
		m_nIndex = 0;
	}
}

CVbhAdapterBase::CVbhPeerConnectSession* CVbhAdapterBase::CVbhPeerConnectSessionSummaryInfo::FindSession(ACE_UINT32 nPeerID)
{
	for (ACE_UINT32 i = 0; i < m_nSessionNum; ++i)
	{
		if (m_pArrPeerSession[i].m_nPeerID == nPeerID)
		{
			return m_pArrPeerSession + i;
		}
	}

	return nullptr;
}

CVbhAdapterBase::CVbhPeerConnectSession* CVbhAdapterBase::CVbhPeerConnectSessionSummaryInfo::AllocSession(void)
{
	if (m_nSessionNum == m_nArrayLen) //数组已满，需要开辟新空间
	{
		ACE_UINT32 nArrayLen = m_nArrayLen + 8; //一次增加8个
		CVbhPeerConnectSession* pArrPeerSession;

		DSC_THREAD_TYPE_ALLOCATE_ARRAY(pArrPeerSession, nArrayLen);

		if (m_pArrPeerSession) //如果原来有session，将其移动到新session数组中
		{
			for (ACE_UINT32 idx = 0; idx < m_nSessionNum; ++idx)
			{
				pArrPeerSession[idx].MoveCopy(m_pArrPeerSession[idx]); //移动拷贝
			}

			ACE_ASSERT(m_nArrayLen);
			DSC_THREAD_TYPE_DEALLOCATE_ARRAY(m_pArrPeerSession, m_nArrayLen);
		}

		m_pArrPeerSession = pArrPeerSession;
		m_nArrayLen = nArrayLen;
	}

	CVbhPeerConnectSession* pSession = m_pArrPeerSession + m_nSessionNum;

	++m_nSessionNum;

	//连接的peer数变动时，重新计算一遍随机值，保证每个peer被选为背书peer的机会均等
	ACE_UINT32 nSeed = ACE_OS::time(0);

	m_nIndex = abs(ACE_OS::rand_r(&nSeed)) % m_nSessionNum;

	return pSession;
}

void CVbhAdapterBase::CVbhPeerConnectSessionSummaryInfo::FreeSession(CVbhPeerConnectSession* pSession)
{
	ACE_UINT32 nPos = 0; //用于找到指定session的位置

	for (; nPos < m_nSessionNum; ++nPos)
	{
		if (m_pArrPeerSession[nPos].m_nPeerID == pSession->m_nPeerID)
		{
			break;//找到了，位置记录于nPos中
		}
	}

	if (nPos < m_nSessionNum)//找到了，否则是没有找到
	{
		//将最后1个session移动到这个空位置 同时清空最后1个session的位置
		m_pArrPeerSession[nPos].MoveCopy(m_pArrPeerSession[m_nSessionNum - 1]);
		--m_nSessionNum;

		//重新计算index值，保证session的平均发送
		if (m_nSessionNum)
		{
			ACE_UINT32 nSeed = ACE_OS::time(0);

			m_nIndex = abs(ACE_OS::rand_r(&nSeed)) % m_nSessionNum;
		}
	}
}

CVbhAdapterBase::IBaseSession::IBaseSession(CVbhAdapterBase& rClientService)
	: m_rClientService(rClientService)
{
}

CVbhAdapterBase::CRegistUserSession::CRegistUserSession(CVbhAdapterBase& rClientService)
	: IBaseSession(rClientService)
{
}

CVbhAdapterBase::CRegistUserWaitNotitySession::CRegistUserWaitNotitySession(CVbhAdapterBase& rClientService)
	: IBaseSession(rClientService)
{
}

CVbhAdapterBase::CCreateInformationSession::CCreateInformationSession(CVbhAdapterBase& rClientService)
	: IBaseSession(rClientService)
{
}

CVbhAdapterBase::CCreateInformationWaitNotitySession::CCreateInformationWaitNotitySession(CVbhAdapterBase& rClientService)
	: IBaseSession(rClientService)
{
}

CVbhAdapterBase::CProposeSession::CProposeSession(CVbhAdapterBase& rClientService)
	: IBaseSession(rClientService)
{
}

CVbhAdapterBase::CProposeWaitNotifySession::CProposeWaitNotifySession(CVbhAdapterBase& rClientService)
	: IBaseSession(rClientService)
{
}

CVbhAdapterBase::CQueryUserSession::CQueryUserSession(CVbhAdapterBase& rClientService)
	: IBaseSession(rClientService)
{
}

CVbhAdapterBase::CQueryTransactionSession::CQueryTransactionSession(CVbhAdapterBase& rClientService)
	: IBaseSession(rClientService)
{
}

ACE_INT32 CVbhAdapterBase::OnInit(void)
{
	if (CDscHtsClientService::OnInit())
	{
		DSC_RUN_LOG_ERROR("bc user client service init failed!");

		return -1;
	}

	if (this->LoadPeerInfo())
	{
		DSC_RUN_LOG_WARNING("load peer info failed");

		return -1;
	}

	//读取背书节点个数// TODO:将来会有一个详细的背书策略数据库需要读取
	ACE_INT32 nEndorsePeerCount;
	if (VBH::GetVbhProfileInt("ENDORSE_PEER_COUNT", nEndorsePeerCount))
	{
		DSC_RUN_LOG_WARNING("cann't read 'ENDORSE_PEER_COUNT' configure item value");

		return -1;
	}

	if (nEndorsePeerCount <= 0)
	{
		DSC_RUN_LOG_WARNING("'ENDORSE_PEER_COUNT' [%d] cann't be zero or negative value", nEndorsePeerCount);

		return -1;
	}
	m_nEndorsePeerCount = (ACE_UINT32)nEndorsePeerCount;

	if (m_nEndorsePeerCount > m_lstVbhPeer.size())
	{
		DSC_RUN_LOG_WARNING("'ENDORSE_PEER_COUNT' [%d] cann't >  peer num:%d", m_nEndorsePeerCount, m_lstVbhPeer.size());

		return -1;
	}

	for (auto peer_it = m_lstVbhPeer.begin(); peer_it != m_lstVbhPeer.end(); ++peer_it)
	{
		CVbhPeer& rPeer = *peer_it;

		for (auto svr_it = rPeer.m_lstEndorserAddr.begin(); svr_it != rPeer.m_lstEndorserAddr.end(); ++svr_it)
		{
			CEndorserAddr& bcServer = *svr_it;
			PROT_COMM::CDscIpAddr remoteAddr(bcServer.m_strIpAddr, bcServer.m_nPort);

			this->DoConnect(remoteAddr, NULL, bcServer.m_nHandleID);
		}
	}

	ACE_OS::srand(ACE_OS::time(0));

	DSC_RUN_LOG_INFO("vbh client service %d init succeed", this->GetID());

	return 0;
}

ACE_INT32 CVbhAdapterBase::OnExit(void)
{
	CRegistUserSession* pRegistUserSession;
	for (auto it = m_mapRegistUserSession.begin(); it != m_mapRegistUserSession.end();)
	{
		pRegistUserSession = it.second;
		this->CancelDscTimer(pRegistUserSession);
		++it;
		DSC_THREAD_TYPE_DEALLOCATE(pRegistUserSession);
	}

	CRegistUserWaitNotitySession* pRegistUserWaitNotitySession;
	for (auto it = m_mapRegistUserWaitNotifySession.begin(); it != m_mapRegistUserWaitNotifySession.end();)
	{
		pRegistUserWaitNotitySession = it.second;
		this->CancelDscTimer(pRegistUserWaitNotitySession);
		++it;
		DSC_THREAD_TYPE_DEALLOCATE(pRegistUserWaitNotitySession);
	}

	CCreateInformationSession* pCreateInformationSession;
	for (auto it = m_mapCreateInfoSession.begin(); it != m_mapCreateInfoSession.end();)
	{
		pCreateInformationSession = it.second;
		this->CancelDscTimer(pCreateInformationSession);
		++it;
		DSC_THREAD_TYPE_DEALLOCATE(pCreateInformationSession);
	}

	CCreateInformationWaitNotitySession* pCreateInformationWaitNotitySession;
	for (auto it = m_mapCreateInfoWaitNotifySession.begin(); it != m_mapCreateInfoWaitNotifySession.end();)
	{
		pCreateInformationWaitNotitySession = it.second;
		this->CancelDscTimer(pCreateInformationWaitNotitySession);
		++it;
		DSC_THREAD_TYPE_DEALLOCATE(pCreateInformationWaitNotitySession);
	}

	CProposeSession* pProposeSession;
	for (auto it = m_mapProposeSession.begin(); it != m_mapProposeSession.end();)
	{
		pProposeSession = it.second;
		this->CancelDscTimer(pProposeSession);
		++it;
		DSC_THREAD_TYPE_DEALLOCATE(pProposeSession);
	}

	CProposeWaitNotifySession* pProposeWaitNotifySession;
	for (auto it = m_mapProposeWaitNotifySession.begin(); it != m_mapProposeWaitNotifySession.end();)
	{
		pProposeWaitNotifySession = it.second;
		this->CancelDscTimer(pProposeWaitNotifySession);
		++it;
		DSC_THREAD_TYPE_DEALLOCATE(pProposeWaitNotifySession);
	}

	CQueryUserSession* pQueryUserSession;
	for (auto it = m_mapQueryUserSession.begin(); it != m_mapQueryUserSession.end();)
	{
		pQueryUserSession = it.second;
		this->CancelDscTimer(pQueryUserSession);
		++it;
		DSC_THREAD_TYPE_DEALLOCATE(pQueryUserSession);
	}

	CQueryTransactionSession* pQueryTransactionSession;
	for (auto it = m_mapQueryTransSession.begin(); it != m_mapQueryTransSession.end();)
	{
		pQueryTransactionSession = it.second;
		this->CancelDscTimer(pQueryTransactionSession);
		++it;
		DSC_THREAD_TYPE_DEALLOCATE(pQueryTransactionSession);
	}

	m_vbhPeerConnSumInfo.FreeSessionArray();

	return CDscHtsClientService::OnExit();
}

void CVbhAdapterBase::OnResponse(CRegistUserSession* pRegistUserSession, const ACE_INT32 nReturnCode,
	const DSC::CDscShortBlob& userKey /*= DSC::CDscShortBlob(nullptr, 0)*/)
{
	OnRegistUserResponse( nReturnCode, pRegistUserSession->m_nSrcSessionID, userKey);
}

void CVbhAdapterBase::OnTimeOut(CRegistUserSession* pRegistUserSession)
{
	DSC_RUN_LOG_INFO("CRegistUserSession timeout, session-id:%d", pRegistUserSession->m_nCltSessionID);

	OnResponse(pRegistUserSession, VBH::EN_TIMEOUT_ERROR_TYPE);
	m_mapRegistUserSession.Erase(pRegistUserSession);
	OnReleaseMultiMcpHandler(pRegistUserSession);
}

void CVbhAdapterBase::OnNetError(CRegistUserSession* pRegistUserSession, CVbhAdapterBaseHandler* pVbhClientServiceHandler)
{
	DSC_RUN_LOG_INFO("CRegistUserSession network error, session-id:%d", pRegistUserSession->m_nCltSessionID);

	this->OnResponse(pRegistUserSession, VBH::EN_NETWORK_ERROR_TYPE);
	this->CancelDscTimer(pRegistUserSession);
	m_mapRegistUserSession.Erase(pRegistUserSession);
	this->OnReleaseMultiMcpHandler(pRegistUserSession, pVbhClientServiceHandler);
}

void CVbhAdapterBase::OnResponse(CRegistUserWaitNotitySession* pNotifySession, const ACE_INT32 nReturnCode)
{
	OnRegistUserResultNotify(nReturnCode, pNotifySession->m_nSrcSessionID);
}

void CVbhAdapterBase::OnTimeOut(CRegistUserWaitNotitySession* pNotifySession)
{
	DSC_RUN_LOG_INFO("CRegistUserWaitNotitySession timeout, session-id:%d", pNotifySession->m_nCltSessionID);

	OnResponse(pNotifySession, VBH::EN_TIMEOUT_ERROR_TYPE);
	m_mapRegistUserWaitNotifySession.Erase(pNotifySession);
	this->OnReleaseSingleMcpHanler(pNotifySession);
}

void CVbhAdapterBase::OnNetError(CRegistUserWaitNotitySession* pNotifySession)
{
	DSC_RUN_LOG_INFO("CRegistUserWaitNotitySession network error, session-id:%d", pNotifySession->m_nCltSessionID);

	this->OnResponse(pNotifySession, VBH::EN_NETWORK_ERROR_TYPE);
	this->CancelDscTimer(pNotifySession);
	m_mapRegistUserWaitNotifySession.Erase(pNotifySession);
	DSC_THREAD_TYPE_DELETE(pNotifySession);
}

void CVbhAdapterBase::OnResponse(CCreateInformationSession* pCreateInfoSession, const ACE_INT32 nReturnCode, const DSC::CDscShortBlob& infoKey /*= DSC::CDscShortBlob(nullptr, 0)*/)
{
	OnCreateInformationRespondse( nReturnCode, pCreateInfoSession->m_nSrcSessionID, infoKey);
}

void CVbhAdapterBase::OnTimeOut(CCreateInformationSession* pCreateInfoSession)
{
	DSC_RUN_LOG_INFO("CCreateInformationSession timeout, session-id:%d", pCreateInfoSession->m_nCltSessionID);

	OnResponse(pCreateInfoSession, VBH::EN_TIMEOUT_ERROR_TYPE);
	m_mapCreateInfoSession.Erase(pCreateInfoSession);
	OnReleaseMultiMcpHandler(pCreateInfoSession);
}

void CVbhAdapterBase::OnNetError(CCreateInformationSession* pCreateInfoSession, CVbhAdapterBaseHandler* pVbhClientServiceHandler)
{
	DSC_RUN_LOG_INFO("CCreateInformationSession network error, session-id:%d", pCreateInfoSession->m_nCltSessionID);

	this->OnResponse(pCreateInfoSession, VBH::EN_NETWORK_ERROR_TYPE);
	this->CancelDscTimer(pCreateInfoSession);
	m_mapCreateInfoSession.Erase(pCreateInfoSession);
	this->OnReleaseMultiMcpHandler(pCreateInfoSession, pVbhClientServiceHandler);
}

void CVbhAdapterBase::OnResponse(CCreateInformationWaitNotitySession* pNotifySession, const ACE_INT32 nReturnCode)
{
	OnCreateInformationResultNotify( nReturnCode, pNotifySession->m_nSrcSessionID);
}

void CVbhAdapterBase::OnTimeOut(CCreateInformationWaitNotitySession* pNotifySession)
{
	DSC_RUN_LOG_INFO("CCreateInformationWaitNotitySession timeout, session-id:%d", pNotifySession->m_nCltSessionID);

	OnResponse(pNotifySession, VBH::EN_TIMEOUT_ERROR_TYPE);
	m_mapCreateInfoWaitNotifySession.Erase(pNotifySession);
	this->OnReleaseSingleMcpHanler(pNotifySession);
}

void CVbhAdapterBase::OnNetError(CCreateInformationWaitNotitySession* pNotifySession)
{
	DSC_RUN_LOG_INFO("CCreateInformationWaitNotitySession network error, session-id:%d", pNotifySession->m_nCltSessionID);

	this->OnResponse(pNotifySession, VBH::EN_NETWORK_ERROR_TYPE);
	this->CancelDscTimer(pNotifySession);
	m_mapCreateInfoWaitNotifySession.Erase(pNotifySession);
	DSC_THREAD_TYPE_DELETE(pNotifySession);
}

void CVbhAdapterBase::OnResponse(CProposeSession* pProposeSession, const ACE_INT32 nRetrunCode, const DSC::CDscShortBlob& transKey)
{
	OnProposeResponse(nRetrunCode, pProposeSession->m_nSrcSessionID, transKey);
}

void CVbhAdapterBase::OnTimeOut(CProposeSession* pProposeSession)
{
	DSC_RUN_LOG_INFO("CProposeSession timeout, session-id:%d", pProposeSession->m_nCltSessionID);

	this->OnResponse(pProposeSession, VBH::EN_TIMEOUT_ERROR_TYPE);
	m_mapProposeSession.Erase(pProposeSession);
	this->OnReleaseMultiMcpHandler(pProposeSession);
}

void CVbhAdapterBase::OnNetError(CProposeSession* pProposeSession, CVbhAdapterBaseHandler* pVbhClientServiceHandler)
{
	DSC_RUN_LOG_INFO("CProposeSession network error, session-id:%d", pProposeSession->m_nCltSessionID);

	this->CancelDscTimer(pProposeSession);
	this->OnResponse(pProposeSession, VBH::EN_NETWORK_ERROR_TYPE);
	m_mapProposeSession.Erase(pProposeSession);
	this->OnReleaseMultiMcpHandler(pProposeSession, pVbhClientServiceHandler);
}

void CVbhAdapterBase::OnResponse(CProposeWaitNotifySession* pNotifySession, const ACE_INT32 nReturnCode)
{
	OnProposeResultNotify(nReturnCode, pNotifySession->m_nSrcSessionID);
}

void CVbhAdapterBase::OnTimeOut(CProposeWaitNotifySession* pNotifySession)
{
	DSC_RUN_LOG_INFO("CProposeWaitNotifySession timeout, session-id:%d", pNotifySession->m_nCltSessionID);

	OnResponse(pNotifySession, VBH::EN_TIMEOUT_ERROR_TYPE);
	m_mapProposeWaitNotifySession.Erase(pNotifySession);
	this->OnReleaseSingleMcpHanler(pNotifySession);
}

void CVbhAdapterBase::OnNetError(CProposeWaitNotifySession* pNotifySession)
{
	DSC_RUN_LOG_INFO("CProposeWaitNotifySession network error, session-id:%d", pNotifySession->m_nCltSessionID);

	this->OnResponse(pNotifySession, VBH::EN_NETWORK_ERROR_TYPE);
	this->CancelDscTimer(pNotifySession);
	m_mapProposeWaitNotifySession.Erase(pNotifySession);
	DSC_THREAD_TYPE_DELETE(pNotifySession);
}

void CVbhAdapterBase::OnResponse(CQueryUserSession* pQueryUserSession, const ACE_INT32 nReturnCode, DSC::CDscShortBlob& userInfo /*= DSC::CDscShortBlob()*/)
{
	OnQueryUserInfoResponse( nReturnCode, pQueryUserSession->m_nSrcSessionID, userInfo);
}

void CVbhAdapterBase::OnTimeOut(CQueryUserSession* pQueryUserSession)
{
	DSC_RUN_LOG_INFO("CQueryUserSession timeout, session-id:%d", pQueryUserSession->m_nCltSessionID);

	DSC::CDscShortBlob userInfo;

	this->CancelDscTimer(pQueryUserSession);
	this->OnResponse(pQueryUserSession, VBH::EN_TIMEOUT_ERROR_TYPE, userInfo);
	m_mapQueryUserSession.Erase(pQueryUserSession);
	this->OnReleaseSingleMcpHanler(pQueryUserSession);
}

void CVbhAdapterBase::OnNetError(CQueryUserSession* pQueryUserSession)
{
	DSC_RUN_LOG_INFO("CQueryUserSession network error, session-id:%d", pQueryUserSession->m_nCltSessionID);

	DSC::CDscShortBlob userInfo;

	this->OnResponse(pQueryUserSession, VBH::EN_NETWORK_ERROR_TYPE, userInfo);
	this->CancelDscTimer(pQueryUserSession);
	m_mapQueryUserSession.Erase(pQueryUserSession);
	DSC_THREAD_TYPE_DELETE(pQueryUserSession);
}

void CVbhAdapterBase::OnResponse(CQueryTransactionSession* pQueryTransSession, const ACE_INT32 nReturnCode, DSC::CDscBlob& transInfo /*= DSC::CDscBlob()*/)
{
	OnQueryTransactionInfoResponse( nReturnCode, pQueryTransSession->m_nSrcSessionID, transInfo);
}

void CVbhAdapterBase::OnTimeOut(CQueryTransactionSession* pQueryTransSession)
{
	DSC_RUN_LOG_INFO("CQueryTransactionSession timeout, session-id:%d", pQueryTransSession->m_nCltSessionID);

	DSC::CDscBlob transInfo;

	this->OnResponse(pQueryTransSession, VBH::EN_TIMEOUT_ERROR_TYPE, transInfo);
	m_mapQueryTransSession.Erase(pQueryTransSession);
	this->OnReleaseSingleMcpHanler(pQueryTransSession);
}

void CVbhAdapterBase::OnNetError(CQueryTransactionSession* pQueryTransSession)
{
	DSC_RUN_LOG_INFO("CQueryTransactionSession network error, session-id:%d", pQueryTransSession->m_nCltSessionID);

	DSC::CDscBlob transInfo;

	this->OnResponse(pQueryTransSession, VBH::EN_NETWORK_ERROR_TYPE, transInfo);
	this->CancelDscTimer(pQueryTransSession);
	m_mapQueryTransSession.Erase(pQueryTransSession);
	DSC_THREAD_TYPE_DELETE(pQueryTransSession);
}

ACE_INT32 CVbhAdapterBase::OnHtsMsg(VBH::CRegistUserEsCltRsp& rRegistUserRsp, CMcpHandler* pMcpHandler)
{
	VBH_MESSAGE_ENTER_TRACE(CVbhAdapterBase, VBH::CRegistUserEsCltRsp);

	CRegistUserSession* pRegistUserSession = m_mapRegistUserSession.Find(rRegistUserRsp.m_nCltSessionID);

	//1.找session
	if (pRegistUserSession)
	{
		ACE_INT32 nReturnCode = rRegistUserRsp.m_nReturnCode;

		if (nReturnCode == VBH::EN_OK_TYPE) //同意创建
		{
			//3.背书
			if (pRegistUserSession->m_strCcGenerateUserInfo.empty()) //初次收到应答
			{
				VBH::Assign(pRegistUserSession->m_strCcGenerateUserInfo, rRegistUserRsp.m_userInitInfo);
				++pRegistUserSession->m_nSucceedRspNum;
			}
			else//第N次收到应答 N>1
			{
				if (VBH::IsEqual(pRegistUserSession->m_strCcGenerateUserInfo, rRegistUserRsp.m_userInitInfo))
				{
					++pRegistUserSession->m_nSucceedRspNum;
				}
				else
				{
					DSC_RUN_LOG_ERROR("endorse failed.");
					nReturnCode = VBH::EN_SYSTEM_ERROR_TYPE;
				}
			}

			if (nReturnCode == VBH::EN_OK_TYPE)
			{
				//是提交的endorser节点发送的应答
				if (pMcpHandler == pRegistUserSession->m_pSubmitEsHandler)
				{
					pRegistUserSession->m_nEsSessionID = rRegistUserRsp.m_nEsSessionID; //endorser session-id
				}

				//TODO: 背书策略或许会调整
				if (pRegistUserSession->m_nSucceedRspNum >= m_nEndorsePeerCount) //背书通过
				{
					//组织回应答(再次提交注册事务)							
					//5.向服务端提交
					VBH::CSubmitRegistUserTransactionCltEsReq req;

					req.m_nEsSessionID = pRegistUserSession->m_nEsSessionID;
					VBH::Assign(req.m_cltPubKey, pRegistUserSession->m_strCltPubKey);
					VBH::Assign(req.m_svrPriKey, pRegistUserSession->m_strSvrPriKey);
					VBH::Assign(req.m_envelopeKey, pRegistUserSession->m_strEnvelopeKey);

					if (DSC_UNLIKELY(this->SendHtsMsg(req, pRegistUserSession->m_pSubmitEsHandler)))
					{
						DSC_RUN_LOG_ERROR("send hts msg failed.");
						nReturnCode = VBH::EN_NETWORK_ERROR_TYPE;
					}
				}
				else
				{
					//不用处理
				}
			}
		}
		else
		{
			DSC_RUN_LOG_INFO("regist user rsp return failed, error code:%d", rRegistUserRsp.m_nReturnCode);
		}

		if (nReturnCode == VBH::EN_OK_TYPE)
		{
			//应答返回后，如果不是提交节点，则删除对应的session 勾连关系
			RemoveNonSubmitMcpHandler(pRegistUserSession, pMcpHandler);
		}
		else
		{
			//出错后，向选中的提交节点发送取消事务的消息//网络出错就不再发送
			if (nReturnCode != VBH::EN_NETWORK_ERROR_TYPE)
			{
				VBH::CCancelRegistUserCltEsReq req;

				req.m_nEsSessionID = pRegistUserSession->m_nEsSessionID;
				this->SendHtsMsg(req, pRegistUserSession->m_pSubmitEsHandler);
			}

			//向客户端上层业务应答，//删除session
			this->OnResponse(pRegistUserSession, nReturnCode);
			this->CancelDscTimer(pRegistUserSession);
			m_mapRegistUserSession.Erase(pRegistUserSession);
			OnReleaseMultiMcpHandler(pRegistUserSession); //删除session
		}
	}
	else
	{
		DSC_RUN_LOG_ERROR("can not find session, session-id:%d", rRegistUserRsp.m_nCltSessionID);
	}

	VBH_MESSAGE_LEAVE_TRACE(CVbhAdapterBase, VBH::CRegistUserEsCltRsp);

	return 0;
}

ACE_INT32 CVbhAdapterBase::OnHtsMsg(VBH::CSubmitRegistUserTransactionEsCltRsp& rSubmitTransRsp, CMcpHandler* pMcpHandler)
{
	VBH_MESSAGE_ENTER_TRACE(CVbhAdapterBase, VBH::CSubmitRegistUserTransactionEsCltRsp);

	CRegistUserSession* pRegistUserSession = m_mapRegistUserSession.Erase(rSubmitTransRsp.m_nCltSessionID);

	if (pRegistUserSession)
	{
		ACE_INT32 nReturnCode = rSubmitTransRsp.m_nReturnCode;

		if (nReturnCode == VBH::EN_OK_TYPE)
		{
			//为用户生成key
			char* pUserKey;
			size_t nUserKeyLen = VBH::HexEncode(pUserKey, rSubmitTransRsp.m_userKey);
			DSC::CDscShortBlob blob(pUserKey, nUserKeyLen);

			//向客户端上层业务应答
			this->OnResponse(pRegistUserSession, nReturnCode, blob);
			blob.FreeBuffer();

			if (pRegistUserSession->m_bWaitNotify)//如果客户等待通知，则创建wait-notify-session
			{
				CRegistUserWaitNotitySession* pNotifySession = DSC_THREAD_TYPE_NEW(CRegistUserWaitNotitySession) CRegistUserWaitNotitySession(*this);

				pNotifySession->m_nSrcSessionID = pRegistUserSession->m_nSrcSessionID;
				pNotifySession->m_nCltSessionID = pRegistUserSession->m_nCltSessionID;
				pNotifySession->m_pHandler = pRegistUserSession->m_pSubmitEsHandler;

				pNotifySession->m_pHandler->m_arrSession.Insert(pNotifySession);

				m_mapRegistUserWaitNotifySession.DirectInsert(pNotifySession->m_nCltSessionID, pNotifySession);
				this->SetDscTimer(pNotifySession, EN_SESSION_WAIT_NOTIFY_TIMEOUT_VALUE);
			}

			//删除session 
			this->CancelDscTimer(pRegistUserSession);
			this->OnReleaseMultiMcpHandler(pRegistUserSession);
		}
		else
		{
			DSC_RUN_LOG_INFO("submit regist transaction rsp return failed, error code:%d", rSubmitTransRsp.m_nReturnCode);
		}
	}
	else
	{
		DSC_RUN_LOG_WARNING("can not find regist session, session-id:%d", rSubmitTransRsp.m_nCltSessionID);
	}

	VBH_MESSAGE_LEAVE_TRACE(CVbhAdapterBase, VBH::CSubmitRegistUserTransactionEsCltRsp);

	return 0;
}

ACE_INT32 CVbhAdapterBase::OnHtsMsg(VBH::CSubscribeRegistUserTransactionEsCltNotify& rNotify, CMcpHandler* pMcpHandler)
{
	VBH_MESSAGE_ENTER_TRACE(CVbhAdapterBase, VBH::CSubscribeRegistUserTransactionEsCltNotify);

	CRegistUserWaitNotitySession* pSession = m_mapRegistUserWaitNotifySession.Erase(rNotify.m_nCltSessionID);

	if (DSC_LIKELY(pSession))
	{
		this->OnResponse(pSession, rNotify.m_nReturnCode);
		this->CancelDscTimer(pSession);
		this->OnReleaseSingleMcpHanler(pSession);
	}
	else
	{
		DSC_RUN_LOG_ERROR("can not find regist user wait notfiy session.");
	}

	VBH_MESSAGE_LEAVE_TRACE(CVbhAdapterBase, VBH::CSubscribeRegistUserTransactionEsCltNotify);

	return 0;
}

ACE_INT32 CVbhAdapterBase::OnHtsMsg(VBH::CCreateInformationEsCltRsp& rCreateInfoRsp, CMcpHandler* pMcpHandler)
{
	VBH_MESSAGE_ENTER_TRACE(CVbhAdapterBase, VBH::CCreateInformationEsCltRsp);

	CCreateInformationSession* pCreateInfoSession = m_mapCreateInfoSession.Find(rCreateInfoRsp.m_nCltSessionID);

	//1.找session
	if (pCreateInfoSession)
	{
		ACE_INT32 nReturnCode = rCreateInfoRsp.m_nReturnCode;

		if (nReturnCode == VBH::EN_OK_TYPE) //同意创建
		{
			//3.背书
			if (pCreateInfoSession->m_strCcGenerateValue.empty()) //初次收到应答
			{
				VBH::Assign(pCreateInfoSession->m_strCcGenerateValue, rCreateInfoRsp.m_initValue);
				++pCreateInfoSession->m_nSucceedRspNum;
			}
			else//第N次收到应答 N>1
			{
				if (VBH::IsEqual(pCreateInfoSession->m_strCcGenerateValue, rCreateInfoRsp.m_initValue))
				{
					++pCreateInfoSession->m_nSucceedRspNum;
				}
				else
				{
					DSC_RUN_LOG_ERROR("endorse failed.");
					nReturnCode = VBH::EN_SYSTEM_ERROR_TYPE;
				}
			}

			if (nReturnCode == VBH::EN_OK_TYPE)
			{
				//是提交的endorser节点发送的应答
				if (pMcpHandler == pCreateInfoSession->m_pSubmitEsHandler)
				{
					pCreateInfoSession->m_nEsSessionID = rCreateInfoRsp.m_nEsSessionID; //endorser session-id
				}

				//TODO: 背书策略或许会调整
				if (pCreateInfoSession->m_nSucceedRspNum >= m_nEndorsePeerCount) //背书通过
				{
					//组织回应答(再次提交注册事务)							
					//5.向服务端提交
					VBH::CSubmitCreateInformationTransactionCltEsReq req;

					req.m_nEsSessionID = pCreateInfoSession->m_nEsSessionID;

					if (DSC_UNLIKELY(this->SendHtsMsg(req, pCreateInfoSession->m_pSubmitEsHandler)))
					{
						DSC_RUN_LOG_ERROR("send hts msg failed.");
						nReturnCode = VBH::EN_NETWORK_ERROR_TYPE;
					}
				}
				else
				{
					//不用处理
				}
			}
		}
		else
		{
			DSC_RUN_LOG_INFO("regist user rsp return failed, error code:%d", rCreateInfoRsp.m_nReturnCode);
		}

		if (nReturnCode == VBH::EN_OK_TYPE)
		{
			//应答返回后，如果不是提交节点，则删除对应的session 勾连关系
			RemoveNonSubmitMcpHandler(pCreateInfoSession, pMcpHandler);
		}
		else
		{
			//出错后，向选中的提交节点发送取消事务的消息//网络出错就不再发送
			if (nReturnCode != VBH::EN_NETWORK_ERROR_TYPE)
			{
				VBH::CCancelCreateInformationCltEsReq req;

				req.m_nEsSessionID = pCreateInfoSession->m_nEsSessionID;
				this->SendHtsMsg(req, pCreateInfoSession->m_pSubmitEsHandler);
			}

			//向客户端上层业务应答，//删除session
			this->OnResponse(pCreateInfoSession, nReturnCode);
			this->CancelDscTimer(pCreateInfoSession);
			m_mapCreateInfoSession.Erase(pCreateInfoSession);
			OnReleaseMultiMcpHandler(pCreateInfoSession); //删除session
		}
	}
	else
	{
		DSC_RUN_LOG_ERROR("can not find session, session-id:%d", rCreateInfoRsp.m_nCltSessionID);
	}

	VBH_MESSAGE_LEAVE_TRACE(CVbhAdapterBase, VBH::CCreateInformationEsCltRsp);

	return 0;
}

ACE_INT32 CVbhAdapterBase::OnHtsMsg(VBH::CSubmitCreateInformationTransactionEsCltRsp& rSubmitTransRsp, CMcpHandler* pMcpHandler)
{
	VBH_MESSAGE_ENTER_TRACE(CVbhAdapterBase, VBH::CSubmitCreateInformationTransactionEsCltRsp);

	CCreateInformationSession* pCreateInfoSession = m_mapCreateInfoSession.Erase(rSubmitTransRsp.m_nCltSessionID);

	if (pCreateInfoSession)
	{
		ACE_INT32 nReturnCode = rSubmitTransRsp.m_nReturnCode;

		if (nReturnCode == VBH::EN_OK_TYPE)
		{
			//为用户生成key和秘钥
			char* pKey;
			size_t nKeyLen = VBH::HexEncode(pKey, rSubmitTransRsp.m_infoKey);
			DSC::CDscShortBlob blobKey(pKey, nKeyLen);

			//向客户端上层业务应答
			this->OnResponse(pCreateInfoSession, nReturnCode, blobKey);
			blobKey.FreeBuffer();

			if (pCreateInfoSession->m_bWaitNotify)//如果客户等待通知，则创建wait-notify-session
			{
				CCreateInformationWaitNotitySession* pNotifySession = DSC_THREAD_TYPE_NEW(CCreateInformationWaitNotitySession) CCreateInformationWaitNotitySession(*this);

				pNotifySession->m_nSrcSessionID = pCreateInfoSession->m_nSrcSessionID;
				pNotifySession->m_nCltSessionID = pCreateInfoSession->m_nCltSessionID;
				pNotifySession->m_pHandler = pCreateInfoSession->m_pSubmitEsHandler;

				pNotifySession->m_pHandler->m_arrSession.Insert(pNotifySession);

				m_mapCreateInfoWaitNotifySession.DirectInsert(pNotifySession->m_nCltSessionID, pNotifySession);
				this->SetDscTimer(pNotifySession, EN_SESSION_WAIT_NOTIFY_TIMEOUT_VALUE);
			}

			//删除session 
			this->CancelDscTimer(pCreateInfoSession);
			this->OnReleaseMultiMcpHandler(pCreateInfoSession);
		}
		else
		{
			DSC_RUN_LOG_INFO("submit create information transaction rsp return failed, error code:%d", rSubmitTransRsp.m_nReturnCode);
		}
	}
	else
	{
		DSC_RUN_LOG_WARNING("can not find create information session, session-id:%d", rSubmitTransRsp.m_nCltSessionID);
	}

	VBH_MESSAGE_LEAVE_TRACE(CVbhAdapterBase, VBH::CSubmitCreateInformationTransactionEsCltRsp);

	return 0;
}

ACE_INT32 CVbhAdapterBase::OnHtsMsg(VBH::CSubscribeCreateInformationTransactionEsCltNotify& rNotify, CMcpHandler* pMcpHandler)
{
	VBH_MESSAGE_ENTER_TRACE(CVbhAdapterBase, VBH::CSubscribeCreateInformationTransactionEsCltNotify);

	CCreateInformationWaitNotitySession* pSession = m_mapCreateInfoWaitNotifySession.Erase(rNotify.m_nCltSessionID);

	if (DSC_LIKELY(pSession))
	{
		this->OnResponse(pSession, rNotify.m_nReturnCode);
		this->CancelDscTimer(pSession);
		this->OnReleaseSingleMcpHanler(pSession);
	}
	else
	{
		DSC_RUN_LOG_ERROR("can not find create information wait notfiy session.");
	}

	VBH_MESSAGE_LEAVE_TRACE(CVbhAdapterBase, VBH::CSubscribeCreateInformationTransactionEsCltNotify);

	return 0;
}

ACE_INT32 CVbhAdapterBase::OnHtsMsg(VBH::CProposeEsCltRsp& rSubmitProposalRsp, CMcpHandler* pMcpHandler)
{
	VBH_MESSAGE_ENTER_TRACE(CVbhAdapterBase, VBH::CProposeEsCltRsp);

	CProposeSession* pProposeSession = m_mapProposeSession.Find(rSubmitProposalRsp.m_nCltSessionID);

	if (DSC_LIKELY(pProposeSession))
	{
		ACE_INT32 nReturnCode = rSubmitProposalRsp.m_nReturnCode;

		if (DSC_LIKELY(nReturnCode == VBH::EN_OK_TYPE))
		{
			if (pProposeSession->m_strTransContent.empty())
			{
				VBH::Assign(pProposeSession->m_strTransContent, rSubmitProposalRsp.m_transContent);
				++pProposeSession->m_nSucceedRspNum;
			}
			else
			{
				if (pProposeSession->m_strTransContent.IsEqual(rSubmitProposalRsp.m_transContent.GetBuffer(), rSubmitProposalRsp.m_transContent.GetSize()))
				{
					++pProposeSession->m_nSucceedRspNum;
				}
				else
				{
					DSC_RUN_LOG_ERROR("endorser failed.");
					nReturnCode = VBH::EN_SYSTEM_ERROR_TYPE;
				}
			}

			//是选中的节点发送的应答
			if ((nReturnCode == VBH::EN_OK_TYPE) && (pMcpHandler == pProposeSession->m_pSubmitEsHandler))
			{
				pProposeSession->m_nEsSessionID = rSubmitProposalRsp.m_nEsSessionID; //记录endorser session-id
			}

			//TODO: 背书策略或许会调整
			if (pProposeSession->m_nSucceedRspNum >= m_nEndorsePeerCount)
			{
				VBH::CSubmitProposalTransactionCltEsReq req;

				req.m_nEsSessionID = pProposeSession->m_nEsSessionID;

				if (DSC_UNLIKELY(this->SendHtsMsg(req, pProposeSession->m_pSubmitEsHandler)))
				{
					DSC_RUN_LOG_ERROR("send hts msg failed.");
					nReturnCode = VBH::EN_NETWORK_ERROR_TYPE;
				}
			}
			else
			{
				//无需处理，再等下一应答消息
			}
		}
		else
		{
			DSC_RUN_LOG_INFO("submit proposal rsp return failed, error code:%d", rSubmitProposalRsp.m_nReturnCode);
		}

		if (nReturnCode == VBH::EN_OK_TYPE)
		{
			//应答返回后，如果不是提交节点，则删除对应的session 勾连关系
			RemoveNonSubmitMcpHandler(pProposeSession, pMcpHandler);
		}
		else
		{
			// 不是网络错误的情况下， 出错后，向选中的提交节点发送取消事务的消息
			if (nReturnCode != VBH::EN_NETWORK_ERROR_TYPE)
			{
				VBH::CCancelProposalTransactionCltEsReq req;

				req.m_nEsSessionID = pProposeSession->m_nEsSessionID;
				this->SendHtsMsg(req, pProposeSession->m_pSubmitEsHandler);
			}

			//向客户端上层通知 //删除session
			this->OnResponse(pProposeSession, nReturnCode);
			this->CancelDscTimer(pProposeSession);
			m_mapProposeSession.Erase(pProposeSession);
			this->OnReleaseMultiMcpHandler(pProposeSession);
		}
	}
	else
	{
		DSC_RUN_LOG_INFO("can not find submit proposal session, session-id:%d", rSubmitProposalRsp.m_nCltSessionID);
	}

	VBH_MESSAGE_LEAVE_TRACE(CVbhAdapterBase, VBH::CProposeEsCltRsp);

	return 0;
}

ACE_INT32 CVbhAdapterBase::OnHtsMsg(VBH::CSubmitProposalTransactionEsCltRsp& rSubmitTransRsp, CMcpHandler* pMcpHandler)
{
	VBH_MESSAGE_ENTER_TRACE(CVbhAdapterBase, VBH::CSubmitProposalTransactionEsCltRsp);

	CProposeSession* pProposeSession = m_mapProposeSession.Erase(rSubmitTransRsp.m_nCltSessionID);

	if (pProposeSession)
	{
		ACE_INT32 nReturnCode = rSubmitTransRsp.m_nReturnCode;

		if (nReturnCode == VBH::EN_OK_TYPE)
		{
			//返回提案应答
			char* pTransKey;
			size_t nTranKeyLen = VBH::HexEncode(pTransKey, rSubmitTransRsp.m_alocTransKey);
			DSC::CDscShortBlob transKey(pTransKey, nTranKeyLen);

			this->OnResponse(pProposeSession, VBH::EN_OK_TYPE, transKey);
			transKey.FreeBuffer();

			if (pProposeSession->m_bWaitNotify) //如果客户等待通知，则创建wait-notify-session
			{
				CProposeWaitNotifySession* pNotifySession = DSC_THREAD_TYPE_NEW(CProposeWaitNotifySession) CProposeWaitNotifySession(*this);

				pNotifySession->m_nSrcSessionID = pProposeSession->m_nSrcSessionID;
				pNotifySession->m_nCltSessionID = pProposeSession->m_nCltSessionID;
				pNotifySession->m_pHandler = pProposeSession->m_pSubmitEsHandler;

				pNotifySession->m_pHandler->m_arrSession.Insert(pNotifySession);

				m_mapProposeWaitNotifySession.DirectInsert(pNotifySession->m_nCltSessionID, pNotifySession);
				this->SetDscTimer(pNotifySession, EN_SESSION_WAIT_NOTIFY_TIMEOUT_VALUE);
			}
			else
			{
				VBH_TRACE_MESSAGE("user do not wait notify, session-id:%d", pProposeSession->m_nCltSessionID);
			}
			//客户不等待通知，则删除session
			this->CancelDscTimer(pProposeSession);
			this->OnReleaseMultiMcpHandler(pProposeSession);
		}
		else
		{
			DSC_RUN_LOG_INFO("submit proposal transaction rsp return failed");
		}
	}
	else
	{
		DSC_RUN_LOG_WARNING("can not find propose session, session-id:%d", rSubmitTransRsp.m_nCltSessionID);
	}

	VBH_MESSAGE_LEAVE_TRACE(CVbhAdapterBase, VBH::CSubmitProposalTransactionEsCltRsp);

	return 0;
}

ACE_VERSIONED_NAMESPACE_NAME::ACE_INT32 CVbhAdapterBase::OnHtsMsg(VBH::CSubscribeProposeTransactionEsCltNotify& rNotify, CMcpHandler* pMcpHandler)
{
	VBH_MESSAGE_ENTER_TRACE(CVbhAdapterBase, VBH::CSubscribeProposeTransactionEsCltNotify);

	CProposeWaitNotifySession* pSession = m_mapProposeWaitNotifySession.Erase(rNotify.m_nCltSessionID);

	if (DSC_LIKELY(pSession))
	{
		this->OnResponse(pSession, rNotify.m_nReturnCode);
		this->CancelDscTimer(pSession);
		this->OnReleaseSingleMcpHanler(pSession);
	}
	else
	{
		DSC_RUN_LOG_ERROR("can not find propose wait notfiy session, session-id:%d.", rNotify.m_nCltSessionID);
	}

	VBH_MESSAGE_LEAVE_TRACE(CVbhAdapterBase, VBH::CSubscribeProposeTransactionEsCltNotify);

	return 0;
}

ACE_INT32 CVbhAdapterBase::OnHtsMsg(VBH::CQueryUserInfoEsCltRsp& rQueryUserRsp, CMcpHandler* pMcpHandler)
{
	VBH_MESSAGE_ENTER_TRACE(CVbhAdapterBase, VBH::CQueryUserInfoEsCltRsp);

	CQueryUserSession* pQueryUserSession = m_mapQueryUserSession.Erase(rQueryUserRsp.m_nSessionID);

	if (DSC_LIKELY(pQueryUserSession))
	{
		this->OnResponse(pQueryUserSession, rQueryUserRsp.m_nReturnCode, rQueryUserRsp.m_userInfo);
		this->CancelDscTimer(pQueryUserSession);
		this->OnReleaseSingleMcpHanler(pQueryUserSession);
	}
	else
	{
		DSC_RUN_LOG_ERROR("can not find query user session.");
	}
	VBH_MESSAGE_LEAVE_TRACE(CVbhAdapterBase, VBH::CQueryUserInfoEsCltRsp);

	return 0;
}

ACE_INT32 CVbhAdapterBase::OnHtsMsg(VBH::CQueryTransInfoEsCltRsp& rQueryTransRsp, CMcpHandler* pMcpHandler)
{
	VBH_MESSAGE_ENTER_TRACE(CVbhAdapterBase, VBH::CQueryTransInfoEsCltRsp);
	CQueryTransactionSession* pQueryTransSession = m_mapQueryTransSession.Erase(rQueryTransRsp.m_nCltSessionID);

	if (DSC_LIKELY(pQueryTransSession))
	{
		this->OnResponse(pQueryTransSession, rQueryTransRsp.m_nReturnCode, rQueryTransRsp.m_transInfo);
		this->CancelDscTimer(pQueryTransSession);
		this->OnReleaseSingleMcpHanler(pQueryTransSession);
	}
	else
	{
		DSC_RUN_LOG_WARNING("can not find query transaction session.");
	}
	VBH_MESSAGE_LEAVE_TRACE(CVbhAdapterBase, VBH::CQueryTransInfoEsCltRsp);
	return 0;
}

ACE_INT32 CVbhAdapterBase::RegistUser(ACE_UINT32 nRequestID, ACE_UINT32 nChannelID, bool bWaitNotify, const DSC::CDscShortBlob& userInfo, const DSC::CDscShortBlob& serverCryptKey)
{
	VBH_MESSAGE_ENTER_TRACE(CVbhAdapterBase, RegistUser);

	ACE_INT32 nReturnCode = VBH::EN_OK_TYPE;

	if (m_vbhPeerConnSumInfo.m_nSessionNum < m_nEndorsePeerCount)
	{
		DSC_RUN_LOG_WARNING("connected peer num:%d cann't less than configed endorse peer count:%d", m_vbhPeerConnSumInfo.m_nSessionNum, m_nEndorsePeerCount);

		nReturnCode = VBH::EN_NETWORK_ERROR_TYPE;
	}
	else
	{
		CRegistUserSession* pRegistUserSession = DSC_THREAD_TYPE_NEW(CRegistUserSession) CRegistUserSession(*this);
		VBH::CRegistUserCltEsReq req;
		DSC::CDscShortBlob cserverCryptKey(serverCryptKey);

		if (VBH::DecodeCryptKey(pRegistUserSession->m_strCltPubKey, pRegistUserSession->m_strSvrPriKey, pRegistUserSession->m_strEnvelopeKey, cserverCryptKey))
		{
			DSC_RUN_LOG_ERROR("decode server crpyt key failed.");
			DSC_THREAD_TYPE_DELETE(pRegistUserSession);
			nReturnCode = VBH::EN_DECODE_ERROR_TYPE;
		}
		else
		{
			req.m_bWaitNotify = bWaitNotify;
			req.m_nCltSessionID = m_nSessionID;
			req.m_nChannelID = nChannelID;
			req.m_userInfo = userInfo;

			nReturnCode = SendMultiEndorseMessage(req, pRegistUserSession);
			if (nReturnCode == VBH::EN_OK_TYPE) //发送时会建立勾连关系
			{
				pRegistUserSession->m_nSrcSessionID = nRequestID;
				pRegistUserSession->m_bWaitNotify = bWaitNotify;
				pRegistUserSession->m_nCltSessionID = m_nSessionID;

				SetDscTimer(pRegistUserSession, EN_SESSION_TIMEOUT_VALUE);
				m_mapRegistUserSession.DirectInsert(m_nSessionID, pRegistUserSession);
				++m_nSessionID;
			}
			else
			{
				OnReleaseMultiMcpHandler(pRegistUserSession); //删除勾连关系的同时，删除session 
			}
		}
	}

	VBH_MESSAGE_LEAVE_TRACE(CVbhAdapterBase, RegistUser);

	return nReturnCode;
}

ACE_INT32 CVbhAdapterBase::CreateInformation(ACE_UINT32 nRequestID, ACE_UINT32 nChannelID, bool bWaitNotify, const DSC::CDscShortBlob &userCryptKey, const DSC::CDscShortBlob &proposal)
{
	VBH_MESSAGE_ENTER_TRACE(CVbhAdapterBase, CreateInformation);

	ACE_INT32 nReturnCode = VBH::EN_OK_TYPE;

	if (m_vbhPeerConnSumInfo.m_nSessionNum < m_nEndorsePeerCount)
	{
		DSC_RUN_LOG_WARNING("connected peer num:%d cann't less than configed endorse peer count:%d", m_vbhPeerConnSumInfo.m_nSessionNum, m_nEndorsePeerCount);

		nReturnCode = VBH::EN_NETWORK_ERROR_TYPE;
	}
	else
	{
		DSC::CDscShortBlob cuserCryptKey(userCryptKey);

		CDscString strSvrPubKey;
		CDscString strCltPriKey;
		CDscString strEnvelopeKey;

		if (VBH::DecodeCryptKey(strSvrPubKey, strCltPriKey, strEnvelopeKey, cuserCryptKey))
		{
			DSC_RUN_LOG_ERROR("decode server crpyt key failed.");
			nReturnCode = VBH::EN_DECODE_ERROR_TYPE;
		}
		else
		{
			CCreateInformationSession* pCreateInfoSession = DSC_THREAD_TYPE_NEW(CCreateInformationSession) CCreateInformationSession(*this);
			VBH::CCreateInformationCltEsReq req;


			req.m_bWaitNotify = bWaitNotify;
			req.m_nCltSessionID = m_nSessionID;
			req.m_nChannelID = nChannelID;
			//TODO: 添加签名
			req.m_proposal = proposal;

			nReturnCode = SendMultiEndorseMessage(req, pCreateInfoSession);
			if (nReturnCode == VBH::EN_OK_TYPE)
			{
				pCreateInfoSession->m_nSrcSessionID = nRequestID;
				pCreateInfoSession->m_bWaitNotify = bWaitNotify;
				pCreateInfoSession->m_nCltSessionID = m_nSessionID;

				SetDscTimer(pCreateInfoSession, EN_SESSION_TIMEOUT_VALUE);
				m_mapCreateInfoSession.DirectInsert(m_nSessionID, pCreateInfoSession);
				++m_nSessionID;
			}
			else
			{
				OnReleaseMultiMcpHandler(pCreateInfoSession); //删除勾连关系的同时，删除session 
			}
		}
	}

	VBH_MESSAGE_LEAVE_TRACE(CVbhAdapterBase, CreateInformation);

	return nReturnCode;
}

ACE_INT32 CVbhAdapterBase::Propose(ACE_UINT32 nRequestID, ACE_UINT32 nChannelID, bool bWaitNotify, ACE_UINT32 nActionID, const DSC::CDscShortBlob& userKey, const DSC::CDscShortBlob& propose)
{
	VBH_MESSAGE_ENTER_TRACE(CVbhAdapterBase, Propose);

	ACE_INT32 nReturnCode = VBH::EN_OK_TYPE;

	if (m_vbhPeerConnSumInfo.m_nSessionNum < m_nEndorsePeerCount)
	{
		DSC_RUN_LOG_WARNING("connected peer num:%d cann't less than configed endorse peer count:%d", m_vbhPeerConnSumInfo.m_nSessionNum, m_nEndorsePeerCount);
		nReturnCode = VBH::EN_NETWORK_ERROR_TYPE;
	}
	else
	{
		VBH::CProposeCltEsReq req;

		if (DSC_UNLIKELY(VBH::HexDecode(req.m_userKey, userKey.GetBuffer(), userKey.GetSize())))
		{
			DSC_RUN_LOG_ERROR("decode client crypt key error.");
			nReturnCode = VBH::EN_DECRYPT_ERROR_TYPE;
		}
		else
		{
			CProposeSession* pSession = DSC_THREAD_TYPE_NEW(CProposeSession) CProposeSession(*this);

			req.m_bWaitNotify = bWaitNotify;
			req.m_nCltSessionID = m_nSessionID;
			req.m_nAction = nActionID;
			req.m_proposal = propose;
			req.m_nChannelID = nChannelID;

			nReturnCode = this->SendMultiEndorseMessage(req, pSession);

			if (nReturnCode == VBH::EN_OK_TYPE)
			{
				pSession->m_bWaitNotify = bWaitNotify;
				pSession->m_nCltSessionID = m_nSessionID;
				pSession->m_nSrcSessionID = nRequestID;

				this->SetDscTimer(pSession, CVbhAdapterBase::EN_SESSION_TIMEOUT_VALUE);
				m_mapProposeSession.Insert(m_nSessionID, pSession);
				++m_nSessionID;
			}
			else
			{
				OnReleaseMultiMcpHandler(pSession); //删除勾连关系的同时，删除session 
			}
		}
	}

	VBH_MESSAGE_LEAVE_TRACE(CVbhAdapterBase, Propose);

	return nReturnCode;
}

ACE_INT32 CVbhAdapterBase::QueryUserInfo(ACE_UINT32 nRequestID, ACE_UINT32 nChannelID, const DSC::CDscShortBlob& userKey)
{
	VBH_MESSAGE_ENTER_TRACE(CVbhAdapterBase, QueryUserInfo);

	ACE_INT32 nReturnCode = VBH::EN_OK_TYPE;
	VBH::CQueryUserInfoCltEsReq req;

	//解码user-key
	if (DSC_UNLIKELY(VBH::HexDecode(req.m_userKey, userKey.GetBuffer(), userKey.GetSize())))
	{
		DSC_RUN_LOG_ERROR("decode client crypt key error.");
		nReturnCode = VBH::EN_DECRYPT_ERROR_TYPE;
	}
	else
	{
		CQueryUserSession* pSession = DSC_THREAD_TYPE_NEW(CQueryUserSession) CQueryUserSession(*this);

		req.m_nCltSessionID = m_nSessionID;
		req.m_nChannelID = nChannelID;

		nReturnCode = this->SendSingleEndoreMessage(req, pSession);

		if (nReturnCode == VBH::EN_OK_TYPE)
		{
			pSession->m_nSrcSessionID = nRequestID;
			pSession->m_nCltSessionID = m_nSessionID;

			this->SetDscTimer(pSession, CVbhAdapterBase::EN_SESSION_TIMEOUT_VALUE);
			m_mapQueryUserSession.Insert(m_nSessionID, pSession);
			++m_nSessionID;
		}
		else
		{
			this->OnReleaseSingleMcpHanler(pSession); //删除勾连关系
		}
	}

	VBH_MESSAGE_LEAVE_TRACE(CVbhAdapterBase, QueryUserInfo);

	return nReturnCode;
}

ACE_INT32 CVbhAdapterBase::QueryTransactionInfo(ACE_UINT32 nRequestID, ACE_UINT32 nChannelID, const DSC::CDscShortBlob& transKey)
{
	VBH_MESSAGE_ENTER_TRACE(CVbhAdapterBase, QueryTransactionInfo);

	ACE_INT32 nReturnCode = VBH::EN_OK_TYPE;
	VBH::CQueryTransInfoCltEsReq req;

	if (VBH::HexDecode(req.m_transKey, transKey.GetBuffer(), transKey.GetSize()))
	{
		DSC_RUN_LOG_ERROR("decode transaction key error.");
		nReturnCode = VBH::EN_DECODE_ERROR_TYPE;
	}
	else
	{
		CMcpHandler* pMcpHandler = this->GetMcpHandler();
		CQueryTransactionSession* pSession = DSC_THREAD_TYPE_NEW(CQueryTransactionSession) CQueryTransactionSession(*this);

		req.m_nCltSessionID = m_nSessionID;
		req.m_nChannelID = nChannelID;

		nReturnCode = this->SendSingleEndoreMessage(req, pSession);

		if (nReturnCode == VBH::EN_OK_TYPE)
		{
			pSession->m_nSrcSessionID = nRequestID;
			pSession->m_nCltSessionID = m_nSessionID;

			this->SetDscTimer(pSession, EN_SESSION_TIMEOUT_VALUE);
			m_mapQueryTransSession.Insert(m_nSessionID, pSession);
			++m_nSessionID;
		}
		else
		{
			this->OnReleaseSingleMcpHanler(pSession);
		}
	}

	VBH_MESSAGE_LEAVE_TRACE(CVbhAdapterBase, QueryTransactionInfo);

	return nReturnCode;
}

//连接通知到来时：找到hanlder所属的peer，将连接信息加入到peer连接记录中
ACE_INT32 CVbhAdapterBase::OnConnectedNodify(CMcpClientHandler* pMcpClientHandler)
{
	//1. 找到peer-id 及这个peer对应的session
	const ACE_INT32 nPeerID = this->GetPeerID(pMcpClientHandler->GetHandleID());
	ACE_ASSERT(nPeerID != -1);

	CVbhPeerConnectSession* pPeerSession = m_vbhPeerConnSumInfo.FindSession(nPeerID); //找到对应的session

	if (nullptr == pPeerSession)//如果对应的session不存在，则开辟sesssion
	{
		pPeerSession = m_vbhPeerConnSumInfo.AllocSession();
		pPeerSession->m_nPeerID = (ACE_UINT16)nPeerID;
	}

	//2. 将handler加到这个session中
	pPeerSession->AddHandler((CVbhAdapterBaseHandler*)pMcpClientHandler);

	return 0;
}

void CVbhAdapterBase::OnNetworkError(CMcpHandler* pMcpHandler)
{
	//1. 断连后，删除在该连接上的所有session
	CVbhAdapterBaseHandler* pVbhClientServiceHandler = (CVbhAdapterBaseHandler*)pMcpHandler;
	CDscTypeArray<IBaseSession>& arrSession = pVbhClientServiceHandler->m_arrSession;
	const ACE_UINT32 nSize = arrSession.Size();

	for (ACE_UINT32 i = 0; i < nSize; ++i)
	{
		arrSession[i]->OnNetError(pVbhClientServiceHandler);
	}
	arrSession.Clear();

	//2. 找到session对应的peer-id，并找到对应的peer-session
	const ACE_INT32 nPeerID = this->GetPeerID(pMcpHandler->GetHandleID());
	ACE_ASSERT(nPeerID != -1);

	CVbhPeerConnectSession* pPeerSession = m_vbhPeerConnSumInfo.FindSession(nPeerID); //找到对应的session

	if (pPeerSession)
	{
		//3. 从session中移除session //如果这个session中没有handler了，则释放该session
		pPeerSession->RemoveHandler(pVbhClientServiceHandler);
		if (pPeerSession->m_nHandlerNum == 0)
		{
			m_vbhPeerConnSumInfo.FreeSession(pPeerSession);
		}
	}
	else //对应的peer-session不存在，这种情况不可能发生，发生则是代码逻辑异常了
	{
		ACE_ASSERT(false);
	}

	//如果和所有的peer都断连了，通知上层应用 //TODO: 后面再定处理策略
	//if (m_vbhPeerConnSumInfo.m_nPeerSessionNum == 0)
	//{
	//	this->OnAbnormal();
	//}

	CDscHtsClientService::OnNetworkError(pMcpHandler);
}

CMcpClientHandler* CVbhAdapterBase::AllocMcpHandler(ACE_HANDLE handle, const ACE_UINT32 nHandleID,
	PROT_COMM::CDscIpAddr& rRemoteAddr, PROT_COMM::CDscIpAddr& localAddr)
{
	return DSC_THREAD_DYNAMIC_TYPE_NEW(CVbhAdapterBaseHandler) CVbhAdapterBaseHandler(*this, handle, nHandleID, rRemoteAddr, localAddr);
}

ACE_INT32 CVbhAdapterBase::GetPeerID(const ACE_UINT32 nHandleID)
{
	for (auto& peer_it : m_lstVbhPeer)
	{
		for (auto& svr_it : peer_it.m_lstEndorserAddr)
		{
			if (nHandleID == svr_it.m_nHandleID)
			{
				return peer_it.m_nPeerID;
			}
		}
	}

	return -1;
}

CVbhAdapterBase::CVbhAdapterBaseHandler* CVbhAdapterBase::GetMcpHandler(void)
{
	if (m_vbhPeerConnSumInfo.m_nSessionNum)
	{
		if (m_vbhPeerConnSumInfo.m_nIndex >= m_vbhPeerConnSumInfo.m_nSessionNum)
		{
			m_vbhPeerConnSumInfo.m_nIndex = 0;
		}

		CVbhPeerConnectSession& rPeerSession = m_vbhPeerConnSumInfo.m_pArrPeerSession[m_vbhPeerConnSumInfo.m_nIndex];

		++m_vbhPeerConnSumInfo.m_nIndex;
		if (rPeerSession.m_nIndex >= rPeerSession.m_nHandlerNum)
		{
			rPeerSession.m_nIndex = 0;
		}

		return rPeerSession.m_ppVbhClientServiceHandler[rPeerSession.m_nIndex++];
	}

	return NULL;
}

class CBlockchainAddr
{
public:
	CBlockchainAddr()
		: m_IpAddr("ES_IP_ADDR")
		, m_port("ES_PORT")
		, m_peerID("PEER_ID")
	{
	}

public:
	PER_BIND_ATTR(m_IpAddr, m_port, m_peerID);

public:
	CColumnWrapper< CDscString > m_IpAddr;
	CColumnWrapper< ACE_INT32 > m_port;
	CColumnWrapper< ACE_INT32 > m_peerID;
};

class CBcAddrCriterion : public CSelectCriterion
{
public:
	virtual void SetCriterion(CPerSelect& rPerSelect)
	{
		rPerSelect.OrderByAsc("PEER_ID");
	}
};

ACE_INT32 CVbhAdapterBase::LoadPeerInfo(void)
{
	CDscDatabase database;
	CDBConnection dbConnection;

	if (CDscDatabaseFactoryDemon::instance()->CreateDatabase(database, dbConnection))
	{
		DSC_RUN_LOG_ERROR("connect database failed.");

		return -1;
	}

	CTableWrapper< CCollectWrapper<CBlockchainAddr> > lstEndorserAddr("ES_ADDR_CFG");
	CBcAddrCriterion criterion;

	if (::PerSelect(lstEndorserAddr, database, dbConnection, &criterion))
	{
		DSC_RUN_LOG_ERROR("select from ES_ADDR_CFG failed");

		return -1;
	}

	CVbhPeer* pVbhPeer = NULL;
	ACE_INT32 nPeerID = std::numeric_limits<ACE_INT32>::min();

	for (auto it = lstEndorserAddr->begin(); it != lstEndorserAddr->end(); ++it)
	{
		if (nPeerID != *it->m_peerID)
		{
			CVbhPeer vbhPeer;

			m_lstVbhPeer.push_back(vbhPeer);
			pVbhPeer = &m_lstVbhPeer.back();

			nPeerID = *it->m_peerID;
			pVbhPeer->m_nPeerID = nPeerID;
		}

		CEndorserAddr endorserAddr;

		endorserAddr.m_nHandleID = this->AllocHandleID();
		endorserAddr.m_strIpAddr = *it->m_IpAddr;
		endorserAddr.m_nPort = *it->m_port;

		if (pVbhPeer)
		{
			pVbhPeer->m_lstEndorserAddr.push_back(endorserAddr);
		}
		else
		{
			DSC_RUN_LOG_ERROR("invalid PEER_ID value: %d", *it->m_peerID);

			return -1;
		}
	}

	return 0;
}
