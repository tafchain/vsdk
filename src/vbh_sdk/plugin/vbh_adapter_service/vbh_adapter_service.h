#ifndef _VBH_ADAPTER_SERVICE_H_520345873689012356
#define _VBH_ADAPTER_SERVICE_H_520345873689012356

#include "vbh_adapter_base/vbh_adapter_base.h"

class CVhbAdapterService : public CVbhAdapterBase
{

public:
	virtual void OnRegistUserResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, const DSC::CDscShortBlob& userKey) override;
	virtual void OnRegistUserResultNotify(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID) override;
	virtual void OnCreateInformationRespondse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, const DSC::CDscShortBlob& infoKey) override;
	virtual void OnCreateInformationResultNotify(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID) override;
	virtual void OnProposeResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, const DSC::CDscShortBlob& transKey) override;
	virtual void OnProposeResultNotify(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID) override;
	virtual void OnQueryUserInfoResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, const DSC::CDscShortBlob& userInfo) override;
	virtual void OnQueryTransactionInfoResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, const DSC::CDscBlob& tranInfo) override;

protected:
	BEGIN_BIND_DSC_MESSAGE
		/* 注册用户 */
		DSC_BIND_MESSAGE(VBH::CRegistUserCltCltReq)
		/* 创建information */
		DSC_BIND_MESSAGE(VBH::CCreateInformationCltCltReq)
		/* 发起提案 */
		DSC_BIND_MESSAGE(VBH::CProposeCltCltReq)
		/* 直接查询 */
		DSC_BIND_MESSAGE(VBH::CQueryUserInfoCltCltReq)
		DSC_BIND_MESSAGE(VBH::CQueryTransInfoCltCltReq)
		END_BIND_DSC_MESSAGE

public:
	/* 注册用户 */
	void OnDscMsg(VBH::CRegistUserCltCltReq& rRegistUserReq, const CDscMsg::CDscMsgAddr& rSrcMsgAddr);
	/* 创建information */
	void OnDscMsg(VBH::CCreateInformationCltCltReq& rCreateInfoReq, const CDscMsg::CDscMsgAddr& rSrcMsgAddr);
	/* 发起提案 */
	void OnDscMsg(VBH::CProposeCltCltReq& rProposeReq, const CDscMsg::CDscMsgAddr& rSrcMsgAddr);
	/* 直接查询 */
	void OnDscMsg(VBH::CQueryUserInfoCltCltReq& rQueryUserReq, const CDscMsg::CDscMsgAddr& rSrcMsgAddr);
	void OnDscMsg(VBH::CQueryTransInfoCltCltReq& rGetBcTransInfoReq, const CDscMsg::CDscMsgAddr& rSrcMsgAddr);


public:
	enum
	{
		EN_SERVICE_TYPE = VBH::EN_VBH_ADAPTER_SERVICE_TYPE
	};


private:
	enum
	{
		EN_HASH_MAP_BITES = 16,
		EN_SESSION_TIMEOUT_VALUE = 60,
		EN_SESSION_WAIT_NOTIFY_TIMEOUT_VALUE = 300
	};

	class CClientAddr 
	{
	public:
		CDscMsg::CDscMsgAddr m_addr;

	public:
		ACE_UINT32 m_nKey = 0;
		CClientAddr* m_pPrev = NULL;
		CClientAddr* m_pNext = NULL;
	};

	void InsertClientAddr(ACE_UINT32 nRequestID, const CDscMsg::CDscMsgAddr& addr);
	CClientAddr* GetClientAddr(ACE_UINT32 nRequestID);
	CClientAddr* PopClientAddr(ACE_UINT32 nRequestID);

	using client_addr_map_type = CBareHashMap<ACE_UINT32, CClientAddr, EN_HASH_MAP_BITES>;

	client_addr_map_type m_mapClientAddr;
};

#endif

