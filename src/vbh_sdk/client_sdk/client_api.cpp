#include "dsc/dispatcher/dsc_dispatcher_center.h"

#include "vbh_comm/vbh_encrypt_lib.h"
#include "vbh_comm/vbh_comm_error_code.h"
#include "vbh_comm/vbh_comm_func.h"
#include "vbh_comm/comm_msg_def/vbh_comm_msg_clt_clt_def.h"
#include "vbh_comm/vbh_key_codec.h"
#include "vbh_comm/vbh_encrypt_lib.h"
#include "vbh_sdk_comm/vbh_sdk_comm_if.h"

#include "vbh_comm/cc_comm_def.h"
#include "vbh_comm/cc_explorer_query_msg_def.h"

#include "client_sdk/client_api.h"
#include "client_sdk/vbh_api_task.h"


CVbhApiTask g_vbhApiTask;

int32_t VBH_SDK::CreateCryptKeyPair(std::string& strPubKey, std::string& strPriKey)
{
	CDscString strTmpPubKey;
	CDscString strTmpPriKey;

	if (VBH::CreateHexCryptKeyPair(strTmpPubKey, strTmpPriKey))
	{
		DSC_RUN_LOG_ERROR("CreateBase64CryptKeyPair failed.");

		return -1;
	}

	strPubKey.assign(strTmpPubKey.c_str(), strTmpPubKey.size());
	strPriKey.assign(strTmpPriKey.c_str(), strTmpPriKey.size());

	return 0;
}

VBH_CLIENT_SDK_EXPORT int32_t VBH_SDK::GenerateUserCryptKey(std::string& strUserCryptKey,
	const char* pPriKey, const size_t nPriKeyLen, const char* pVbhCryptKey, const size_t nVbhCryptKeyLen, const uint32_t nChannelID)
{
	CDscString strTmpUserCryptKey;

	if (VBH::GenerateUserCryptKey(strTmpUserCryptKey, pPriKey, nPriKeyLen, pVbhCryptKey, nVbhCryptKeyLen, nChannelID))
	{
		DSC_RUN_LOG_ERROR("GenerateBase64Uuid failed.");
		return -1;
	}

	strUserCryptKey.assign(strTmpUserCryptKey.c_str(), strTmpUserCryptKey.size());

	return 0;
}

const char* VBH_SDK::GetErrorString(int nErrorCode)
{
	return VBH::GetErrorString(nErrorCode);
}

int32_t VBH_SDK::InitClientSdk(const int16_t nAppID, IClientSdkMsgCallback* pCallBack)
{
	if (VBH_SDK::InitVbhSdk(VBH::EN_VBH_SDK_APP_TYPE, nAppID))
	{
		return -1;
	}
	
	g_vbhApiTask.SetType(CVbhApiTask::EN_SERVICE_TYPE);
	g_vbhApiTask.SetID(CVbhApiTask::EN_SERVICE_ID);
	g_vbhApiTask.SetCallBack(pCallBack);
	CDscDispatcherCenterDemon::instance()->RegistDscTask(&g_vbhApiTask, CVbhApiTask::EN_SERVICE_TYPE, CVbhApiTask::EN_SERVICE_ID, false);

	return 0;
}

int32_t VBH_SDK::RegistUser(CRegistUserReq& rRegistUserReq, TEST_CC::CCcCheckInfoOnUserRegist info)
{
	InitThreadAllocator();
	if (rRegistUserReq.m_pCltPubKey && rRegistUserReq.m_nCltPubKeyLen)
	{
		VBH::CRegistUserCltCltReq req;
		size_t nUserInfoLen = 0;
		DSC::Encode(info, rRegistUserReq.m_pUserInfo, nUserInfoLen);

		req.m_nChannelID = rRegistUserReq.m_nChannelID;
		req.m_nSessionID = rRegistUserReq.m_nSessionID;
		req.m_userInfo.Set(rRegistUserReq.m_pUserInfo, nUserInfoLen);
		req.m_cltPubKey.Set(rRegistUserReq.m_pCltPubKey, rRegistUserReq.m_nCltPubKeyLen);

		return g_vbhApiTask.SendVbhApiMessage(req);

		DSC_THREAD_FREE(rRegistUserReq.m_pUserInfo);
	}
	else
	{
		return -1;
	}
}

int32_t VBH_SDK::Propose(CProposeReq& rProposeReq)
{
	InitThreadAllocator();
	if (rProposeReq.m_pProposal && rProposeReq.m_nProposalLen
		&& rProposeReq.m_pUserCryptKey && rProposeReq.m_nUserCryptKeyLen)
	{
		VBH::CProposeCltCltReq req;

		req.m_nActionID = rProposeReq.m_nActionID;
		req.m_nSrcSessionID = rProposeReq.m_nSessionID;
		req.m_proposal.Set((char*)rProposeReq.m_pProposal, rProposeReq.m_nProposalLen);
		req.m_userCryptKey.Set((char*)rProposeReq.m_pUserCryptKey, rProposeReq.m_nUserCryptKeyLen);

		return g_vbhApiTask.SendVbhApiMessage(req);
	}
	else
	{
		return -1;
	}
}

int32_t VBH_SDK::ProposeTrade(CProposeReq& rProposeReq, TEST_CC::CTradeAction action)
{
	InitThreadAllocator();
	if (rProposeReq.m_pProposal && rProposeReq.m_nProposalLen
		&& rProposeReq.m_pUserCryptKey && rProposeReq.m_nUserCryptKeyLen)
	{
		VBH::CProposeCltCltReq req;
		size_t nUserInfoLen = 0;
		DSC::Encode(action, rProposeReq.m_pProposal, nUserInfoLen);

		req.m_nActionID = rProposeReq.m_nActionID;
		req.m_nSrcSessionID = rProposeReq.m_nSessionID;
		req.m_proposal.Set((char*)rProposeReq.m_pProposal, nUserInfoLen);
		req.m_userCryptKey.Set((char*)rProposeReq.m_pUserCryptKey, rProposeReq.m_nUserCryptKeyLen);

		return g_vbhApiTask.SendVbhApiMessage(req);
		DSC_THREAD_FREE(rProposeReq.m_pProposal);
	}
	else
	{
		return -1;
	}
}

int32_t VBH_SDK::ProposeCreatInfoID(CProposeReq& rProposeReq, char* pInitInformation, const uint32_t nInitInfoLen)
{
	InitThreadAllocator();
	if (rProposeReq.m_pProposal && rProposeReq.m_nProposalLen
		&& rProposeReq.m_pUserCryptKey && rProposeReq.m_nUserCryptKeyLen)
	{
		VBH::CProposeCltCltReq req;
		size_t nUserInfoLen = 0;

		TEST_CC::CCreateInformationAction action;

		DSC::CDscShortBlob initInfo;

		initInfo.Set(pInitInformation, nInitInfoLen);
		action.m_lstInitInfo.push_back(initInfo);
		DSC::Encode(action, rProposeReq.m_pProposal, nUserInfoLen);

		req.m_nActionID = rProposeReq.m_nActionID;
		req.m_nSrcSessionID = rProposeReq.m_nSessionID;
		req.m_proposal.Set((char*)rProposeReq.m_pProposal, nUserInfoLen);
		req.m_userCryptKey.Set((char*)rProposeReq.m_pUserCryptKey, rProposeReq.m_nUserCryptKeyLen);

		return g_vbhApiTask.SendVbhApiMessage(req);
		DSC_THREAD_FREE(rProposeReq.m_pProposal);
	}
	else
	{
		return -1;
	}
}


int32_t VBH_SDK::ProposeCommitInfo(CProposeReq& rProposeReq, TEST_CC::CCommitInformationAction action)
{
	InitThreadAllocator();
	if (rProposeReq.m_pProposal && rProposeReq.m_nProposalLen
		&& rProposeReq.m_pUserCryptKey && rProposeReq.m_nUserCryptKeyLen)
	{
		VBH::CProposeCltCltReq req;
		size_t nUserInfoLen = 0;
		DSC::Encode(action, rProposeReq.m_pProposal, nUserInfoLen);

		req.m_nActionID = rProposeReq.m_nActionID;
		req.m_nSrcSessionID = rProposeReq.m_nSessionID;
		req.m_proposal.Set((char*)rProposeReq.m_pProposal, nUserInfoLen);
		req.m_userCryptKey.Set((char*)rProposeReq.m_pUserCryptKey, rProposeReq.m_nUserCryptKeyLen);

		return g_vbhApiTask.SendVbhApiMessage(req);
		DSC_THREAD_FREE(rProposeReq.m_pProposal);
	}
	else
	{
		return -1;
	}
}


int32_t VBH_SDK::Query(CQueryReq& rQueryReq)
{
	InitThreadAllocator();
	if (rQueryReq.m_pUserCryptKey && rQueryReq.m_nUserCryptKeyLen)
	{
		VBH::CQueryCltCltReq req;

		req.m_nDstPeerID = rQueryReq.m_nDstPeerID;
		req.m_nSrcSessionID = rQueryReq.m_nSrcSessionID;
		req.m_nActionID = rQueryReq.m_nActionID;
		req.m_param.Set(rQueryReq.m_pParam, rQueryReq.m_nParamLen);
		req.m_userCryptKey.Set(rQueryReq.m_pUserCryptKey, rQueryReq.m_nUserCryptKeyLen);

		return g_vbhApiTask.SendVbhApiMessage(req);
	}
	else
	{
		return -1;
	}

	return 0;
}

int32_t VBH_SDK::QueryUser(CQueryReq& rQueryReq, TEST_CC::CQueryUserAction action)
{
	InitThreadAllocator();
	if (rQueryReq.m_pUserCryptKey && rQueryReq.m_nUserCryptKeyLen)
	{
		VBH::CQueryCltCltReq req;
		size_t nReqParamLen;
		DSC::Encode(action, rQueryReq.m_pParam, nReqParamLen);

		req.m_nDstPeerID = rQueryReq.m_nDstPeerID;
		req.m_nSrcSessionID = rQueryReq.m_nSrcSessionID;
		req.m_nActionID = rQueryReq.m_nActionID;
		req.m_param.Set(rQueryReq.m_pParam, nReqParamLen);
		req.m_userCryptKey.Set(rQueryReq.m_pUserCryptKey, rQueryReq.m_nUserCryptKeyLen);

		return g_vbhApiTask.SendVbhApiMessage(req);
		DSC_THREAD_FREE(rQueryReq.m_pParam);
	}
	else
	{
		return -1;
	}

	return 0;
}

int32_t VBH_SDK::QueryTrans(CQueryReq& rQueryReq, TEST_CC::CQueryTransAction action)
{
	InitThreadAllocator();
	if (rQueryReq.m_pUserCryptKey && rQueryReq.m_nUserCryptKeyLen)
	{
		VBH::CQueryCltCltReq req;
		size_t nReqParamLen;
		DSC::Encode(action, rQueryReq.m_pParam, nReqParamLen);

		req.m_nDstPeerID = rQueryReq.m_nDstPeerID;
		req.m_nSrcSessionID = rQueryReq.m_nSrcSessionID;
		req.m_nActionID = rQueryReq.m_nActionID;
		req.m_param.Set(rQueryReq.m_pParam, nReqParamLen);
		req.m_userCryptKey.Set(rQueryReq.m_pUserCryptKey, rQueryReq.m_nUserCryptKeyLen);

		return g_vbhApiTask.SendVbhApiMessage(req);
		DSC_THREAD_FREE(rQueryReq.m_pParam);
	}
	else
	{
		return -1;
	}

	return 0;
}

int32_t VBH_SDK::ExplorerQueryBlock(const uint32_t nChannelID,
	const uint32_t nRequestID, const uint64_t nBlockID, 
	char* pUserCryptKey, const uint32_t nUserCryptKeyLen, uint16_t nDstPeerID)
{
	InitThreadAllocator();

	if (pUserCryptKey && nUserCryptKeyLen)
	{
		VBH_SDK::CQueryReq req;
		ACE_INT32 nReturnCode = VBH::EN_OK_TYPE;

		EXPLORER_QUERY::CQueryBlockHeaderInfoAction msg;
		size_t nParamLen;

		msg.m_nBlockID = nBlockID;

		DSC::Encode(msg, req.m_pParam, nParamLen);
		req.m_nDstPeerID = nDstPeerID;
		req.m_nParamLen = (uint32_t)nParamLen;
		req.m_nSrcSessionID = nRequestID;
		req.m_nActionID = EXPLORER_QUERY::CQueryBlockHeaderInfoAction::EN_ACTION_ID;
		req.m_pUserCryptKey = pUserCryptKey;
		req.m_nUserCryptKeyLen = nUserCryptKeyLen;

		nReturnCode = Query(req);

		DSC_THREAD_SIZE_FREE(req.m_pParam, nParamLen);

		return nReturnCode;
	}
	else
	{
		return -1;
	}
}

int32_t VBH_SDK::ExplorerQueryBlockTranList(const uint32_t nChannelID,
	const uint32_t nRequestID, const uint64_t nBlockID, uint16_t nPageSize , uint16_t nPageIndex ,
	char* pUserCryptKey, const uint32_t nUserCryptKeyLen, uint16_t nDstPeerID)
{
	InitThreadAllocator();

	if (pUserCryptKey && nUserCryptKeyLen)
	{
		VBH_SDK::CQueryReq req;
		ACE_INT32 nReturnCode = VBH::EN_OK_TYPE;

		EXPLORER_QUERY::CQueryBlockTransListAction msg;
		size_t nParamLen;

		msg.m_nBlockID = nBlockID;
		msg.m_nPageSize = nPageSize;
		msg.m_nPageIndex = nPageIndex;

		DSC::Encode(msg, req.m_pParam, nParamLen);
		req.m_nDstPeerID = nDstPeerID;
		req.m_nParamLen = (uint32_t)nParamLen;
		req.m_nSrcSessionID = nRequestID;
		req.m_nActionID = EXPLORER_QUERY::CQueryBlockTransListAction::EN_ACTION_ID;
		req.m_pUserCryptKey = pUserCryptKey;
		req.m_nUserCryptKeyLen = nUserCryptKeyLen;

		nReturnCode = Query(req);

		DSC_THREAD_SIZE_FREE(req.m_pParam, nParamLen);

		return nReturnCode;
	}
	else
	{
		return -1;
	}
}

int32_t VBH_SDK::ExplorerQueryBlockCount(const uint32_t nChannelID, const uint32_t nRequestID, 
	char* pUserCryptKey, const uint32_t nUserCryptKeyLen, uint16_t nDstPeerID)
{
	InitThreadAllocator();

	if (pUserCryptKey && nUserCryptKeyLen)
	{
		VBH_SDK::CQueryReq req;

		req.m_nDstPeerID = nDstPeerID;
		req.m_nSrcSessionID = nRequestID;
		req.m_nActionID = EXPLORER_QUERY::CQueryBlockCountAction::EN_ACTION_ID;

		req.m_pUserCryptKey = pUserCryptKey;
		req.m_nUserCryptKeyLen = nUserCryptKeyLen;
		req.m_nParamLen = 0;
		req.m_pParam = NULL;

		return Query(req);
	}
	else
	{
		return -1;
	}
}

int32_t VBH_SDK::ExplorerQueryUser(const uint32_t nChannelID, const uint32_t nRequestID, char* pUserKey, const uint32_t nUserKeyLen, /*用户key*/ char* pUserCryptKey, const uint32_t nUserCryptKeyLen/*用户秘钥*/)
{
	InitThreadAllocator();

	if (pUserKey && nUserKeyLen
		&& pUserCryptKey && nUserCryptKeyLen)
	{
		VBH_SDK::CQueryReq req;
		ACE_INT32 nReturnCode = VBH::EN_OK_TYPE;

		EXPLORER_QUERY::CQueryWriteSetAction action;
		size_t nParamLen;

		action.m_userID.Set(pUserKey, nUserKeyLen);

		DSC::Encode(action, req.m_pParam, nParamLen);
		req.m_nParamLen = (uint32_t)nParamLen;
		req.m_nSrcSessionID = nRequestID;
		req.m_nActionID = EXPLORER_QUERY::CQueryWriteSetAction::EN_ACTION_ID;
		req.m_pUserCryptKey = pUserCryptKey;
		req.m_nUserCryptKeyLen = nUserCryptKeyLen;

		nReturnCode = Query(req);

		DSC_THREAD_SIZE_FREE(req.m_pParam, nParamLen);

		return nReturnCode;
	}
	else
	{
		return -1;
	}
}

int32_t VBH_SDK::ExplorerQueryTrans(const uint32_t nChannelID, const uint32_t nRequestID,
	char* pTransKey, const uint32_t nTransKeyLen, /*交易ID*/
	char* pUserCryptKey, const uint32_t nUserCryptKeyLen,/*用户秘钥*/
	uint16_t nDstPeerID)
{
	InitThreadAllocator();

	if (pTransKey && nTransKeyLen && pUserCryptKey && nUserCryptKeyLen)
	{
		VBH_SDK::CQueryReq req;
		ACE_INT32 nReturnCode = VBH::EN_OK_TYPE;

		EXPLORER_QUERY::CQueryTransInfoAction action;
		size_t nParamLen;

		action.m_transKey.Set(pTransKey, nTransKeyLen);

		DSC::Encode(action, req.m_pParam, nParamLen);
		req.m_nParamLen = (uint32_t)nParamLen;
		req.m_nSrcSessionID = nRequestID;
		req.m_nActionID = EXPLORER_QUERY::CQueryTransInfoAction::EN_ACTION_ID;
		req.m_pUserCryptKey = pUserCryptKey;
		req.m_nUserCryptKeyLen = nUserCryptKeyLen;
		req.m_nDstPeerID = nDstPeerID;

		nReturnCode = Query(req);

		DSC_THREAD_SIZE_FREE(req.m_pParam, nParamLen);

		return nReturnCode;
	}
	return -1;
}


void FormatUser(CDscString& strOut, const ACE_UINT32 nVersion, const CDscString& key, const CDscString& value)
{
	strOut.clear();

	strOut += "key:";
	strOut += key;
	strOut += "\r\n";

	strOut += "value:";
	strOut += value;
	strOut += "\r\n";

	strOut += "version:";
	strOut += nVersion;
}

int32_t VBH_SDK::SDKDecode(const char* pMsg, const size_t nMsgBufLen, const uint32_t nActionID, std::string& strMsg)
{
	//DSC::Decode(TYPE & rCode, const char* pMsg, const size_t nMsgBufLen)
		switch (nActionID)
		{
		case TEST_CC::CQueryUserAction::EN_ACTION_ID:
		{
			TEST_CC::CQueryUserRsp rsp;

			if (DSC::Decode(rsp, pMsg, nMsgBufLen))
			{
				return -1;
			}
			else
			{
				CDscString strTmp;

				//strTmp += "key:";
				//strTmp += rsp.m_key;
				//strTmp += "\r\n";

				//strTmp += "value:";
				//strTmp += rsp.m_value;
				//strTmp += "\r\n";

				//strTmp += "version:";
				//strTmp += rsp.m_nVersion;
				FormatUser(strTmp, rsp.m_nVersion, rsp.m_key, rsp.m_value);

				strMsg.assign(strTmp.c_str(), strTmp.size());
			}
		}
		break;
		case TEST_CC::CQueryTransAction::EN_ACTION_ID:
		{
			TEST_CC::CQueryTransRsp rsp;



			if (DSC::Decode(rsp, pMsg, nMsgBufLen))
			{
				return -1;
			}
			else
			{

				CDscString c1;
				CDscString c2;
				CDscString signature;
				char* pHexBuf;
				size_t hexBufSize;
				pHexBuf = DSC_THREAD_SIZE_MALLOC(rsp.m_signature.size() << 1);
				::memset(pHexBuf, 0, (rsp.m_signature.size() << 1));
				hexBufSize = DSC::ToHex(pHexBuf, rsp.m_signature.c_str(), rsp.m_signature.size());
				signature.assign(pHexBuf, hexBufSize);

				c1 += "trans-key:";
				c1 += rsp.m_transKey;
				c1 += "\r\n";

				c1 += "user:";
				c1 += rsp.m_userKey;
				c1 += "\r\n";

				c1 += "signature:";
				c1 += signature;
				c1 += "\r\n";

				c1 += "propose:";
				c1 += rsp.m_proposal;
				c1 += "\r\n";

				c1 += "-----------------------\r\n";

				for (ACE_UINT16 idx = 0; idx < rsp.m_vecWs.Size(); ++idx)
				{
					FormatUser(c2, rsp.m_vecWs[idx].m_nVersion, rsp.m_vecWs[idx].m_key, rsp.m_vecWs[idx].m_value);

					c1 += c2;
					c1 += "\r\n-----------------------\r\n";
				}
				strMsg.assign(c1.c_str(), c1.size());
			}
		}
		break;
		{
			strMsg.assign(pMsg, nMsgBufLen);
			return -1;
		}
		}

		return 0;
}

