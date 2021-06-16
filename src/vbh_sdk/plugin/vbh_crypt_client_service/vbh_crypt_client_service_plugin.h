#ifndef VBH_CRYPT_CLIENT_SERVICE_PLUG_IN_H_32743289876432658786
#define VBH_CRYPT_CLIENT_SERVICE_PLUG_IN_H_32743289876432658786

#include "dsc/plugin/i_dsc_plugin.h"

class CSdkClientServicePlugIn : public IDscPlugin
{ 
public: 
	ACE_INT32 OnInit(void); 
}; 
#endif