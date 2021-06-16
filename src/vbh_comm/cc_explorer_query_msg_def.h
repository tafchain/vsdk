#ifndef CC_EXPLORER_QUERY_MSG_DEF_H_48978945646548945645648674896
#define CC_EXPLORER_QUERY_MSG_DEF_H_48978945646548945645648674896

#include "dsc/codec/dsc_codec/dsc_codec.h"
#include "dsc/codec/json_codec/json_codec.h"

#include "vbh_comm/cc_comm_def.h"

namespace EXPLORER_QUERY
{
	//��ѯ�����������
	class CQueryBlockCountAction
	{
	public:
		enum
		{
			EN_ACTION_ID = EN_QUERY_ACTION_QUERY_BLOCK_COUNT
		};
	};

	//��ѯ�������Ӧ��
	class CQueryBlockCountRsp
	{
	public:
		DSC_BIND_ATTR(blockCount);
		JSON_BIND_ATTR(blockCount);

	public:
		ACE_UINT64 blockCount = 0;
	};

	//��ѯ������Ϣ����
	class CQueryBlockHeaderInfoAction
	{
	public:
		enum 
		{
			EN_ACTION_ID = EN_QUERY_ACTION_QUERY_BLOCK_HEADER_INFO
		};

	public:
		DSC_BIND_ATTR(m_nBlockID);

	public:
		ACE_UINT64 m_nBlockID = 0;
	};

	//��ѯ������ϢӦ��
	class CQueryBlockHeaderInfoRsp
	{
	public:
		DSC_BIND_ATTR(blockID, preHash, timeStamp, transactionMroot, transCount);
		JSON_BIND_ATTR(blockID, preHash, timeStamp, transactionMroot, transCount);

	public:
		ACE_UINT64 blockID = 0;
		CDscString preHash;
		CDscString timeStamp;
		CDscString transactionMroot;
		ACE_UINT16 transCount;
	};
	
	//��ѯ�����н����б�����
	class CQueryBlockTransListAction
	{
	public:
		enum
		{
			EN_ACTION_ID = EN_QUERY_ACTION_QUERY_BLOCK_TRANS_LIST
		};

	public:
		DSC_BIND_ATTR(m_nBlockID, m_nPageSize, m_nPageIndex);

	public:
		ACE_UINT64 m_nBlockID = 0;
		ACE_UINT16 m_nPageSize = 0;
		ACE_UINT16 m_nPageIndex = 0;
	};
	class CCommitInformationAction
	{
	public:
		enum
		{
			EN_ACTION_ID = 3
		};

	public:
		DSC_BIND_ATTR(InformationKey, InformationValue);

	public:
		DSC::CDscShortBlob InformationKey;
		DSC::CDscShortBlob InformationValue;
	};

	//��ѯ�����б�Ӧ��
	class CQueryBlockTransListRsp
	{
	public:

		class CInfoJson
		{
		public:
			DSC_BIND_ATTR(informationKey, informationValue);
			JSON_BIND_ATTR(informationKey, informationValue);

		public:
			CDscString informationKey;
			CDscString informationValue;
		};
		class CTrans
		{
		public:
#ifdef VBH_USE_SIGNATURE
			DSC_BIND_ATTR(txid, time, sender, signatures, proposal);
			JSON_BIND_ATTR(txid, time, sender, signatures, proposal);
#else
			DSC_BIND_ATTR(txid, time, sender, proposal);
			JSON_BIND_ATTR(txid, time, sender, proposal);
#endif

		public:
			CDscString txid; //����ID
			CDscString time; //����ʱ�䣬ĿǰΪ��
			CDscString sender; //������
#ifdef VBH_USE_SIGNATURE
			CDscString signatures; //�᰸ǩ��
#endif
			CInfoJson proposal; //�����᰸
		};

	public:
		DSC_BIND_ATTR(blockTime, preBlockHash, blockHash, transCount, transList);
		JSON_BIND_ATTR(blockTime, preBlockHash, blockHash, transCount, transList);

	public:
		ACE_INT64  blockTime; 
		CDscString preBlockHash;
		CDscString blockHash;
		ACE_UINT16 transCount = 0;
		DSC::CDscList<CTrans> transList;
	};






	//��ѯ�û���Ϣ����
	class CQueryWriteSetAction
	{
	public:
		enum
		{
			EN_ACTION_ID = EN_QUERY_ACTION_QUERY_WRITE_SET
		};

	public:
		DSC_BIND_ATTR(m_userID);

	public:
		DSC::CDscShortBlob m_userID;
	};
	
	//��ѯ�û���ϢӦ��
	class CQueryWriteSetRsp
	{
	public:
		DSC_BIND_ATTR(m_userInfo);

	public:
		DSC::CDscShortBlob m_userInfo;
	};

	//��ѯ������Ϣ����
	class CQueryTransInfoAction
	{
	public:
		enum
		{
			EN_ACTION_ID = EN_QUERY_ACTION_QUERY_TRANS_INFO
		};

	public:
		DSC_BIND_ATTR(m_transKey);

	public:
		DSC::CDscShortBlob m_transKey;
	};

	//��ѯ������ϢӦ��
	class CQueryTransInfoRsp
	{
	public:

		class CInfoJson
		{
		public:
			DSC_BIND_ATTR(informationKey, informationValue);
			JSON_BIND_ATTR(informationKey, informationValue);

		public:
			CDscString informationKey;
			CDscString informationValue;
		};
#ifdef VBH_USE_SIGNATURE
		DSC_BIND_ATTR(blockId, transId, sender, signatures, proposal);
		JSON_BIND_ATTR(blockId, transId, sender, signatures, proposal);
#else
		DSC_BIND_ATTR(blockId, transId, sender, proposal);
		JSON_BIND_ATTR(blockId, transId, sender, proposal);
#endif

	public:
		ACE_UINT64 blockId; //����߶�
		CDscString transId; //����ID
		CDscString sender; //������
#ifdef VBH_USE_SIGNATURE
		CDscString signatures; //�᰸ǩ��
#endif
		CDscString proposal; //�����᰸
	};

	//��ȡָ����Ľ�������
	class CQueryBlockTransCountAction
	{
	public:
		enum
		{
			EN_ACTION_ID = EN_QUERY_ACTION_QUERY_BLOCK_TRANS_COUNT
		};

	public:
		DSC_BIND_ATTR(m_nBlockID);

	public:
		ACE_UINT64 m_nBlockID = 0;
	};

	class CQueryBlockTransCountRsp
	{
	public:
		DSC_BIND_ATTR(m_nBlockID, m_nTransCount);

	public:
		ACE_UINT64 m_nBlockID = 0;
		ACE_UINT32 m_nTransCount = 0;
	};


}

#endif // !__vbh_commmon_wrapper_message_h

