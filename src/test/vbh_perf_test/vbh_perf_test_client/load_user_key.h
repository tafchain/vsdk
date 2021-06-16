#ifndef VBH_LOAD_USER_KEY_H_4654687946416546464641654
#define VBH_LOAD_USER_KEY_H_4654687946416546464641654

#include "dsc/dsc_log.h"
#include "dsc/dsc_reactor.h"
#include "dsc/container/bare_hash_map.h"


class CLoadUserKey 
{

public:
	void GetUserKeyPath(CDscString& strUserKeyPath, ACE_UINT32 m_nChannelID);
	//������������û���user-id��user-crypt-key
	ACE_INT32 LoadUserKey(CDscString& userID, CDscString& userCryptKey, ACE_UINT32 nChannelID, const ACE_UINT32 nUserIdx);

	//�����ļ�����
	ACE_INT32 LoadFile(CDscString& strFileContent, const CDscString& strFilePath);

};

#endif
