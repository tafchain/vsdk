#ifndef COMM_MSG_ES_DEF_H_78979876467967967674579878949678979454678
#define COMM_MSG_ES_DEF_H_78979876467967967674579878949678979454678

#include "dsc/codec/dsc_codec/dsc_codec.h"
#include "dsc/mem_mng/dsc_stl_type.h"

#include "vbh_comm/vbh_comm_id_def.h"
#include "vbh_comm/vbh_comm_def_export.h"
#include "vbh_comm/comm_msg_def/vbh_comm_class_def.h"

namespace VBH
{

	
	// client -> endorser 
	class CLoadOuterCcUserCltEsReq
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_LOAD_OUTER_CC_CLT_ES_REQ
		};

	public:
		DSC_BIND_ATTR(m_nChannelID, m_nSessionID, m_nCcID, m_ccName);

	public:
		ACE_UINT32 m_nChannelID; //�û�����channel
		ACE_UINT32 m_nSessionID; // �ϲ�ҵ��service��������ʱ�����ص�sessionID
		ACE_UINT32 m_nCcID;       //cc_id
		DSC::CDscShortBlob m_ccName; //CC��dll����
	};

	// endorser -> client
	class CLoadOuterCcEsCltRsp
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_LOAD_OUTER_CC_ES_CLT_RSP
		};

	public:
		DSC_BIND_ATTR(m_nReturnCode, m_nCltSessionID);

	public:
		ACE_INT32 m_nReturnCode;
		ACE_UINT32 m_nCltSessionID;
	};

//===============================ע������=��Ϣ==================================

	class CCryptRegistUserCltEsReq
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_CRYPT_REGIST_USER_CLT_ES_REQ
		};

	public:
		DSC_BIND_ATTR(m_nCltSessionID, m_userData);

	public:
		ACE_UINT32 m_nCltSessionID;
		DSC::CDscShortBlob m_userData; //bWaitNotify + bSubmitNode + cc-id + channel-id + user-info + nonce
	};

	// endorser -> client
	class CCryptRegistUserEsCltRsp
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_CRYPT_REGIST_USER_ES_CLT_RSP
		};

	public:
		DSC_BIND_ATTR(m_nReturnCode, m_nCltSessionID, m_nEsSessionID, m_userData);

	public:
		ACE_INT32 m_nReturnCode;
		ACE_UINT32 m_nCltSessionID;
		ACE_UINT32 m_nEsSessionID; //endorser �˵�session id
		DSC::CDscBlob m_userData; //���ܺ���û����ݣ��û���ʼ��Ϣ + �����
	};

	// client -> endorser 
	class CRegistUserCltEsReq
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_REGIST_USER_CLT_ES_REQ
		};

	public:
		DSC_BIND_ATTR(m_bSubmitNode, m_nCltSessionID, m_nChannelID, m_userInfo);

	public:
		bool m_bSubmitNode; //����Ƿ�ѡ�е�
		ACE_UINT32 m_nCltSessionID;
		ACE_UINT32 m_nChannelID; //�û�����channel
		DSC::CDscShortBlob m_userInfo; //�û���Ϣ
	};

	// endorser -> client
	class CRegistUserEsCltRsp
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_REGIST_USER_ES_CLT_RSP
		};

	public:
		DSC_BIND_ATTR(m_nReturnCode, m_nCltSessionID, m_nEsSessionID, m_userInitInfo);

	public:
		ACE_INT32 m_nReturnCode;
		ACE_UINT32 m_nCltSessionID;
		ACE_UINT32 m_nEsSessionID; //endorser �˵�session id
		DSC::CDscShortBlob m_userInitInfo; //����ע��ʱ�ĳ�ʼ��Ϣ
	};

//client -> endorser //����ͨ���������ύ�û�ע�����������
	class CCryptSubmitRegistUserTransactionCltEsReq
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_CRYPT_SUBMIT_REGIST_USER_TRANSACTION_CLT_ES_REQ
		};

	public:
		DSC_BIND_ATTR(m_nEsSessionID, m_userData);

	public:
		ACE_UINT32 m_nEsSessionID; //endorser �˵�sessionID
		DSC::CDscShortBlob m_userData; //���ܺ�: �ͻ��˹�Կ + �����˽Կ + �Գ���Կ �����
	};

	//endorser -> client
	class CCryptSubmitRegistUserTransactionEsCltRsp
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_CRYPT_SUBMIT_REGIST_USER_TRANSACTION_ES_CLT_RSP
		};

	public:
		DSC_BIND_ATTR(m_nReturnCode, m_nCltSessionID, m_userData);

	public:
		ACE_INT32 m_nReturnCode;
		ACE_UINT32 m_nCltSessionID;
		DSC::CDscShortBlob m_userData; //���ܺ�����: nonce +  allocUserKey+svrPubCryptKey+cltPriCryptKey+envelopeKey
	};

	//client -> endorser //����ͨ���������ύ�û�ע�������
	class CSubmitRegistUserTransactionCltEsReq
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_SUBMIT_REGIST_USER_TRANSACTION_CLT_ES_REQ
		};

	public:
		DSC_BIND_ATTR(m_nEsSessionID, m_cltPubKey, m_svrPriKey, m_envelopeKey);

	public:
		ACE_UINT32 m_nEsSessionID; //endorser �˵�sessionID
		DSC::CDscShortBlob m_cltPubKey; // �ͻ��˹�Կ
		DSC::CDscShortBlob m_svrPriKey; //�����˽Կ
		DSC::CDscShortBlob m_envelopeKey; //�Գ���Կ
	};

	//endorser -> client
	//���ظ��ͻ��˵���Կ���ӳٵ��ͻ��˱���ͨ������endorser������
	class CSubmitRegistUserTransactionEsCltRsp
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_SUBMIT_REGIST_USER_TRANSACTION_ES_CLT_RSP
		};

	public:
		DSC_BIND_ATTR(m_nReturnCode, m_nCltSessionID, m_userKey);

	public:
		ACE_INT32 m_nReturnCode;
		ACE_UINT32 m_nCltSessionID;
		VBH::CVbhAllocatedKey m_userKey; //vbhϵͳΪע���û����ɵ�key
	};

	// endorser service -> transform agent service
	class CSubmitRegistUserTransactionEsTasReq
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_SUBMIT_REGIST_USER_TRANSACTION_ES_TAS_REQ
		};

	public:
		DSC_BIND_ATTR(m_nEsSessionID, m_nChannelID, m_userInfo, m_cltPubKey, m_svrPriKey, m_envelopeKey);

	public:
		ACE_UINT32 m_nEsSessionID;
		ACE_UINT32 m_nChannelID;

		DSC::CDscShortBlob m_userInfo; //�������� -//ccΪ�û����ɵĳ�ʼ��Ϣ
		DSC::CDscShortBlob m_cltPubKey; //��������  -// �ͻ��˹�Կ
		DSC::CDscShortBlob m_svrPriKey; //��������  -//�����˽Կ
		DSC::CDscShortBlob m_envelopeKey; //��������  -//�Գ���Կ
	};

	//transform agent service -> endorser service  �ύע������Ӧ��
	class CSubmitRegistUserTransactionTasEsRsp
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_SUBMIT_REGIST_USER_TRANSACTION_TAS_ES_RSP
		};

	public:
		DSC_BIND_ATTR(m_nReturnCode, m_nEsSessionID, m_userKey, m_registTransUrl);

	public:
		ACE_INT32 m_nReturnCode;
		ACE_UINT32 m_nEsSessionID;
		VBH::CVbhAllocatedKey m_userKey; //ע�����ɵ�user-key
		VBH::CTransactionUrl m_registTransUrl; //ע�������URL�������а�����Կ //
	};

	//transform agent service -> order-cps
	class CSubmitRegistUserTransactionTasCpsReq
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_SUBMIT_REGIST_USER_TRANSACTION_TAS_CPS_REQ
		};

	public:
		DSC_BIND_ATTR(m_nTasSessionID, m_userInfo, m_cltPubKey, m_svrPriKey, m_envelopeKey);

	public:
		ACE_UINT32 m_nTasSessionID;

		DSC::CDscShortBlob m_userInfo; //�������� -//ccΪ�û����ɵĳ�ʼ��Ϣ
		DSC::CDscShortBlob m_cltPubKey; //��������  -//ע��ʱ������Կ�� �ͻ��˹�Կ
		DSC::CDscShortBlob m_svrPriKey; //��������  -//ע��ʱ������Կ�������˽Կ
		DSC::CDscShortBlob m_envelopeKey; //��������  -//ע��ʱ������Կ���Գ���Կ
	};

	//order-cps -> transform agent service  �ύע������Ӧ��
	class CSubmitRegistUserTransactionCpsTasRsp
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_SUBMIT_REGIST_USER_TRANSACTION_CPS_TAS_RSP
		};

	public:
		DSC_BIND_ATTR(m_nReturnCode, m_nTasSessionID, m_userKey, m_registTransUrl);

	public:
		ACE_INT32 m_nReturnCode;
		ACE_UINT32 m_nTasSessionID;
		VBH::CVbhAllocatedKey m_userKey; //ע�����ɵ�user-key
		VBH::CTransactionUrl m_registTransUrl; //ע�������URL�������а�����Կ //
	};

	//�ύ���������ͬ�᰸����

	

	//�ύ���������ͬ�᰸����

	//===============================�����᰸ ����=��Ϣ==================================
//client -> endorser
	class CCryptProposeCltEsReq
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_CRYPT_PROPOSE_CLT_ES_REQ
		};

	public:
		DSC_BIND_ATTR(m_nActionID, m_nChannelID, m_nCltSessionID, m_userKey, m_transUrl, m_userData);

	public:
		ACE_UINT32 m_nActionID;
		ACE_UINT32 m_nChannelID;
		ACE_UINT32 m_nCltSessionID; //�ͻ���sessionID
		VBH::CVbhAllocatedKey m_userKey; //�᰸������ //ע��ɹ�ʱ���صģ�ϵͳΪ��ע���û������user-key
		VBH::CTransactionUrl m_transUrl; //�û�ע��ʱ�����ע����Ϣ������url
		DSC::CDscBlob m_userData; //���ܺ����ݣ�nonce + signature + proposal + action-id
	};

	//endorser -> client
	class CCryptProposeEsCltRsp
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_CRYPT_PROPOSE_ES_CLT_RSP
		};

	public:
		DSC_BIND_ATTR(m_nActionID, m_nReturnCode, m_nEsSessionID, m_nCltSessionID, m_userData);

	public:
		ACE_UINT32 m_nActionID;
		ACE_INT32 m_nReturnCode;
		ACE_UINT32 m_nEsSessionID; //endorser session id
		ACE_UINT32 m_nCltSessionID; //client session id
		DSC::CDscBlob m_userData; //���ܺ����ݣ� nonce + transaction-content(transaction��contentnt�������ʽ���)
	};

	//client -> endorser
	class CProposeCltEsReq
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_PROPOSE_CLT_ES_REQ
		};

	public:
		DSC_BIND_ATTR(m_bSubmitNode, m_nChannelID, m_nCltSessionID, m_nActionID, m_userKey, m_signature, m_proposal);

	public:
		bool m_bSubmitNode; //֪ͨendorser����session�����ں���commit transaction
		ACE_UINT32 m_nChannelID;
		ACE_UINT32 m_nCltSessionID; //�ͻ���sessionID
		ACE_UINT32 m_nActionID;
		VBH::CVbhAllocatedKey m_userKey; //�᰸������
		DSC::CDscShortBlob m_signature; //����᰸��ǩ��
		DSC::CDscShortBlob m_proposal; //�᰸
	};

	//endorser -> client
	//��cc���ɵ��������ݾ��Ǵ���õģ�ֻ�Ǵ��ʱ���½�information�͸���information���ڲ�ͬ�Ķ��У�ά��Ŀǰ�������е�״̬��
	class CProposeEsCltRsp
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_PROPOSE_ES_CLT_RSP
		};

	public:
		DSC_BIND_ATTR(m_nReturnCode, m_nEsSessionID, m_nCltSessionID, m_transContent);

	public:
		ACE_INT32 m_nReturnCode;
		ACE_UINT32 m_nEsSessionID; //endorser session id
		ACE_UINT32 m_nCltSessionID; //client session id
		DSC::CDscBlob m_transContent; //��������
	};

	//endoser -> x committer service
	class CQueryCryptKeyProposeEsCsReq
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_QUERY_CRYPT_KEY_PROPOSE_ES_CS_REQ
		};

	public:
		DSC_BIND_ATTR(m_transUrl, m_nEsSessionID);

	public:
		VBH::CTransactionUrl m_transUrl; //�û�ע��ʱ�����ע����Ϣ��transaction-url
		ACE_UINT32 m_nEsSessionID;
	};

	//x committer service -> endorser
	class CQueryCryptKeyProposeCsEsRsp
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_QUERY_CRYPT_KEY_PROPOSE_CS_ES_RSP
		};

	public:
		DSC_BIND_ATTR(m_nReturnCode, m_nEsSessionID, m_cltPubKey, m_envelopeKey);

	public:
		ACE_INT32 m_nReturnCode;
		ACE_UINT32 m_nEsSessionID;
		DSC::CDscShortBlob m_cltPubKey; //�û���Կ
		DSC::CDscShortBlob m_envelopeKey;  //�õ��ĶԳ���Կ
	};

// client -> endorser
	class CCryptSubmitProposalTransactionCltEsReq
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_CRYPT_SUBMIT_PROPOSAL_TRANSACTION_CLT_ES_REQ
		};

	public:
		DSC_BIND_ATTR(m_nActionID, m_nEsSessionID, m_userData);

	public:
		ACE_UINT32 m_nActionID;
		ACE_UINT32 m_nEsSessionID;
		DSC::CDscShortBlob m_userData; //���ܺ����ݣ�nonce
	};

	// endorser -> sdk �ύ�᰸�����Ӧ��
	class CCryptSubmitProposalTransactionEsCltRsp
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_CRYPT_SUBMIT_PROPOSAL_TRANSACTION_ES_CLT_RSP
		};

	public:
		DSC_BIND_ATTR(m_nActionID,m_nReturnCode, m_nCltSessionID, m_userData,m_vecInfoID);

	public:
		ACE_UINT32 m_nActionID;
		ACE_INT32 m_nReturnCode;
		ACE_UINT32 m_nCltSessionID; //client session id
		DSC::CDscShortBlob m_userData; //���ܺ�����: nonce + gen-trans-key
		VBH::CSimpleVector<VBH::CVbhAllocatedKey> m_vecInfoID;
	};

	// client -> endorser
	class CSubmitProposalTransactionCltEsReq
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_SUBMIT_PROPOSAL_TRANSACTION_CLT_ES_REQ
		};

	public:
		DSC_BIND_ATTR(m_nEsSessionID);

	public:
		ACE_UINT32 m_nEsSessionID;
	};

	// endorser -> sdk �ύ�᰸�����Ӧ��
	class CSubmitProposalTransactionEsCltRsp
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_SUBMIT_PROPOSAL_TRANSACTION_ES_CLT_RSP
		};

	public:
		DSC_BIND_ATTR(m_nActionID, m_nReturnCode, m_nCltSessionID, m_alocTransKey,m_vecInfoID);

	public:
		ACE_UINT32 m_nActionID;
		ACE_INT32 m_nReturnCode;
		ACE_UINT32 m_nCltSessionID; //client session id
		VBH::CVbhAllocatedTransactionKey m_alocTransKey;
		VBH::CSimpleVector<VBH::CVbhAllocatedKey> m_vecInfoID;
	};

	// endorser service -> transform agent service
	class CSubmitProposalTransactionEsTasReq
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_SUBMIT_PROPOSAL_TRANSACTION_ES_TAS_REQ
		};

	public:
		DSC_BIND_ATTR(m_nActionID, m_nEsSessionID, m_nChannelID, m_transContent);

	public:
		ACE_UINT32 m_nActionID;
		ACE_UINT32 m_nEsSessionID;
		ACE_UINT32 m_nChannelID;
		DSC::CDscBlob m_transContent; //��������
	};

	//transform agent service -> endorser service �ύ�᰸����Ӧ��
	class CSubmitProposalTransactionTasEsRsp
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_SUBMIT_PROPOSAL_TRANSACTION_TAS_ES_RSP
		};

	public:
		DSC_BIND_ATTR(m_nActionID, m_nReturnCode, m_nEsSessionID, m_alocTransKey, m_vecInfoID);

	public:
		ACE_UINT32 m_nActionID;
		ACE_INT32 m_nReturnCode;
		ACE_UINT32 m_nEsSessionID;
		VBH::CVbhAllocatedTransactionKey m_alocTransKey;
		VBH::CSimpleVector<VBH::CVbhAllocatedKey> m_vecInfoID;
	};

	//transform agent service -> order-cps
	class CSubmitProposalTransactionTasCpsReq
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_SUBMIT_PROPOSAL_TRANSACTION_TAS_CPS_REQ
		};

	public:
		DSC_BIND_ATTR(m_nActionID, m_nTasSessionID, m_nType, m_transContent);

	public:
		ACE_UINT32 m_nActionID;
		ACE_UINT32 m_nTasSessionID;
		ACE_UINT32 m_nType;
		DSC::CDscBlob m_transContent; //��������
	};

	//order-cps -> transform agent service  �ύ�᰸����Ӧ��
	class CSubmitProposalTransactionCpsTasRsp
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_SUBMIT_PROPOSAL_TRANSACTION_CPS_TAS_RSP
		};

	public:
		DSC_BIND_ATTR(m_nActionID, m_nReturnCode, m_nTasSessionID, m_alocTransKey, m_vecInfoID);

	public:
		ACE_UINT32 m_nActionID;
		ACE_INT32 m_nReturnCode;
		ACE_UINT32 m_nTasSessionID;
		VBH::CVbhAllocatedTransactionKey m_alocTransKey;
		VBH::CSimpleVector<VBH::CVbhAllocatedKey> m_vecInfoID;
	};



// client -> endorser
	class CCryptQueryCltEsReq
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_CRYPT_QUERY_USER_INFO_CLT_ES_REQ
		};

	public:
		DSC_BIND_ATTR(m_nCltSessionID, m_nChannelID, m_userKey, m_transUrl, m_userData);

	public:
		ACE_UINT32 m_nCltSessionID;
		ACE_UINT32 m_nChannelID; //user������channel
		VBH::CVbhAllocatedKey m_userKey; //ע��ɹ�ʱ���صģ�ϵͳΪ��ע���û������user-key
		VBH::CTransactionUrl m_transUrl; //����û�ע����Ϣ��transaction-url, ��transaction�д������Կ
		DSC::CDscShortBlob m_userData; //���ܺ����ݣ�nonce + action-id + param
	};

	//endorser -> client
	class CCryptQueryEsCltRsp
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_CRYPT_QUERY_USER_INFO_ES_CLT_RSP
		};

	public:
		DSC_BIND_ATTR(m_nReturnCode, m_nCltSessionID, m_userData);

	public:
		ACE_INT32 m_nReturnCode;
		ACE_UINT32 m_nCltSessionID;
		DSC::CDscShortBlob m_userData; //���ܺ����ݣ�nonce + info // CQueryUserInfoRspWrapper���
	};

	//todo:�Ǽ���ͨ����������
	// client -> endorser
	class CQueryUserInfoCltEsReq
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_QUERY_USER_INFO_CLT_ES_REQ
		};

	public:
		DSC_BIND_ATTR(m_nCltSessionID, m_nChannelID, m_userKey);

	public:
		ACE_UINT32 m_nCltSessionID;
		ACE_UINT32 m_nChannelID;
		VBH::CVbhAllocatedKey m_userKey; //ע��ɹ�ʱ���صģ�ϵͳΪ��ע���û������user-key
	};

	//endorser -> client
	class CQueryUserInfoEsCltRsp
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_QUERY_USER_INFO_ES_CLT_RSP
		};

	public:
		DSC_BIND_ATTR(m_nReturnCode, m_nSessionID, m_userInfo);

	public:
		ACE_INT32 m_nReturnCode;
		ACE_UINT32 m_nSessionID;
		DSC::CDscShortBlob m_userInfo;
	};

	//endorser -> committer service
	class CQueryCryptKeyQueryEsCsReq
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_QUERY_CRYPT_KEY_QUERY_ES_CS_REQ
		};

	public:
		DSC_BIND_ATTR(m_nEsSessionID, m_transUrl);

	public:
		ACE_UINT32 m_nEsSessionID;
		VBH::CTransactionUrl m_transUrl; 
	};

	//committer service -> endorser
	class CQueryCryptKeyQueryCsEsRsp
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_QUERY_CRYPT_KEY_QUERY_CS_ES_RSP
		};

	public:
		DSC_BIND_ATTR(m_nReturnCode, m_nEsSessionID, m_cltPubKey, m_envelopeKey);

	public:
		ACE_INT32 m_nReturnCode;
		ACE_UINT32 m_nEsSessionID;
		DSC::CDscShortBlob m_cltPubKey; //�û���Կ
		DSC::CDscShortBlob m_envelopeKey;  //�Գ���Կ
	};


}


#endif
