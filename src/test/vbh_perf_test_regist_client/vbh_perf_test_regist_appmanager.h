#ifndef VBH_PERF_TEST_REGIST_APPMANAGER_H_98796465413546754984512
#define VBH_PERF_TEST_REGIST_APPMANAGER_H_98796465413546754984512

#include "dsc/dsc_app_mng.h"

class CVbhPerfTestRegistAppManager : public CDscAppManager
{
public:
	ACE_UINT32 GetTestUserNum(void) const;

protected:
	virtual ACE_INT32 OnInit(void);
	virtual void OnParam(const int argc, ACE_TCHAR* argv);

public:
	ACE_UINT32 m_nTestUserNum = 0; //�����û�����
	ACE_UINT32 m_nChannelNum = 1; //channel��
	ACE_UINT32 m_nStartNum = 0; //channel��
	bool m_bCryptPath = true; //Ĭ���߼���ͨ��
};

#endif
