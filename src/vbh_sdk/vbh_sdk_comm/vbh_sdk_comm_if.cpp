#include <atomic>

#include "ace/Init_ACE.h"
#include "ace/Thread_Manager.h"
#include "ace/Event.h"
#include "ace/OS_NS_fcntl.h"
#include "ace/OS_NS_string.h"
#include "ace/OS_NS_sys_stat.h"

#include "dsc/mem_mng/dsc_allocator.h"
#include "dsc/dsc_app_mng.h"
#include "dsc/dsc_log.h"

#include "vbh_comm/vbh_comm_error_code.h"
#include "vbh_sdk_comm/vbh_sdk_comm_if.h"
#include "vbh_sdk_comm/vbh_sdk_appmanager.h"


class CInitThreadParam
{
public:
	ACE_INT16 m_nAppType;
	ACE_INT16 m_nAppID;
	ACE_Event m_event;
};
//交易核心客户端线程入口函数
ACE_THR_FUNC_RETURN ThreadFunction(void* para)
{
	InitThreadAllocator();
	CSdkAppManager* pAppManager = NULL;

	DSC_NEW(pAppManager, CSdkAppManager);

	CInitThreadParam* pParam = (CInitThreadParam* )para;

	ACE_TCHAR* argv[3] = {"vbh_sdk", "-h" };

	pAppManager->SetNodeType(pParam->m_nAppType);
	pAppManager->SetNodeID(pParam->m_nAppID);
	argv[2] = getenv("WORK_ROOT");
	if (!argv[2])
	{
		ACE_OS::printf("Please check the setting of 'WORK_ROOT'\n");
		DSC_DELETE(pAppManager);

		pParam->m_nAppID = -1;
		pParam->m_event.signal();

		return 0;
	}

	if (pAppManager->Init(3, argv))
	{
		ACE_OS::printf("app init failed please check log\n");
		pAppManager->Exit();
		DSC_DELETE(pAppManager);

		pParam->m_nAppID = -1;
		pParam->m_event.signal();

		return 0;
	}

	pParam->m_event.signal();
	pAppManager->Run_Loop();
	
	DSC_DELETE(pAppManager);

	DeleteThreadAllocator();

	return 0;
}

std::atomic_flag m_lock = ATOMIC_FLAG_INIT;
bool m_bInited = false;

ACE_INT32 VBH_SDK::InitVbhSdk(const ACE_INT16 nAppType, const ACE_INT16 nAppID)
{
	while (m_lock.test_and_set(std::memory_order_acquire));

	if(m_bInited)
	{//重复调用
		m_lock.clear(std::memory_order_release);
		
		return 0;
	}
	else
	{
		m_bInited = true;
		m_lock.clear(std::memory_order_release);
	}

	InitThreadAllocator();
	ACE::init();

	CInitThreadParam param;

	param.m_nAppType = nAppType;
	param.m_nAppID = nAppID;

	//创建交易核心客户端线程
	if (-1 != ACE_Thread_Manager::instance()->spawn(ThreadFunction, &param, THR_NEW_LWP | THR_DETACHED))
	{
		param.m_event.wait();
		if (param.m_nAppID == nAppID)
		{
			return 0;
		}
		else
		{
			return -1;
		}
	}

	return -1;
}

ACE_INT32 VBH_SDK::FnitVbhSdk(void)
{
	CDscAppManager::Instance()->SetAppExited();

	return 0;
}

const char* VBH_SDK::GetVbhErrorString(const ACE_INT32 nErrorCode)
{
	return VBH::GetErrorString(nErrorCode);
}
