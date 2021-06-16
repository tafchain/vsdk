#include "ace/OS_main.h"
#include "ace/OS_NS_stdio.h"

#include "dsc/mem_mng/dsc_allocator.h"

//#include "common/vbh_test_comm_def.h"
#include "vbh_perf_test_comm/vbh_perf_test_comm_id_def.h"

#include "vbh_perf_test_client/appmanager.h"


int ACE_TMAIN(int argc, ACE_TCHAR *argv[]) 
{ 
	CPerfTestAppManager* pAppManager = NULL;

	DSC_NEW(pAppManager, CPerfTestAppManager);
	if(!pAppManager) 
	{ 
		ACE_OS::printf("failed to new vbh_perf_test_client appmanager!"); 
		
		return -1; 
	} 

	pAppManager->SetNodeType(VBH_TEST::VBH_PERF_TEST_CLIENT_NODE_TYPE);
	if( pAppManager->Init(argc, argv) )
	{ 
		ACE_OS::printf("vbh_perf_test_client init failed, now exit!\n"); 
		pAppManager->Exit();
		delete pAppManager;

		return -1; 
	} 
	
	ACE_OS::printf("vbh_perf_test_client init succeed, running...\n"); 
	pAppManager->Run_Loop(); 
	delete pAppManager;
	ACE_OS::printf("vbh_perf_test_client terminated!\n"); 
	
	return 0; 
}