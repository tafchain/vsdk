#ifndef CLIENT_API_H_432874329876128761286
#define CLIENT_API_H_432874329876128761286

#include <string>

#include "client_sdk/client_sdk_export.h"
#include "client_sdk/client_api_msg.h"
#include "vbh_comm/test_def.h"

//TODO: �ͻ����ṩ�ߵ�CC�Ĳ�ѯ�ӿڣ�������ѯ�û��Ͳ�ѯtransaction
namespace VBH_SDK
{
	class IClientSdkMsgCallback
	{
	public:
		virtual void OnRegistUserRsp(VBH_SDK::CRegistUserRsp& rRegistUserRsp) = 0;
		virtual void OnProposeRsp(VBH_SDK::CProposeRsp& rProposeRsp) = 0;
		virtual void OnQueryRsp(VBH_SDK::CQueryRsp& rQueryRsp) = 0;

		//----------------------------------�������ز�ѯ�ӿ�------------------------------
		virtual void OnExplorerQueryBlockRsp(VBH_SDK::CExplorerQueryBlockInfoRsp& rBlockInfoRsp) = 0;
		virtual void OnExplorerQueryBlockCountRsp(VBH_SDK::CExplorerQueryBlockCountRsp& rBlockCountRsp) = 0;
		virtual void OnExplorerQueryUserRsp(VBH_SDK::CExplorerQueryUserRsp& rUserInfo) = 0;
		virtual void OnExplorerQueryTransRsp(VBH_SDK::CExplorerQueryTransRsp& rTransInfo) = 0;
		virtual void OnExplorerQueryBlockTransListRsp(VBH_SDK::CExplorerQueryBlockTransListRsp& rBlockTransList) = 0;
	};

	//�������к����� return 0: succeed; return -1 error

	//������Կ������һ����Կ����hex������ʽ����
	VBH_CLIENT_SDK_EXPORT int32_t CreateCryptKeyPair(std::string& strPubKey, std::string& strPriKey);

	//Ϊ�û�����user-crypt-key�� ����Ϊ�û���˽Կ��ע��ʱ���ص�vbh-crypt-key
	VBH_CLIENT_SDK_EXPORT int32_t GenerateUserCryptKey(std::string& strUserCryptKey, 
		const char* pPriKey, const size_t nPriKeyLen, const char* pVbhCryptKey, const size_t nVbhCryptKeyLen, const uint32_t nChannelID);

	//��ȡ�������Ӧ�Ĵ�����Ϣ
	VBH_CLIENT_SDK_EXPORT const char* GetErrorString(int nErrorCode);

	//��ʼ��SDK
	VBH_CLIENT_SDK_EXPORT int32_t InitClientSdk(const int16_t nAppID, IClientSdkMsgCallback* pCallBack);

	//ע���û�
	VBH_CLIENT_SDK_EXPORT int32_t RegistUser(CRegistUserReq& rRegistUserReq, TEST_CC::CCcCheckInfoOnUserRegist info);

	//�����᰸
	VBH_CLIENT_SDK_EXPORT int32_t Propose(CProposeReq& rSetUserInfoReq);
	VBH_CLIENT_SDK_EXPORT int32_t ProposeTrade(CProposeReq& rProposeReq, TEST_CC::CTradeAction action);
	VBH_CLIENT_SDK_EXPORT int32_t ProposeCreatInfoID(CProposeReq& rProposeReq, char* pInitInformation, const uint32_t nInitInfoLen);
	VBH_CLIENT_SDK_EXPORT int32_t ProposeCommitInfo(CProposeReq& rProposeReq, TEST_CC::CCommitInformationAction action);

	//�����ѯ��
	VBH_CLIENT_SDK_EXPORT int32_t Query(CQueryReq& rQueryReq);
	VBH_CLIENT_SDK_EXPORT int32_t QueryUser(CQueryReq& rQueryReq,TEST_CC::CQueryUserAction action);
	VBH_CLIENT_SDK_EXPORT int32_t QueryTrans(CQueryReq& rQueryReq, TEST_CC::CQueryTransAction action);


	//----------------------------------�������ز�ѯ-----------------------------------------------------
	//�������ѯ��ȡ����ͷ
	VBH_CLIENT_SDK_EXPORT int32_t ExplorerQueryBlock(const uint32_t nChannelID, const uint32_t nRequestID,
		const uint64_t nBlockID,/*����ID*/
		char* pUserCryptKey, const uint32_t nUserCryptKeyLen/*�û���Կ*/, uint16_t nDstPeerID = 1);
	//�������ѯ��ȡ���齻���б�
	VBH_CLIENT_SDK_EXPORT int32_t ExplorerQueryBlockTranList(const uint32_t nChannelID,
		const uint32_t nRequestID, const uint64_t nBlockID, uint16_t nPageSize, uint16_t nPageIndex,
		char* pUserCryptKey, const uint32_t nUserCryptKeyLen, uint16_t nDstPeerID = 1);

	//�������ѯ��ȡ��������
	VBH_CLIENT_SDK_EXPORT int32_t ExplorerQueryBlockCount(const uint32_t nChannelID, const uint32_t nRequestID,
		char* pUserCryptKey, const uint32_t nUserCryptKeyLen/*�û���Կ*/, uint16_t nDstPeerID = 1);

	//�������ѯ��ȡ�û���Ϣ
	VBH_CLIENT_SDK_EXPORT int32_t ExplorerQueryUser(const uint32_t nChannelID, const uint32_t nRequestID,
		char* pUserKey, const uint32_t nUserKeyLen, /*�û�key*/
		char* pUserCryptKey, const uint32_t nUserCryptKeyLen/*�û���Կ*/);

	VBH_CLIENT_SDK_EXPORT int32_t ExplorerQueryTrans(const uint32_t nChannelID, const uint32_t nRequestID,
		char* pTransKey, const uint32_t nTransKeyLen, /*����ID*/
		char* pUserCryptKey, const uint32_t nUserCryptKeyLen/*�û���Կ*/, uint16_t nDstPeerID);

	VBH_CLIENT_SDK_EXPORT int32_t SDKDecode(const char* pMsg, const size_t nMsgBufLen, const uint32_t nActionID, std::string& strMsg);
}

#endif
