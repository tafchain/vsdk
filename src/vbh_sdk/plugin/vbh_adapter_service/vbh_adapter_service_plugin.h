#ifndef VBH_ADAPTER_SERVICE_PLUG_IN_H_2768356722357
#define VBH_ADAPTER_SERVICE_PLUG_IN_H_2768356722357

#include "dsc/plugin/i_dsc_plugin.h"

class CSdkClientServicePlugIn : public IDscPlugin
{
public:
	ACE_INT32 OnInit(void);
};
#endif