#include "common/vbh_comm_error_code.h"

inline void CVbhAdapterBase::CRegistUserSession::OnTimer(void)
{
	m_rClientService.OnTimeOut(this);
}

inline void CVbhAdapterBase::CRegistUserSession::OnNetError(CVbhAdapterBaseHandler* pVbhClientServiceHandler)
{
	m_rClientService.OnNetError(this, pVbhClientServiceHandler);
}

inline void CVbhAdapterBase::CRegistUserWaitNotitySession::OnTimer(void)
{
	m_rClientService.OnTimeOut(this);
}

inline void CVbhAdapterBase::CRegistUserWaitNotitySession::OnNetError(CVbhAdapterBaseHandler* pVbhClientServiceHandler)
{
	m_rClientService.OnNetError(this);
}

inline void CVbhAdapterBase::CCreateInformationSession::OnTimer(void)
{
	m_rClientService.OnTimeOut(this);
}

inline void CVbhAdapterBase::CCreateInformationSession::OnNetError(CVbhAdapterBaseHandler* pVbhClientServiceHandler)
{
	m_rClientService.OnNetError(this, pVbhClientServiceHandler);
}

inline void CVbhAdapterBase::CCreateInformationWaitNotitySession::OnTimer(void)
{
	m_rClientService.OnTimeOut(this);
}

inline void CVbhAdapterBase::CCreateInformationWaitNotitySession::OnNetError(CVbhAdapterBaseHandler* pVbhClientServiceHandler)
{
	m_rClientService.OnNetError(this);
}

inline void CVbhAdapterBase::CQueryUserSession::OnTimer(void)
{
	m_rClientService.OnTimeOut(this);
}

inline void CVbhAdapterBase::CProposeWaitNotifySession::OnTimer(void)
{
	m_rClientService.OnTimeOut(this);
}

inline void CVbhAdapterBase::CProposeWaitNotifySession::OnNetError(CVbhAdapterBaseHandler* pVbhClientServiceHandler)
{
	m_rClientService.OnNetError(this);
}

inline void CVbhAdapterBase::CQueryUserSession::OnNetError(CVbhAdapterBaseHandler* pVbhClientServiceHandler)
{
	m_rClientService.OnNetError(this);
}

inline void CVbhAdapterBase::CProposeSession::OnTimer(void)
{
	m_rClientService.OnTimeOut(this);
}

inline void CVbhAdapterBase::CProposeSession::OnNetError(CVbhAdapterBaseHandler* pVbhClientServiceHandler)
{
	m_rClientService.OnNetError(this, pVbhClientServiceHandler);
}

inline void CVbhAdapterBase::CQueryTransactionSession::OnTimer(void)
{
	m_rClientService.OnTimeOut(this);
}

inline void CVbhAdapterBase::CQueryTransactionSession::OnNetError(CVbhAdapterBaseHandler* pVbhClientServiceHandler)
{
	m_rClientService.OnNetError(this);
}


template <typename SESSION_TYPE>
void CVbhAdapterBase::OnReleaseMultiMcpHandler(SESSION_TYPE* pSession, CVbhAdapterBaseHandler* pIgnoreMcpHandler)
{
	//从所有的背书 handle-session中删除本 -session
	CVbhAdapterBaseHandler* pHandler;
	for (ACE_UINT32 idx = 0; idx < pSession->m_arrHandler.Size(); ++idx)
	{
		pHandler = pSession->m_arrHandler[idx];
		if (pHandler != pIgnoreMcpHandler)
		{
			pHandler->m_arrSession.Erase(pSession);
		}
	}
	DSC_THREAD_TYPE_DELETE(pSession);
}

template <typename SESSION_TYPE>
void CVbhAdapterBase::OnReleaseMultiMcpHandler(SESSION_TYPE* pSession)
{
	//从所有的背书 handle-session中删除本 -session
	CVbhAdapterBaseHandler* pHandler;
	for (ACE_UINT32 idx = 0; idx < pSession->m_arrHandler.Size(); ++idx)
	{
		pHandler = pSession->m_arrHandler[idx];
		pHandler->m_arrSession.Erase(pSession);
	}
	DSC_THREAD_TYPE_DELETE(pSession);
}

template <typename SESSION_TYPE>
void CVbhAdapterBase::RemoveNonSubmitMcpHandler(SESSION_TYPE* pSession, CMcpHandler* pMcpHandler)
{
	if (pSession->m_pSubmitEsHandler != pMcpHandler) //不是提交节点
	{
		CVbhAdapterBaseHandler* pVbhClientServiceHandler = (CVbhAdapterBaseHandler*)pMcpHandler;

		//解除业务session和mcp-handler的勾连关系
		pVbhClientServiceHandler->m_arrSession.Erase(pSession);
		pSession->m_arrHandler.Erase(pVbhClientServiceHandler);
	}
}

template <typename SESSION_TYPE>
void CVbhAdapterBase::OnReleaseSingleMcpHanler(SESSION_TYPE* pSession)
{
	pSession->m_pHandler->m_arrSession.Erase(pSession);
	DSC_THREAD_TYPE_DEALLOCATE(pSession);
}

template <typename REQ_TYPE, typename SESSION_TYPE>
ACE_INT32 CVbhAdapterBase::SendMultiEndorseMessage(REQ_TYPE& req, SESSION_TYPE* pSession)
{
	CVbhAdapterBaseHandler* pMcpHandler;
	const ACE_UINT32 nRand = abs(ACE_OS::rand_r(&m_seed)) % m_nEndorsePeerCount; //TODO:将来背书策略有调整

	for (ACE_UINT32 idx = 0; idx < m_nEndorsePeerCount; ++idx)
	{
		//记录mcp-handler和 业务session 的勾连关系
		pMcpHandler = this->GetMcpHandler();
		pSession->m_arrHandler.Insert(pMcpHandler);
		pMcpHandler->m_arrSession.Insert(pSession);

		if (nRand == idx)
		{
			pSession->m_pSubmitEsHandler = pMcpHandler;
			req.m_bSubmitNode = true;
		}
		else
		{
			req.m_bSubmitNode = false;
		}

		if (DSC_UNLIKELY(this->SendHtsMsg(req, pMcpHandler)))
		{
			DSC_RUN_LOG_ERROR("network error, send-hts-message failed.");

			return VBH::EN_NETWORK_ERROR_TYPE;
		}
	}

	return VBH::EN_OK_TYPE;
}

template <typename REQ_TYPE, typename SESSION_TYPE>
inline ACE_INT32 CVbhAdapterBase::SendSingleEndoreMessage(REQ_TYPE& req, SESSION_TYPE* pSession)
{
	CVbhAdapterBaseHandler* pMcpHandler = this->GetMcpHandler();
	pSession->m_pHandler = pMcpHandler;
	pMcpHandler->m_arrSession.Insert(pSession);

	if (this->SendHtsMsg(req, pMcpHandler))
	{
		DSC_RUN_LOG_ERROR("network error, send-hts-message failed.");

		return VBH::EN_NETWORK_ERROR_TYPE;
	}
	else
	{
		return VBH::EN_OK_TYPE;
	}
}
