#include "dsc/plugin/dsc_service_factory.h"
#include "dsc/service/dsc_service_container.h"

#include "vbh_adapter_service/vbh_adapter_service.h"
#include "vbh_adapter_service/vbh_adapter_service_plugin.h"

ACE_INT32 CSdkClientServicePlugIn::OnInit(void)
{
	CDscServiceFactory<CVhbAdapterService> serviceFactory;
	ACE_INT32 nRet = ::RegistOneReactorDscService(&serviceFactory);

	if (nRet)
	{
		DSC_RUN_LOG_ERROR("regist user service regist failed");
	}
	else
	{
		DSC_RUN_LOG_FINE("regist user service regist successfully");
	}

	return nRet;
}

extern "C" PLUGIN_EXPORT void* CreateDscPlugin(void)
{
	CSdkClientServicePlugIn* pPlugIn = NULL;

	DSC_NEW(pPlugIn, CSdkClientServicePlugIn);

	return pPlugIn;
}
