#ifndef COMM_MSG_DEF_H_423809423798432798479843286468432849894865484684386456
#define COMM_MSG_DEF_H_423809423798432798479843286468432849894865484684386456

#include "dsc/codec/dsc_codec/dsc_codec.h"
#include "dsc/mem_mng/dsc_stl_type.h"

#include "vbh_comm/vbh_comm_def_export.h"

namespace VBH
{
	//��ʾ��ͬ��������peer Cas�ĵ�ַ��Ϣ
	class CSyncSourcePeerCasAddress
	{
	public:
		DSC_BIND_ATTR(m_nPeerID, m_nPort, m_strIpAddr);

	public:
		ACE_UINT16 m_nPeerID;
		ACE_UINT16 m_nPort;
		CDscString m_strIpAddr; //���Խ�������ͬ���ĵ�ַ

	};

	//vbhϵͳΪ�û���information�����keyֵ
	class CVbhAllocatedKey
	{
	public:
		enum
		{
			EN_SIZE = sizeof(ACE_UINT32) + sizeof(ACE_UINT64)
		};
	public:
		DSC_BIND_ATTR(m_nSequenceNumber, m_nAllocatedID);

	public:
		bool operator==(const CVbhAllocatedKey& key) const;

	public:
		ACE_UINT32 m_nSequenceNumber = 0; //У������ˮ��
		ACE_UINT64 m_nAllocatedID = 0; //ϵͳ����� �û�ID �� information-ID
	};

	//��ʾϵͳ�� version > 0 �� д��item
	//k.v(version).v(value)
	// key = m_nAllocatedID
	// v = m_nVersio
	// v = m_value
	class CVbhWriteSetItem
	{
	public:
		DSC_BIND_ATTR(m_nVersion, m_nAllocatedID, m_value);

	public:
		ACE_UINT32 m_nVersion; //value�İ汾�� //д���б���İ汾����ʵ����һ�εİ汾�ţ�ʵ�ʴ洢ʱҪ+1�洢
		ACE_UINT64 m_nAllocatedID; //ϵͳ����� �û�ID �� information-ID //key
		DSC::CDscShortBlob m_value;
	};

	//�����url
	class CTransactionUrl
	{
	public:
		DSC_BIND_ATTR(m_nTransIdx, m_nBlockID);

	public:
		ACE_UINT16 m_nTransIdx; //�����������е���ţ� ��0��ʼ
		ACE_UINT64 m_nBlockID; //����ID
	};

	//������д����url
	class CWriteSetUrl
	{
	public:
		enum
		{
			EN_SIZE = sizeof(ACE_UINT16) * 2 + sizeof(ACE_UINT64)
		};

	public:
		DSC_BIND_ATTR(m_nWsIdx, m_nTransIdx, m_nBlockID);

	public:
		ACE_UINT16 m_nWsIdx; //д����д���б�����ţ���0��ʼ
		ACE_UINT16 m_nTransIdx; //�����������е���ţ� ��0��ʼ
		ACE_UINT64 m_nBlockID; //����ID
	};



	//��ʾϵͳ�� verion>0 ��user
	using CVbhUser = CVbhWriteSetItem;
	//��ʾϵͳ�� version > 0 ��information
	using CVbhInformation = CVbhWriteSetItem;

	class CCcEndorsePolicy //CC �ı������
	{
	public:
		DSC_BIND_ATTR(m_lstPeerID);

	public:
		DSC::CDscShortList<ACE_INT16> m_lstPeerID;
	};

	//��vector,�ṩ���������ʹ�����飬һ���Կ������ÿռ䣬//�ɱ��� //���Զ��ͷſռ�
	template <typename TYPE>
	class CSimpleVector
	{
	public:
		~CSimpleVector(); //�������������ͷ��ڴ�

	public: //��ͨ���ʺ���
		//���ô洢�ռ��С
		void Open(const ACE_UINT16 nItemCount);
		void Close(void);

	public: //��ͨ���ʺ���
		//��ȡԪ�ظ���
		ACE_UINT16 Size(void) const;

		//����[]�����
		TYPE& operator[](const ACE_UINT16 idx);

		//���ظ�ֵ�����
		CSimpleVector<TYPE>& operator= (const CSimpleVector<TYPE>& vec);

	public:
		void GetSize(DSC::CDscCodecGetSizer& rDscCodecGetSizer);

		template<typename ENCODER_TYPE>
		void Encode(ENCODER_TYPE& rDscCodecEncoder);

		template<typename DECODER_TYPE>
		ACE_INT32 Decode(DECODER_TYPE& rDscCodecDecoder);

	private:
		ACE_UINT16 m_nItemCount = 0; //Ԫ�ظ���
		TYPE* m_pItemBuf = NULL; //Ԫ�ػ�����
	};

	class CKeyVersion //k.v���ݽṹ
	{
	public:
		DSC_BIND_ATTR(m_nAllocatedID, m_nVersion);

	public:
		ACE_UINT64 m_nAllocatedID; //write-set-id
		ACE_UINT32 m_nVersion; //��Ӧ�İ汾��
	};

	//===============================Transaction�����Ϣ����==================================

	//vbhϵͳΪtransaction���ɵ�key���� //order����
	class CVbhAllocatedTransactionKey
	{
	public:
		DSC_BIND_ATTR(m_nTransIdx, m_nSequenceNumber, m_nBlockID);

	public:
		ACE_UINT16 m_nTransIdx; //�����������е���ţ� ��0��ʼ //һ������������ĸ���������2^16 //===//�������û�ID
		ACE_UINT32 m_nSequenceNumber; //�������ʱ����ˮ�� //===//У���õ�ID
		ACE_UINT64 m_nBlockID; //����ID //===//�������û�ID
	};

	//��ѯ�õ����᰸����ṹ //��ѯ��֧��ע������Ĳ�ѯ
	class CProposeTransactionAtQuery
	{
	public:
		class CWs
		{
		public:
			DSC_BIND_ATTR(m_nVersion, m_key, m_value);

		public:
			ACE_UINT32 m_nVersion; //value�İ汾�� //д���б���İ汾����ʵ����һ�εİ汾�ţ�ʵ�ʴ洢ʱҪ+1�洢
			CVbhAllocatedKey m_key; //ϵͳ����� �û�ID �� information-ID //key
			DSC::CDscShortBlob m_value;
		};

	public:
		DSC_BIND_ATTR(m_vecWs, m_nActionID, m_userKey, m_signature, m_proposal);

	public:
		VBH::CSimpleVector<CWs> m_vecWs; //д��
		ACE_UINT32 m_nActionID;
		CVbhAllocatedKey m_userKey;  //��������
		DSC::CDscShortBlob m_signature;
		DSC::CDscShortBlob m_proposal;
	};

	//��ѯ�õ����᰸����ṹ(�����ʱ) //��ѯ��֧��ע������Ĳ�ѯ
	class CProposeTransactionAtExplorer
	{
	public:
		DSC_BIND_ATTR(m_nActionID, m_userKey, m_signature, m_proposal);

	public:
		ACE_UINT32 m_nActionID;
		CVbhAllocatedKey m_userKey;  //��������
		DSC::CDscShortBlob m_signature;
		DSC::CDscShortBlob m_proposal; //�᰸Ϊ��ʱ����ʾ���񲻿������ ��������£��᰸����Ϊ��
	};


}

#include "vbh_comm/comm_msg_def/vbh_comm_class_def.inl"

#endif
