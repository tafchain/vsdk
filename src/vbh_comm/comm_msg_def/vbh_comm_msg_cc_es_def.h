#ifndef COMM_MSG_CC_ES_DEF_H_784546743434576437867967354976543246754
#define COMM_MSG_CC_ES_DEF_H_784546743434576437867967354976543246754

#include "dsc/codec/dsc_codec/dsc_codec.h"
#include "dsc/mem_mng/dsc_stl_type.h"

#include "vbh_comm/vbh_comm_id_def.h"
#include "vbh_comm/vbh_comm_def_export.h"
#include "vbh_comm/comm_msg_def/vbh_comm_class_def.h"

namespace VBH
{
	// endorser -> cc
	class CRegistUserEsCcReq
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_REGIST_USER_ES_CC_REQ
		};

	public:
		DSC_BIND_ATTR(m_nEsSessionID, m_nChannelID, m_userInfo);

	public:
		ACE_UINT32 m_nEsSessionID; //endorser �˵�sessionID
		ACE_UINT32 m_nChannelID; //�û�����channel
		DSC::CDscShortBlob m_userInfo; //ע��ʱЯ���û���Ϣ
	};

	//cc -> endorser
	class CRegistUserCcEsRsp
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_REGIST_USER_CC_ES_RSP
		};

	public:
		DSC_BIND_ATTR(m_nReturnCode, m_nEsSessionID, m_userInitInfo);

	public:
		ACE_INT32 m_nReturnCode;
		ACE_UINT32 m_nEsSessionID;
		DSC::CDscShortBlob m_userInitInfo; //ע���ĳ�ʼ��Ϣ������������
	};

		//endorser -> cc
	class CProposeEsCcReq
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_PROPOSE_ES_CC_REQ
		};

	public:
		DSC_BIND_ATTR(m_nEsSessionID, m_nActionID, m_nChannelID, m_proposeUserKey, m_signature, m_proposal);

	public:
		ACE_UINT32 m_nEsSessionID;
		ACE_UINT32 m_nActionID;
		ACE_UINT32 m_nChannelID;
		VBH::CVbhAllocatedKey m_proposeUserKey; //�᰸������
		DSC::CDscShortBlob m_signature; //�ͻ����ύ�ģ�����᰸��ǩ��
		DSC::CDscShortBlob m_proposal;//�ͻ����ύ���᰸
	};

	//cc -> endorser
	class CProposeCcEsRsp
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_PROPOSE_CC_ES_RSP
		};

	public:
		DSC_BIND_ATTR(m_nActionID, m_nReturnCode, m_nEsSessionID, m_receipt, m_transContent);

	public:
		ACE_UINT32 m_nActionID;
		ACE_INT32 m_nReturnCode;
		ACE_UINT32 m_nEsSessionID;
		DSC::CDscShortBlob m_receipt; //ccΪ�᰸���ɵĻ�ִ
		DSC::CDscBlob m_transContent; //�������� ��Ӧ CRoughProposalTransactionWrapper �ṹ�� Ϊ�����÷�װ�����Ч�ʣ�����ɢ�����ݴ�������Ǹô���������������е�����
	};


	
	
	// endorser -> cc
	class CLoadOuterCcEsCcReq
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_LOAD_OUTER_CC_ES_CC_REQ
		};

	public:
	public:
		DSC_BIND_ATTR(m_nChannelID, m_nSessionID, m_nCcID, m_ccName);

	public:
		ACE_UINT32 m_nChannelID; //�û�����channel
		ACE_UINT32 m_nSessionID; // �ϲ�ҵ��service��������ʱ�����ص�sessionID
		ACE_UINT32 m_nCcID;       //cc_id
		DSC::CDscShortBlob m_ccName; //CC��dll����
	};
	//cc -> endorser
	class CLoadOuterCcCcEsRsp
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_LOAD_OUTER_CC_CC_ES_RSP
		};

	public:
		DSC_BIND_ATTR(m_nReturnCode, m_nEsSessionID);

	public:
		ACE_INT32 m_nReturnCode;
		ACE_UINT32 m_nEsSessionID;
	};
	
		//endorser -> cc
	class CQueryEsCcReq
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_QUERY_ES_CC_REQ
		};

	public:
		DSC_BIND_ATTR(m_nEsSessionID, m_nActionID, m_nChannelID, m_param);

	public:
		ACE_UINT32 m_nEsSessionID;
		ACE_UINT32 m_nActionID; //��ѯʱָ����action-id
		ACE_UINT32 m_nChannelID;
		DSC::CDscShortBlob m_param; //��ѯ����
	};

	//cc -> endorser
	class CQueryCcEsRsp
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_QUERY_CC_ES_RSP
		};

	public:
		DSC_BIND_ATTR(m_nReturnCode, m_nEsSessionID, m_info);

	public:
		ACE_INT32 m_nReturnCode;
		ACE_UINT32 m_nEsSessionID;
		DSC::CDscBlob m_info; //cc���ɵĲ�ѯ����
	};

}


#endif
