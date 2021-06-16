#ifndef VBH_COMM_FUNC_H_432843298432658431469719764325685
#define VBH_COMM_FUNC_H_432843298432658431469719764325685

#include "ace/OS_NS_fcntl.h"
#include "ace/Shared_Memory_MM.h"

#include "dsc/container/dsc_string.h"
#include "dsc/container/dsc_queue.h"
#include "dsc/codec/dsc_codec/dsc_codec.h"
#include "dsc/configure/dsc_configure.h"

#include "vbh_comm/vbh_comm_def_export.h"

namespace VBH
{
	//blob, string�ĸ�ֵ�ͱȶ�
	bool IsEqual(const CDscString& str, const DSC::CDscBlob& blob); //string��blob�Ƿ����
	void Assign(CDscString& str, const DSC::CDscBlob& blob); //��blob���ݸ���string
	void Assign(DSC::CDscBlob& blob, CDscString& str); //��stirng���ݸ���blb

	//short-blob, string�ĸ�ֵ�ͱȶ�
	bool IsEqual(const CDscString& str, const DSC::CDscShortBlob& blob); //string��blob�Ƿ����
	void Assign(CDscString& str, const DSC::CDscShortBlob& blob); //��blob���ݸ���string
	void Assign(DSC::CDscShortBlob& blob, CDscString& str); //��stirng���ݸ���blb

	//��� CDscUnboundQueue 
	template<typename ITEM_TYPE>
	void ClearDscUnboundQueue(CDscUnboundQueue<ITEM_TYPE>& queue);

	//�ϲ����� CDscUnboundQueue from->to
	template<typename ITEM_TYPE>
	void MergeDscUnboundQueue(CDscUnboundQueue<ITEM_TYPE>& queueTo, CDscUnboundQueue<ITEM_TYPE>& queueFrom);


	//��ȡͨ������
	ACE_INT32 GetVbhProfileInt(const char* pszKeyName, ACE_INT32& nValue); //��ȡ�������͵�����ֵ
	ACE_INT32 GetVbhProfileString(const char* pszKeyName, CDscString& strValue); //��ȡ�ַ������͵�����ֵ

	//�������ݵ�hashֵ
	ACE_UINT16 GetDataHashValue(const DSC::CDscBlob& data, const ACE_UINT32 nHashBits);

	//�ڴ�ӳ�������ļ��Ĵ����ͼ��أ�ͨ�ú��� (���ã���־���Ⱦ���ʹ��)
	template <typename CFG_TYPE>
	ACE_INT32 CreateCfgFile(const CDscString& strFilePath, CFG_TYPE& initCfg); //initCfg�ǳ�ʼд���ļ�������
	template<typename CFG_TYPE>
	ACE_INT32 LoadMmapCfgFile(const CDscString& strFilePath, ACE_Shared_Memory_MM& shmHandle, CFG_TYPE*& pCfg);

	//����ָ����С��mmap�ļ�
	VBH_COMM_DEF_EXPORT ACE_INT32 CreateMmapFile(const CDscString& strFilePath, const ACE_UINT32 nSize);
	//����ָ����mmap�ļ������ļ���С�����ڴ�ӳ�� //ͨ��nSize�����ļ��Ĵ�С
	VBH_COMM_DEF_EXPORT ACE_INT32 LoadMmapFile(const CDscString& strFilePath, ACE_Shared_Memory_MM& shmHandle, char*& pContent, ACE_UINT32& nSize);
	//����ָ��mmap�ļ��Ĵ�С�������¼���mmap�ļ� //nSizeΪָ����size
	VBH_COMM_DEF_EXPORT ACE_INT32 ResizeMmapFile(const CDscString& strFilePath, ACE_Shared_Memory_MM& shmHandle, char*& pContent, const ACE_UINT32 nSize);
}

#include "vbh_comm/vbh_comm_func.inl"
#endif
