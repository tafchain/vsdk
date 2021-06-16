#ifndef VBH_KEY_CODEC_H_LKJERLJEW5435L43J6543J643JL4KHJK34H5
#define VBH_KEY_CODEC_H_LKJERLJEW5435L43J6543J643JL4KHJK34H5

#include "dsc/dsc_base64.h"

#include "vbh_comm/vbh_comm_def_export.h"
#include "vbh_comm/comm_msg_def/vbh_comm_class_def.h"

namespace VBH
{
	//ע��ʱ�õ��� vbh��Կ //wrapper����
	class VBH_COMM_DEF_EXPORT CVbhCryptKeyWrapper
	{
	public:
		CVbhCryptKeyWrapper(CDscString& strSvrPubKey, VBH::CVbhAllocatedKey& userKey, VBH::CTransactionUrl& transUrl);

	public:
		DSC_BIND_ATTR(m_strSvrPubKey, m_userKey, m_transUrl);

	public:
		CDscString& m_strSvrPubKey;
		VBH::CVbhAllocatedKey& m_userKey;
		VBH::CTransactionUrl& m_transUrl;
	};

	//�û���Կ //
	class VBH_COMM_DEF_EXPORT CUserCryptKeyWrapper
	{
	public:
		CUserCryptKeyWrapper(CDscString& strCltPriKey, CDscString& strSvrPubKey, CDscString& strEnvelopeKey,
			VBH::CVbhAllocatedKey& userKey, VBH::CTransactionUrl& transUrl, ACE_UINT32& nChannelID);

	public:
		DSC_BIND_ATTR(m_strCltPriKey, m_strSvrPubKey, m_strEnvelopeKey, m_userKey, m_transUrl, m_nChannelID);

	public:
		CDscString& m_strCltPriKey;
		CDscString& m_strSvrPubKey;
		CDscString& m_strEnvelopeKey;
		VBH::CVbhAllocatedKey& m_userKey;
		VBH::CTransactionUrl& m_transUrl;
		ACE_UINT32& m_nChannelID;
	};

	//�û��Ķ�UUID
	using CUserShortUuidWrapper = CVbhAllocatedKey;

	//����1��Bin��ʽ�Ĺ�Կ��˽Կ(���ɼ��ַ�)
	VBH_COMM_DEF_EXPORT ACE_INT32 CreateBinCryptKeyPair(CDscString& strPubKey, CDscString& strPriKey);

	//����1��Hex����Ĺ�Կ��˽Կ
	VBH_COMM_DEF_EXPORT ACE_INT32 CreateHexCryptKeyPair(CDscString& strPubKey, CDscString& strPriKey);

	//����base64����� user-crypt-key
	VBH_COMM_DEF_EXPORT ACE_INT32 GenerateUserCryptKey(CDscString& strUserCryptKey, 
		const char* pPriKey, const size_t nPriKeyLen, const char* pVbhCryptKey, const size_t nVbhCryptKeyLen, ACE_UINT32 nChannelID);

	//��������ַ�������hex���� //���ص�pOutBufҪʹ�� DSC_THREAD_SIZE_FREE �ͷ�
	size_t HexEncode(char*& pOutBuf, const char* pszIn, const size_t nInLen);

	//���ص�pOutBufҪʹ�� DSC_THREAD_SIZE_FREE �ͷ�
	//TYPE�в��ܰ���blob���͵ĳ�Ա����
	template<typename TYPE>
	size_t HexEncode(char*& pOutBuf, TYPE& t);

	//��������ַ�������Base64���� //���ص�pOutBufҪʹ�� DSC_THREAD_FREE �ͷ�
	VBH_COMM_DEF_EXPORT ACE_INT32 HexDecode(char*& pOutBuf, size_t& nOutLen, const char* pszIn, const size_t nInLen);

	//TYPE�в��ܰ���blob���͵ĳ�Ա����
	template<typename TYPE>
	ACE_INT32 HexDecode(TYPE& t, const char* pszIn, const size_t nInLen);

	//----------------------------------

	//��������ַ�������Base64���� //���ص�pOutBufҪʹ�� DSC_THREAD_FREE �ͷ�
	size_t Base64Encode(char*& pOutBuf, const char* pszIn, const size_t nInLen);

	//���ص�pOutBufҪʹ�� DSC_THREAD_FREE �ͷ�
	//TYPE�в��ܰ���blob���͵ĳ�Ա����
	template<typename TYPE>
	size_t Base64Encode(char*& pOutBuf, TYPE& t);

	//��������ַ�������Base64���� //���ص�pOutBufҪʹ�� DSC_THREAD_FREE �ͷ�
	VBH_COMM_DEF_EXPORT ACE_INT32 Base64Decode(char*& pOutBuf, size_t& nOutLen, const char* pszIn, const size_t nInLen);

	//TYPE�в��ܰ���blob���͵ĳ�Ա����
	template<typename TYPE>
	ACE_INT32 Base64Decode(TYPE& t, const char* pszIn, const size_t nInLen);

}

#include "vbh_comm/vbh_key_codec.inl"

#endif // !VBH_KEY_CODEC_H_LKJERLJEW5435L43J6543J643JL4KHJK34H5
