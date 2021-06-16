#ifndef SDK_APPMANAGER_H_43287943287287697643286432865326
#define SDK_APPMANAGER_H_43287943287287697643286432865326

#include "dsc/dsc_app_mng.h"

class CSdkAppManager : public CDscAppManager
{

protected:
	virtual ACE_INT32 OnInit(void);
	virtual ACE_INT32 OnExit(void);
};

#endif
