#ifndef CLIENT_C_API_H_328928987643289
#define CLIENT_C_API_H_328928987643289

#include <stdint.h>

#include "client_c_sdk/client_c_sdk_export.h"

#ifdef __cplusplus 
extern "C" {
#endif

	//注册用户应答
	typedef void(*on_regist_vbh_user_rsp) (const int32_t nReturnCode, const uint32_t nRequestID,
		const char* pUserKey, const uint32_t nUserKey /*注册时生成的user-key*/, const char *pUserId, const uint32_t nUserIdLen);

	//发起交易提案提案应答
	typedef void(*on_vbh_propose_trade_rsp)  (const int32_t nReturnCode, const uint32_t nRequestID, 
		const char* pTransKey, const uint32_t nTransKeyLen /*提案生成的trans-key*/, const char *pReceipt, const uint32_t nReceiptLen);

	//发起生成存证ID提案应答
	typedef void(*on_vbh_propose_creat_info_id_rsp)  (const int32_t nReturnCode, const uint32_t nRequestID,
		const char* pInfoKey, const uint32_t nInfoKeyLen /*返回生成的存证id*/);

	//发起提交存证提案应答
	typedef void(*on_vbh_propose_commit_info_rsp)  (const int32_t nReturnCode, const uint32_t nRequestID,
		const char* pTransKey, const uint32_t nTransKeyLen /*返回生成的存证id*/);

	//查询应答
	typedef void(*on_query_vbh_rsp)(const int32_t nReturnCode, const uint32_t nActionID, const uint32_t nSrcSessionID, 
		const char* pInfo, const uint32_t nInfoLen);

	//----------------------------------浏览器相关查询-----------------------------------------------------
	//浏览器查询区块信息应答
	typedef void(*on_explorer_query_vbh_block_rsp)(const int32_t nReturnCode, const uint32_t nRequestID,
		const char* pJsonBlockInfo, const uint32_t nJsonBLockInfoLen /*以json格式返回的块信息*/);

	//浏览器查询区块个数应答
	typedef void(*on_explorer_query_vbh_block_count_rsp)(const int32_t nReturnCode, const uint32_t nRequestID,
		const uint64_t nBlockCount);



	//浏览器查询区块交易列表应答
	typedef void(*on_explorer_query_vbh_block_tran_list_rsp)(const int32_t nReturnCode, const uint32_t nRequestID,
		const char* pJsonBlockTranList, const uint32_t nJsonBLockTranListLen /*以json格式返回的区块交易列表信息*/);
	//---------------------------------------------------------------------------------------------------

	//初始化
	VBH_CLIENT_C_SDK_EXPORT int32_t init_vbh_c_sdk(const uint16_t nAppID, 
		on_regist_vbh_user_rsp pOnRegistUserRsp,
		on_vbh_propose_trade_rsp pOnProposeTradeRsp,
		on_vbh_propose_creat_info_id_rsp pOnProposeCreatInfoIdRsp,
		on_vbh_propose_commit_info_rsp pOnProposeCommitInfoRsp,
		on_query_vbh_rsp pOnQueryRsp,
		/*浏览器相关查询应答回调函数*/
		on_explorer_query_vbh_block_rsp pOnExplorerQueryVbhBlockRsp,
		on_explorer_query_vbh_block_count_rsp pOnExplorerQueryVbhBlockCountRsp,
		on_explorer_query_vbh_block_tran_list_rsp m_pOnExplorerQueryVbhBlockTranListRsp);


	//创建秘钥：创建一对秘钥，以base64编码形式返回, key的内存有api内部malloc， 上层业务自己free
	VBH_CLIENT_C_SDK_EXPORT int32_t create_crypt_key_pair(char** ppPubKey, uint32_t* pnPubKeyLen, char** ppPriKey, uint32_t* pnPriKeyLen);
	//为用户生成user-crypt-key， 输入为用户的私钥和注册时返回的vbh-crypt-key
	VBH_CLIENT_C_SDK_EXPORT int32_t generate_user_crypt_key(char** ppUserCryptKey, uint32_t* pUserCryptKeyLen,
		const char* pPriKey, const size_t nPriKeyLen, const char* pVbhCryptKey, const size_t nVbhCryptKeyLen, const uint32_t nChannelID);

	//注册用户
	VBH_CLIENT_C_SDK_EXPORT int32_t reg_vbh_user(const uint32_t nChannelID, const uint32_t nRequestID,
		const uint32_t nAssert, char* pUserName, const uint32_t nUserNameLen,
		char* pidCardNo, const uint32_t nidCardNoLen,
		char* pServerCryptKey, const uint32_t nServerCryptLen);

	//发起交易提案
	VBH_CLIENT_C_SDK_EXPORT int32_t vbh_propose_trade(const uint32_t nRequestID,
		const uint32_t nAssert,
		char* pfromUsr, const uint32_t nfromUsrLen, /*发起方用户*/
		char* ptoUsr, const uint32_t ntoUsrLen, /*接收方用户*/
		char* pUserCryptKey, const uint32_t nUserCryptKeyLen/*发起方用户秘钥*/);
	//发起生成信息ID提案
	VBH_CLIENT_C_SDK_EXPORT int32_t vbh_propose_creat_information_id(const uint32_t nRequestID,
		char* pInitInformation, const uint32_t nInitInfoLen, /*申请存证id的初始信息*/
		char* pUserCryptKey, const uint32_t nUserCryptKeyLen/*发起方用户秘钥*/);
	//发起信息存证提案
	VBH_CLIENT_C_SDK_EXPORT int32_t vbh_propose_commit_information(uint32_t isAnsiToUtf8, const uint32_t nRequestID,
		char* pInformationKey, const uint32_t nInfoKeyLen, /*存证id*/
		char* pInformationValue, const uint32_t nInfoValueLen, /*存证内容*/
		char* pUserCryptKey, const uint32_t nUserCryptKeyLen/*发起方用户秘钥*/);

	//直接查询
	VBH_CLIENT_C_SDK_EXPORT int32_t query_vbh(const uint32_t nRequestID, const uint32_t nActionID,
		char* pParam, const uint32_t nParamLen,  //查询参数，交由CC，根据action-id解析
		char* pUserCryptKey, const uint32_t nUserCryptKeyLen/*用户秘钥*/, uint16_t nDstPeerID);

	VBH_CLIENT_C_SDK_EXPORT int32_t query_vbh_user(const uint32_t nRequestID,
		char* pParam, const uint32_t nParamLen,  //查询参数，用户ID
		char* pUserCryptKey, const uint32_t nUserCryptKeyLen/*用户秘钥*/, uint16_t nDstPeerID);
	VBH_CLIENT_C_SDK_EXPORT int32_t query_vbh_trans(const uint32_t nRequestID,
		char* pParam, const uint32_t nParamLen,  //查询参数，交易ID
		char* pUserCryptKey, const uint32_t nUserCryptKeyLen/*用户秘钥*/, uint16_t nDstPeerID);
	//----------------------------------浏览器相关查询-----------------------------------------------------
	//浏览器查询获取区块
	VBH_CLIENT_C_SDK_EXPORT int32_t explorer_query_vbh_block(const uint32_t nChannelID, const uint32_t nRequestID,
		const uint64_t nBlockID,/*区块ID*/
		char* pUserCryptKey, const uint32_t nUserCryptKeyLen/*用户秘钥*/, uint16_t nDstPeerID);

	//浏览器查询获取区块数量
	VBH_CLIENT_C_SDK_EXPORT int32_t explorer_query_vbh_block_count(const uint32_t nChannelID, const uint32_t nRequestID,
		char* pUserCryptKey, const uint32_t nUserCryptKeyLen/*用户秘钥*/, uint16_t nDstPeerID);


	VBH_CLIENT_C_SDK_EXPORT int32_t explorer_query_block_tran_list(const uint32_t nChannelID,
		const uint32_t nRequestID, const uint64_t nBlockID,
		char* pUserCryptKey, const uint32_t nUserCryptKeyLen, uint16_t nDstPeerID);

	//---------------------------------------------------------------------------------------------------

#ifdef __cplusplus 
}
#endif


#endif
