#include "ace/OS_NS_fcntl.h"
#include "ace/OS_NS_unistd.h"
#include "ace/OS_NS_sys_stat.h"

#include "dsc/configure/dsc_configure.h"

#include "vbh_comm_func.h"


ACE_INT32 VBH::CreateMmapFile(const CDscString& strFilePath, const ACE_UINT32 nSize)
{
	ACE_HANDLE handle = ACE_OS::open(strFilePath.c_str(), O_RDWR | O_BINARY | O_CREAT);

	if (ACE_INVALID_HANDLE == handle)
	{
		int nLastError = ACE_OS::last_error();
		DSC_RUN_LOG_ERROR("cann't open file:%s, last error code:%d, last error reason:%s.", strFilePath.c_str(), nLastError, ACE_OS::strerror(nLastError));
		return -1;
	}

	if (-1 == ACE_OS::ftruncate(handle, nSize)) //ftruncate �õ����ļ������ݶ�ȡ�����϶�Ϊ '\0'
	{
		int nLastError = ACE_OS::last_error();
		DSC_RUN_LOG_ERROR("truncate file failed, file name:%s, last error code:%d, last error reason:%s.", strFilePath.c_str(), nLastError, ACE_OS::strerror(nLastError));
		ACE_OS::close(handle);

		return -1;
	}

	ACE_OS::close(handle);

	return 0;
}

ACE_INT32 VBH::LoadMmapFile(const CDscString& strFilePath, ACE_Shared_Memory_MM& shmHandle, char*& pContent, ACE_UINT32& nSize)
{
	ACE_stat stat;

	//�ж��ļ��Ƿ����
	if (-1 == ACE_OS::stat(strFilePath.c_str(), &stat))
	{
		DSC_RUN_LOG_ERROR("file:%s is not exist.", strFilePath.c_str());
		return -1;
	}

	nSize = (ACE_UINT32)stat.st_size;

	//�򿪰������ڴ���ļ�
	if (-1 == shmHandle.open(strFilePath.c_str(), nSize))
	{
		int nLastError = ACE_OS::last_error();
		DSC_RUN_LOG_ERROR("open share memory failed, file name:%s, last error code:%d, last error reason:%s.", strFilePath.c_str(), nLastError, ACE_OS::strerror(nLastError));

		return -1;
	}

	pContent = (char*)shmHandle.malloc();
	if (NULL == pContent)
	{
		DSC_RUN_LOG_ERROR("alloc memory for memory map file failed, file name:%s.", strFilePath.c_str());
		shmHandle.close();

		return -1;
	}

	return 0;
}

ACE_INT32 VBH::ResizeMmapFile(const CDscString& strFilePath, ACE_Shared_Memory_MM& shmHandle, char*& pContent, const ACE_UINT32 nSize)
{
	//1. �ر�ԭʼӳ��
	shmHandle.close();

	//2. ���´��ļ���truncate �ļ��Ĵ�С
	ACE_HANDLE handle = ACE_OS::open(strFilePath.c_str(), O_RDWR | O_BINARY);

	if (ACE_INVALID_HANDLE == handle)
	{
		int nLastError = ACE_OS::last_error();
		DSC_RUN_LOG_ERROR("cann't open file:%s, last error code:%d, last error reason:%s.", strFilePath.c_str(), nLastError, ACE_OS::strerror(nLastError));

		return -1;
	}
	if (-1 == ACE_OS::ftruncate(handle, nSize))
	{
		int nLastError = ACE_OS::last_error();
		DSC_RUN_LOG_ERROR("truncate file failed, file name:%s, last error code:%d, last error reason:%s.", strFilePath.c_str(), nLastError, ACE_OS::strerror(nLastError));
		ACE_OS::close(handle);

		return -1;
	}
	ACE_OS::close(handle);

	//3. ����ӳ���ļ�
	if (-1 == shmHandle.open(strFilePath.c_str(), nSize))
	{
		int nLastError = ACE_OS::last_error();
		DSC_RUN_LOG_ERROR("open share memory failed, file name:%s, last error code:%d, last error reason:%s.", strFilePath.c_str(), nLastError, ACE_OS::strerror(nLastError));

		return -1;
	}

	pContent = (char*)shmHandle.malloc();
	if (NULL == pContent)
	{
		DSC_RUN_LOG_ERROR("alloc memory for memory map file failed, file name:%s.", strFilePath.c_str());
		shmHandle.close();

		return -1;
	}

	return 0;
}
