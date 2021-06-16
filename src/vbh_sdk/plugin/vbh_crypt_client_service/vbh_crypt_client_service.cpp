#include "dsc/mem_mng/dsc_allocator.h"
#include "dsc/configure/dsc_configure.h"

#include "vbh_comm/vbh_comm_error_code.h"
#include "vbh_comm/vbh_comm_macro_def.h"
#include "vbh_comm/comm_msg_def/vbh_comm_msg_clt_clt_def.h"
#include "vbh_comm/vbh_comm_wrap_msg_def.h"
#include "vbh_comm/vbh_comm_func.h"
#include "vbh_comm/vbh_encrypt_lib.h"
#include "vbh_comm/vbh_key_codec.h"

#include "vbh_crypt_client_service/vbh_crypt_client_service.h"


CVbhCryptClientService::CVbhClientServiceHandler::CVbhClientServiceHandler(CMcpClientService& rService, ACE_HANDLE handle, const ACE_INT32 nHandleID,
	PROT_COMM::CDscIpAddr& rRemoteAddr, PROT_COMM::CDscIpAddr& localAddr)
	: CMcpClientHandler(rService, handle, nHandleID, rRemoteAddr, localAddr)
{
}

void CVbhCryptClientService::CVbhPeerConnectSession::FreeHandlerArray(void)
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

void CVbhCryptClientService::CVbhPeerConnectSession::AddHandler(CVbhClientServiceHandler* pHandler)
{
	if (m_nHandlerNum == m_nArrayLen)//空间不够了，要重新开辟空间
	{
		//先开辟需要的空间
		ACE_UINT32 nArrayLen = m_nArrayLen + 8; //每次增长8个
		CVbhClientServiceHandlerPtr* ppMcpHandler;

		DSC_THREAD_TYPE_ALLOCATE_ARRAY(ppMcpHandler, nArrayLen); //开辟新数组
		if (m_ppVbhClientServiceHandler) // 如果原来有，则移动过来
		{
			ACE_ASSERT(m_nArrayLen);
			ACE_OS::memcpy(ppMcpHandler, m_ppVbhClientServiceHandler, sizeof(CVbhClientServiceHandlerPtr) * m_nHandlerNum);
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

void CVbhCryptClientService::CVbhPeerConnectSession::RemoveHandler(CVbhClientServiceHandler* pHandler)
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

void CVbhCryptClientService::CVbhPeerConnectSession::MoveCopy(CVbhPeerConnectSession& srcSession)
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

void CVbhCryptClientService::CVbhPeerConnectSessionSummaryInfo::FreeSessionArray(void)
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

CVbhCryptClientService::CVbhPeerConnectSession* CVbhCryptClientService::CVbhPeerConnectSessionSummaryInfo::FindSession(const ACE_UINT16 nPeerID)
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

CVbhCryptClientService::CVbhPeerConnectSession* CVbhCryptClientService::CVbhPeerConnectSessionSummaryInfo::AllocSession(void)
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

void CVbhCryptClientService::CVbhPeerConnectSessionSummaryInfo::FreeSession(CVbhPeerConnectSession* pSession)
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

CVbhCryptClientService::IBaseSession::IBaseSession(CVbhCryptClientService& rClientService)
	: m_rClientService(rClientService)
{
}

CVbhCryptClientService::CRegistUserSession::CRegistUserSession(CVbhCryptClientService& rClientService)
	: IBaseSession(rClientService)
{
}

CVbhCryptClientService::CProposeSession::CProposeSession(CVbhCryptClientService& rClientService)
	: IBaseSession(rClientService)
{
}

CVbhCryptClientService::CQuerySession::CQuerySession(CVbhCryptClientService& rClientService)
	: IBaseSession(rClientService)
{
}


ACE_INT32 CVbhCryptClientService::OnInit(void)
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

			this->DoConnect(remoteAddr, nullptr, bcServer.m_nHandleID);
		}
	}

	//读取peer的公钥，用于非对称加密通信
	if (VBH::GetVbhProfileString("PEER_ENVELOPE_KEY", m_peerEnvelopeKey))
	{
		DSC_RUN_LOG_WARNING("cann't read 'PEER_ENVELOPE_KEY' configure item value");
		return -1;
	}
	if (m_peerEnvelopeKey.empty())
	{
		DSC_RUN_LOG_WARNING("'PEER_ENVELOPE_KEY' cann't be empty");
		return -1;
	}

	m_pGroup = VBH::vbhEcGroup();
	if (!m_pGroup)
	{
		DSC_RUN_LOG_ERROR("create ec group failed");
		return-1;
	}

	m_nSeed = ACE_OS::time(0);
	ACE_OS::srand(m_nSeed);

	DSC_RUN_LOG_INFO("vbh crypt client service %d init succeed", this->GetID());

	return 0;
}

ACE_INT32 CVbhCryptClientService::OnExit(void)
{
	CRegistUserSession* pRegistUserSession;
	for (auto it = m_mapRegistUserSession.begin(); it != m_mapRegistUserSession.end();)
	{
		pRegistUserSession = it.second;
		this->CancelDscTimer(pRegistUserSession);
		++it;
		DSC_THREAD_TYPE_DEALLOCATE(pRegistUserSession);
	}


	CProposeSession* pProposeSession;
	for (auto it = m_mapProposeSession.begin(); it != m_mapProposeSession.end();)
	{
		pProposeSession = it.second;
		this->CancelDscTimer(pProposeSession);
		++it;
		DSC_THREAD_TYPE_DEALLOCATE(pProposeSession);
	}

	CQuerySession* pQueryUserSession;
	for (auto it = m_mapQuerySession.begin(); it != m_mapQuerySession.end();)
	{
		pQueryUserSession = it.second;
		this->CancelDscTimer(pQueryUserSession);
		++it;
		DSC_THREAD_TYPE_DEALLOCATE(pQueryUserSession);
	}

	m_vbhPeerConnSumInfo.FreeSessionArray();

	if (m_pGroup)
	{
		EC_GROUP_free(m_pGroup);
		m_pGroup = nullptr;
	}

	return CDscHtsClientService::OnExit();
}

void CVbhCryptClientService::OnResponse(CRegistUserSession* pRegistUserSession, const ACE_INT32 nReturnCode,
	const DSC::CDscShortBlob& vbhCryptKey /*= DSC::CDscShortBlob(nullptr, 0)*/,
	const DSC::CDscShortBlob& vbhUserID /*= DSC::CDscShortBlob(nullptr, 0)*/)
{
	VBH::CRegistUserCltCltRsp rsp;

	rsp.m_nReturnCode = nReturnCode;
	rsp.m_nSrcSessionID = pRegistUserSession->m_nSrcSessionID;
	rsp.m_vbhCryptKey = vbhCryptKey;
	rsp.m_vbhUserID = vbhUserID;

	this->SendDscMessage(rsp, pRegistUserSession->m_srcMsgAddr);
}

void CVbhCryptClientService::OnTimeOut(CRegistUserSession* pRegistUserSession)
{
	DSC_RUN_LOG_INFO("CRegistUserSession timeout, session-id:%d", pRegistUserSession->m_nCltSessionID);

	OnResponse(pRegistUserSession, VBH::EN_TIMEOUT_ERROR_TYPE);
	m_mapRegistUserSession.Erase(pRegistUserSession);
	OnReleaseSingleMcpHanler(pRegistUserSession);
}

void CVbhCryptClientService::OnNetError(CRegistUserSession* pRegistUserSession, CVbhClientServiceHandler* pVbhClientServiceHandler)
{
	DSC_RUN_LOG_INFO("CRegistUserSession network error, session-id:%d", pRegistUserSession->m_nCltSessionID);

	this->OnResponse(pRegistUserSession, VBH::EN_NETWORK_ERROR_TYPE);
	this->CancelDscTimer(pRegistUserSession);
	m_mapRegistUserSession.Erase(pRegistUserSession);
	DSC_THREAD_TYPE_DELETE(pRegistUserSession);
}


void CVbhCryptClientService::OnResponse(CProposeSession* pProposeSession, const ACE_INT32 nRetrunCode,
	ACE_UINT32 nActionID, const DSC::CDscShortBlob& infokey,
	const DSC::CDscShortBlob& receipt, const DSC::CDscShortBlob& transKey )
{
	VBH::CProposeCltCltRsp rsp;

	rsp.m_nReturnCode = nRetrunCode;
	rsp.m_nSrcSessionID = pProposeSession->m_nSrcSessionID;
	rsp.m_receipt = receipt;
	rsp.m_transKey = transKey;
	rsp.m_nActionID = nActionID;
	rsp.m_InfoKey = infokey;

	this->SendDscMessage(rsp, pProposeSession->m_srcMsgAddr);
}

void CVbhCryptClientService::OnTimeOut(CProposeSession* pProposeSession)
{
	DSC_RUN_LOG_INFO("CProposeSession timeout, session-id:%d", pProposeSession->m_nCltSessionID);
	this->OnResponse(pProposeSession, VBH::EN_TIMEOUT_ERROR_TYPE,0xffff);
	m_mapProposeSession.Erase(pProposeSession);
	this->OnReleaseSingleMcpHanler(pProposeSession);
}

void CVbhCryptClientService::OnNetError(CProposeSession* pProposeSession, CVbhClientServiceHandler* pVbhClientServiceHandler)
{
	DSC_RUN_LOG_INFO("CProposeSession network error, session-id:%d", pProposeSession->m_nCltSessionID);

	this->CancelDscTimer(pProposeSession);
	this->OnResponse(pProposeSession, VBH::EN_NETWORK_ERROR_TYPE,0xffff);
	m_mapProposeSession.Erase(pProposeSession);
	DSC_THREAD_TYPE_DELETE(pProposeSession);
}

void CVbhCryptClientService::OnResponse(CQuerySession* pQueryUserSession, const ACE_INT32 nRetrunCode, 
	const ACE_UINT32 nActionID, const DSC::CDscBlob& info /*= DSC::CDscBlob()*/)
{
	VBH::CQueryCltCltRsp rsp;

	rsp.m_nReturnCode = nRetrunCode;
	rsp.m_nSrcSessionID = pQueryUserSession->m_nSrcSessionID;
	rsp.m_nActionID = nActionID;
	rsp.m_info = info;

	this->SendDscMessage(rsp, pQueryUserSession->m_srcMsgAddr);
}

void CVbhCryptClientService::OnTimeOut(CQuerySession* pQueryUserSession)
{
	DSC_RUN_LOG_INFO("CQueryUserSession timeout, session-id:%d", pQueryUserSession->m_nCltSessionID);

	this->CancelDscTimer(pQueryUserSession);
	this->OnResponse(pQueryUserSession, VBH::EN_TIMEOUT_ERROR_TYPE, pQueryUserSession->m_nActionID);
	m_mapQuerySession.Erase(pQueryUserSession);
	this->OnReleaseSingleMcpHanler(pQueryUserSession);
}

void CVbhCryptClientService::OnNetError(CQuerySession* pQueryUserSession)
{
	DSC_RUN_LOG_INFO("CQueryUserSession network error, session-id:%d", pQueryUserSession->m_nCltSessionID);

	this->OnResponse(pQueryUserSession, VBH::EN_NETWORK_ERROR_TYPE, pQueryUserSession->m_nActionID);
	this->CancelDscTimer(pQueryUserSession);
	m_mapQuerySession.Erase(pQueryUserSession);
	DSC_THREAD_TYPE_DELETE(pQueryUserSession);
}



ACE_INT32 CVbhCryptClientService::OnHtsMsg(VBH::CCryptRegistUserEsCltRsp& rRegistUserRsp, CMcpHandler* pMcpHandler)
{
	VBH_MESSAGE_ENTER_TRACE(CVbhCryptClientService, VBH::CCryptRegistUserEsCltRsp);

	CRegistUserSession* pSession = m_mapRegistUserSession.Find(rRegistUserRsp.m_nCltSessionID);

	//1.找session
	if (pSession)
	{
		ACE_INT32 nReturnCode = rRegistUserRsp.m_nReturnCode;

		if (nReturnCode == VBH::EN_OK_TYPE)
		{
			//2.解密、解码
			int nDecryptBufLen;
			char* pDecryptBuf = VBH::vbhDecrypt((unsigned char*)m_peerEnvelopeKey.data(), nDecryptBufLen, rRegistUserRsp.m_userData.GetBuffer(), rRegistUserRsp.m_userData.GetSize());

			if (pDecryptBuf)
			{
				DSC::CDscShortBlob userInitInfo;
				ACE_INT32 nNone;
				VBH::CCryptRegistUserEsCltRspDataWrapper wrapper(userInitInfo, nNone);

				if (DSC::Decode(wrapper, pDecryptBuf, nDecryptBufLen))
				{
					DSC_RUN_LOG_ERROR("decode error.");
					nReturnCode = VBH::EN_DECODE_ERROR_TYPE;
				}
				else
				{
					if (pSession->m_nNonce == nNone)
					{
						//3.背书
						if (pSession->m_strCcGenerateUserInfo.empty()) //初次收到应答
						{
							VBH::Assign(pSession->m_strCcGenerateUserInfo, userInitInfo);
							++pSession->m_nSucceedRspNum;
						}
						else//第N次收到应答 N>1
						{
							if (VBH::IsEqual(pSession->m_strCcGenerateUserInfo, userInitInfo))
							{
								++pSession->m_nSucceedRspNum;
							}
							else
							{
								DSC_RUN_LOG_ERROR("endorse failed.");
								nReturnCode = VBH::EN_SYSTEM_ERROR_TYPE;
							}
						}

						//是选中的endorser节点发送的应答
						if ((nReturnCode == VBH::EN_OK_TYPE) && (pMcpHandler == pSession->m_pHandler))
						{
							pSession->m_nEsSessionID = rRegistUserRsp.m_nEsSessionID; //endorser session-id
						}

						//TODO: 背书策略或许会调整
						if (pSession->m_nSucceedRspNum >= m_nEndorsePeerCount) //背书通过
						{
							//组织回应答(再次提交注册事务)	
							DSC::CDscShortBlob cltPubKey(pSession->m_strCltPubKey.data(), pSession->m_strCltPubKey.size()); // 客户端公钥
							VBH::CCryptSubmitRegistUserTransactionCltEsReqDataWrapper wrapper(cltPubKey, pSession->m_nNonce);
							char* pSubmitMsgBuf;
							size_t nSubmitMsgBufLen;

							DSC::Encode(wrapper, pSubmitMsgBuf, nSubmitMsgBufLen);

							int nEncryptBufLen;
							char* pEncryptBuf = VBH::vbhEncrypt((unsigned char*)m_peerEnvelopeKey.data(), nEncryptBufLen, pSubmitMsgBuf, nSubmitMsgBufLen);
							
							DSC_THREAD_SIZE_FREE(pSubmitMsgBuf, nSubmitMsgBufLen);

							if (DSC_LIKELY(pEncryptBuf))
							{
								//5.向服务端提交
								VBH::CCryptSubmitRegistUserTransactionCltEsReq req;

								req.m_nEsSessionID = pSession->m_nEsSessionID;
								req.m_userData.Set(pEncryptBuf, nEncryptBufLen);

								if (DSC_UNLIKELY(this->SendHtsMsg(req, pSession->m_pHandler)))
								{
									DSC_RUN_LOG_ERROR("send hts msg:CCryptSubmitRegistUserTransactionCltEsReq failed.");
									pSession = nullptr;
									nReturnCode = VBH::EN_NETWORK_ERROR_TYPE;
								}
								else
								{
									this->ResetDscTimer(pSession, EN_SESSION_TIMEOUT_VALUE); //第2阶段的应答，重置timer
								}

								DSC_THREAD_FREE(pEncryptBuf);
							}
							else
							{
								DSC_RUN_LOG_ERROR("encrypt error.");
								nReturnCode = VBH::EN_ENCRYPT_ERROR_TYPE;
							}
						}
						else
						{
							//不用处理
						}
					}
					else
					{
						DSC_RUN_LOG_WARNING("nonce is error.");
						nReturnCode = VBH::EN_NONCE_VERIFY_ERROR_TYPE;
					}
				}

				DSC_THREAD_FREE(pDecryptBuf);
			}
			else
			{
				DSC_RUN_LOG_ERROR("decrypt error.");
				nReturnCode = VBH::EN_DECRYPT_ERROR_TYPE;
			}
		}
		else
		{
			DSC_RUN_LOG_INFO("regist user rsp return failed, error code:%d, error-string:%s", rRegistUserRsp.m_nReturnCode, VBH::GetErrorString(rRegistUserRsp.m_nReturnCode));
		}

		if (nReturnCode != VBH::EN_OK_TYPE)
		{
			//向客户端上层业务应答，//删除session
			if (pSession)
			{
				ACE_ASSERT(m_mapRegistUserSession.Find(rRegistUserRsp.m_nCltSessionID) != nullptr);
				this->OnResponse(pSession, nReturnCode);
				this->CancelDscTimer(pSession);
				m_mapRegistUserSession.Erase(pSession);
				this->OnReleaseSingleMcpHanler(pSession); //删除session
			}
		}
	}
	else
	{
		DSC_RUN_LOG_ERROR("can not find session, session-id:%d", rRegistUserRsp.m_nCltSessionID);
	}

	VBH_MESSAGE_LEAVE_TRACE(CVbhCryptClientService, VBH::CCryptRegistUserEsCltRsp);

	return 0;
}

ACE_INT32 CVbhCryptClientService::OnHtsMsg(VBH::CCryptSubmitRegistUserTransactionEsCltRsp& rSubmitTransRsp, CMcpHandler* pMcpHandler)
{
	VBH_MESSAGE_ENTER_TRACE(CVbhCryptClientService, VBH::CCryptSubmitRegistUserTransactionEsCltRsp);

	CRegistUserSession* pSession = m_mapRegistUserSession.Erase(rSubmitTransRsp.m_nCltSessionID);

	if (pSession)
	{
		ACE_INT32 nReturnCode = rSubmitTransRsp.m_nReturnCode;
		DSC::CDscShortBlob blobVbhCryptKey;
		DSC::CDscShortBlob blobVbhUserID;

		if (nReturnCode == VBH::EN_OK_TYPE)
		{
			//1. 解密数据
			int nDecryptBufLen;
			char* pDecryptBuf = VBH::vbhDecrypt((unsigned char*)m_peerEnvelopeKey.data(), nDecryptBufLen,
				rSubmitTransRsp.m_userData.GetBuffer(), rSubmitTransRsp.m_userData.GetSize());

			if (DSC_LIKELY(pDecryptBuf))
			{
				DSC::CDscShortBlob svrPubKey;
				VBH::CVbhAllocatedKey userKey;
				VBH::CTransactionUrl transUrl;
				ACE_INT32 nNonce;
				VBH::CCryptSubmitRegistUserTransactionEsCltRspDataWrapper wrapper(svrPubKey, userKey, transUrl, nNonce);

				//2. 解码解密后的数据，得到应答实际内容
				if (DSC::Decode(wrapper, pDecryptBuf, nDecryptBufLen))
				{
					nReturnCode = VBH::EN_DECODE_ERROR_TYPE;
					DSC_RUN_LOG_INFO("decode error.");
				}
				else
				{
					//3. 校验nonce
					if (pSession->m_nNonce == nNonce)
					{
						//为用户生成 用户注册序列号
						CDscString strSvrPubKey(svrPubKey.GetBuffer(), svrPubKey.GetSize());
						VBH::CVbhCryptKeyWrapper vbhCryptKey(strSvrPubKey, userKey, transUrl);
						char* pVbhCryptKey;
						size_t nVbhCryptKeyLen = VBH::HexEncode(pVbhCryptKey, vbhCryptKey);

						blobVbhCryptKey.Set(pVbhCryptKey, nVbhCryptKeyLen);

						char* pVbhUserID;
						size_t nVbhUserIDLen = VBH::HexEncode(pVbhUserID, userKey);

						blobVbhUserID.Set(pVbhUserID, nVbhUserIDLen);
					}
					else
					{
						nReturnCode = VBH::EN_NONCE_VERIFY_ERROR_TYPE;
						DSC_RUN_LOG_INFO("verify nonce failed.");
					}
				}

				DSC_THREAD_FREE(pDecryptBuf);
			}
			else
			{
				nReturnCode = VBH::EN_DECRYPT_ERROR_TYPE;
				DSC_RUN_LOG_INFO("decrypt_error");
			}
		}
		else
		{
			DSC_RUN_LOG_INFO("submit regist transaction rsp return failed, error code:%d, error-string:%s", rSubmitTransRsp.m_nReturnCode, VBH::GetErrorString(rSubmitTransRsp.m_nReturnCode));
		}

		//向客户端上层业务应答
		this->OnResponse(pSession, nReturnCode, blobVbhCryptKey, blobVbhUserID);
		blobVbhCryptKey.FreeBuffer();
		blobVbhUserID.FreeBuffer();
		//删除session 
		this->CancelDscTimer(pSession);
		this->OnReleaseSingleMcpHanler(pSession);
	}
	else
	{
		DSC_RUN_LOG_WARNING("can not find regist user session, session-id:%d", rSubmitTransRsp.m_nCltSessionID);
	}

	VBH_MESSAGE_LEAVE_TRACE(CVbhCryptClientService, VBH::CCryptSubmitRegistUserTransactionEsCltRsp);

	return 0;
}


ACE_INT32 CVbhCryptClientService::OnHtsMsg(VBH::CCryptProposeEsCltRsp& rProposeRsp, CMcpHandler* pMcpHandler)
{
	VBH_MESSAGE_ENTER_TRACE(CVbhCryptClientService, VBH::CCryptProposeEsCltRsp);

	CProposeSession* pSession = m_mapProposeSession.Find(rProposeRsp.m_nCltSessionID);

	if (pSession)
	{
		ACE_INT32 nReturnCode = rProposeRsp.m_nReturnCode;

		if (DSC_LIKELY(nReturnCode == VBH::EN_OK_TYPE))
		{
			int nDecryptBufLen;
			char* pDecryptBuf = VBH::vbhDecrypt((unsigned char*)pSession->m_strEnvelopeKey.data(), nDecryptBufLen, rProposeRsp.m_userData.GetBuffer(), rProposeRsp.m_userData.GetSize());

			if (DSC_LIKELY(pDecryptBuf))
			{
				DSC::CDscBlob transContent; //事务内容， 参与背书
				ACE_INT32 nNonce;
				VBH::CCryptSubmitProposalEsCltRspDataWrapper wrapper(transContent, nNonce);

				if (DSC_UNLIKELY(DSC::Decode(wrapper, pDecryptBuf, nDecryptBufLen)))
				{
					DSC_RUN_LOG_ERROR("decode error.");
					nReturnCode = VBH::EN_DECODE_ERROR_TYPE;
				}
				else
				{
					if (pSession->m_nNonce == nNonce) //校验随机数
					{
						if (pSession->m_strTransContent.empty())
						{
							VBH::Assign(pSession->m_strTransContent, transContent);
							++pSession->m_nSucceedRspNum;
						}
						else
						{
							if (VBH::IsEqual(pSession->m_strTransContent, transContent))
							{
								++pSession->m_nSucceedRspNum;
							}
							else
							{
								DSC_RUN_LOG_ERROR("endorse failed.");
								nReturnCode = VBH::EN_SYSTEM_ERROR_TYPE;
							}
						}

						//是选中的节点发送的应答
						if (nReturnCode == VBH::EN_OK_TYPE)
						{
							if (pMcpHandler == pSession->m_pHandler)
							{
								pSession->m_nEsSessionID = rProposeRsp.m_nEsSessionID; //记录endorser session-id
							}

							//TODO: 背书策略或许会调整
							if (pSession->m_nSucceedRspNum >= m_nEndorsePeerCount)
							{
								int nEncryptBufLen;
								ACE_INT32 nNonce = pSession->m_nNonce;

								DSC::DscHtonx(nNonce);

								char* pEncryptBuf = VBH::vbhEncrypt((unsigned char*)pSession->m_strEnvelopeKey.data(), nEncryptBufLen,
									(char*)&nNonce, sizeof(nNonce));

								if (DSC_LIKELY(pEncryptBuf))
								{
									VBH::CCryptSubmitProposalTransactionCltEsReq req;
									req.m_nActionID = rProposeRsp.m_nActionID;
									req.m_nEsSessionID = pSession->m_nEsSessionID;
									req.m_userData.Set(pEncryptBuf, nEncryptBufLen);

									if (DSC_UNLIKELY(this->SendHtsMsg(req, pSession->m_pHandler)))
									{
										DSC_RUN_LOG_ERROR("send hts msg:CCryptSubmitProposalTransactionCltEsReq failed.");
										pSession = nullptr;
										nReturnCode = VBH::EN_NETWORK_ERROR_TYPE;
									}
									else
									{
										this->ResetDscTimer(pSession, EN_SESSION_TIMEOUT_VALUE); //第2阶段的应答，重置timer
									}

									DSC_THREAD_FREE(pEncryptBuf);
								}
								else
								{
									DSC_RUN_LOG_ERROR("encrypt error.");
									nReturnCode = VBH::EN_ENCRYPT_ERROR_TYPE;
								}
							}
						}
						else
						{
							//无需处理，再等下一应答消息
						}
					}
					else
					{
						DSC_RUN_LOG_WARNING("nonce is error.");
						nReturnCode = VBH::EN_NONCE_VERIFY_ERROR_TYPE;
					}
				}

				DSC_THREAD_FREE(pDecryptBuf);
			}
			else
			{
				DSC_RUN_LOG_ERROR("decrypt error.");
				nReturnCode = VBH::EN_DECRYPT_ERROR_TYPE;
			}
		}
		else
		{
			DSC_RUN_LOG_INFO("submit proposal rsp return failed, error code:%d, error-string:%s", rProposeRsp.m_nReturnCode, VBH::GetErrorString(rProposeRsp.m_nReturnCode));
		}

		if (nReturnCode != VBH::EN_OK_TYPE)
		{
			// 不是网络错误的情况下， 出错后，向选中的提交节点发送取消事务的消息 //todo: 后面没有notify流程了，也就不需要cancel了。
			//if (nReturnCode != VBH::EN_NETWORK_ERROR_TYPE)
			//{
			//	VBH::CCryptCancelProposalTransactionCltEsReq req;
			//	int nEncryptBufLen;
			//	ACE_INT32 nNonce = pSession->m_nNonce;
			//	DSC::DscHtonx(nNonce);

			//	char* pEncryptBuf = VBH::vbhEncrypt((unsigned char*)pSession->m_strEnvelopeKey.data(), nEncryptBufLen,
			//		(char*)&nNonce, sizeof(nNonce));

			//	if (pEncryptBuf)
			//	{
			//		req.m_nEsSessionID = pSession->m_nEsSessionID;
			//		req.m_userData.Set(pEncryptBuf, nEncryptBufLen);

			//		if (this->SendHtsMsg(req, pSession->m_pHandler))
			//		{
			//			DSC_RUN_LOG_ERROR("send hts message:CCryptCancelProposalTransactionCltEsReq failed.");
			//			pSession = nullptr;
			//			ACE_ASSERT(m_mapProposeSession.Find(rProposeRsp.m_nCltSessionID) == nullptr);
			//		}

			//		DSC_THREAD_FREE(pEncryptBuf);
			//	}
			//	else
			//	{
			//		DSC_RUN_LOG_ERROR("encrypt error.");
			//	}
			//}

			//向客户端上层业务应答，//删除session
			if (pSession)
			{
#ifndef NDEBUG
				char strIPAddr[INET_ADDRSTRLEN];

				DSC::DscGetRemoteAddr(strIPAddr, pMcpHandler->get_handle());
				DSC_RUN_LOG_INFO("process CCryptProposeEsCltRsp error, session-id:%d, return-code:%d, error-string:%s, remote-ip:%s", rProposeRsp.m_nCltSessionID, nReturnCode, VBH::GetErrorString(rProposeRsp.m_nReturnCode), strIPAddr);
#else
				DSC_RUN_LOG_INFO("process CCryptProposeEsCltRsp error, session-id:%d, return-code:%d, error-string:%s", rProposeRsp.m_nCltSessionID, nReturnCode, VBH::GetErrorString(nReturnCode));
#endif

				ACE_ASSERT(m_mapProposeSession.Find(rProposeRsp.m_nCltSessionID) != nullptr);
				this->OnResponse(pSession, VBH::EN_NETWORK_ERROR_TYPE, 0xffff);
				this->CancelDscTimer(pSession);
				m_mapProposeSession.Erase(pSession);
				this->OnReleaseSingleMcpHanler(pSession); //删除session
			}
		}
	}
	else
	{
#ifndef NDEBUG
		char strIPAddr[INET_ADDRSTRLEN];

		DSC::DscGetRemoteAddr(strIPAddr, pMcpHandler->get_handle());
		DSC_RUN_LOG_INFO("can not find session, session-id:%d, return-code:%d, error-string:%s, remote-ip:%s", rProposeRsp.m_nCltSessionID, rProposeRsp.m_nReturnCode, VBH::GetErrorString(rProposeRsp.m_nReturnCode), strIPAddr);
#else
		DSC_RUN_LOG_INFO("can not find session, session-id:%d, return-code:%d, error-string:%s", rProposeRsp.m_nCltSessionID, rProposeRsp.m_nReturnCode, VBH::GetErrorString(rProposeRsp.m_nReturnCode));
#endif
	}

	VBH_MESSAGE_LEAVE_TRACE(CVbhCryptClientService, VBH::CCryptProposeEsCltRsp);

	return 0;
}

ACE_INT32 CVbhCryptClientService::OnHtsMsg(VBH::CCryptSubmitProposalTransactionEsCltRsp& rSubmitTransRsp, CMcpHandler* pMcpHandler)
{
	VBH_MESSAGE_ENTER_TRACE(CVbhCryptClientService, VBH::CCryptSubmitProposalTransactionEsCltRsp);

	CProposeSession* pSession = m_mapProposeSession.Erase(rSubmitTransRsp.m_nCltSessionID);

	if (pSession)
	{
		ACE_INT32 nReturnCode = rSubmitTransRsp.m_nReturnCode;
		DSC::CDscShortBlob blobReceipt;
		DSC::CDscShortBlob blobTransKey;
		DSC::CDscShortBlob blobInfoKey;

		if (nReturnCode == VBH::EN_OK_TYPE)
		{
			//1. 解密数据
			int nDecryptBufLen;
			char* pDecryptBuf = VBH::vbhDecrypt((unsigned char*)pSession->m_strEnvelopeKey.data(), nDecryptBufLen,
				rSubmitTransRsp.m_userData.GetBuffer(), rSubmitTransRsp.m_userData.GetSize());

			if (DSC_LIKELY(pDecryptBuf))
			{
				VBH::CVbhAllocatedTransactionKey alocTransKey;
				ACE_INT32 nNonce;
				VBH::CCryptSubmitProposalTransactionEsCltRspDataWrapper wrapper(alocTransKey, blobReceipt, nNonce);

				//2. 解码解密后的数据，得到应答实际内容
				if (DSC::Decode(wrapper, pDecryptBuf, nDecryptBufLen))
				{
					nReturnCode = VBH::EN_DECODE_ERROR_TYPE;
					DSC_RUN_LOG_ERROR("decode error.");
				}
				else
				{
					//3. 校验nonce
					if (pSession->m_nNonce == nNonce)
					{
						//为用户生成key
						char* pTransKey;
						size_t nTransKeyLen = VBH::HexEncode(pTransKey, alocTransKey);

						blobTransKey.Set(pTransKey, nTransKeyLen);
						if (1 == rSubmitTransRsp.m_nActionID)
						{
							char* pInfoKey;
							//m_vecInfoID接口预留批量接口，但是当前对外支持一个
							size_t nInfoLen = VBH::HexEncode(pInfoKey, rSubmitTransRsp.m_vecInfoID[0]);
							blobInfoKey.Set(pInfoKey, nInfoLen);
						}
					}
					else
					{
						nReturnCode = VBH::EN_NONCE_VERIFY_ERROR_TYPE;
						DSC_RUN_LOG_ERROR("verify nonce failed.");
					}
				}

				DSC_THREAD_FREE(pDecryptBuf);
			}
			else
			{
				nReturnCode = VBH::EN_DECRYPT_ERROR_TYPE;
				DSC_RUN_LOG_ERROR("decrypt_error");
			}
		}
		else
		{
			DSC_RUN_LOG_ERROR("submit proposal transaction rsp return failed, error code:%d, , error-string:%s", rSubmitTransRsp.m_nReturnCode, VBH::GetErrorString(rSubmitTransRsp.m_nReturnCode));
		}

		//向客户端上层业务应答
		this->OnResponse(pSession, nReturnCode,rSubmitTransRsp.m_nActionID,
			blobInfoKey,blobReceipt, blobTransKey);
		blobTransKey.FreeBuffer();
		//删除session 
		this->CancelDscTimer(pSession);
		this->OnReleaseSingleMcpHanler(pSession);
	}
	else
	{
		DSC_RUN_LOG_WARNING("can not find propose session, session-id:%d", rSubmitTransRsp.m_nCltSessionID);
	}
	VBH_MESSAGE_LEAVE_TRACE(CVbhCryptClientService, VBH::CCryptSubmitProposalTransactionEsCltRsp);
	return 0;
}

ACE_INT32 CVbhCryptClientService::OnHtsMsg(VBH::CCryptQueryEsCltRsp& rQueryUserRsp, CMcpHandler* pMcpHandler)
{
	VBH_MESSAGE_ENTER_TRACE(CVbhCryptClientService, VBH::CCryptQueryEsCltRsp);

	CQuerySession* pSession = m_mapQuerySession.Erase(rQueryUserRsp.m_nCltSessionID);

	if (DSC_LIKELY(pSession))
	{
		ACE_INT32 nReturnCode = rQueryUserRsp.m_nReturnCode;
		char* pDecryptBuf;
		DSC::CDscBlob blobInfo;

		if (nReturnCode == VBH::EN_OK_TYPE)
		{
			int nDecryptBufLen;

			pDecryptBuf = VBH::vbhDecrypt((unsigned char*)pSession->m_strEnvelopeKey.data(), nDecryptBufLen, rQueryUserRsp.m_userData.GetBuffer(), rQueryUserRsp.m_userData.GetSize());
			if (DSC_LIKELY(pDecryptBuf))
			{
				ACE_INT32 nNonce;
				VBH::CCryptQueryEsCltRspDataWrapper wrapper(blobInfo, nNonce);

				if (DSC::Decode(wrapper, pDecryptBuf, nDecryptBufLen))
				{
					DSC_RUN_LOG_ERROR("decode error.");
					nReturnCode = VBH::EN_DECRYPT_ERROR_TYPE;
				}
				else
				{
					if (pSession->m_nNonce != nNonce)
					{
						DSC_RUN_LOG_ERROR("verify nonce failed.");
						nReturnCode = VBH::EN_NONCE_VERIFY_ERROR_TYPE;
					}
				}
			}
			else
			{
				DSC_RUN_LOG_ERROR("decrypt error.");
				nReturnCode = VBH::EN_DECRYPT_ERROR_TYPE;
			}
		}
		else
		{
			DSC_RUN_LOG_ERROR("query rsp return failed, error code:%d", rQueryUserRsp.m_nReturnCode);
			pDecryptBuf = nullptr;
		}

		this->OnResponse(pSession, nReturnCode, pSession->m_nActionID, blobInfo);
		this->CancelDscTimer(pSession);
		this->OnReleaseSingleMcpHanler(pSession);

		if (pDecryptBuf)
		{
			DSC_THREAD_FREE(pDecryptBuf);
		}
	}
	else
	{
		DSC_RUN_LOG_ERROR("can not find query user session.");
	}
	VBH_MESSAGE_LEAVE_TRACE(CVbhCryptClientService, VBH::CCryptQueryEsCltRsp);
	return 0;
}

void CVbhCryptClientService::OnDscMsg(VBH::CRegistUserCltCltReq& rRegistUserReq, const CDscMsg::CDscMsgAddr& rSrcMsgAddr)
{
	VBH_MESSAGE_ENTER_TRACE(CVbhCryptClientService, VBH::CRegistUserCltCltReq);

	ACE_INT32 nReturnCode = VBH::EN_OK_TYPE;

	if (m_vbhPeerConnSumInfo.m_nSessionNum >= m_nEndorsePeerCount)//当前连接上服务端个数必须大于背书个数。
	{
		char* pBinPubKey;
		size_t nBinPubKeyLen;

		if (!VBH::HexDecode(pBinPubKey, nBinPubKeyLen, rRegistUserReq.m_cltPubKey.GetBuffer(), rRegistUserReq.m_cltPubKey.GetSize()))
		{
			CRegistUserSession* pSession = DSC_THREAD_TYPE_NEW(CRegistUserSession) CRegistUserSession(*this);

			pSession->m_nNonce = ACE_OS::rand_r(&m_nSeed);
			pSession->m_strCltPubKey.assign(pBinPubKey, nBinPubKeyLen);
			DSC_THREAD_SIZE_FREE(pBinPubKey, nBinPubKeyLen);

			bool bSubmitNode; //标记是否是被选中的
			VBH::CCryptRegistUserCltEsReqDataWrapper wrapper(bSubmitNode, rRegistUserReq.m_nChannelID, rRegistUserReq.m_userInfo, pSession->m_nNonce);
			char* pTrueMsgBuf;
			size_t nTrueMsgBufLen; //标记被选中的消息 编码后缓冲区
			char* pFalseMsgBuf;
			size_t nFalseMsgBufLen; //标记未被选中的消息 编码后缓冲区

			bSubmitNode = true;
			DSC::Encode(wrapper, pTrueMsgBuf, nTrueMsgBufLen);
			bSubmitNode = false;
			DSC::Encode(wrapper, pFalseMsgBuf, nFalseMsgBufLen);
			//如果Encode失败，则表明系统已经出了严重的问题，正常情况下是不会的；故对Encode执行结果不做判断

			char* pEncryptTrueMsgBuf;
			int nEncryptTrueMsgBufLen;
			char* pEncryptFalseMsgBuf;
			int nEncryptFalseMsgBufLen;

			//TODO: 将来肯定要调整，暂时先用从数据库中读取到的对称秘钥对数据进行加密
			pEncryptTrueMsgBuf = VBH::vbhEncrypt((unsigned char*)m_peerEnvelopeKey.data(), nEncryptTrueMsgBufLen, pTrueMsgBuf, nTrueMsgBufLen);
			pEncryptFalseMsgBuf = VBH::vbhEncrypt((unsigned char*)m_peerEnvelopeKey.data(), nEncryptFalseMsgBufLen, pFalseMsgBuf, nFalseMsgBufLen);

			DSC_THREAD_SIZE_FREE(pTrueMsgBuf, nTrueMsgBufLen);
			DSC_THREAD_SIZE_FREE(pFalseMsgBuf, nFalseMsgBufLen); //释放Encode时开辟的资源

			if (DSC_LIKELY(pEncryptTrueMsgBuf && pEncryptFalseMsgBuf))
			{
				VBH::CCryptRegistUserCltEsReq req; //发送到非选中节点的请
				VBH::CCryptRegistUserCltEsReq req4SubmitNode; //发送到选中节点的请求
				const ACE_UINT32 nCltSessionID = AllocSessionID();

				req.m_nCltSessionID = nCltSessionID;
				req.m_userData.Set(pEncryptFalseMsgBuf, nEncryptFalseMsgBufLen);

				req4SubmitNode.m_nCltSessionID = nCltSessionID;
				req4SubmitNode.m_userData.Set(pEncryptTrueMsgBuf, nEncryptTrueMsgBufLen);

				CVbhClientServiceHandler* pSubmitMcpHandler = this->SendMultiEndorseMessage(req, req4SubmitNode);

				if (DSC_LIKELY(pSubmitMcpHandler))
				{
					pSubmitMcpHandler->m_arrSession.Insert(pSession);
					pSession->m_pHandler = pSubmitMcpHandler;

					pSession->m_srcMsgAddr = rSrcMsgAddr;
					pSession->m_nCltSessionID = nCltSessionID;
					pSession->m_nSrcSessionID = rRegistUserReq.m_nSessionID;

					this->SetDscTimer(pSession, EN_SESSION_TIMEOUT_VALUE);
					m_mapRegistUserSession.DirectInsert(nCltSessionID, pSession);
				}
				else //发送失败时，没有建立勾连关系，只删除创建的session
				{
					nReturnCode = VBH::EN_NETWORK_ERROR_TYPE;
					DSC_THREAD_TYPE_DELETE(pSession);
				}
			}
			else
			{
				DSC_RUN_LOG_ERROR("encrypt error.");
				DSC_THREAD_TYPE_DELETE(pSession);

				nReturnCode = VBH::EN_ENCRYPT_ERROR_TYPE;
			}

			if (pEncryptTrueMsgBuf)
			{
				DSC_THREAD_FREE(pEncryptTrueMsgBuf);
			}
			if (pEncryptFalseMsgBuf)
			{
				DSC_THREAD_FREE(pEncryptFalseMsgBuf);
			}
		}
		else
		{
			DSC_RUN_LOG_ERROR("decode user-public-key failed.");
			nReturnCode = VBH::EN_DECODE_ERROR_TYPE;
		}
	}
	else
	{
		DSC_RUN_LOG_INFO("connected peer num:%d cann't less than configed endorse peer count:%d", m_vbhPeerConnSumInfo.m_nSessionNum, m_nEndorsePeerCount);

		nReturnCode = VBH::EN_NETWORK_ERROR_TYPE;
	}

	if (nReturnCode != VBH::EN_OK_TYPE) //失败时，直接回复应答
	{
		VBH::CRegistUserCltCltRsp rsp;

		rsp.m_nReturnCode = nReturnCode;
		rsp.m_nSrcSessionID = rRegistUserReq.m_nSessionID; // 上层业务service发起请求时，发送方的sessionID
		//失败场景下，rsp其他属性不必要填写；
		this->SendDscMessage(rsp, rSrcMsgAddr);
	}

	VBH_MESSAGE_LEAVE_TRACE(CVbhCryptClientService, VBH::CRegistUserCltCltReq);
}

void CVbhCryptClientService::OnDscMsg(VBH::CProposeCltCltReq& rProposeReq, const CDscMsg::CDscMsgAddr& rSrcMsgAddr)
{
	VBH_MESSAGE_ENTER_TRACE(CVbhCryptClientService, VBH::CProposeCltCltReq);

	ACE_INT32 nReturnCode = VBH::EN_OK_TYPE;

	if (m_vbhPeerConnSumInfo.m_nSessionNum < m_nEndorsePeerCount)
	{
		DSC_RUN_LOG_WARNING("connected peer num:%d cann't less than configed endorse peer count:%d", m_vbhPeerConnSumInfo.m_nSessionNum, m_nEndorsePeerCount);
		nReturnCode = VBH::EN_NETWORK_ERROR_TYPE;
	}
	else
	{
		CDscString strCltPriKey;
		CDscString strSvrPubKey;
		CDscString strEnvelopeKey;
		VBH::CVbhAllocatedKey userKey;
		VBH::CTransactionUrl transUrl;
		ACE_UINT32 nChannelID;

		VBH::CUserCryptKeyWrapper userCryptKeyWrapper(strCltPriKey, strSvrPubKey, strEnvelopeKey, userKey, transUrl, nChannelID);

		if (VBH::Base64Decode(userCryptKeyWrapper, rProposeReq.m_userCryptKey.GetBuffer(), rProposeReq.m_userCryptKey.GetSize()))
		{
			DSC_RUN_LOG_ERROR("decode user-crypt-key failed.");
			nReturnCode = VBH::EN_DECODE_ERROR_TYPE;
		}
		else
		{
#ifdef VBH_USE_SIGNATURE
			unsigned int nSignatureLen;
			char* pSignature = VBH::vbhSign(strCltPriKey.data(), strCltPriKey.size(), m_pGroup, rProposeReq.m_proposal.GetBuffer(), rProposeReq.m_proposal.GetSize(), nSignatureLen);

			if (pSignature)
			{
				DSC::CDscShortBlob signature(pSignature, nSignatureLen);
#else
				DSC::CDscShortBlob signature;
#endif

				CProposeSession* pSession = DSC_THREAD_TYPE_NEW(CProposeSession) CProposeSession(*this);
				pSession->m_nNonce = ACE_OS::rand_r(&m_nSeed);

				ACE_UINT8 nSubmitNodeType; //标记是否是被选中的
				VBH::CCryptProposeCltEsReqDataWrapper wrapper(nSubmitNodeType, rProposeReq.m_nActionID, signature, rProposeReq.m_proposal, pSession->m_nNonce);
				char* pTrueMsgBuf;
				size_t nTrueMsgBufLen; //标记被选中的消息 编码后缓冲区
				char* pFalseMsgBuf;
				size_t nFalseMsgBufLen; //标记未被选中的消息 编码后缓冲区

				if (m_nEndorsePeerCount > 1)
				{
					nSubmitNodeType = VBH::EN_IS_SUBMIT_NODE_TYPE;
				}
				else
				{
					nSubmitNodeType = VBH::EN_ONLY_ONE_SUBMIT_NODE_TYPE;
				}
				DSC::Encode(wrapper, pTrueMsgBuf, nTrueMsgBufLen);
				nSubmitNodeType = VBH::EN_NOT_SUBMIT_NODE_TYPE;
				DSC::Encode(wrapper, pFalseMsgBuf, nFalseMsgBufLen);
#ifdef VBH_USE_SIGNATURE
				DSC_THREAD_FREE(pSignature); //释放签名时开辟的内存
#endif
				char* pEncryptTrueMsgBuf;
				int nEncryptTrueMsgBufLen;
				char* pEncryptFalseMsgBuf;
				int nEncryptFalseMsgBufLen;

				//TODO: 将来肯定要调整，暂时先用从数据库中读取到的对称秘钥对数据进行加密
				pEncryptTrueMsgBuf = VBH::vbhEncrypt((unsigned char*)strEnvelopeKey.data(), nEncryptTrueMsgBufLen, pTrueMsgBuf, nTrueMsgBufLen);
				pEncryptFalseMsgBuf = VBH::vbhEncrypt((unsigned char*)strEnvelopeKey.data(), nEncryptFalseMsgBufLen, pFalseMsgBuf, nFalseMsgBufLen);

				DSC_THREAD_SIZE_FREE(pTrueMsgBuf, nTrueMsgBufLen);
				DSC_THREAD_SIZE_FREE(pFalseMsgBuf, nFalseMsgBufLen); //释放Encode时释放的资源

				if (DSC_LIKELY(pEncryptTrueMsgBuf && pEncryptFalseMsgBuf))
				{
					VBH::CCryptProposeCltEsReq req; //发送到非选中节点的请
					VBH::CCryptProposeCltEsReq req4SubmitNode; //发送到选中节点的请求
					const ACE_UINT32 nCltSessionID = AllocSessionID();

					req.m_nActionID = rProposeReq.m_nActionID;
					req.m_nCltSessionID = nCltSessionID;
					req.m_nChannelID = nChannelID;
					req.m_userKey = userKey;
					req.m_transUrl = transUrl;
					req.m_userData.Set(pEncryptFalseMsgBuf, nEncryptFalseMsgBufLen);

					req4SubmitNode.m_nActionID = rProposeReq.m_nActionID;
					req4SubmitNode.m_nCltSessionID = nCltSessionID;
					req4SubmitNode.m_nChannelID = nChannelID;
					req4SubmitNode.m_userKey = userKey;
					req4SubmitNode.m_transUrl = transUrl;
					req4SubmitNode.m_userData.Set(pEncryptTrueMsgBuf, nEncryptTrueMsgBufLen);

					CVbhClientServiceHandler* pSubmitMcpHandler = this->SendMultiEndorseMessage(req, req4SubmitNode);

					if (DSC_LIKELY(pSubmitMcpHandler))
					{
						pSubmitMcpHandler->m_arrSession.Insert(pSession);
						pSession->m_pHandler = pSubmitMcpHandler;

						pSession->m_strEnvelopeKey = strEnvelopeKey;
						pSession->m_srcMsgAddr = rSrcMsgAddr;
						pSession->m_nCltSessionID = nCltSessionID;
						pSession->m_nSrcSessionID = rProposeReq.m_nSrcSessionID;

						this->SetDscTimer(pSession, EN_SESSION_TIMEOUT_VALUE);
						m_mapProposeSession.DirectInsert(nCltSessionID, pSession);
					}
					else //发送失败时，没有建立勾连关系，只删除创建的session
					{
						DSC_RUN_LOG_ERROR("send message:CCryptProposeCltEsReq failed.");
						nReturnCode = VBH::EN_NETWORK_ERROR_TYPE;
						DSC_THREAD_TYPE_DELETE(pSession);
					}
				}
				else
				{
					DSC_RUN_LOG_ERROR("encrypt error.");
					DSC_THREAD_TYPE_DELETE(pSession);

					nReturnCode = VBH::EN_ENCRYPT_ERROR_TYPE;
				}

				if (pEncryptTrueMsgBuf)
				{
					DSC_THREAD_FREE(pEncryptTrueMsgBuf);
				}
				if (pEncryptFalseMsgBuf)
				{
					DSC_THREAD_FREE(pEncryptFalseMsgBuf);
				}
#ifdef VBH_USE_SIGNATURE
			}
			else
			{
				DSC_RUN_LOG_ERROR("sign on proposal failed.");
				nReturnCode = VBH::EN_ENCRYPT_ERROR_TYPE;
			}
#endif
		}
	}

	if (nReturnCode != VBH::EN_OK_TYPE) //失败时，直接回复应答
	{
		VBH::CProposeCltCltRsp rsp;

		rsp.m_nReturnCode = nReturnCode;
		rsp.m_nSrcSessionID = rProposeReq.m_nSrcSessionID;

		this->SendDscMessage(rsp, rSrcMsgAddr);
	}

	VBH_MESSAGE_LEAVE_TRACE(CVbhCryptClientService, VBH::CProposeCltCltReq);
}


void CVbhCryptClientService::OnDscMsg(VBH::CQueryCltCltReq& rQueryReq, const CDscMsg::CDscMsgAddr& rSrcMsgAddr)
{
	VBH_MESSAGE_ENTER_TRACE(CVbhCryptClientService, VBH::CQueryCltCltReq);

	ACE_INT32 nReturnCode = VBH::EN_OK_TYPE;

	CDscString strCltPriKey;
	CDscString strSvrPubKey;
	CDscString strEnvelopeKey;
	VBH::CVbhAllocatedKey userKey;
	VBH::CTransactionUrl transUrl;
	ACE_UINT32 nChannelID;

	VBH::CUserCryptKeyWrapper userCryptKeyWrapper(strCltPriKey, strSvrPubKey, strEnvelopeKey, userKey, transUrl, nChannelID);

	if (!VBH::Base64Decode(userCryptKeyWrapper, rQueryReq.m_userCryptKey.GetBuffer(), rQueryReq.m_userCryptKey.GetSize()))
	{
		VBH::CCryptQueryCltEsReq req;
		int nEncryptMsgBufLen;
		ACE_INT32 nNonce = ACE_OS::rand_r(&m_nSeed);
		VBH::CCryptQueryCltEsReqDataWrapper wrapper(nNonce, rQueryReq.m_nActionID, rQueryReq.m_param);
		char* pMsgBuf;
		size_t nMsgBufLen;

		DSC::Encode(wrapper, pMsgBuf, nMsgBufLen);

		char* pEncryptMsgBuf = VBH::vbhEncrypt((unsigned char*)strEnvelopeKey.data(), nEncryptMsgBufLen, pMsgBuf, nMsgBufLen);

		DSC_THREAD_SIZE_FREE(pMsgBuf, nMsgBufLen);

		if (pEncryptMsgBuf)
		{
			const ACE_UINT32 nCltSessionID = AllocSessionID();

			req.m_nChannelID = nChannelID;
			req.m_nCltSessionID = nCltSessionID;
			req.m_userKey = userKey;
			req.m_transUrl = transUrl;
			req.m_userData.Set(pEncryptMsgBuf, nEncryptMsgBufLen);

			CVbhClientServiceHandler* pMcpHandler = this->GetMcpHandler(rQueryReq.m_nDstPeerID); //找到指定peer的handler

			if (pMcpHandler)
			{
				if (this->SendHtsMsg(req, pMcpHandler))
				{
					DSC_RUN_LOG_INFO("network error, send-hts-message failed.");
				}
				else
				{
					CQuerySession* pSession = DSC_THREAD_TYPE_NEW(CQuerySession) CQuerySession(*this);

					pSession->m_nNonce = nNonce;
					pSession->m_pHandler = pMcpHandler;
					pMcpHandler->m_arrSession.Insert(pSession);

					pSession->m_strEnvelopeKey = strEnvelopeKey;
					pSession->m_srcMsgAddr = rSrcMsgAddr;
					pSession->m_nCltSessionID = nCltSessionID;
					pSession->m_nSrcSessionID = rQueryReq.m_nSrcSessionID;
					pSession->m_nActionID = rQueryReq.m_nActionID;

					this->SetDscTimer(pSession, CVbhCryptClientService::EN_SESSION_TIMEOUT_VALUE);
					m_mapQuerySession.Insert(nCltSessionID, pSession);
				}
			}
			else
			{
				DSC_RUN_LOG_INFO("network error: can not find peer:%d 's connection.", rQueryReq.m_nDstPeerID);
				nReturnCode = VBH::EN_NETWORK_ERROR_TYPE;
			}

			DSC_THREAD_FREE(pEncryptMsgBuf);
		}
		else
		{
			DSC_RUN_LOG_ERROR("encrypt error.");
			nReturnCode = VBH::EN_ENCRYPT_ERROR_TYPE;
		}
	}
	else
	{
		DSC_RUN_LOG_ERROR("decode user-crypt-key failed.");
		nReturnCode = VBH::EN_DECODE_ERROR_TYPE;
	}

	if (nReturnCode != VBH::EN_OK_TYPE) //失败了，直接从请求处发送应答
	{
		VBH::CQueryCltCltRsp rsp;

		rsp.m_nReturnCode = nReturnCode;
		rsp.m_nSrcSessionID = rQueryReq.m_nSrcSessionID;

		this->SendDscMessage(rsp, rSrcMsgAddr);
	}

	VBH_MESSAGE_LEAVE_TRACE(CVbhCryptClientService, VBH::CQueryCltCltReq);
}


//连接通知到来时：找到hanlder所属的peer，将连接信息加入到peer连接记录中
ACE_INT32 CVbhCryptClientService::OnConnectedNodify(CMcpClientHandler* pMcpClientHandler)
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
	pPeerSession->AddHandler((CVbhClientServiceHandler*)pMcpClientHandler);

	return 0;
}

void CVbhCryptClientService::OnNetworkError(CMcpHandler* pMcpHandler)
{
	//1. 断连后，删除在该连接上的所有session
	CVbhClientServiceHandler* pVbhClientServiceHandler = (CVbhClientServiceHandler*)pMcpHandler;
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

	CDscHtsClientService::OnNetworkError(pMcpHandler);
}

CMcpClientHandler* CVbhCryptClientService::AllocMcpHandler(ACE_HANDLE handle, const ACE_UINT32 nHandleID,
	PROT_COMM::CDscIpAddr& rRemoteAddr, PROT_COMM::CDscIpAddr& localAddr)
{
	return DSC_THREAD_DYNAMIC_TYPE_NEW(CVbhClientServiceHandler) CVbhClientServiceHandler(*this, handle, nHandleID, rRemoteAddr, localAddr);
}

ACE_INT32 CVbhCryptClientService::GetPeerID(const ACE_UINT32 nHandleID)
{
	for (DSC::CDscList <CVbhPeer>::iterator peer_it = m_lstVbhPeer.begin(); peer_it != m_lstVbhPeer.end(); ++peer_it)
	{
		CVbhPeer& rPeer = *peer_it;

		for (DSC::CDscList <CEndorserAddr>::iterator svr_it = rPeer.m_lstEndorserAddr.begin(); svr_it != rPeer.m_lstEndorserAddr.end(); ++svr_it)
		{
			if (nHandleID == svr_it->m_nHandleID)
			{
				return rPeer.m_nPeerID;
			}
		}
	}

	return -1;
}

CVbhCryptClientService::CVbhClientServiceHandler* CVbhCryptClientService::GetMcpHandler(void)
{
	if (m_vbhPeerConnSumInfo.m_nSessionNum)
	{
		++m_vbhPeerConnSumInfo.m_nIndex;
		if (m_vbhPeerConnSumInfo.m_nIndex >= m_vbhPeerConnSumInfo.m_nSessionNum)
		{
			m_vbhPeerConnSumInfo.m_nIndex = 0;
		}

		CVbhPeerConnectSession& rPeerSession = m_vbhPeerConnSumInfo.m_pArrPeerSession[m_vbhPeerConnSumInfo.m_nIndex];

		++rPeerSession.m_nIndex;
		if (rPeerSession.m_nIndex >= rPeerSession.m_nHandlerNum)
		{
			rPeerSession.m_nIndex = 0;
		}

		return rPeerSession.m_ppVbhClientServiceHandler[rPeerSession.m_nIndex];
	}

	return nullptr;
}

CVbhCryptClientService::CVbhClientServiceHandler* CVbhCryptClientService::GetMcpHandler(const ACE_UINT16 nPeerID)
{
	CVbhPeerConnectSession* pPeerSession = m_vbhPeerConnSumInfo.FindSession(nPeerID);

	if (pPeerSession)
	{
		++pPeerSession->m_nIndex;
		if (pPeerSession->m_nIndex >= pPeerSession->m_nHandlerNum)
		{
			pPeerSession->m_nIndex = 0;
		}

		return pPeerSession->m_ppVbhClientServiceHandler[pPeerSession->m_nIndex];
	}
	else
	{
		return nullptr;
	}
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

ACE_INT32 CVbhCryptClientService::LoadPeerInfo(void)
{
	CTableWrapper< CCollectWrapper<CBlockchainAddr> > lstEndorserAddr("CRYPT_ES_ADDR_CFG");

	CDscDatabase database;
	CDBConnection dbConnection;
	CBcAddrCriterion criterion;

	if (CDscDatabaseFactoryDemon::instance()->CreateDatabase(database, dbConnection))
	{
		DSC_RUN_LOG_ERROR("connect database failed.");

		return -1;
	}

	if (::PerSelect(lstEndorserAddr, database, dbConnection, &criterion))
	{
		DSC_RUN_LOG_ERROR("select from ES_ADDR_CFG failed");

		return -1;
	}

	CVbhPeer* pVbhPeer = nullptr;
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
