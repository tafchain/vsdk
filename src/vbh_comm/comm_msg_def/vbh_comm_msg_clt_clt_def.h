#ifndef COMM_MSG_CLT_CLT_DEF_H_789454464343213764313245646786448575
#define COMM_MSG_CLT_CLT_DEF_H_789454464343213764313245646786448575

#include "dsc/codec/dsc_codec/dsc_codec.h"
#include "dsc/mem_mng/dsc_stl_type.h"

#include "vbh_comm/vbh_comm_id_def.h"
#include "vbh_comm/vbh_comm_def_export.h"

namespace VBH
{
		//client�ϲ�ҵ���߼� -> client service
	class CRegistUserCltCltReq
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_REGIST_USER_CLT_CLT_REQ
		};

	public:
		DSC_BIND_ATTR(m_nChannelID, m_nSessionID, m_userInfo, m_cltPubKey);

	public:
		ACE_UINT32 m_nChannelID; //�û�����channel
		ACE_UINT32 m_nSessionID; // �ϲ�ҵ��service��������ʱ�����ص�sessionID
		DSC::CDscShortBlob m_userInfo; //�ύ��CC���û���Ϣ����CC���û������ر���
		DSC::CDscShortBlob m_cltPubKey; //�ͻ��˹�Կ //Base64�����ʽ
	};


	//client service -> client�ϲ�ҵ���߼�
	class CRegistUserCltCltRsp
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_REGIST_USER_CLT_CLT_RSP
		};

	public:
		DSC_BIND_ATTR(m_nReturnCode, m_nSrcSessionID, m_vbhCryptKey, m_vbhUserID);

	public:
		ACE_INT32 m_nReturnCode; //����ֵΪVBH::EN_OK_TYPE����ע��ɹ�������ֵΪע��ʧ��
		ACE_UINT32 m_nSrcSessionID; // �ϲ�ҵ��service��������ʱ�����ͷ���sessionID
		DSC::CDscShortBlob m_vbhCryptKey; //ע���û�ʱΪ�û����ɵ�vbh��Կ����Ӧ��VBH::CUserSequenceWrapper �ṹ //hex����
		DSC::CDscShortBlob m_vbhUserID; //ע�����ɵ��û�id���ڽ������漰�û�ʱʹ�ã� //ʵ������hex����� VBH::CVbhAllocatedKey
	};

	//client�ϲ�ҵ���߼� -> client service
	class CProposeCltCltReq
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_PROPOSE_CLT_CLT_REQ
		};

	public:
		DSC_BIND_ATTR(m_nActionID, m_nSrcSessionID, m_proposal, m_userCryptKey);

	public:
		ACE_UINT32 m_nActionID;
		ACE_UINT32 m_nSrcSessionID; // �ϲ�ҵ��service��������ʱ�����ص�sessionID
		DSC::CDscShortBlob m_proposal;
		DSC::CDscShortBlob m_userCryptKey; //�����ߵ���Կ //��Ҫ base64����->V���룬����˹�Կ���ͻ���˽Կ���Գ���Կ
	};

	//client service -> client�ϲ�ҵ���߼�
	class CProposeCltCltRsp
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_PROPOSE_CLT_CLT_RSP
		};

	public:
		DSC_BIND_ATTR(m_nActionID, m_nReturnCode, m_nSrcSessionID, m_receipt, m_transKey, m_InfoKey);

	public:
		ACE_UINT32 m_nActionID;
		ACE_INT32 m_nReturnCode;
		ACE_UINT32 m_nSrcSessionID; // �ϲ�ҵ��service��������ʱ�����ͷ���sessionID
		DSC::CDscShortBlob m_receipt; //ccΪ�᰸���ɵĻ�ִ
		DSC::CDscShortBlob m_transKey; //ϵͳΪtransaction���ɵ�key
		DSC::CDscShortBlob m_InfoKey;
	};
	//===============================��ѯ����=��Ϣ==================================
	//client�ϲ�ҵ�� -> client service 
	class CQueryCltCltReq
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_QUERY_CLT_CLT_REQ
		};

	public:
		DSC_BIND_ATTR(m_nDstPeerID, m_nSrcSessionID, m_param, m_nActionID, m_userCryptKey);

	public:
		ACE_UINT16 m_nDstPeerID; //Ҫ��ѯ��peer-id
		ACE_UINT32 m_nSrcSessionID;
		ACE_UINT32 m_nActionID;
		DSC::CDscShortBlob m_param; //�͵�CC�Ĳ�ѯ����
		DSC::CDscShortBlob m_userCryptKey; //base64������ַ���
	};

	//client service  -> client�ϲ�ҵ��
	class CQueryCltCltRsp
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_QUERY_CLT_CLT_RSP
		};

	public:
		DSC_BIND_ATTR(m_nReturnCode, m_nActionID, m_nSrcSessionID, m_info);

	public:
		ACE_INT32 m_nReturnCode;
		ACE_UINT32 m_nActionID;
		ACE_UINT32 m_nSrcSessionID;
		DSC::CDscBlob m_info; //��ѯ�õ�����Ϣ
	};

}


#endif
