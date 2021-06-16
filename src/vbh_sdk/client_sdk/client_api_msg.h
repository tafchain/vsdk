#ifndef CLIENT_API_MSG_H_221F987CAF6911E9A08860F18A3A20D1
#define CLIENT_API_MSG_H_221F987CAF6911E9A08860F18A3A20D1

#include <stdint.h>

namespace VBH_SDK
{
	//关于用户的秘钥：key-gen工具生成用户的公钥和私钥；注册成功后，返回一个服务端公钥信息；用户再使用key-merge工具，将服务端公钥+客户端私钥merge成客户秘钥文件
	// 注册用户请求和响应
	class CRegistUserReq
	{
	public:
		uint32_t m_nChannelID;
		uint32_t m_nSessionID; //本次请求的ID，可以是上层业务的sessionID

		char* m_pUserInfo; //提交到CC的用户参考信息，供CC对用户进行特别处理
		uint32_t m_nUserInfoLen;

		char* m_pCltPubKey;//客户端公钥 //Hex编码格式
		uint32_t m_nCltPubKeyLen;
	};

	class CRegistUserRsp
	{
	public:
		int32_t m_nReturnCode;
		uint32_t m_nSessionID; //本次请求的ID，可以是上层业务的sessionID

		char* m_pVbhCryptKey;
		uint32_t m_nVbhCryptKeyLen; //hex编码

		char* m_pVbhUserID;
		uint32_t m_nVbhUserIDLen; //hex编码
	};

	//发起提案请求和响应 //暂不考虑用户和提案涉及对象不在同一channel的场景
	class CProposeReq
	{
	public:
		uint32_t m_nActionID; 
		uint32_t m_nSessionID; //本次请求的ID，可以是上层业务的sessionID

		char* m_pProposal;
		uint32_t m_nProposalLen;//提案

		char* m_pUserCryptKey;
		uint32_t m_nUserCryptKeyLen; //base64编码
	};

	class CProposeRsp
	{
	public:
		uint32_t m_nActionID;
		int32_t m_nReturnCode;
		uint32_t m_nSrcSessionID; //本次请求的ID，可以是上层业务的sessionID

		//CC填写此参数，
		char* m_pReceipt;
		uint32_t m_nReceiptLen; //cc为提案生成的回执，提案成功执行后，返回到客户端，该信息不上链

		char* m_pTransKey;		
		uint32_t m_nTransKeyLen; //成功时带回来的transaction-uuid
		char *m_pInfoID;
		uint32_t m_nInfoIdLen;
	};

	using CCreateInformationReq = CProposeReq;

	class CCreateInformationRsp
	{
	public:
		int32_t m_nReturnCode;
		uint32_t m_nSrcSessionID; //本次请求的ID，可以是上层业务的sessionID

		//创建的information-id列表
	};

	//系统中不存在直接查询，所有的查询都经CC
	class CQueryReq
	{
	public:
		uint16_t m_nDstPeerID; //要查询的peer-id
		uint32_t m_nActionID;
		uint32_t m_nSrcSessionID; //本次请求的ID，可以是上层业务的sessionID

		char* m_pParam;
		uint32_t m_nParamLen; //查询参数，交由CC，根据action-id解析

		char* m_pUserCryptKey;
		uint32_t m_nUserCryptKeyLen; // base64编码
	};

	class CQueryRsp
	{
	public:
		int32_t m_nReturnCode;
		uint32_t m_nActionID; //查询时的action-id
		uint32_t m_nSrcSessionID; //本次请求的ID，可以是上层业务的sessionID

		char* m_pInfo;
		uint32_t m_nInfoLen; //查询所得结果
	};

	//查询事务请求和响应
	class CGetTransactionInfoReq
	{
	public:
		uint32_t m_nSrcSessionID; //本次请求的ID，可以是上层业务的sessionID

		char* m_pUserCryptKey;
		uint32_t m_nUserCryptKeyLen;

		char* m_pTransKey;
		uint32_t m_nTransKeyLen; //要查询的transaction-uuid
	};

	class CGetTransactionInfoRsp
	{
	public:
		int32_t m_nReturnCode;
		uint32_t m_nSrcSessionID; //本次请求的ID，可以是上层业务的sessionID

		char* m_pUserKey;
		uint32_t m_nUserKeyLen; //查询到的内容

		char* m_pSignature;
		uint32_t m_nSignatureLen; //查询到的内容

		char* m_pProposal;
		uint32_t m_nProposalLen; //查询到的内容
	};

	//浏览器查询应答-查询区块信息应答
	class CExplorerQueryBlockInfoRsp
	{
	public:
		int32_t m_nReturnCode;
		uint32_t m_nSrcSessionID;

		char* m_pJsonBlockInfo;
		uint32_t m_nJsonBlockInfo;
	};

	//浏览器查询应答-查询区块数量
	class CExplorerQueryBlockCountRsp
	{
	public:
		int32_t m_nReturnCode;
		uint32_t m_nSrcSessionID;

		uint64_t m_nBlockCount;
	};

	//浏览器查询应答-查询区块hash
	class CExplorerQueryBlockHashRsp
	{
	public:
		int32_t m_nReturnCode;
		uint32_t m_nSrcSessionID;

		char* m_pBlockHash;
		uint32_t m_nBlockHashLen;
	};

	//浏览器查询应答-查询用户信息
	class CExplorerQueryUserRsp
	{
	public:
		int32_t m_nReturnCode;
		uint32_t m_nSrcSessionID;

		char* m_pUserInfo;
		uint32_t m_nUserInfoLen;
	};

	class CExplorerQueryTransRsp
	{
	public:
		int32_t m_nReturnCode;
		uint32_t m_nSrcSessionID;

		char* m_pTransInfo;
		uint32_t m_nTransInfoLen;
	};

	//浏览器查询应答-查询区块信息应答
	class CExplorerQueryBlockTransListRsp
	{
	public:
		int32_t m_nReturnCode;
		uint32_t m_nSrcSessionID;

		char* m_pJsonBlockTransList;
		uint32_t m_nJsonBlockTransListLen;
	};

}

#endif
