#ifndef VBH_API_TASK_H_342789423809732182139785892349782317556823482318875486723478
#define VBH_API_TASK_H_342789423809732182139785892349782317556823482318875486723478

#include "dsc/i_dsc_task.h"

#include "vbh_comm/vbh_comm_id_def.h"
#include "client_sdk/client_api.h"

class CVbhApiTask : public IDscTask
{
public:
	enum
	{
		EN_SERVICE_TYPE = VBH::EN_VBH_API_SERVICE_TYPE,
		EN_SERVICE_ID = 1
	};

public:
	virtual ACE_INT32 PostDscMessage(CDscMsg* pDscMsg) override;
	virtual ACE_INT32 PostDscMessage(CDscAsyncCtrlMsg* pAsyncCtrlMsg)  override;
	virtual ACE_INT32 PostDscMessage(CDscSynchCtrlMsg* pSyncCtrlMsg)  override;

public:
	ACE_INT32 SendVbhApiMessage(CDscMsg* pDscMsg);

	template<typename TYPE>
	ACE_INT32 SendVbhApiMessage(TYPE& t, const ACE_UINT16 nMessageType, const CDscMsg::CDscMsgAddr& rAddress);

	template<typename TYPE>
	ACE_INT32 SendVbhApiMessage(TYPE& t, const CDscMsg::CDscMsgAddr& rAddress);

	template<typename TYPE>
	ACE_INT32 SendVbhApiMessage(TYPE& t);

public:
	void SetCallBack(VBH_SDK::IClientSdkMsgCallback* pCallBack);

private:
	template<typename TYPE>
	CDscMsg* GetDscMsg(TYPE& t, const CDscMsg::CDscMsgAddr& rAddress, const ACE_UINT16 nMessageType);

	//处理浏览器特有的查询应答，已经处理应答了，返回0，//未处理返回-1
	ACE_INT32 PostExplorerQueryRsp(VBH_SDK::CQueryRsp& rsp);

private:
	VBH_SDK::IClientSdkMsgCallback* m_pCallBack = nullptr;
};

#include "client_sdk/vbh_api_task.inl"

#endif
