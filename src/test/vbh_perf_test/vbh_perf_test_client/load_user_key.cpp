#include "ace/OS_NS_sys_stat.h"
#include "dsc/dsc_log.h"
#include "vbh_comm/vbh_comm_func.h"
#include "test/vbh_perf_test/vbh_perf_test_client/load_user_key.h"


void CLoadUserKey::GetUserKeyPath(CDscString& strUserKeyPath, ACE_UINT32 nChannelID)
{

	strUserKeyPath = CDscAppManager::Instance()->GetWorkRoot();
	strUserKeyPath += DSC_FILE_PATH_SPLIT_CHAR;
	strUserKeyPath += "test";
	strUserKeyPath += DSC_FILE_PATH_SPLIT_CHAR;
	strUserKeyPath += "channel_";
	strUserKeyPath += nChannelID;
	strUserKeyPath += DSC_FILE_PATH_SPLIT_CHAR;
	strUserKeyPath += "user_key";
	strUserKeyPath += DSC_FILE_PATH_SPLIT_CHAR;

}

ACE_INT32 CLoadUserKey::LoadUserKey(CDscString& userID, CDscString& userCryptKey, ACE_UINT32 nChannelID, const ACE_UINT32 nUserIdx)
{
	CDscString strUserKeyPath;
	GetUserKeyPath(strUserKeyPath, nChannelID);

	CDscString strCryptKeyFilePath(strUserKeyPath);
	CDscString strUserIDFilePath(strUserKeyPath);

	strCryptKeyFilePath += nUserIdx;
	strCryptKeyFilePath += ".ckey";
	strUserIDFilePath += nUserIdx;
	strUserIDFilePath += ".id";

	if (LoadFile(userCryptKey, strCryptKeyFilePath))
	{
		return -1;
	}

	if (LoadFile(userID, strUserIDFilePath))
	{
		return -1;
	}

	return 0;
}

ACE_INT32 CLoadUserKey::LoadFile(CDscString& strFileContent, const CDscString& strFilePath)
{
	ACE_stat stat;

	if (-1 == ACE_OS::stat(strFilePath.c_str(), &stat)) //文件不存
	{
		DSC_RUN_LOG_ERROR("can not find file:%s", strFilePath.c_str());
		return -1;
	}
	else
	{
		char* pFileBuf = DSC_THREAD_SIZE_MALLOC(stat.st_size); //开辟缓冲区存放文件内容

		ACE_HANDLE handle = ACE_OS::open(strFilePath.c_str(), O_RDONLY | O_BINARY);

		if (ACE_INVALID_HANDLE == handle)
		{
			int nLastError = ACE_OS::last_error();
			DSC_RUN_LOG_ERROR("cann't open file:%s, last error code:%d, last error reason:%s.", strFilePath.c_str(), nLastError, ACE_OS::strerror(nLastError));
			DSC_THREAD_SIZE_FREE(pFileBuf, stat.st_size);

			return -1;
		}

		if (ACE_OS::read(handle, pFileBuf, stat.st_size) < stat.st_size)
		{
			int nLastError = ACE_OS::last_error();
			DSC_RUN_LOG_ERROR("read file:%s error, last error code:%d, last error reason:%s.", strFilePath.c_str(), nLastError, ACE_OS::strerror(nLastError));
			DSC_THREAD_SIZE_FREE(pFileBuf, stat.st_size);
			ACE_OS::close(handle);

			return -1;
		}

		ACE_OS::close(handle);
		strFileContent.assign(pFileBuf, stat.st_size);
		DSC_THREAD_SIZE_FREE(pFileBuf, stat.st_size);
	}

	return 0;
}
