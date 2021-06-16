#ifndef COMM_MSG_CLT_CLT_DEF_H_789454464343213764313245646786448575
#define COMM_MSG_CLT_CLT_DEF_H_789454464343213764313245646786448575

#include "dsc/codec/dsc_codec/dsc_codec.h"
#include "dsc/mem_mng/dsc_stl_type.h"

#include "vbh_comm/vbh_comm_id_def.h"
#include "vbh_comm/vbh_comm_def_export.h"
#include "vbh_comm/comm_msg_def/vbh_comm_class_def.h"

namespace VBH
{
	//cs(x committer service) -> order-cps(channel process service)
	class CRegistCsCpsReq
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_REGISTER_CS_CPS_REQ
		};

	public:
		DSC_BIND_ATTR(m_nPeerID, m_nCasPort, m_nMaxBlockID, m_strCasIpAddr, m_strMaxBlockHash);

	public:
		ACE_UINT16 m_nPeerID;
		ACE_UINT16 m_nCasPort; //������������Ķ˿ں�
		ACE_UINT64 m_nMaxBlockID; //peer��ǰ�������߶�
		CDscString m_strCasIpAddr; //�������������IP��ַ
		CDscString m_strMaxBlockHash; //��������Hashֵ
	};

	// order-cps -> cs //ע��ʱ��order����csִ��ͬ��
	class CSyncBlockOnRegistCpsCsNotify
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_SYNC_BLOCK_ON_REGIST_CPS_CS_NOTIFY
		};

		enum
		{
			EN_HASH_INVALID = 0, //�Ƿ�
			EN_HASH_NOT_VERIFY //δ��֤
		};

	public:
		DSC_BIND_ATTR(m_nMaxBlockHashIsValid, m_nSyncSrcPeerID, m_nSyncSrcPort, m_nRegisterBlockID, m_nKafkaBlockID, m_strSyncSrcIpAddr);

	public:
		ACE_UINT8 m_nMaxBlockHashIsValid; //��������hashֵ�Ƿ�Ϸ�
		ACE_UINT16 m_nSyncSrcPeerID; //���Խ�������ͬ����PeerID(ͬ��ԴPeerID)
		ACE_UINT16 m_nSyncSrcPort;
		ACE_UINT64 m_nRegisterBlockID; //peerע��ʱ���͵�Block-id
		ACE_UINT64 m_nKafkaBlockID; //order�˴��kafka��ʶ������߶�
		CDscString m_strSyncSrcIpAddr; //���Խ�������ͬ���ĵ�ַ
	};

	// order-cps -> cs //ע��ʱ��order����csִ��ͬ��
	class CSyncBlockOnRegistCpsCsOnOrderMasterStateNotify
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_SYNC_BLOCK_ON_REGIST_CPS_CS_ON_ORDER_MASTER_STATE_NOTIFY
		};

	public:
		DSC_BIND_ATTR(m_nKafkaBlockID, m_strKafkaBlockHash, m_lstPeerAddr);

	public:
		ACE_UINT64 m_nKafkaBlockID; //order�˴��kafka��ʶ������߶�
		CDscString m_strKafkaBlockHash; //���kafka��ʶ������hash
		DSC::CDscList<CSyncSourcePeerCasAddress> m_lstPeerAddr; //peer��ַ�б�
	};

	//cps -> cs //�Ƿ�peer֪ͨ 
	class CInvalidPeerCpsCsNotify
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_INVALID_PEER_CPS_CS_NOTIFY
		};

	public:
		enum
		{
			EN_REGIST_BLOCK_ID_INVALID //ע��ʱ�ϱ�������ID�Ƿ�
		};
	public:
		DSC_BIND_ATTR(m_nInvalidReason);

	public:
		ACE_INT32 m_nInvalidReason; //֪ͨpeer�Ƿ���ԭ��
	};

	// order-cps -> cs //order����csִ�� �����˸�
	class CBackspaceBlockCpsCsNotify
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_BACKSPACE_BLOCK_CPS_CS_NOTIFY
		};

	public:
		DSC_BIND_ATTR(m_nRegisterBlockID, m_nKafkaBlockID);

	public:
		ACE_UINT64 m_nRegisterBlockID; //peerע��ʱ���͵�Block-id
		ACE_UINT64 m_nKafkaBlockID; //order�˴��kafka��ʶ������߶�
	};

	//order-cps -> peer-cs //master-order����ͬ��K.V����peer
	class CMasterSyncVersionTableCpsCsReq
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_MASTER_SYNC_VERSION_TABLE_CPS_CS_REQ
		};

	public:
		DSC_BIND_ATTR(m_nBlockID);

	public:
		ACE_UINT64 m_nBlockID; //order��Ҫͬ��K.V������߶�
	};

	//peer-cs -> order-cps //master-order����K.V�б�
	class CMasterSyncVersionTableCsCpsRsp
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_MASTER_SYNC_VERSION_TABLE_CS_CPS_RSP
		};

	public:
		DSC_BIND_ATTR(m_nBlockID, m_strBlockHash, m_lstKv);

	public:
		ACE_UINT64 m_nBlockID; //����ID
		CDscString m_strBlockHash; //��ǰ�����hash
		DSC::CDscShortList<VBH::CKeyVersion> m_lstKv; //���ڰ�����K.V�б� //Ϊ�ձ�ʾPeer�˲����ڸÿ��K.V�б�
	};

	//order-cps -> peer-cs //slave-order����ͬ��K.V����peer
	class CSlaveSyncVersionTableCpsCsReq
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_SLAVE_SYNC_VERSION_TABLE_CPS_CS_REQ
		};

	public:
		DSC_BIND_ATTR(m_nBlockID);

	public:
		ACE_UINT64 m_nBlockID; //order������ͬ��K.V������߶�
	};

	//peer-cs -> order-cps //slave-order����K.V�б�
	class CSlaveSyncVersionTableCsCpsRsp
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_SLAVE_SYNC_VERSION_TABLE_CS_CPS_RSP
		};

	public:
		DSC_BIND_ATTR(m_nBlockID, m_strBlockHash, m_lstKv);

	public:
		ACE_UINT64 m_nBlockID; //����ID
		CDscString m_strBlockHash; //��ǰ�����hash
		DSC::CDscShortList<VBH::CKeyVersion> m_lstKv; //���ڰ�����K.V�б� //Ϊ�ձ�ʾPeer�˲����ڸÿ��K.V�б�
	};

	//===============================Peerͬ������=��Ϣ==================================
	//peer-cs -> order-cps //cs��Order��ѯ��Ҫ������ͬ�� ����
	class CQuerySyncSourcePeerCsCpsReq
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_QUERY_SYNC_SOURCE_PEER_CS_CPS_REQ
		};

	public:
		DSC_BIND_ATTR(m_nPeerID, m_nTargetBlockID);

	public:
		ACE_UINT16 m_nPeerID;
		ACE_UINT64 m_nTargetBlockID; //Ŀ������߶�
	};

	//order-cps -> peer-cs //
	class CQuerySyncSourcePeerCpsCsRsp
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_QUERY_SYNC_SOURCE_PEER_CPS_CS_RSP
		};

	public:
		DSC_BIND_ATTR(m_nKafkaBlockID, m_strKafkaBlockHash, m_lstPeerAddr);

	public:
		ACE_UINT64 m_nKafkaBlockID; //order�˴��kafka��ʶ������߶�
		CDscString m_strKafkaBlockHash; //���kafka��ʶ������hash
		DSC::CDscList<CSyncSourcePeerCasAddress> m_lstPeerAddr; //peer��ַ�б�
	};

	// order-cps -> committer-cs
	class CDistributeBlockCpsCsReq
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_DISTRIBUTE_BLOCK_CPS_XCS_REQ
		};

	public:
		DSC_BIND_ATTR(m_nBlockID, m_vbhBlock);

	public:
		ACE_UINT64 m_nBlockID; //����ID
		DSC::CDscBlob m_vbhBlock; //��������
	};

	// committer-cs -> order-cps //committer�洢�ɹ��Ż�Ӧ�𣬴洢ʧ�ܲ���Ӧ�� //order���ڳ�ʱ���ط�
	class CDistributeBlockXcsCpsRsp
	{
	public:
		enum
		{
			EN_MSG_ID = VBH::EN_DISTRIBUTE_BLOCK_XCS_CPS_RSP
		};

	public:
		DSC_BIND_ATTR(m_nPeerID, m_nBlockID);

	public:
		ACE_UINT16 m_nPeerID;
		ACE_UINT64 m_nBlockID; //����ID
	};


}

#endif