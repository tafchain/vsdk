#include "dsc/dsc_comm_def.h"

#include "vbh_comm/vbh_comm_error_code.h"

const char* VBH::GetErrorString(int nErrorCode)
{
	static const char* pOkString = "ok";

	//common ������
	static const char* pArrCommonErrorString[EN_COMMON_ERROR_CODE_NUM] = {
		"common error begin value", //��Ӧ EN_MAX_COMMON_ERROR_VALUE
		"not ready error", //δ׼������
		"reentry error", //��������ظ��ύ����
		"timeout error", //��ʱ����
		"network error", //�������
		"vbh system inner error", //�������ڲ�����
		"decode error", //�������
		"cannot find session in memory, most because of timeout",
		"invalid input param, please check your input carefully", //ϵͳʹ��������Ĳ������Ϸ�
		"there is logic failed, please wait a moment and try agin, or check your input param", //�߼�ʧ�ܴ���
		"order service is not ready, please try again later", //order��δ׼������
		"this transaction not support query", //��֧�ֲ�ѯ����������
		nullptr
	};

	//config ������
	static const char* pArrConfigErrorString[EN_CONFIG_ERROR_CODE_NUM] = {
		"config error begin value", //��Ӧ EN_MAX_COMMON_ERROR_VALUE
		"cannot find committer belong to this channel", //δ�ҵ�ָ��channel��committer
		"cannot find order-cps belong to this channel", //δ�ҵ�ָ��channel��order-cps��ַ
		nullptr
	};

	//crypt ������
	static const char* pArrCryptErrorString[EN_CRYPT_ERROR_CODE_NUM] = {
		"crypt error begin value", //��Ӧ EN_MAX_COMMON_ERROR_VALUE
		"encrypt error, please check your encrypt key.", //���ܴ���
		"decrypt error, please check your decrypt key.", //���ܴ���
		"verify signature failed, this is a invalid access.", //��֤ǩ������
		"nonce verify faild, this must be a hostile attack", //�����У��ʧ�ܴ���
		nullptr
	};

	//persistence ������
	static const char* pArrPersistenceErrorString[EN_PERSISTENCE_ERROR_CODE_NUM] = {
		"persistence error begin value", //��Ӧ EN_MAX_COMMON_ERROR_VALUE
		"block cache in order-cps is full, may be distribute block encoter problem.", // order-cps�� block cache����
		"version is conflic when update world state", //��ˮ�Ų�һ�£�������ͻ ����
		"write-set system id used up", //�û���ϵͳID�ľ�
		"read const length table failed", //��ȡ�����洢��ʧ��
		"modify(append/update) const length table failed", //�޸Ķ����洢��ʧ��
		"write disk failed",//д����ʧ��
		nullptr
	};

	//cc ������
	static const char* pArrCcErrorString[EN_CC_ERROR_CODE_NUM] = {
		"cc error begin value", //��Ӧ EN_MAX_COMMON_ERROR_VALUE
		"cc common error", //cc���ͨ�ô���
		"cc refused regist user", //cc�ܾ��û�ע��
		nullptr
	};


	if (nErrorCode == VBH::EN_OK_TYPE)
	{
		return pOkString;
	}
	else if ((nErrorCode <= EN_MAX_COMMON_ERROR_VALUE) && (nErrorCode > (EN_MAX_COMMON_ERROR_VALUE - EN_COMMON_ERROR_CODE_NUM)))
	{
		nErrorCode -= EN_MAX_COMMON_ERROR_VALUE;
		DSC_ABS(nErrorCode); //��nErrorCodeת��Ϊ�����±�

		return pArrCommonErrorString[nErrorCode];
	}
	else if ((nErrorCode <= EN_MAX_CONFIG_ERROR_VALUE) && (nErrorCode > (EN_MAX_CONFIG_ERROR_VALUE - EN_CONFIG_ERROR_CODE_NUM)))
	{
		nErrorCode -= EN_MAX_CONFIG_ERROR_VALUE;
		DSC_ABS(nErrorCode); //��nErrorCodeת��Ϊ�����±�

		return pArrConfigErrorString[nErrorCode];
	}
	else if ((nErrorCode <= EN_MAX_CRYPT_ERROR_VALUE) && (nErrorCode > (EN_MAX_CRYPT_ERROR_VALUE - EN_CRYPT_ERROR_CODE_NUM)))
	{
		nErrorCode -= EN_MAX_CRYPT_ERROR_VALUE;
		DSC_ABS(nErrorCode); //��nErrorCodeת��Ϊ�����±�

		return pArrCryptErrorString[nErrorCode];
	}
	else if ((nErrorCode <= EN_MAX_PERSISTENCE_ERROR_VALUE) && (nErrorCode > (EN_MAX_PERSISTENCE_ERROR_VALUE - EN_PERSISTENCE_ERROR_CODE_NUM)))
	{
		nErrorCode -= EN_MAX_PERSISTENCE_ERROR_VALUE;
		DSC_ABS(nErrorCode); //��nErrorCodeת��Ϊ�����±�

		return pArrPersistenceErrorString[nErrorCode];
	}
	else if ((nErrorCode <= EN_MAX_CC_ERROR_VALUE) && (nErrorCode > (EN_MAX_CC_ERROR_VALUE - EN_CC_ERROR_CODE_NUM)))
	{
		nErrorCode -= EN_MAX_CC_ERROR_VALUE;
		DSC_ABS(nErrorCode); //��nErrorCodeת��Ϊ�����±�

		return pArrCcErrorString[nErrorCode];
	}
	else
	{
		return nullptr;
	}
}