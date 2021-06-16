#include "dsc/dsc_comm_func.h"
#include "dsc/dispatcher/dsc_dispatcher_center.h"

#include "vbh_comm/vbh_encrypt_lib.h"

#include "test/vbh_perf_test_regist_client/vbh_perf_test_regist_appmanager.h"
#include "vbh_perf_test_regist_client/vbh_perf_test_regist_service.h"


ACE_INT32 CVbhPerfTestRegistAppManager::OnInit()
{
	VBH::InitOpenSsl();

	DSC_FORWARD_CALL(CDscAppManager::OnInit() );

	CVbhPerfTestRegistService* pVbhPerfRegistTestService;

	if (m_nTestUserNum < 1)
	{
		m_nTestUserNum = 1; //没有设置，则设置一个默认值
	}
	DSC_NEW(pVbhPerfRegistTestService, CVbhPerfTestRegistService(m_nTestUserNum, m_nChannelNum, m_nStartNum, m_bCryptPath));

	CDscDispatcherCenterDemon::instance()->RegistDscTask(pVbhPerfRegistTestService, CVbhPerfTestRegistService::EN_SERVICE_TYPE, 1);

	return 0;
}

void CVbhPerfTestRegistAppManager::OnParam(const int argc, ACE_TCHAR* argv)
{
	if (argc == 'u' || argc == 'U')
	{
		DSC::DscAtoi(argv, m_nTestUserNum);
	}
	else if (argc == 'c' || argc == 'C') //channel号，默认为1
	{
		DSC::DscAtoi(argv, m_nChannelNum);
	}
	else if (argc == 'y' || argc == 'Y') //是否加密, 默认加密
	{
		if (strcmp(argv, "no-crypt") == 0)
		{
			m_bCryptPath = false;
		}
	}
	else if (argc == 's' || argc == 'S') //m_nStartNum号，默认为1
	{
		DSC::DscAtoi(argv, m_nStartNum);
	}
}

ACE_UINT32 CVbhPerfTestRegistAppManager::GetTestUserNum(void) const
{
	return m_nTestUserNum;
}
