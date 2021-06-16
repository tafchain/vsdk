#include <cstddef>
#include <string.h>

#include "client_sdk/client_api.h"
#include "client_c_sdk/client_c_api.h"
#include "vbh_comm/test_def.h"
//�������ѯ�û���ϢӦ��
typedef void(*on_explorer_query_vbh_user_rsp)(const int32_t nReturnCode, const uint32_t nRequestID,
	const char* pUserInfo, const uint32_t nUserInfoLen /*���ַ�����ʽ���ص��û���Ϣ*/);

//�������ѯ����Ӧ��
typedef void(*on_explorer_query_vbh_trans_rsp)(const int32_t nReturnCode, const uint32_t nRequestID,
	const char* pTransInfo, const uint32_t nTransInfoLen, const char* pAnsiTransInfo, const uint32_t nAnsiTransInfoLen /*���ַ�����ʽ���ص��û���Ϣ*/);

class CWrapperCallback : public VBH_SDK::IClientSdkMsgCallback
{

public:
	virtual void OnRegistUserRsp(VBH_SDK::CRegistUserRsp& rRegistUserRsp) override;
	virtual void OnProposeRsp(VBH_SDK::CProposeRsp& rProposeRsp) override;
	virtual void OnQueryRsp(VBH_SDK::CQueryRsp& rQueryRsp) override;

	//----------------------------------�������ز�ѯ�ӿ�------------------------------
	virtual void OnExplorerQueryBlockRsp(VBH_SDK::CExplorerQueryBlockInfoRsp& rBlockInfoRsp) override;
	virtual void OnExplorerQueryBlockCountRsp(VBH_SDK::CExplorerQueryBlockCountRsp& rBlockCountRsp) override;
	virtual void OnExplorerQueryUserRsp(VBH_SDK::CExplorerQueryUserRsp& rUserInfo) override;
	virtual void OnExplorerQueryTransRsp(VBH_SDK::CExplorerQueryTransRsp& rUserInfo) override;
	virtual void OnExplorerQueryBlockTransListRsp(VBH_SDK::CExplorerQueryBlockTransListRsp& rBlockTransList) override;

public:
	on_regist_vbh_user_rsp m_pOnRegistUserRsp;
	on_vbh_propose_trade_rsp m_pOnProposeTradeRsp;
	on_vbh_propose_creat_info_id_rsp m_pOnProposeCreatInfoIdRsp;
	on_vbh_propose_commit_info_rsp m_pOnProposeCommitInfoRsp;
	on_query_vbh_rsp m_pOnQueryRsp;

	on_explorer_query_vbh_block_rsp m_pOnExplorerQueryVbhBlockRsp;
	on_explorer_query_vbh_block_count_rsp m_pOnExplorerQueryVbhBlockCountRsp;
	on_explorer_query_vbh_user_rsp m_pOnExplorerQueryVbhUserRsp;
	on_explorer_query_vbh_trans_rsp m_pOnExplorerQueryVbhTransRsp;
	on_explorer_query_vbh_block_tran_list_rsp m_pOnExplorerQueryVbhBlockTranListRsp;
};

#define RET_OK 0;
#define RET_FAIL -1;

#ifdef WIN32

char* UTF8toANSI(char* strUTF8, int& nOutBuffLen, int nInBuffLen)
{
	//��ȡת��Ϊ���ֽں���Ҫ�Ļ�������С���������ֽڻ�����
	UINT nLen = MultiByteToWideChar(CP_UTF8, NULL, strUTF8, nInBuffLen, NULL, NULL);
	wchar_t* wszBuffer = new wchar_t[nLen + 1];
	nLen = MultiByteToWideChar(CP_UTF8, NULL, strUTF8, nInBuffLen, wszBuffer, nLen);
	wszBuffer[nLen] = 0;

	nLen = WideCharToMultiByte(936, NULL, wszBuffer, -1, NULL, NULL, NULL, NULL);
	char* szBuffer = new char[nLen + 1];
	nLen = WideCharToMultiByte(936, NULL, wszBuffer, -1, szBuffer, nLen, NULL, NULL);
	szBuffer[nLen] = 0;
	nOutBuffLen = nLen-1;
	delete[]wszBuffer;

	return szBuffer;
}

char* ANSItoUTF8(char* strAnsi,int& nOutBuffLen)
{
	//��ȡת��Ϊ���ֽں���Ҫ�Ļ�������С���������ֽڻ�������936Ϊ��������GB2312����ҳ
	UINT nLen = MultiByteToWideChar(936, NULL, strAnsi, -1, NULL, NULL);
	wchar_t* wszBuffer = new wchar_t[nLen + 1];
	nLen = MultiByteToWideChar(936, NULL, strAnsi, -1, wszBuffer, nLen);
	wszBuffer[nLen] = 0;
	//��ȡתΪUTF8���ֽں���Ҫ�Ļ�������С���������ֽڻ�����
	nLen = WideCharToMultiByte(CP_UTF8, NULL, wszBuffer, -1, NULL, NULL, NULL, NULL);
	char* szBuffer = new char[nLen + 1];
	nLen = WideCharToMultiByte(CP_UTF8, NULL, wszBuffer, -1, szBuffer, nLen, NULL, NULL);
	szBuffer[nLen] = 0;
	nOutBuffLen = nLen-1;
	//�ڴ�����
	delete[]wszBuffer;
	return szBuffer;
}

#endif

int32_t init_vbh_c_sdk(const uint16_t nAppID,
	on_regist_vbh_user_rsp pOnRegistUserRsp,
	on_vbh_propose_trade_rsp pOnProposeTradeRsp,
	on_vbh_propose_creat_info_id_rsp pOnProposeCreatInfoIdRsp,
	on_vbh_propose_commit_info_rsp pOnProposeCommitInfoRsp,
	on_query_vbh_rsp pOnQueryRsp,
	/*�������ز�ѯӦ��ص�����*/
	on_explorer_query_vbh_block_rsp pOnExplorerQueryVbhBlockRsp,
	on_explorer_query_vbh_block_count_rsp pOnExplorerQueryVbhBlockCountRsp,
	on_explorer_query_vbh_block_tran_list_rsp pOnExplorerQueryVbhBlockTranListRsp)
{
	if (pOnRegistUserRsp && pOnProposeTradeRsp
		&& pOnProposeCreatInfoIdRsp
		&& pOnProposeCommitInfoRsp
		&& pOnQueryRsp
		&& pOnExplorerQueryVbhBlockRsp && pOnExplorerQueryVbhBlockCountRsp
		&& pOnExplorerQueryVbhBlockTranListRsp)

	{
		CWrapperCallback* pWrapperCallback = new CWrapperCallback;


		pWrapperCallback->m_pOnRegistUserRsp = pOnRegistUserRsp;
		pWrapperCallback->m_pOnProposeTradeRsp = pOnProposeTradeRsp;
		pWrapperCallback->m_pOnProposeCreatInfoIdRsp = pOnProposeCreatInfoIdRsp;
		pWrapperCallback->m_pOnProposeCommitInfoRsp = pOnProposeCommitInfoRsp;
		pWrapperCallback->m_pOnQueryRsp = pOnQueryRsp;
		pWrapperCallback->m_pOnExplorerQueryVbhBlockRsp = pOnExplorerQueryVbhBlockRsp;
		pWrapperCallback->m_pOnExplorerQueryVbhBlockCountRsp = pOnExplorerQueryVbhBlockCountRsp;
		pWrapperCallback->m_pOnExplorerQueryVbhUserRsp = nullptr;
		pWrapperCallback->m_pOnExplorerQueryVbhTransRsp = nullptr;
		pWrapperCallback->m_pOnExplorerQueryVbhBlockTranListRsp = pOnExplorerQueryVbhBlockTranListRsp;
		return InitClientSdk(nAppID, pWrapperCallback);
	}
	else
	{
		return -1;
	}
}

int32_t create_crypt_key_pair(char** ppPubKey, uint32_t* pnPubKeyLen, char** ppPriKey, uint32_t* pnPriKeyLen)
{
	std::string pubKey;
	std::string priKey;
	int32_t ret;

	ret = VBH_SDK::CreateCryptKeyPair(pubKey, priKey);
	if (ret != 0) 
	{
		return RET_FAIL;
	}
	*pnPubKeyLen = pubKey.size();
	*ppPubKey = new char[*pnPubKeyLen];
	strncpy(*ppPubKey, pubKey.c_str(), *pnPubKeyLen);
	*pnPriKeyLen = priKey.size();
	*ppPriKey = new char[*pnPriKeyLen];
	strncpy(*ppPriKey, priKey.c_str(), *pnPriKeyLen);

	return RET_OK;
}

int32_t generate_user_crypt_key(char **ppUserCryptKey, uint32_t *pUserCryptKeyLen,
								const char* pPriKey, const size_t nPriKeyLen, const char* pVbhCryptKey, const size_t nVbhCryptKeyLen, const uint32_t nChannelID)
{
	std::string stUserCryptKey;
	int32_t lRet;

	lRet = VBH_SDK::GenerateUserCryptKey(stUserCryptKey, pPriKey, nPriKeyLen, pVbhCryptKey, nVbhCryptKeyLen, nChannelID);
	if (lRet != 0)
	{
		return RET_FAIL;
	}
	*pUserCryptKeyLen = stUserCryptKey.size();
	*ppUserCryptKey = new char[*pUserCryptKeyLen];
	strncpy(*ppUserCryptKey, stUserCryptKey.c_str(), *pUserCryptKeyLen);

	return RET_OK;
}


int32_t reg_vbh_user(const uint32_t nChannelID, const uint32_t nRequestID,
	const uint32_t nAssert, char* pUserName, const uint32_t nUserNameLen, 
	char* pidCardNo, const uint32_t nidCardNoLen,
	char* pServerCryptKey, const uint32_t nServerCryptLen)
{
	if (pUserName && nUserNameLen && pidCardNo && nidCardNoLen && pServerCryptKey && nServerCryptLen)
	{
		VBH_SDK::CRegistUserReq req;
		TEST_CC::CCcCheckInfoOnUserRegist info;
		int32_t nRet = 0;

		req.m_nChannelID = nChannelID;
		req.m_nSessionID = nRequestID;

		info.m_nAsset = nAssert;
		info.m_userName.Set(pUserName, nUserNameLen);
		info.m_idCardNo.Set(pidCardNo, nidCardNoLen);

		req.m_pCltPubKey = pServerCryptKey;
		req.m_nCltPubKeyLen = nServerCryptLen;
		
		nRet = VBH_SDK::RegistUser(req, info);
		return nRet;
	}
	else
	{
		return -1;
	}
}

int32_t vbh_propose_trade(const uint32_t nRequestID,
	const uint32_t nAssert, 
	char* pfromUsr, const uint32_t nfromUsrLen, /*�����û�*/
	char* ptoUsr, const uint32_t ntoUsrLen, /*���շ��û�*/
	char* pUserCryptKey, const uint32_t nUserCryptKeyLen/*�����û���Կ*/)
{
	if (pfromUsr && nfromUsrLen > 0 && ptoUsr && ntoUsrLen > 0 
		&& pUserCryptKey && nUserCryptKeyLen > 0)
	{
		VBH_SDK::CProposeReq req;
		TEST_CC::CTradeAction info;
		int32_t nRet = 0;

		req.m_nActionID = TEST_CC::EN_TEST_TRADE_ACTION_ID;
		req.m_nSessionID = nRequestID;

		info.m_nAsset = nAssert;
		info.m_fromUserKey.Set(pfromUsr, nfromUsrLen);
		info.m_toUserKey.Set(ptoUsr, ntoUsrLen);

		req.m_pUserCryptKey = pUserCryptKey;
		req.m_nUserCryptKeyLen = nUserCryptKeyLen;

		nRet = VBH_SDK::ProposeTrade(req,info);
		return nRet;
	}
	else
	{
		return -1;
	}
}


int32_t vbh_propose_creat_information_id(const uint32_t nRequestID,
	char* pInitInformation, const uint32_t nInitInfoLen, /*�����֤id�ĳ�ʼ��Ϣ*/
	char* pUserCryptKey, const uint32_t nUserCryptKeyLen/*�����û���Կ*/)
{
	if (pInitInformation && nInitInfoLen == 20 
		&& pUserCryptKey && nUserCryptKeyLen > 0)
	{
		VBH_SDK::CProposeReq req;
		int32_t nRet = 0;

		req.m_nActionID = TEST_CC::EN_CREATE_INFORMATION_ACTION_ID;
		req.m_nSessionID = nRequestID;
		req.m_pUserCryptKey = pUserCryptKey;
		req.m_nUserCryptKeyLen = nUserCryptKeyLen;

		nRet = VBH_SDK::ProposeCreatInfoID(req, pInitInformation, nInitInfoLen);


		return nRet;
	}
	else
	{
		return -1;
	}
}

int32_t vbh_propose_commit_information(uint32_t isAnsiToUtf8, const uint32_t nRequestID,
	char* pInformationKey, const uint32_t nInfoKeyLen, /*��֤id*/
	char* pInformationValue, const uint32_t nInfoValueLen, /*��֤����*/
	char* pUserCryptKey, const uint32_t nUserCryptKeyLen/*�����û���Կ*/)
{
	if (pInformationKey && nInfoKeyLen > 0
		&& pInformationValue && nInfoValueLen >0
		&& pUserCryptKey && nUserCryptKeyLen > 0)
	{
		VBH_SDK::CProposeReq req;
		int32_t nRet = 0;
		int32_t nOutLen = 0;
		TEST_CC::CCommitInformationAction action;

		char* pInformationValueUtf8 = NULL;
#ifdef WIN32
		pInformationValueUtf8 = ANSItoUTF8(pInformationValue, nOutLen);
#endif
		action.m_InformationKey.Set(pInformationKey, nInfoKeyLen);
		if (isAnsiToUtf8)
		{
			action.m_InformationValue.Set(pInformationValueUtf8, nOutLen);
		}
		else
		{
			action.m_InformationValue.Set(pInformationValue, nInfoValueLen);
		}

		req.m_nActionID = TEST_CC::EN_COMMIT_INFORMATION_ACTION_ID;
		req.m_nSessionID = nRequestID;
		req.m_pUserCryptKey = pUserCryptKey;
		req.m_nUserCryptKeyLen = nUserCryptKeyLen;

		nRet = VBH_SDK::ProposeCommitInfo(req, action);
#ifdef WIN32
		delete[]pInformationValueUtf8;
#endif

		return nRet;
	}
	else
	{
		return -1;
	}

}

int32_t query_vbh(const uint32_t nRequestID, const uint32_t nActionID, 
	char* pParam, const uint32_t nParamLen,  //��ѯ����������CC������action-id����
	char* pUserCryptKey, const uint32_t nUserCryptKeyLen/*�û���Կ*/, uint16_t nDstPeerID)
{
	if (pUserCryptKey && nUserCryptKeyLen > 0)
	{
		VBH_SDK::CQueryReq req;

		req.m_nSrcSessionID = nRequestID;
		req.m_nActionID = nActionID;
		req.m_nDstPeerID = nDstPeerID;
		req.m_pParam = pParam;
		req.m_nParamLen = nParamLen;
		req.m_pUserCryptKey = pUserCryptKey;
		req.m_nUserCryptKeyLen = nUserCryptKeyLen;

		return VBH_SDK::Query(req);
	}
	else
	{
		return -1;
	}
}

int32_t query_vbh_user(const uint32_t nRequestID,
	char* pParam, const uint32_t nParamLen,  //��ѯ�������û�ID
	char* pUserCryptKey, const uint32_t nUserCryptKeyLen/*�û���Կ*/, uint16_t nDstPeerID)
{
	if (pUserCryptKey && nUserCryptKeyLen > 0)
	{
		VBH_SDK::CQueryReq req;
		TEST_CC::CQueryUserAction action;

		action.m_userID.Set(pParam, nParamLen);
		req.m_nDstPeerID = nDstPeerID; //��ʱ�ȹ̶�Ϊ1�� �����ڽ����ϼ�peer-id�����
		req.m_nActionID = TEST_CC::CQueryUserAction::EN_ACTION_ID;
		req.m_nSrcSessionID = nRequestID;
		req.m_pUserCryptKey = pUserCryptKey;
		req.m_nUserCryptKeyLen = nUserCryptKeyLen;

		return VBH_SDK::QueryUser(req,action);
	}
	else
	{
		return -1;
	}
}

int32_t query_vbh_trans(const uint32_t nRequestID,
	char* pParam, const uint32_t nParamLen,  //��ѯ����������ID
	char* pUserCryptKey, const uint32_t nUserCryptKeyLen/*�û���Կ*/, uint16_t nDstPeerID)
{
	if (pUserCryptKey && nUserCryptKeyLen > 0)
	{
		VBH_SDK::CQueryReq req;
		TEST_CC::CQueryTransAction action;

		action.m_transID.Set(pParam, nParamLen);
		req.m_nDstPeerID = nDstPeerID; //��ʱ�ȹ̶�Ϊ1�� �����ڽ����ϼ�peer-id�����
		req.m_nActionID = TEST_CC::CQueryTransAction::EN_ACTION_ID;
		req.m_nSrcSessionID = nRequestID;
		req.m_pUserCryptKey = pUserCryptKey;
		req.m_nUserCryptKeyLen = nUserCryptKeyLen;

		return VBH_SDK::QueryTrans(req, action);
	}
	else
	{
		return -1;
	}
}


int32_t explorer_query_vbh_block(const uint32_t nChannelID, const uint32_t nRequestID, 
	const uint64_t nBlockID,/*����ID*/  
	char* pUserCryptKey, const uint32_t nUserCryptKeyLen/*�û���Կ*/, uint16_t nDstPeerID)
{
	return VBH_SDK::ExplorerQueryBlock(nChannelID, nRequestID, nBlockID, pUserCryptKey, nUserCryptKeyLen, nDstPeerID);
}


int32_t explorer_query_vbh_block_count(const uint32_t nChannelID, const uint32_t nRequestID, 
	char* pUserCryptKey, const uint32_t nUserCryptKeyLen/*�û���Կ*/, uint16_t nDstPeerID)
{
	return VBH_SDK::ExplorerQueryBlockCount(nChannelID, nRequestID, pUserCryptKey, nUserCryptKeyLen, nDstPeerID);
}


int32_t explorer_query_block_tran_list(const uint32_t nChannelID,
	const uint32_t nRequestID, const uint64_t nBlockID, 
	char* pUserCryptKey, const uint32_t nUserCryptKeyLen, uint16_t nDstPeerID)
{
	return VBH_SDK::ExplorerQueryBlockTranList(nChannelID, nRequestID, nBlockID, 2, 1, pUserCryptKey, nUserCryptKeyLen, nDstPeerID );
}


void CWrapperCallback::OnRegistUserRsp(VBH_SDK::CRegistUserRsp& rRegistUserRsp)
{
	this->m_pOnRegistUserRsp(rRegistUserRsp.m_nReturnCode, rRegistUserRsp.m_nSessionID,
		rRegistUserRsp.m_pVbhCryptKey, rRegistUserRsp.m_nVbhCryptKeyLen, rRegistUserRsp.m_pVbhUserID, rRegistUserRsp.m_nVbhUserIDLen);
}

void CWrapperCallback::OnProposeRsp(VBH_SDK::CProposeRsp& rProposeRsp)
{
	switch (rProposeRsp.m_nActionID)
	{
		case TEST_CC::EN_TEST_TRADE_ACTION_ID:
		{
			this->m_pOnProposeTradeRsp(rProposeRsp.m_nReturnCode, rProposeRsp.m_nSrcSessionID,
				rProposeRsp.m_pTransKey, rProposeRsp.m_nTransKeyLen, rProposeRsp.m_pReceipt, rProposeRsp.m_nReceiptLen);
		}
		break;
		case TEST_CC::EN_CREATE_INFORMATION_ACTION_ID:
		{
			this->m_pOnProposeCreatInfoIdRsp(rProposeRsp.m_nReturnCode, rProposeRsp.m_nSrcSessionID,
				rProposeRsp.m_pInfoID, rProposeRsp.m_nInfoIdLen);
		}
		break;
		case TEST_CC::EN_COMMIT_INFORMATION_ACTION_ID:
		{
			this->m_pOnProposeCommitInfoRsp(rProposeRsp.m_nReturnCode, rProposeRsp.m_nSrcSessionID,
				rProposeRsp.m_pTransKey, rProposeRsp.m_nTransKeyLen);
		}
		break;
		default:
		{
		}
	}
}

void CWrapperCallback::OnQueryRsp(VBH_SDK::CQueryRsp& rQueryRsp)
{

	std::string strMsg;

	if (!VBH_SDK::SDKDecode(rQueryRsp.m_pInfo, rQueryRsp.m_nInfoLen, rQueryRsp.m_nActionID, strMsg))
	{
		m_pOnQueryRsp(rQueryRsp.m_nReturnCode, rQueryRsp.m_nActionID,
			rQueryRsp.m_nSrcSessionID, strMsg.c_str(), strMsg.size());
	}
	else
	{
		m_pOnQueryRsp(rQueryRsp.m_nReturnCode, rQueryRsp.m_nActionID,
			rQueryRsp.m_nSrcSessionID, rQueryRsp.m_pInfo, rQueryRsp.m_nInfoLen);
	}
}

void CWrapperCallback::OnExplorerQueryBlockRsp(VBH_SDK::CExplorerQueryBlockInfoRsp& rBlockInfoRsp)
{
	this->m_pOnExplorerQueryVbhBlockRsp(rBlockInfoRsp.m_nReturnCode, rBlockInfoRsp.m_nSrcSessionID,
		rBlockInfoRsp.m_pJsonBlockInfo, rBlockInfoRsp.m_nJsonBlockInfo);
}

void CWrapperCallback::OnExplorerQueryBlockCountRsp(VBH_SDK::CExplorerQueryBlockCountRsp& rBlockCountRsp)
{
	this->m_pOnExplorerQueryVbhBlockCountRsp(rBlockCountRsp.m_nReturnCode, rBlockCountRsp.m_nSrcSessionID,
		rBlockCountRsp.m_nBlockCount);
}

void CWrapperCallback::OnExplorerQueryUserRsp(VBH_SDK::CExplorerQueryUserRsp& rUserInfo)
{
	this->m_pOnExplorerQueryVbhUserRsp(rUserInfo.m_nReturnCode, rUserInfo.m_nSrcSessionID,
		rUserInfo.m_pUserInfo, rUserInfo.m_nUserInfoLen);
}

void CWrapperCallback::OnExplorerQueryTransRsp(VBH_SDK::CExplorerQueryTransRsp& rUserInfo)
{
	int32_t nOutLen = 0;
	char* pTransInfoInsi = NULL;
#ifdef WIN32
	pTransInfoInsi = UTF8toANSI(rUserInfo.m_pTransInfo, nOutLen, rUserInfo.m_nTransInfoLen);
#endif
	m_pOnExplorerQueryVbhTransRsp(rUserInfo.m_nReturnCode, rUserInfo.m_nSrcSessionID, rUserInfo.m_pTransInfo, rUserInfo.m_nTransInfoLen, pTransInfoInsi, nOutLen);
	delete[]pTransInfoInsi;
}

void CWrapperCallback::OnExplorerQueryBlockTransListRsp(VBH_SDK::CExplorerQueryBlockTransListRsp& rBlockTransList)
{
	m_pOnExplorerQueryVbhBlockTranListRsp(rBlockTransList.m_nReturnCode, rBlockTransList.m_nSrcSessionID, rBlockTransList.m_pJsonBlockTransList, rBlockTransList.m_nJsonBlockTransListLen);
}

