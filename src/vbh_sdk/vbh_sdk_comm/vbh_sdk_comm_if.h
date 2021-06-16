#ifndef SDK_COMM_IF_H_7EB16902999E11E99D4960F18A3A20D1
#define SDK_COMM_IF_H_7EB16902999E11E99D4960F18A3A20D1

#include "ace/Basic_Types.h"

#include "vbh_sdk_comm/vbh_sdk_comm_def_export.h"

namespace VBH_SDK
{
	VBH_SDK_COMM_DEF_EXPORT ACE_INT32 InitVbhSdk(const ACE_INT16 nAppType, const ACE_INT16 nAppID);
	VBH_SDK_COMM_DEF_EXPORT ACE_INT32 FnitVbhSdk(void);
	VBH_SDK_COMM_DEF_EXPORT const char* GetVbhErrorString(const ACE_INT32 nErrorCode);
}


#endif
