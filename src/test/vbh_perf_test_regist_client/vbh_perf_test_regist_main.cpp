#include "ace/OS_main.h"
#include "ace/OS_NS_stdio.h"

#include "dsc/mem_mng/dsc_allocator.h"

#include "vbh_perf_test_comm/vbh_perf_test_comm_id_def.h"

#include "vbh_perf_test_regist_client/vbh_perf_test_regist_appmanager.h"


int ACE_TMAIN(int argc, ACE_TCHAR *argv[]) 
{ 
	CVbhPerfTestRegistAppManager* pAppManager = NULL;

	DSC_NEW(pAppManager, CVbhPerfTestRegistAppManager);
	if(!pAppManager) 
	{ 
		ACE_OS::printf("failed to new vbh_perf_test_regist_client appmanager!"); 
		
		return -1; 
	} 

	pAppManager->SetNodeType(VBH_TEST::VBH_PERF_TEST_REGIST_CLIENT_NODE_TYPE);
	if( pAppManager->Init(argc, argv) )
	{ 
		ACE_OS::printf("vbh_perf_test_regist_client init failed, now exit!\n"); 
		pAppManager->Exit();
		delete pAppManager;

		return -1; 
	} 
	
	ACE_OS::printf("vbh_perf_test_regist_client init succeed, running...\n"); 
	pAppManager->Run_Loop(); 
	delete pAppManager;
	ACE_OS::printf("vbh_perf_test_regist_client terminated!\n"); 
	
	return 0; 
}