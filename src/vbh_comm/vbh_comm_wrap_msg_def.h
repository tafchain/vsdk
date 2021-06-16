#ifndef __VBH_COMMMON_WRAPPER_MESSAGE_DEF_H
#define __VBH_COMMMON_WRAPPER_MESSAGE_DEF_H

#include "dsc/codec/dsc_codec/dsc_codec.h"
#include "dsc/container/dsc_array.h"

#include "vbh_comm/vbh_comm_def_export.h"
#include "vbh_comm/comm_msg_def/vbh_comm_class_def.h"

namespace VBH
{
	enum __SUBMIT_NODE_TYPE
	{
		EN_NOT_SUBMIT_NODE_TYPE = 0, //���ύ�ڵ�
		EN_IS_SUBMIT_NODE_TYPE, //�ύ�ڵ�
		EN_ONLY_ONE_SUBMIT_NODE_TYPE //Ψһ�ύ�ڵ�
	};

	//====================ע���û�����======================
	class CCryptRegistUserCltEsReqDataWrapper
	{
	public:
		VBH_COMM_DEF_EXPORT CCryptRegistUserCltEsReqDataWrapper(bool& bSubmitNode, ACE_UINT32& nChannelID, DSC::CDscShortBlob& userInfo, ACE_INT32& nNonce);

	public:
		DSC_BIND_ATTR(m_bSubmitNode, m_nChannelID, m_userInfo, m_nNonce);

	public:
		bool& m_bSubmitNode; //����Ƿ�ѡ�е�
		ACE_UINT32& m_nChannelID; //�û�����channel
		DSC::CDscShortBlob& m_userInfo; //�û���Ϣ
		ACE_INT32& m_nNonce; //ͨ���������
	};

	class CCryptRegistUserEsCltRspDataWrapper
	{
	public:
		VBH_COMM_DEF_EXPORT CCryptRegistUserEsCltRspDataWrapper(DSC::CDscShortBlob& userInitInfo, ACE_INT32& nNonce);

	public:
		DSC_BIND_ATTR(m_userInitInfo, m_nNonce);

	public:
		DSC::CDscShortBlob& m_userInitInfo; //����ע��ʱ�ĳ�ʼ��Ϣ
		ACE_INT32& m_nNonce; //ͨ���������
	};

	class CCryptSubmitRegistUserTransactionCltEsReqDataWrapper
	{
	public:
		VBH_COMM_DEF_EXPORT CCryptSubmitRegistUserTransactionCltEsReqDataWrapper(DSC::CDscShortBlob& cltPubKey, ACE_INT32& nNonce);

	public:
		DSC_BIND_ATTR(m_cltPubKey, m_nNonce);

	public:
		DSC::CDscShortBlob& m_cltPubKey; // �ͻ��˹�Կ
		ACE_INT32& m_nNonce; //ͨ���������
	};

	class CCryptSubmitRegistUserTransactionEsCltRspDataWrapper
	{
	public:
		VBH_COMM_DEF_EXPORT CCryptSubmitRegistUserTransactionEsCltRspDataWrapper(DSC::CDscShortBlob& svrPubKey, VBH::CVbhAllocatedKey& userKey, VBH::CTransactionUrl& transUrl, ACE_INT32& nNonce);

	public:
		DSC_BIND_ATTR(m_svrPubKey, m_userKey, m_transUrl, m_nNonce);

	public:
		DSC::CDscShortBlob& m_svrPubKey;
		VBH::CVbhAllocatedKey& m_userKey;
		VBH::CTransactionUrl& m_transUrl;
		ACE_INT32& m_nNonce;
	};

	//====================����information����======================
	class CCryptCreateInformationCltEsReqDataWrapper
	{
	public:
		VBH_COMM_DEF_EXPORT CCryptCreateInformationCltEsReqDataWrapper(ACE_UINT8& nSubmitNodeType,
			ACE_INT32& nNonce, ACE_UINT32& nActionID,
			DSC::CDscShortBlob& signature, DSC::CDscShortBlob& proposal);

	public:
		DSC_BIND_ATTR(m_nSubmitNodeType, m_nNonce, m_nActionID, m_signature, m_proposal);

	public:
		ACE_UINT8& m_nSubmitNodeType; //����Ƿ�ѡ�е�
		ACE_INT32& m_nNonce; //ͨ���������
		ACE_UINT32& m_nActionID;
		DSC::CDscShortBlob& m_signature; //�᰸ǩ��
		DSC::CDscShortBlob& m_proposal; //�᰸
	};

	class CCryptCreateInformationEsCltRspDataWrapper
	{
	public:
		VBH_COMM_DEF_EXPORT CCryptCreateInformationEsCltRspDataWrapper(DSC::CDscBlob& initValue, ACE_INT32& nNonce);

	public:
		DSC_BIND_ATTR(m_initValue, m_nNonce);

	public:
		DSC::CDscBlob& m_initValue; //����ע��ʱ�ĳ�ʼ��Ϣ
		ACE_INT32& m_nNonce; //ͨ���������
	};

	class CCryptSubmitCreateInformationTransactionEsCltRspDataWrapper
	{
	public:
		VBH_COMM_DEF_EXPORT CCryptSubmitCreateInformationTransactionEsCltRspDataWrapper(VBH::CSimpleVector<VBH::CVbhAllocatedKey>& vecInfoKey, ACE_INT32& nNonce);

	public:
		DSC_BIND_ATTR(m_vecInfoKey, m_nNonce);

	public:
		VBH::CSimpleVector<VBH::CVbhAllocatedKey>& m_vecInfoKey;
		ACE_INT32& m_nNonce;
	};

	//====================�����᰸����======================
	class CCryptProposeCltEsReqDataWrapper
	{
	public:
		VBH_COMM_DEF_EXPORT CCryptProposeCltEsReqDataWrapper(ACE_UINT8& nSubmitNodeType, ACE_UINT32& nActionID,
			DSC::CDscShortBlob& signature, DSC::CDscShortBlob& proposal, ACE_INT32& nNonce);

	public:
		DSC_BIND_ATTR(m_nSubmitNodeType, m_nActionID, m_signature, m_proposal, m_nNonce);

	public:
		ACE_UINT8& m_nSubmitNodeType; //֪ͨendorser����session�����ں���commit transaction
		ACE_UINT32& m_nActionID;
		DSC::CDscShortBlob& m_signature; //�᰸ǩ��
		DSC::CDscShortBlob& m_proposal;
		ACE_INT32& m_nNonce;
	};

	class CCryptSubmitProposalEsCltRspDataWrapper
	{
	public:
		VBH_COMM_DEF_EXPORT CCryptSubmitProposalEsCltRspDataWrapper(DSC::CDscBlob& transContent, ACE_INT32& nNonce);

	public:
		DSC_BIND_ATTR(m_transContent, m_nNonce);

	public:
		DSC::CDscBlob& m_transContent; //��������
		ACE_INT32& m_nNonce;
	};

	class CCryptSubmitProposalTransactionEsCltRspDataWrapper
	{
	public:
		VBH_COMM_DEF_EXPORT CCryptSubmitProposalTransactionEsCltRspDataWrapper(VBH::CVbhAllocatedTransactionKey& alocTransKey, DSC::CDscShortBlob& receipt, ACE_INT32& nNonce);

	public:
		DSC_BIND_ATTR(m_alocTransKey, m_nNonce);

	public:
		VBH::CVbhAllocatedTransactionKey& m_alocTransKey;
		DSC::CDscShortBlob& m_receipt;
		ACE_INT32& m_nNonce;
	};

	//====================��ѯ����======================
	class CCryptQueryCltEsReqDataWrapper
	{
	public:
		VBH_COMM_DEF_EXPORT CCryptQueryCltEsReqDataWrapper(ACE_INT32& nNonce, ACE_UINT32& nActionID, DSC::CDscShortBlob& param);

	public:
		DSC_BIND_ATTR(m_nNonce, m_nActionID, m_param);

	public:
		ACE_INT32& m_nNonce;
		ACE_UINT32& m_nActionID;
		DSC::CDscShortBlob& m_param;
	};

	class CCryptQueryEsCltRspDataWrapper
	{
	public:
		VBH_COMM_DEF_EXPORT CCryptQueryEsCltRspDataWrapper(DSC::CDscBlob& info, ACE_INT32& nNonce);

	public:
		DSC_BIND_ATTR(m_info, m_nNonce);

	public:
		DSC::CDscBlob& m_info;
		ACE_INT32& m_nNonce;
	};

	//====================ֱ�Ӳ�ѯ��������======================
	class CCryptQueryTransactionReqDataWrapper
	{
	public:
		VBH_COMM_DEF_EXPORT CCryptQueryTransactionReqDataWrapper(ACE_INT32& nNonce, VBH::CVbhAllocatedTransactionKey& transKey);

	public:
		DSC_BIND_ATTR(m_nNonce, m_transKey);

	public:
		ACE_INT32& m_nNonce;
		VBH::CVbhAllocatedTransactionKey& m_transKey;
	};

	class CCryptQueryTransactionRspDataWrapper
	{
	public:
		VBH_COMM_DEF_EXPORT CCryptQueryTransactionRspDataWrapper(ACE_INT32& nNonce, VBH::CVbhAllocatedKey& userKey, DSC::CDscShortBlob& signature, DSC::CDscShortBlob& proposal);

	public:
		DSC_BIND_ATTR(m_nNonce, m_userKey, m_signature, m_proposal);

	public:
		ACE_INT32& m_nNonce;
		VBH::CVbhAllocatedKey& m_userKey; //�����᰸���û���Ҳ�Ƕ��᰸����ǩ�����û�
		DSC::CDscShortBlob& m_signature; //�᰸ǩ��
		DSC::CDscShortBlob& m_proposal; //�����е��᰸
	};

	//====================ֱ�Ӳ�ѯ��������======================
	class CCryptQueryBlockReqDataWrapper
	{
	public:
		VBH_COMM_DEF_EXPORT CCryptQueryBlockReqDataWrapper(ACE_INT32& nNonce, ACE_UINT64& nBlockID);

	public:
		DSC_BIND_ATTR(m_nNonce, m_nBlockID);

	public:
		ACE_INT32& m_nNonce;
		ACE_UINT64& m_nBlockID;
	};

	class CCryptQueryBlockRspDataWrapper
	{
	public:
		VBH_COMM_DEF_EXPORT CCryptQueryBlockRspDataWrapper(ACE_UINT16& nTransCount, ACE_INT32& nNonce, ACE_UINT64& nBlockTime, DSC::CDscShortBlob& preBlockHash, DSC::CDscShortBlob& merkelTreeRootHash);

	public:
		DSC_BIND_ATTR(m_nTransCount, m_nNonce, m_nBlockTime, m_preBlockHash, m_merkelTreeRootHash);

	public:
		ACE_UINT16& m_nTransCount; //����������е�������� //��������಻��
		ACE_INT32& m_nNonce;
		ACE_UINT64& m_nBlockTime; //��������ʱ��
		DSC::CDscShortBlob& m_preBlockHash; //ǰһ�����hashֵ
		DSC::CDscShortBlob& m_merkelTreeRootHash; //������merkel������hashֵ
	};

}

#endif // !__vbh_commmon_wrapper_message_h

