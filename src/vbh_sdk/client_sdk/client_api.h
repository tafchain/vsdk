#ifndef CLIENT_API_H_432874329876128761286
#define CLIENT_API_H_432874329876128761286

#include <string>

#include "client_sdk/client_sdk_export.h"
#include "client_sdk/client_api_msg.h"
#include "vbh_comm/test_def.h"

//TODO: 客户端提供走到CC的查询接口，包括查询用户和查询transaction
namespace VBH_SDK
{
	class IClientSdkMsgCallback
	{
	public:
		virtual void OnRegistUserRsp(VBH_SDK::CRegistUserRsp& rRegistUserRsp) = 0;
		virtual void OnProposeRsp(VBH_SDK::CProposeRsp& rProposeRsp) = 0;
		virtual void OnQueryRsp(VBH_SDK::CQueryRsp& rQueryRsp) = 0;

		//----------------------------------浏览器相关查询接口------------------------------
		virtual void OnExplorerQueryBlockRsp(VBH_SDK::CExplorerQueryBlockInfoRsp& rBlockInfoRsp) = 0;
		virtual void OnExplorerQueryBlockCountRsp(VBH_SDK::CExplorerQueryBlockCountRsp& rBlockCountRsp) = 0;
		virtual void OnExplorerQueryUserRsp(VBH_SDK::CExplorerQueryUserRsp& rUserInfo) = 0;
		virtual void OnExplorerQueryTransRsp(VBH_SDK::CExplorerQueryTransRsp& rTransInfo) = 0;
		virtual void OnExplorerQueryBlockTransListRsp(VBH_SDK::CExplorerQueryBlockTransListRsp& rBlockTransList) = 0;
	};

	//以下所有函数： return 0: succeed; return -1 error

	//创建秘钥：创建一对秘钥，以hex编码形式返回
	VBH_CLIENT_SDK_EXPORT int32_t CreateCryptKeyPair(std::string& strPubKey, std::string& strPriKey);

	//为用户生成user-crypt-key， 输入为用户的私钥和注册时返回的vbh-crypt-key
	VBH_CLIENT_SDK_EXPORT int32_t GenerateUserCryptKey(std::string& strUserCryptKey, 
		const char* pPriKey, const size_t nPriKeyLen, const char* pVbhCryptKey, const size_t nVbhCryptKeyLen, const uint32_t nChannelID);

	//获取错误码对应的错误信息
	VBH_CLIENT_SDK_EXPORT const char* GetErrorString(int nErrorCode);

	//初始化SDK
	VBH_CLIENT_SDK_EXPORT int32_t InitClientSdk(const int16_t nAppID, IClientSdkMsgCallback* pCallBack);

	//注册用户
	VBH_CLIENT_SDK_EXPORT int32_t RegistUser(CRegistUserReq& rRegistUserReq, TEST_CC::CCcCheckInfoOnUserRegist info);

	//发起提案
	VBH_CLIENT_SDK_EXPORT int32_t Propose(CProposeReq& rSetUserInfoReq);
	VBH_CLIENT_SDK_EXPORT int32_t ProposeTrade(CProposeReq& rProposeReq, TEST_CC::CTradeAction action);
	VBH_CLIENT_SDK_EXPORT int32_t ProposeCreatInfoID(CProposeReq& rProposeReq, char* pInitInformation, const uint32_t nInitInfoLen);
	VBH_CLIENT_SDK_EXPORT int32_t ProposeCommitInfo(CProposeReq& rProposeReq, TEST_CC::CCommitInformationAction action);

	//发起查询，
	VBH_CLIENT_SDK_EXPORT int32_t Query(CQueryReq& rQueryReq);
	VBH_CLIENT_SDK_EXPORT int32_t QueryUser(CQueryReq& rQueryReq,TEST_CC::CQueryUserAction action);
	VBH_CLIENT_SDK_EXPORT int32_t QueryTrans(CQueryReq& rQueryReq, TEST_CC::CQueryTransAction action);


	//----------------------------------浏览器相关查询-----------------------------------------------------
	//浏览器查询获取区块头
	VBH_CLIENT_SDK_EXPORT int32_t ExplorerQueryBlock(const uint32_t nChannelID, const uint32_t nRequestID,
		const uint64_t nBlockID,/*区块ID*/
		char* pUserCryptKey, const uint32_t nUserCryptKeyLen/*用户秘钥*/, uint16_t nDstPeerID = 1);
	//浏览器查询获取区块交易列表
	VBH_CLIENT_SDK_EXPORT int32_t ExplorerQueryBlockTranList(const uint32_t nChannelID,
		const uint32_t nRequestID, const uint64_t nBlockID, uint16_t nPageSize, uint16_t nPageIndex,
		char* pUserCryptKey, const uint32_t nUserCryptKeyLen, uint16_t nDstPeerID = 1);

	//浏览器查询获取区块数量
	VBH_CLIENT_SDK_EXPORT int32_t ExplorerQueryBlockCount(const uint32_t nChannelID, const uint32_t nRequestID,
		char* pUserCryptKey, const uint32_t nUserCryptKeyLen/*用户秘钥*/, uint16_t nDstPeerID = 1);

	//浏览器查询获取用户信息
	VBH_CLIENT_SDK_EXPORT int32_t ExplorerQueryUser(const uint32_t nChannelID, const uint32_t nRequestID,
		char* pUserKey, const uint32_t nUserKeyLen, /*用户key*/
		char* pUserCryptKey, const uint32_t nUserCryptKeyLen/*用户秘钥*/);

	VBH_CLIENT_SDK_EXPORT int32_t ExplorerQueryTrans(const uint32_t nChannelID, const uint32_t nRequestID,
		char* pTransKey, const uint32_t nTransKeyLen, /*交易ID*/
		char* pUserCryptKey, const uint32_t nUserCryptKeyLen/*用户秘钥*/, uint16_t nDstPeerID);

	VBH_CLIENT_SDK_EXPORT int32_t SDKDecode(const char* pMsg, const size_t nMsgBufLen, const uint32_t nActionID, std::string& strMsg);
}

#endif
