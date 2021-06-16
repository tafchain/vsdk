#include "dsc/dsc_app_mng.h"
#include "dsc/dsc_log.h"
#include "dsc/dsc_msg_allocator.h"
#include "dsc/dsc_comm_func.h"

template<typename TYPE>
ACE_INT32 CVbhApiTask::SendVbhApiMessage(TYPE& t, const ACE_UINT16 nMessageType, const CDscMsg::CDscMsgAddr& rAddress)
{
	CDscMsg* pDscMsg = CVbhApiTask::GetDscMsg(t, rAddress, nMessageType);
	if (DSC_LIKELY(pDscMsg))
	{
		if (DSC_UNLIKELY(SendVbhApiMessage(pDscMsg)))
		{
			DSC_RUN_LOG_WARNING("send dsc message failed, msg type:%d", pDscMsg->GetMsgType());
#ifdef DONNT_USING_MEMORY_POOL
			DSC::FreeDscMsg(pDscMsg);
#else
			CDscMsgAllocatorDemon::instance()->deallocate(pDscMsg);
#endif

			return -1;
		}
		else
		{
			return 0;
		}
	}

	return -1;
}

template<typename TYPE>
inline ACE_INT32 CVbhApiTask::SendVbhApiMessage(TYPE& t, const CDscMsg::CDscMsgAddr& rAddress)
{
	return SendVbhApiMessage(t, TYPE::EN_MSG_ID, rAddress);
}

template<typename TYPE>
inline ACE_INT32 CVbhApiTask::SendVbhApiMessage(TYPE& t)
{
	const CDscMsg::CDscMsgAddr addr(VBH::EN_VBH_CRYPT_CLIENT_SERVICE_TYPE, DSC::EN_INVALID_ID);
	
	return SendVbhApiMessage(t, TYPE::EN_MSG_ID, addr);
}

template<typename TYPE>
CDscMsg* CVbhApiTask::GetDscMsg(TYPE& t, const CDscMsg::CDscMsgAddr& rAddress, const ACE_UINT16 nMessageType)
{
	DSC::CDscNetCodecEncoder codecEncoder;

	t.GetSize(codecEncoder);

#ifdef DONNT_USING_MEMORY_POOL
	CDscMsg* pDscMsg = DSC::NewDscMsg(codecEncoder.GetSize());
#else
	CDscMsg* pDscMsg = CDscMsgAllocatorDemon::instance()->allocate(codecEncoder.GetSize());
#endif
	if (DSC_LIKELY(pDscMsg))
	{
		codecEncoder.SetBuffer(pDscMsg->GetMessageBodyPtr());

		t.Encode(codecEncoder);

		pDscMsg->SetSrcNodeType(CDscAppManager::m_nNodeType);
		pDscMsg->SetSrcNodeID(CDscAppManager::m_nNodeID);
		pDscMsg->SetSrcServiceType(CVbhApiTask::EN_SERVICE_TYPE);
		pDscMsg->SetSrcServiceID(CVbhApiTask::EN_SERVICE_ID);

		pDscMsg->SetDesMsgAddr(rAddress);
		pDscMsg->SetMsgType(nMessageType);
	}

	return pDscMsg;
}

