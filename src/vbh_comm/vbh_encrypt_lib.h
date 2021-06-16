#ifndef VBH_ENCRYPT_LIB_H_7FAC6382B1DD11E9B5D660F18A3A20D1
#define VBH_ENCRYPT_LIB_H_7FAC6382B1DD11E9B5D660F18A3A20D1

#include <openssl/evp.h>
#include <openssl/ec.h>
#include <openssl/ecdsa.h>
#include <openssl/sha.h>

#include "dsc/mem_mng/dsc_allocator.h"

#include "vbh_comm/vbh_comm_def_export.h"


//��ϣժҪ�ĳ���
#ifdef VBH_USE_SMX
#define VBH_BLOCK_DIGEST_LENGTH 32  // crypto/include/internal/sm3.h �ж��� SM3_DIGEST_LENGTH ֵΪ32
#define VBH_ENVELOPE_KEY_LENGTH 32  //�ŷ���Կ����, ����Ϊ������
#define VBH_PUBLIC_KEY_LENGTH 33 //��Կ��󳤶� //openssl1.1.1.dʱ��sm2���㹫Կ����  BIN:33, HEX:66
#define VBH_PRIVATE_KEY_LENGTH 32 //˽Կ��󳤶� //openssl1.1.1.dʱ��sm2����˽Կ����  BIN:32, HEX:64
#else
#define VBH_BLOCK_DIGEST_LENGTH SHA256_DIGEST_LENGTH 
#define VBH_ENVELOPE_KEY_LENGTH 32  //�ŷ���Կ����, ����Ϊ������
#define VBH_PUBLIC_KEY_LENGTH 33 //��Կ��󳤶� //openssl1.1.1.dʱ�����㹫Կ����  BIN:33, HEX:66
#define VBH_PRIVATE_KEY_LENGTH 32 //˽Կ��󳤶� //openssl1.1.1.dʱ������˽Կ����  BIN:32, HEX:64
#endif

namespace VBH
{
	//��ʼ��openssl��
	VBH_COMM_DEF_EXPORT void InitOpenSsl(void);

	VBH_COMM_DEF_EXPORT EC_GROUP* vbhEcGroup(void);
	VBH_COMM_DEF_EXPORT EC_KEY* vbhCreateEcKey(const EC_GROUP* pGroup);
	
	VBH_COMM_DEF_EXPORT char* vbhGetBinPublicKeyFromEcKey(int& nBinKeyLen, const EC_KEY* key);
	VBH_COMM_DEF_EXPORT char* vbhGetBinPrivateKeyFromEcKey(int& nBinKeyLen, const EC_KEY* key);

	VBH_COMM_DEF_EXPORT EC_KEY* vbhCreateEcKeyFromBinPublicKey(const char* pBinKeyBuf, const int nBinKeyLen, const EC_GROUP* pGroup);
	VBH_COMM_DEF_EXPORT EC_KEY* vbhCreateEcKeyFromBinPrivateKey(const char* pBinKeyBuf, const int nBinKeyLen, const EC_GROUP* pGroup);

	//����ECDH�㷨�еĶԳ���Կ���ŷ���Կ����ʹ��Զ�̹�Կ������EcKey�����ŷ���Կ
	//pszRemoteHexKey ������0��β�ַ���
	//���ص��ַ���ʹ�� DSC_THREAD_SIZE_FREE �ͷ�
	VBH_COMM_DEF_EXPORT char* vbhGenEcdhEnvKey(int& nEnvKeyLen, const char* pszRemoteBinKey, const int nRemoteBinKeyLen, const EC_KEY* pLocalEcKey);

	//�ԳƼ��ܺ���; ���ص�ָ����ʹ����Ϻ���Ҫ���� DSC_THREAD_FREE �����ͷ�
	VBH_COMM_DEF_EXPORT char* vbhEncrypt(const unsigned char* envelope_key, int& nOutLen, char* data, const int nDataLen);

	//�Գƽ��ܺ���; ���ص�ָ����ʹ����Ϻ���Ҫ���� DSC_THREAD_FREE �����ͷ�
	VBH_COMM_DEF_EXPORT char* vbhDecrypt(const unsigned char* envelope_key, int& nOutLen, char* data, const int nDataLen);

#ifdef VBH_USE_SMX //ʹ�ù���ʱ��Ҫ��������
	//ǩ��������//pEcKey �� vbh_sign �лᱻ�ͷ�
	VBH_COMM_DEF_EXPORT char* vbhSign(EC_KEY* pEcKey, char* buff, int buff_len, unsigned int& nSignature);

	//��ǩ������//pEcKey �� vbh_verify_sign �лᱻ�ͷ�
	VBH_COMM_DEF_EXPORT bool vbhVerifySign(EC_KEY* pEcKey, char* buff, size_t buff_len, char* pSignature, const int nSignature);

	//�����ϣժҪ����; ���������õ���
	VBH_COMM_DEF_EXPORT void vbhDigest(const char* data, size_t n, char* digest);

#else //ʹ��aes-gcmʱ����Ҫ����
	//ǩ��������//pEcKey �� vbh_sign �лᱻ�ͷ�
	char* vbhSign(EC_KEY* pEcKey, char* buff, int buff_len, unsigned int& nSignature);

	//��ǩ������//pEcKey �� vbh_verify_sign �лᱻ�ͷ�
	bool vbhVerifySign(EC_KEY* pEcKey, char* buff, size_t buff_len, char* pSignature, const int nSignature);

	//�����ϣժҪ����; ���������õ���
	void vbhDigest(const char* data, size_t n, char* digest);

#endif

	//ֱ��ʹ���ַ�����ʽ��Կ��˽Կ��ǩ����ǩ���� //����ֱ�ӵ���
	char* vbhSign(char* pPriKeyBuf, int nPriKeyLen, EC_GROUP* pGroup, char* pDataBuf, size_t nDataLen, unsigned int& nSignatureLen);
	bool vbhVerifySign(char* pPubKeyBuf, int nPubKeyLen, EC_GROUP* pGroup, char* pDataBuf, size_t nDataLen, char* pSignatureBuf, size_t nSignatureLen);
}

#include "vbh_comm/vbh_encrypt_lib.inl"

#endif
