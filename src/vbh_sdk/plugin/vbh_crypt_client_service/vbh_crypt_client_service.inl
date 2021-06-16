#include "vbh_comm/vbh_comm_error_code.h"

inline void CVbhCryptClientService::CRegistUserSession::OnTimer(void)
{
	m_rClientService.OnTimeOut(this);
}

inline void CVbhCryptClientService::CRegistUserSession::OnNetError(CVbhClientServiceHandler* pVbhClientServiceHandler)
{
	m_rClientService.OnNetError(this, pVbhClientServiceHandler);
}


inline void CVbhCryptClientService::CQuerySession::OnTimer(void)
{
	m_rClientService.OnTimeOut(this);
}

inline void CVbhCryptClientService::CQuerySession::OnNetError(CVbhClientServiceHandler* pVbhClientServiceHandler)
{
	m_rClientService.OnNetError(this);
}

inline void CVbhCryptClientService::CProposeSession::OnTimer(void)
{
	m_rClientService.OnTimeOut(this);
}

inline void CVbhCryptClientService::CProposeSession::OnNetError(CVbhClientServiceHandler* pVbhClientServiceHandler)
{
	m_rClientService.OnNetError(this, pVbhClientServiceHandler);
}

template <typename SESSION_TYPE>
inline void CVbhCryptClientService::OnReleaseSingleMcpHanler(SESSION_TYPE* pSession)
{
	pSession->m_pHandler->m_arrSession.Erase(pSession);
	DSC_THREAD_TYPE_DELETE(pSession);
}

template <typename REQ_TYPE>
CVbhCryptClientService::CVbhClientServiceHandler* CVbhCryptClientService::SendMultiEndorseMessage(REQ_TYPE& req, REQ_TYPE& req4SubmitNode)
{
	CVbhClientServiceHandler* pMcpHandler;
	CVbhClientServiceHandler* pSubmitMcpHandler;
	const ACE_UINT32 nRand = abs(ACE_OS::rand_r(&m_nSeed)) % m_nEndorsePeerCount; //TODO:������������е���

	for (ACE_UINT32 idx = 0; idx < m_nEndorsePeerCount; ++idx)
	{
		//�ⲿ���õ�������ʱ���Ѿ���֤�����Ӹ������ڵ��� ����ڵ� �������������� GetMcpHandler һ�����᷵��null
		pMcpHandler = this->GetMcpHandler();
		
		if (pMcpHandler)
		{
			if (nRand == idx)
			{
				//���ͳɹ�ʱ��¼���ύ�ڵ��mcp-handler�����κη���ʧ�ܣ���ɾ�����ύ�ڵ��mcp-handler
				if (this->SendHtsMsg(req4SubmitNode, pMcpHandler))
				{
					DSC_RUN_LOG_INFO("network error, send-hts-message failed.");
					pSubmitMcpHandler = nullptr;

					break;
				}
				else
				{
					pSubmitMcpHandler = pMcpHandler;
				}
			}
			else
			{
				if (this->SendHtsMsg(req, pMcpHandler))
				{
					DSC_RUN_LOG_INFO("network error, send-hts-message failed.");
					pSubmitMcpHandler = nullptr;

					break;
				}
			}
		}
		else
		{
			DSC_RUN_LOG_INFO("network error, send-hts-message failed.");
			pSubmitMcpHandler = nullptr;
		}
	}

	return pSubmitMcpHandler;
}

inline ACE_UINT32 CVbhCryptClientService::AllocSessionID(void)
{
	return ++m_nSessionID ? m_nSessionID : ++m_nSessionID; //���0
}
