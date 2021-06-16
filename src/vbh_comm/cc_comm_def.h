#ifndef CC_EXPLORER_QUERY_DEF_H_9874654351889451684986158548
#define CC_EXPLORER_QUERY_DEF_H_9874654351889451684986158548

#include "vbh_comm/comm_msg_def/vbh_comm_class_def.h"

enum
{
	EN_QUERY_ACTION_QUERY_BLOCK_COUNT = 1,
	EN_QUERY_ACTION_QUERY_BLOCK_HEADER_INFO, //
	EN_QUERY_ACTION_QUERY_BLOCK_TRANS_LIST,		


	EN_QUERY_ACTION_QUERY_WRITE_SET,
	EN_QUERY_ACTION_QUERY_TRANS_INFO,
	EN_QUERY_ACTION_QUERY_BLOCK_TRANS_COUNT,


	EN_USER_MIN_QUERY_ACTION_ID = 64 //�û����õ���С ��ѯaction-id
};


enum
{
	EN_HASH_MAP_BITES = 16,
	EN_PROPOSE_BATCH_QUERY_HASH_MAP_BITS = 10,
	EN_SESSION_TIMEOUT_VALUE = 60
};

enum //д���ı�������
{
	EN_WS_USER_TYPE = 1,
	EN_WS_INFORMATION_TYPE
};

//������session��ʹ�ã���������ʱ���������Գ�Աʹ��Blob���� //�᰸ʱʹ��
class CProposeSimpleWriteSet
{
public:
	DSC::CDscShortBlob m_key;
	DSC::CDscShortBlob m_value;
};
using CProposeSimpleUser = CProposeSimpleWriteSet;
using CProposeSimpleInformation = CProposeSimpleWriteSet;

//������session��ʹ�ã���������ʱ���������Գ�Աʹ��Blob���� //��ѯʱʹ��
class CQuerySimpleWriteSet
{
public:
	ACE_UINT32 m_nVersion = 0;
	DSC::CDscShortBlob m_key;
	DSC::CDscShortBlob m_value;
};
using CQuerySimpleUser = CQuerySimpleWriteSet;
using CQuerySimpleInformation = CQuerySimpleWriteSet;

//������session��ʹ�ã���������ʱ���������Գ�Աʹ��Blob���� //��ѯʱʹ��
class CQuerySimpleTransaction
{
public:
	VBH::CSimpleVector<CQuerySimpleWriteSet> m_vecWs; //д��
	ACE_UINT32 m_nActionID;
	DSC::CDscShortBlob m_transKey; //transaction-key
	DSC::CDscShortBlob m_userKey;  //��������
	DSC::CDscShortBlob m_signature;
	DSC::CDscShortBlob m_proposal;
};

class CCcWsKV
{
public:
	ACE_UINT32 m_nVersion = 0; //д�����ݵİ汾�ţ�0��ʾ��ʼ�汾��ÿ������+1
	ACE_UINT64 m_nAllocatedID = 0; //ϵͳ����� �û�ID �� information-ID

public: //ʹ�� CDscUnboundQueue ����ʱ������߱���ģ��ӿ�
	CCcWsKV* m_pNext;
};


#endif
