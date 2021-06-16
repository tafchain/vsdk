#ifndef CLIENT_C_API_H_328928987643289
#define CLIENT_C_API_H_328928987643289

#include <stdint.h>

#include "client_c_sdk/client_c_sdk_export.h"

#ifdef __cplusplus 
extern "C" {
#endif

	//ע���û�Ӧ��
	typedef void(*on_regist_vbh_user_rsp) (const int32_t nReturnCode, const uint32_t nRequestID,
		const char* pUserKey, const uint32_t nUserKey /*ע��ʱ���ɵ�user-key*/, const char *pUserId, const uint32_t nUserIdLen);

	//�������᰸�᰸Ӧ��
	typedef void(*on_vbh_propose_trade_rsp)  (const int32_t nReturnCode, const uint32_t nRequestID, 
		const char* pTransKey, const uint32_t nTransKeyLen /*�᰸���ɵ�trans-key*/, const char *pReceipt, const uint32_t nReceiptLen);

	//�������ɴ�֤ID�᰸Ӧ��
	typedef void(*on_vbh_propose_creat_info_id_rsp)  (const int32_t nReturnCode, const uint32_t nRequestID,
		const char* pInfoKey, const uint32_t nInfoKeyLen /*�������ɵĴ�֤id*/);

	//�����ύ��֤�᰸Ӧ��
	typedef void(*on_vbh_propose_commit_info_rsp)  (const int32_t nReturnCode, const uint32_t nRequestID,
		const char* pTransKey, const uint32_t nTransKeyLen /*�������ɵĴ�֤id*/);

	//��ѯӦ��
	typedef void(*on_query_vbh_rsp)(const int32_t nReturnCode, const uint32_t nActionID, const uint32_t nSrcSessionID, 
		const char* pInfo, const uint32_t nInfoLen);

	//----------------------------------�������ز�ѯ-----------------------------------------------------
	//�������ѯ������ϢӦ��
	typedef void(*on_explorer_query_vbh_block_rsp)(const int32_t nReturnCode, const uint32_t nRequestID,
		const char* pJsonBlockInfo, const uint32_t nJsonBLockInfoLen /*��json��ʽ���صĿ���Ϣ*/);

	//�������ѯ�������Ӧ��
	typedef void(*on_explorer_query_vbh_block_count_rsp)(const int32_t nReturnCode, const uint32_t nRequestID,
		const uint64_t nBlockCount);



	//�������ѯ���齻���б�Ӧ��
	typedef void(*on_explorer_query_vbh_block_tran_list_rsp)(const int32_t nReturnCode, const uint32_t nRequestID,
		const char* pJsonBlockTranList, const uint32_t nJsonBLockTranListLen /*��json��ʽ���ص����齻���б���Ϣ*/);
	//---------------------------------------------------------------------------------------------------

	//��ʼ��
	VBH_CLIENT_C_SDK_EXPORT int32_t init_vbh_c_sdk(const uint16_t nAppID, 
		on_regist_vbh_user_rsp pOnRegistUserRsp,
		on_vbh_propose_trade_rsp pOnProposeTradeRsp,
		on_vbh_propose_creat_info_id_rsp pOnProposeCreatInfoIdRsp,
		on_vbh_propose_commit_info_rsp pOnProposeCommitInfoRsp,
		on_query_vbh_rsp pOnQueryRsp,
		/*�������ز�ѯӦ��ص�����*/
		on_explorer_query_vbh_block_rsp pOnExplorerQueryVbhBlockRsp,
		on_explorer_query_vbh_block_count_rsp pOnExplorerQueryVbhBlockCountRsp,
		on_explorer_query_vbh_block_tran_list_rsp m_pOnExplorerQueryVbhBlockTranListRsp);


	//������Կ������һ����Կ����base64������ʽ����, key���ڴ���api�ڲ�malloc�� �ϲ�ҵ���Լ�free
	VBH_CLIENT_C_SDK_EXPORT int32_t create_crypt_key_pair(char** ppPubKey, uint32_t* pnPubKeyLen, char** ppPriKey, uint32_t* pnPriKeyLen);
	//Ϊ�û�����user-crypt-key�� ����Ϊ�û���˽Կ��ע��ʱ���ص�vbh-crypt-key
	VBH_CLIENT_C_SDK_EXPORT int32_t generate_user_crypt_key(char** ppUserCryptKey, uint32_t* pUserCryptKeyLen,
		const char* pPriKey, const size_t nPriKeyLen, const char* pVbhCryptKey, const size_t nVbhCryptKeyLen, const uint32_t nChannelID);

	//ע���û�
	VBH_CLIENT_C_SDK_EXPORT int32_t reg_vbh_user(const uint32_t nChannelID, const uint32_t nRequestID,
		const uint32_t nAssert, char* pUserName, const uint32_t nUserNameLen,
		char* pidCardNo, const uint32_t nidCardNoLen,
		char* pServerCryptKey, const uint32_t nServerCryptLen);

	//�������᰸
	VBH_CLIENT_C_SDK_EXPORT int32_t vbh_propose_trade(const uint32_t nRequestID,
		const uint32_t nAssert,
		char* pfromUsr, const uint32_t nfromUsrLen, /*�����û�*/
		char* ptoUsr, const uint32_t ntoUsrLen, /*���շ��û�*/
		char* pUserCryptKey, const uint32_t nUserCryptKeyLen/*�����û���Կ*/);
	//����������ϢID�᰸
	VBH_CLIENT_C_SDK_EXPORT int32_t vbh_propose_creat_information_id(const uint32_t nRequestID,
		char* pInitInformation, const uint32_t nInitInfoLen, /*�����֤id�ĳ�ʼ��Ϣ*/
		char* pUserCryptKey, const uint32_t nUserCryptKeyLen/*�����û���Կ*/);
	//������Ϣ��֤�᰸
	VBH_CLIENT_C_SDK_EXPORT int32_t vbh_propose_commit_information(uint32_t isAnsiToUtf8, const uint32_t nRequestID,
		char* pInformationKey, const uint32_t nInfoKeyLen, /*��֤id*/
		char* pInformationValue, const uint32_t nInfoValueLen, /*��֤����*/
		char* pUserCryptKey, const uint32_t nUserCryptKeyLen/*�����û���Կ*/);

	//ֱ�Ӳ�ѯ
	VBH_CLIENT_C_SDK_EXPORT int32_t query_vbh(const uint32_t nRequestID, const uint32_t nActionID,
		char* pParam, const uint32_t nParamLen,  //��ѯ����������CC������action-id����
		char* pUserCryptKey, const uint32_t nUserCryptKeyLen/*�û���Կ*/, uint16_t nDstPeerID);

	VBH_CLIENT_C_SDK_EXPORT int32_t query_vbh_user(const uint32_t nRequestID,
		char* pParam, const uint32_t nParamLen,  //��ѯ�������û�ID
		char* pUserCryptKey, const uint32_t nUserCryptKeyLen/*�û���Կ*/, uint16_t nDstPeerID);
	VBH_CLIENT_C_SDK_EXPORT int32_t query_vbh_trans(const uint32_t nRequestID,
		char* pParam, const uint32_t nParamLen,  //��ѯ����������ID
		char* pUserCryptKey, const uint32_t nUserCryptKeyLen/*�û���Կ*/, uint16_t nDstPeerID);
	//----------------------------------�������ز�ѯ-----------------------------------------------------
	//�������ѯ��ȡ����
	VBH_CLIENT_C_SDK_EXPORT int32_t explorer_query_vbh_block(const uint32_t nChannelID, const uint32_t nRequestID,
		const uint64_t nBlockID,/*����ID*/
		char* pUserCryptKey, const uint32_t nUserCryptKeyLen/*�û���Կ*/, uint16_t nDstPeerID);

	//�������ѯ��ȡ��������
	VBH_CLIENT_C_SDK_EXPORT int32_t explorer_query_vbh_block_count(const uint32_t nChannelID, const uint32_t nRequestID,
		char* pUserCryptKey, const uint32_t nUserCryptKeyLen/*�û���Կ*/, uint16_t nDstPeerID);


	VBH_CLIENT_C_SDK_EXPORT int32_t explorer_query_block_tran_list(const uint32_t nChannelID,
		const uint32_t nRequestID, const uint64_t nBlockID,
		char* pUserCryptKey, const uint32_t nUserCryptKeyLen, uint16_t nDstPeerID);

	//---------------------------------------------------------------------------------------------------

#ifdef __cplusplus 
}
#endif


#endif
