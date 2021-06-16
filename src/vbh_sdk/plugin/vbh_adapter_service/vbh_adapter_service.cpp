#include "vbh_adapter_service.h"

void CVhbAdapterService::OnRegistUserResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, const DSC::CDscShortBlob& userKey)
{
	auto pAddr = PopClientAddr(nRequestID);

	if (pAddr)
	{
		VBH::CRegistUserCltCltRsp rsp;

		rsp.m_nReturnCode = nReturnCode;
		rsp.m_nSrcSessionID = nRequestID;
		if (nReturnCode == VBH::EN_OK_TYPE)
		{
			rsp.m_userKey = userKey;
		}

		this->SendDscMessage(rsp, pAddr->m_addr);

		DSC_THREAD_TYPE_DELETE(pAddr);
	}
}

void CVhbAdapterService::OnRegistUserResultNotify(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID)
{
	auto pAddr = PopClientAddr(nRequestID);

	if (pAddr)
	{
		VBH::CRegistUserCltCltNotify notify;

		notify.m_nReturnCode = nReturnCode;
		notify.m_nSrcSessionID = nRequestID;

		this->SendDscMessage(notify, pAddr->m_addr);

		DSC_THREAD_TYPE_DELETE(pAddr);
	}
}

void CVhbAdapterService::OnCreateInformationRespondse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, const DSC::CDscShortBlob& infoKey)
{
	auto pAddr = PopClientAddr(nRequestID);

	if (pAddr)
	{
		VBH::CCreateInformationCltCltRsp rsp;

		rsp.m_nReturnCode = nReturnCode;
		rsp.m_nSrcSessionID = nRequestID;
		rsp.m_infoKey = infoKey;

		this->SendDscMessage(rsp, pAddr->m_addr);

		DSC_THREAD_TYPE_DELETE(pAddr);
	}
}

void CVhbAdapterService::OnCreateInformationResultNotify(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID)
{
	auto pAddr = PopClientAddr(nRequestID);

	if (pAddr)
	{
		VBH::CCreateInformationCltCltNotify notify;

		notify.m_nReturnCode = nReturnCode;
		notify.m_nSrcSessionID = nRequestID;

		this->SendDscMessage(notify, pAddr->m_addr);

		DSC_THREAD_TYPE_DELETE(pAddr);
	}
}

void CVhbAdapterService::OnProposeResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, const DSC::CDscShortBlob& transKey)
{
	auto pAddr = PopClientAddr(nRequestID);

	if (pAddr)
	{
		VBH::CProposeCltCltRsp rsp;

		rsp.m_nReturnCode = nReturnCode;
		rsp.m_nSrcSessionID = nRequestID;
		if (VBH::EN_OK_TYPE == nReturnCode)
		{
			rsp.m_alocTransKey = transKey;
		}

		this->SendDscMessage(rsp, pAddr->m_addr);

		DSC_THREAD_TYPE_DELETE(pAddr);
	}
}

void CVhbAdapterService::OnProposeResultNotify(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID)
{
	auto pAddr = PopClientAddr(nRequestID);

	if (pAddr)
	{
		VBH::CProposeCltCltNotify notify;

		notify.m_nReturnCode = nReturnCode;
		notify.m_nSrcSessionID = nRequestID;

		this->SendDscMessage(notify, pAddr->m_addr);

		DSC_THREAD_TYPE_DELETE(pAddr);
	}
}

void CVhbAdapterService::OnQueryUserInfoResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, const DSC::CDscShortBlob& userInfo)
{
	auto pAddr = PopClientAddr(nRequestID);

	if (pAddr)
	{
		VBH::CQueryUserInfoCltCltRsp rsp;

		rsp.m_nReturnCode = nReturnCode;
		rsp.m_nSrcSessionID = nRequestID;
		if (VBH::EN_OK_TYPE == nReturnCode)
		{
			rsp.m_userInfo = userInfo;
		}

		this->SendDscMessage(rsp, pAddr->m_addr);

		DSC_THREAD_TYPE_DELETE(pAddr);
	}
}

void CVhbAdapterService::OnQueryTransactionInfoResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, const DSC::CDscBlob& tranInfo)
{
	auto pAddr = PopClientAddr(nRequestID);

	if (pAddr)
	{
		VBH::CQueryTransInfoCltCltRsp rsp;

		rsp.m_nReturnCode = nReturnCode;
		rsp.m_nSrcSessionID = nRequestID;
		if (nReturnCode == VBH::EN_OK_TYPE)
		{
			rsp.m_proposal.Set(tranInfo.GetBuffer(), tranInfo.GetSize());// = tranInfo;
		}

		this->SendDscMessage(rsp, pAddr->m_addr);

		DSC_THREAD_TYPE_DELETE(pAddr);
	}
}

void CVhbAdapterService::OnDscMsg(VBH::CRegistUserCltCltReq& rRegistUserReq, const CDscMsg::CDscMsgAddr& rSrcMsgAddr)
{
	auto nReturnCode = this->RegistUser(rRegistUserReq.m_nSrcSessionID, rRegistUserReq.m_nChannelID, rRegistUserReq.m_bWaitNotify, rRegistUserReq.m_userInfo, rRegistUserReq.m_serverCryptKey);
	
	if (nReturnCode != VBH::EN_OK_TYPE)
	{
		VBH::CRegistUserCltCltRsp rsp;

		rsp.m_nReturnCode = nReturnCode;
		rsp.m_nSrcSessionID = rRegistUserReq.m_nSrcSessionID; // 上层业务service发起请求时，发送方的sessionID
		//失败场景下，rsp其他属性不必要填写；
		this->SendDscMessage(rsp, rSrcMsgAddr);
	}
	else
	{
		InsertClientAddr(rRegistUserReq.m_nSrcSessionID, rSrcMsgAddr);
	}
}

void CVhbAdapterService::OnDscMsg(VBH::CCreateInformationCltCltReq& rCreateInfoReq, const CDscMsg::CDscMsgAddr& rSrcMsgAddr)
{
	auto nReturnCode = this->CreateInformation(rCreateInfoReq.m_nSrcSessionID, rCreateInfoReq.m_nChannelID, rCreateInfoReq.m_bWaitNotify, rCreateInfoReq.m_userCryptKey, rCreateInfoReq.m_proposal);

	if (nReturnCode != VBH::EN_OK_TYPE)
	{
		VBH::CCreateInformationCltCltRsp rsp;

		rsp.m_nReturnCode = nReturnCode;
		rsp.m_nSrcSessionID = rCreateInfoReq.m_nSrcSessionID; // 上层业务service发起请求时，发送方的sessionID
		//失败场景下，rsp其他属性不必要填写；
		this->SendDscMessage(rsp, rSrcMsgAddr);
	}
	else
	{
		InsertClientAddr(rCreateInfoReq.m_nSrcSessionID, rSrcMsgAddr);
	}
}

void CVhbAdapterService::OnDscMsg(VBH::CProposeCltCltReq& rProposeReq, const CDscMsg::CDscMsgAddr& rSrcMsgAddr)
{
	auto nReturnCode = this->Propose(rProposeReq.m_nSrcSessionID, rProposeReq.m_nChannelID, rProposeReq.m_bWaitNotify, rProposeReq.m_nActionID, rProposeReq.m_userKey, rProposeReq.m_proposal);

	if (DSC_UNLIKELY(nReturnCode))
	{
		VBH::CProposeCltCltRsp rsp;

		rsp.m_nReturnCode = nReturnCode;
		rsp.m_nSrcSessionID = rProposeReq.m_nSrcSessionID;

		this->SendDscMessage(rsp, rSrcMsgAddr);
	}
	else
	{
		InsertClientAddr(rProposeReq.m_nSrcSessionID, rSrcMsgAddr);
	}
}

void CVhbAdapterService::OnDscMsg(VBH::CQueryUserInfoCltCltReq& rQueryUserReq, const CDscMsg::CDscMsgAddr& rSrcMsgAddr)
{
	auto nReturnCode = this->QueryUserInfo(rQueryUserReq.m_nSrcSessionID, rQueryUserReq.m_nChannelID, rQueryUserReq.m_userKey);

	if (DSC_UNLIKELY(nReturnCode))
	{
		VBH::CQueryUserInfoCltCltRsp rsp;

		rsp.m_nReturnCode = nReturnCode;
		rsp.m_nSrcSessionID = rQueryUserReq.m_nSrcSessionID;

		this->SendDscMessage(rsp, rSrcMsgAddr);
	}
	else
	{
		InsertClientAddr(rQueryUserReq.m_nSrcSessionID, rSrcMsgAddr);
	}
}

void CVhbAdapterService::OnDscMsg(VBH::CQueryTransInfoCltCltReq& rGetTransInfoReq, const CDscMsg::CDscMsgAddr& rSrcMsgAddr)
{
	auto nReturnCode = this->QueryTransactionInfo(rGetTransInfoReq.m_nSrcSessionID, rGetTransInfoReq.m_nChannelID, rGetTransInfoReq.m_transKey);
	
	if (nReturnCode != VBH::EN_OK_TYPE)
	{
		VBH::CQueryTransInfoCltCltRsp rsp;

		rsp.m_nReturnCode = nReturnCode;
		rsp.m_nSrcSessionID = rGetTransInfoReq.m_nSrcSessionID;

		this->SendDscMessage(rsp, rSrcMsgAddr);
	}
	else
	{
		InsertClientAddr(rGetTransInfoReq.m_nSrcSessionID, rSrcMsgAddr);
	}
}

void CVhbAdapterService::InsertClientAddr(ACE_UINT32 nRequestID, const CDscMsg::CDscMsgAddr& addr)
{
	if (!m_mapClientAddr.Find(nRequestID))
	{
		auto pAddr = DSC_THREAD_TYPE_NEW(CClientAddr) CClientAddr;

		pAddr->m_addr = addr;
		m_mapClientAddr.Insert(nRequestID, pAddr);
	}
}

CVhbAdapterService::CClientAddr* CVhbAdapterService::GetClientAddr(ACE_UINT32 nRequestID)
{
	return m_mapClientAddr.Find(nRequestID);
}

CVhbAdapterService::CClientAddr* CVhbAdapterService::PopClientAddr(ACE_UINT32 nRequestID)
{
	return m_mapClientAddr.Erase(nRequestID);
}
