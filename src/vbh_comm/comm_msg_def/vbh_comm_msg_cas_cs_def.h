#ifndef COMM_MSG_CAS_CS_DEF_H_456465465461245455678577867968878
#define COMM_MSG_CAS_CS_DEF_H_456465465461245455678577867968878
#include "dsc/codec/dsc_codec/dsc_codec.h"
#include "dsc/mem_mng/dsc_stl_type.h"

#include "vbh_comm/vbh_comm_id_def.h"
#include "vbh_comm/vbh_comm_def_export.h"
#include "vbh_comm/comm_msg_def/vbh_comm_class_def.h"

namespace VBH
{


	//peer-cs -> other-peer-cas //��ȡ����
	class CGetBlockCsCasReq
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_GET_BLOCK_CS_CAS_REQ
		};

	public:
		DSC_BIND_ATTR(m_nBlockID);

	public:
		ACE_UINT64 m_nBlockID; //Ҫ��ȡ������
	};

	//other-peer-cas -> peer-cs //��ȡ����Ӧ��
	class CGetBlockCasCsRsp
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_GET_BLOCK_CAS_CS_RSP
		};

	public:
		DSC_BIND_ATTR(m_nReturnCode, m_nBlockID, m_blockData);

	public:
		ACE_INT32 m_nReturnCode;
		ACE_UINT64 m_nBlockID; //Ҫ��ȡ������
		DSC::CDscBlob m_blockData; //��������
	};


	//peer-cs -> other-peer-cas //У��ָ�������hash������
	class CCheckBlockHashCsCasReq
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_CHECK_BLOCK_HASH_CS_CAS_REQ
		};

	public:
		DSC_BIND_ATTR(m_nBlockID, m_strBlockHash);

	public:
		ACE_UINT64 m_nBlockID; //ҪУ�������
		CDscString m_strBlockHash; //ָ�������hashֵ
	};

	//other-peer-cas -> peer-cs //У��ָ�������hashӦ��
	class CCheckBlockHashCasCsRsp
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_CHECK_BLOCK_HASH_CAS_CS_RSP
		};

	public:
		enum
		{
			EN_HASH_CHECK_FAILED = 0,
			EN_HASH_CHECK_OK
		};

	public:
		DSC_BIND_ATTR(m_nReturnCode, m_nCheckResult, m_nBlockID, m_blockData);

	public:
		ACE_INT32 m_nReturnCode;
		ACE_UINT32 m_nCheckResult; //����hash��У���������У��ʧ�ܣ��ڻ������ȷ���������
		ACE_UINT64 m_nBlockID; //ҪУ�������
		DSC::CDscBlob m_blockData; //�������ݣ����У�鲻ͨ��������ֶ�Я����ȷ����������
	};

	class CVerifyPeerStateCsCasReq
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_VERIFY_PEER_STATE_CS_CAS_REQ
		};

	public:
		DSC_BIND_ATTR(m_nKafkaBlockID, m_strKafkaBlockHash);

	public:
		ACE_UINT32 m_nKafkaBlockID;
		CDscString m_strKafkaBlockHash;
	};

	// //��֤peer�Ƿ�����������
	class CVerifyPeerStateCasCsRsp
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_VERIFY_PEER_STATE_CAS_CS_RSP
		};

	public:
		enum
		{
			EN_VERIFY_OK, //��֤ͨ��
			EN_VERIFY_FAILED //��֤δͨ��
		};

	public:
		DSC_BIND_ATTR(m_nReturnCode, m_nVerifyResult, m_CasAddress);

	public:
		ACE_INT32 m_nReturnCode;
		ACE_UINT32 m_nVerifyResult;
		CSyncSourcePeerCasAddress m_CasAddress;
	};
}
#endif