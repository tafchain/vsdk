#include "dsc/dispatcher/dsc_dispatcher_center.h"

#include "vbh_comm/comm_msg_def/vbh_comm_msg_clt_clt_def.h"
#include "vbh_comm/vbh_comm_error_code.h"
#include "vbh_comm/cc_explorer_query_msg_def.h"

#include "client_sdk/vbh_api_task.h"

void CVbhApiTask::SetCallBack(VBH_SDK::IClientSdkMsgCallback* pCallBack)
{
	m_pCallBack = pCallBack;
}

ACE_INT32 CVbhApiTask::SendVbhApiMessage(CDscMsg* pDscMsg)
{
	ACE_ASSERT(pDscMsg);

	CDscMsg::CDscMsgAddr& rDesMsgAddr = pDscMsg->GetDesMsgAddr();

	if ((rDesMsgAddr.GetNodeType() == DSC::EN_INVALID_TYPE) || (rDesMsgAddr.GetServiceType() == DSC::EN_INVALID_TYPE))
	{
		return CDscDispatcherCenterDemon::instance()->SendDscMessageByMsgType(pDscMsg);
	}
	else
	{
		if ((rDesMsgAddr.GetNodeID() == DSC::EN_INVALID_ID) || (rDesMsgAddr.GetServiceID() == DSC::EN_INVALID_ID))
		{
			return CDscDispatcherCenterDemon::instance()->SendDscMessageByTaskType(pDscMsg);
		}
		else
		{
			return CDscDispatcherCenterDemon::instance()->SendDscMessageByAddr(pDscMsg);
		}
	}
}

ACE_INT32 CVbhApiTask::PostDscMessage(CDscMsg* pDscMsg)
{

	ACE_ASSERT(m_pCallBack);

	switch (pDscMsg->GetMsgType())
	{
	case VBH::CQueryCltCltRsp::EN_MSG_ID:
	{
		VBH::CQueryCltCltRsp rsp;

		if (DSC::Decode(rsp, pDscMsg))
		{
			DSC_RUN_LOG_WARNING("Decode CQueryUserInfoCltCltRsp failed, msg type:%d", pDscMsg->GetMsgType());
		}
		else
		{
			VBH_SDK::CQueryRsp queryRsp;

			queryRsp.m_nReturnCode = rsp.m_nReturnCode;
			queryRsp.m_nActionID = rsp.m_nActionID;
			queryRsp.m_nSrcSessionID = rsp.m_nSrcSessionID;
			queryRsp.m_pInfo = rsp.m_info.GetBuffer();
			queryRsp.m_nInfoLen = rsp.m_info.GetSize();


			if (PostExplorerQueryRsp(queryRsp)) //优先让浏览器处理，浏览器处理不了的，调用用户实现处理
			{
				m_pCallBack->OnQueryRsp(queryRsp);
			}
		}
	}
	break;
	case VBH::CProposeCltCltRsp::EN_MSG_ID:
	{
		VBH::CProposeCltCltRsp rsp;

		if (DSC::Decode(rsp, pDscMsg))
		{
			DSC_RUN_LOG_WARNING("Decode CProposeRsp failed, msg type:%d", pDscMsg->GetMsgType());
		}
		else
		{
			VBH_SDK::CProposeRsp proposeRsp;

			proposeRsp.m_nReturnCode = rsp.m_nReturnCode;
			proposeRsp.m_nSrcSessionID = rsp.m_nSrcSessionID;
			proposeRsp.m_pReceipt = rsp.m_receipt.GetBuffer();
			proposeRsp.m_nReceiptLen = rsp.m_receipt.GetSize();
			proposeRsp.m_pTransKey = rsp.m_transKey.GetBuffer();
			proposeRsp.m_nTransKeyLen = rsp.m_transKey.GetSize();
			proposeRsp.m_nActionID = rsp.m_nActionID;
			proposeRsp.m_nInfoIdLen = rsp.m_InfoKey.GetSize();
			proposeRsp.m_pInfoID = rsp.m_InfoKey.GetBuffer();

			m_pCallBack->OnProposeRsp(proposeRsp);
		}
	}
	break;
	case VBH::CRegistUserCltCltRsp::EN_MSG_ID:
	{
		VBH::CRegistUserCltCltRsp rsp;

		if (DSC::Decode(rsp, pDscMsg))
		{
			DSC_RUN_LOG_WARNING("Decode CRegistUserRsp failed, msg type:%d", pDscMsg->GetMsgType());
		}
		else
		{
			VBH_SDK::CRegistUserRsp registUserRsp;

			registUserRsp.m_nReturnCode = rsp.m_nReturnCode;
			registUserRsp.m_nSessionID = rsp.m_nSrcSessionID;
			registUserRsp.m_pVbhCryptKey = rsp.m_vbhCryptKey.GetBuffer();
			registUserRsp.m_nVbhCryptKeyLen = rsp.m_vbhCryptKey.GetSize();
			registUserRsp.m_pVbhUserID = rsp.m_vbhUserID.GetBuffer();
			registUserRsp.m_nVbhUserIDLen = rsp.m_vbhUserID.GetSize();

			m_pCallBack->OnRegistUserRsp(registUserRsp);
		}
	}
	break;
	default:
	{
		DSC_RUN_LOG_INFO("donn't need proc msg, msg type:%d", pDscMsg->GetMsgType());
	}
	break;
	}

#ifdef DONNT_USING_MEMORY_POOL
	DSC::FreeDscMsg(pDscMsg);
#else
	CDscMsgAllocatorDemon::instance()->deallocate(pDscMsg);
#endif

	return 0;
}

ACE_INT32 CVbhApiTask::PostDscMessage(CDscAsyncCtrlMsg* pAsyncCtrlMsg)
{

	return 0;
}

ACE_INT32 CVbhApiTask::PostDscMessage(CDscSynchCtrlMsg* pSyncCtrlMsg)
{
	return 0;
}

ACE_INT32 CVbhApiTask::PostExplorerQueryRsp(VBH_SDK::CQueryRsp& rsp)
{
	ACE_INT32 nReturnCode = rsp.m_nReturnCode;

	switch (rsp.m_nActionID)
	{
	case EXPLORER_QUERY::CQueryBlockHeaderInfoAction::EN_ACTION_ID:
	{
		EXPLORER_QUERY::CQueryBlockHeaderInfoRsp msg;
		char* pJsonBlockInfo = nullptr;
		size_t nJsonBlockInfo = 0;

		if (nReturnCode == VBH::EN_OK_TYPE)
		{
			if (DSC::Decode(msg, rsp.m_pInfo, rsp.m_nInfoLen))
			{
				DSC_RUN_LOG_ERROR("decode CQueryBlockInfoRspMsg failed.");
				nReturnCode = VBH::EN_DECODE_ERROR_TYPE;
			}
		}

		if (nReturnCode == VBH::EN_OK_TYPE)
		{
			SJsonWriter* writer = JSON_CODE::Encode(msg);
			if (writer)
			{
				nJsonBlockInfo = writer->m_nCurrentPos;
				pJsonBlockInfo = DSC_THREAD_SIZE_MALLOC(nJsonBlockInfo);

				::memcpy(pJsonBlockInfo, writer->m_pBuffer, writer->m_nCurrentPos);
				json_writer_free(writer);
			}
		}

		VBH_SDK::CExplorerQueryBlockInfoRsp explorerQueryBlockInfoRsp;

		explorerQueryBlockInfoRsp.m_nReturnCode = nReturnCode;
		explorerQueryBlockInfoRsp.m_nSrcSessionID = rsp.m_nSrcSessionID;
		explorerQueryBlockInfoRsp.m_pJsonBlockInfo = pJsonBlockInfo;
		explorerQueryBlockInfoRsp.m_nJsonBlockInfo = (uint32_t)nJsonBlockInfo;

		m_pCallBack->OnExplorerQueryBlockRsp(explorerQueryBlockInfoRsp);

		if (pJsonBlockInfo)
		{
			DSC_THREAD_SIZE_FREE(pJsonBlockInfo, nJsonBlockInfo);
		}
	}
	return 0;
	case EXPLORER_QUERY::CQueryBlockCountAction::EN_ACTION_ID:
	{
		EXPLORER_QUERY::CQueryBlockCountRsp msg;

		if (nReturnCode == VBH::EN_OK_TYPE)
		{
			if (DSC::Decode(msg, rsp.m_pInfo, rsp.m_nInfoLen))
			{
				DSC_RUN_LOG_ERROR("decode CQueryBlockCountRspMsg failed.");
				nReturnCode = VBH::EN_DECODE_ERROR_TYPE;
			}
		}

		VBH_SDK::CExplorerQueryBlockCountRsp explorerQueryBlockCountRsp;

		explorerQueryBlockCountRsp.m_nReturnCode = nReturnCode;
		explorerQueryBlockCountRsp.m_nSrcSessionID = rsp.m_nSrcSessionID;
		explorerQueryBlockCountRsp.m_nBlockCount = msg.blockCount;
		m_pCallBack->OnExplorerQueryBlockCountRsp(explorerQueryBlockCountRsp);
	}
	return 0;
	case EXPLORER_QUERY::CQueryTransInfoAction::EN_ACTION_ID:
	{
		EXPLORER_QUERY::CQueryTransInfoRsp msg;
		char* pJsonTransInfo = nullptr;
		size_t nJsonTransInfo = 0;

		if (nReturnCode == VBH::EN_OK_TYPE)
		{
			if (DSC::Decode(msg, rsp.m_pInfo, rsp.m_nInfoLen))
			{
				DSC_RUN_LOG_ERROR("decode CQueryBlockInfoRspMsg failed.");
				nReturnCode = VBH::EN_DECODE_ERROR_TYPE;
			}
		}

		if (nReturnCode == VBH::EN_OK_TYPE)
		{
			SJsonWriter* writer = JSON_CODE::Encode(msg);
			if (writer)
			{
				nJsonTransInfo = writer->m_nCurrentPos;
				pJsonTransInfo = DSC_THREAD_SIZE_MALLOC(nJsonTransInfo);

				::memcpy(pJsonTransInfo, writer->m_pBuffer, writer->m_nCurrentPos);
				json_writer_free(writer);
			}
		}

		VBH_SDK::CExplorerQueryTransRsp explorerQueryTransInfoRsp;

		explorerQueryTransInfoRsp.m_nReturnCode = nReturnCode;
		explorerQueryTransInfoRsp.m_nSrcSessionID = rsp.m_nSrcSessionID;
		explorerQueryTransInfoRsp.m_pTransInfo = pJsonTransInfo;
		explorerQueryTransInfoRsp.m_nTransInfoLen = (uint32_t)nJsonTransInfo;

		m_pCallBack->OnExplorerQueryTransRsp(explorerQueryTransInfoRsp);

		if (pJsonTransInfo)
		{
			DSC_THREAD_SIZE_FREE(pJsonTransInfo, nJsonTransInfo);
		}
	}
	return 0;
	case EXPLORER_QUERY::CQueryBlockTransListAction::EN_ACTION_ID:
	{

		EXPLORER_QUERY::CQueryBlockTransListRsp msg;
		char* pJsonTransList = nullptr;
		size_t nJsonBlockTransList = 0;

		if (nReturnCode == VBH::EN_OK_TYPE)
		{
			if (DSC::Decode(msg, rsp.m_pInfo, rsp.m_nInfoLen))
			{
				DSC_RUN_LOG_ERROR("decode CQueryBlockInfoRspMsg failed.");
				nReturnCode = VBH::EN_DECODE_ERROR_TYPE;
			}
		}

		if (nReturnCode == VBH::EN_OK_TYPE)
		{
			SJsonWriter* writer = JSON_CODE::Encode(msg);
			if (writer)
			{
				nJsonBlockTransList = writer->m_nCurrentPos;
				pJsonTransList = DSC_THREAD_SIZE_MALLOC(nJsonBlockTransList);

				::memcpy(pJsonTransList, writer->m_pBuffer, writer->m_nCurrentPos);
				json_writer_free(writer);
			}
		}

		VBH_SDK::CExplorerQueryBlockTransListRsp explorerQueryBlockTransListRsp;

		explorerQueryBlockTransListRsp.m_nReturnCode = nReturnCode;
		explorerQueryBlockTransListRsp.m_nSrcSessionID = rsp.m_nSrcSessionID;
		explorerQueryBlockTransListRsp.m_pJsonBlockTransList = pJsonTransList;
		explorerQueryBlockTransListRsp.m_nJsonBlockTransListLen = (uint32_t)nJsonBlockTransList;

		m_pCallBack->OnExplorerQueryBlockTransListRsp(explorerQueryBlockTransListRsp);

		if (pJsonTransList)
		{
			DSC_THREAD_SIZE_FREE(pJsonTransList, nJsonBlockTransList);
		}

	}
	return 0;
	default: //没有处理， 返回-1
	{
		return -1;
	}
	}
}
