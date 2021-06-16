//#include "ace/OS_NS_sys_stat.h"
//
//#include "dsc/dsc_log.h"
//
//#include "vbh_perf_test_client/vbh_perf_test_query_block_client_service.h"
//#include "vbh_comm/vbh_comm_msg_def.h"
//#include "vbh_comm/vbh_comm_error_code.h"
//#include "vbh_comm/vbh_comm_func.h"
//#include "vbh_comm/vbh_comm_macro_def.h"
//#include "vbh_comm/vbh_key_codec.h"
//#include "common/test_def.h"
//#include "vbh_perf_test_comm/vbh_perf_test_comm_msg_def.h"
//
//
//CVbhPerfTestQueryBlockClientService::CVbhPerfTestQueryBlockClientService(ACE_UINT64 nBlockNum, ACE_UINT32 nQueryGroupCount, ACE_UINT32 nChannelNum, bool bCrypt, ACE_INT32 nLimitTimes)
//	: m_nQueryGroupCount(nQueryGroupCount)
//	, m_nChannelNum(nChannelNum)
//	, m_nLimitTimes(nLimitTimes)
//	, m_nBlockNum(nBlockNum)
//{
//	if (bCrypt) //加密，走加密通道
//	{
//		m_nClientServiceType = VBH::EN_VBH_CRYPT_CLIENT_SERVICE_TYPE;
//	}
//	else //走不加密通道
//	{
//		m_nClientServiceType = VBH::EN_VBH_CLIENT_SERVICE_TYPE;
//	}
//
//	if (m_nLimitTimes > 0)
//	{
//		m_bLimitTimes = true;
//	}
//}
//
//ACE_INT32 CVbhPerfTestQueryBlockClientService::OnInit(void)
//{
//	ACE_OS::sleep(6);
//
//	if(CDscReactor::OnInit())
//	{
//		DSC_RUN_LOG_ERROR("no-encrypt-sdk-test-propose-client service init failed!");
//
//		return -1;
//	}
//
//	CDscString strCryptKeyPath;
//	CDscString strUserKeyPath;
//
//	strCryptKeyPath = CDscAppManager::Instance()->GetWorkRoot();
//	strCryptKeyPath += DSC_FILE_PATH_SPLIT_CHAR;
//	strCryptKeyPath += "test";
//	strCryptKeyPath += DSC_FILE_PATH_SPLIT_CHAR;
//	strCryptKeyPath += "crypt_key";
//	strCryptKeyPath += DSC_FILE_PATH_SPLIT_CHAR;
//
//	strUserKeyPath = CDscAppManager::Instance()->GetWorkRoot();
//	strUserKeyPath += DSC_FILE_PATH_SPLIT_CHAR;
//	strUserKeyPath += "test";
//	strUserKeyPath += DSC_FILE_PATH_SPLIT_CHAR;
//	strUserKeyPath += "user_key";
//	strUserKeyPath += DSC_FILE_PATH_SPLIT_CHAR;
//
//	ACE_UINT32 nSessionID = 0;
//	CQueryBlockSession* pQueryBlockSession;
//
//	for (size_t nChannel = 1; nChannel <= m_nChannelNum; ++nChannel)
//	{
//		for (size_t nUser = 1; nUser <= m_nQueryGroupCount; ++nUser)
//		{
//			pQueryBlockSession = DSC_THREAD_TYPE_NEW(CQueryBlockSession) CQueryBlockSession(*this);
//
//			if (LoadClientCryptKey(strCryptKeyPath, pQueryBlockSession->m_user.m_strUserCryptKey, nChannel + 200, nUser))
//			{
//				return -1;
//			}
//
//			if (LoadUserKey(strUserKeyPath, pQueryBlockSession->m_user.m_strUserID, nChannel + 200, nUser))
//			{
//				return -1;
//			}
//
//			pQueryBlockSession->m_nLimitTimes = m_nLimitTimes;
//			pQueryBlockSession->m_nChannelID = nChannel + 200;
//			pQueryBlockSession->m_nSessionID = ++nSessionID;
//			Query(pQueryBlockSession);
//			this->SetDscTimer(pQueryBlockSession, EN_SESSION_TIMEOUT_VALUE, true);
//			m_mapQueryBlockSession.DirectInsert(pQueryBlockSession->m_nSessionID, pQueryBlockSession);
//		}
//	}
//
//	this->SetDscTimer(this, 1, true);
//
//	return 0;
//}
//
//ACE_INT32 CVbhPerfTestQueryBlockClientService::OnExit( void )
//{
//	this->CancelDscTimer(this);
//
//	return CDscReactor::OnExit();
//}
//
//void CVbhPerfTestQueryBlockClientService::OnTimer(void)
//{
//	//double dAvg = 0.0;
//
//	//if (m_nTradeSum)
//	//{
//	//	++m_nCount;
//	//}
//
//	//if (m_nCount)
//	//{
//	//	dAvg = m_nTradeSum / m_nCount;
//	//}
//
//	//if (dAvg > 1)
//	//{
//	//	ACE_OS::printf("cur sec trade sum:%d, avg:%0.1f!\n", m_nTradeSum, dAvg);
//	//}
//	//VBH_TEST::CTradeSumMsg sumMsg;
//	//CDscMsg::CDscMsgAddr addr(VBH_TEST::VBH_TEST_STATIC_NODE_TYPE, 1,
//	//	VBH_TEST::VBH_PERF_TEST_STATIC_SERVICE_TYPE, DSC::EN_INVALID_ID);
//
//	////ACE_OS::printf("msg sum:%d!\n", m_nGetSum);
//
//	//sumMsg.m_nTradeSum = m_nQuerySum;
//	//if (this->SendDscMessage(sumMsg, addr))
//	//{
//	//	DSC_RUN_LOG_INFO("send static msg failed");
//	//}
//
//	//if (m_nQuerySum)
//	//{
//		m_nSum += m_nQuerySum;
//		++m_nSumCount;
//
//		double fAvg = m_nSum;
//
//		fAvg /= m_nSumCount;
//
//		ACE_OS::printf("Cur Second Query Block Sum: %d!\t", m_nQuerySum);
//		ACE_OS::printf("Cur Second Query Block Time Delay: %f ms!\t", 1000 / (double)m_nQuerySum);
//		//ACE_OS::printf("Query Time Delay: %f ms!\t", 1000 / fAvg);
//		//ACE_OS::printf("Query TPS: %.2f!\t", fAvg);
//		ACE_OS::printf("Success: %.2f%%\n", 100 * (1 - m_nTotalFailedRsp / (double)m_nTotalReq));
//	//}
//
//	if ((m_bLimitTimes) && (m_nTotalRsp >= (m_nChannelNum * m_nQueryGroupCount * m_nLimitTimes)))
//	{
//#ifdef WIN32
//		ACE_OS::system("pause");
//#endif
//		ACE_OS::exit(0);
//	}
//	m_nQuerySum = 0;
//}
//
//void CVbhPerfTestQueryBlockClientService::OnTimeOut(CQueryBlockSession* pSession)
//{
//	this->Query(pSession);
//}
//
//ACE_INT32 CVbhPerfTestQueryBlockClientService::LoadClientCryptKey(CDscString strCryptKeyPath, CDscString& strClientCryptKey, const ACE_UINT32 nChannelID, const ACE_UINT32 nUserID)
//{
//	ACE_stat stat;
//	CDscString strFileName = strCryptKeyPath;
//	strFileName += nChannelID;
//	strFileName += '_';
//	strFileName += nUserID;
//	strFileName += "_client.key";
//
//	if (-1 == ACE_OS::stat(strFileName.c_str(), &stat)) //文件不存
//	{
//		DSC_RUN_LOG_ERROR("can not find file:%s", strFileName.c_str());
//		return -1;
//	}
//	else
//	{
//		char* pFileBuf = DSC_THREAD_SIZE_MALLOC(stat.st_size); //开辟缓冲区存放文件内容
//
//		ACE_HANDLE handle = ACE_OS::open(strFileName.c_str(), O_RDONLY | O_BINARY);
//
//		if (ACE_INVALID_HANDLE == handle)
//		{
//			int nLastError = ACE_OS::last_error();
//			DSC_RUN_LOG_ERROR("cann't open file:%s, last error code:%d, last error reason:%s.", strFileName.c_str(), nLastError, ACE_OS::strerror(nLastError));
//			DSC_THREAD_SIZE_FREE(pFileBuf, stat.st_size);
//
//			return -1;
//		}
//
//		if (ACE_OS::read(handle, pFileBuf, stat.st_size) < stat.st_size)
//		{
//			int nLastError = ACE_OS::last_error();
//			DSC_RUN_LOG_ERROR("read file:%s error, last error code:%d, last error reason:%s.", strFileName.c_str(), nLastError, ACE_OS::strerror(nLastError));
//			DSC_THREAD_SIZE_FREE(pFileBuf, stat.st_size);
//			ACE_OS::close(handle);
//
//			return -1;
//		}
//
//		ACE_OS::close(handle);
//		strClientCryptKey.assign(pFileBuf, stat.st_size);
//		DSC_THREAD_SIZE_FREE(pFileBuf, stat.st_size);
//	}
//
//	return 0;
//}
//
//ACE_INT32 CVbhPerfTestQueryBlockClientService::LoadUserKey(CDscString strUserKeyPath, CDscString& strUserKey, const ACE_UINT32 nChannelID, const ACE_UINT32 nUserID)
//{
//	ACE_stat stat;
//	CDscString strFileName(strUserKeyPath);
//
//	strFileName += nChannelID;
//	strFileName += '_';
//	strFileName += nUserID;
//	strFileName += ".key";
//
//	if (-1 == ACE_OS::stat(strFileName.c_str(), &stat)) //文件不存
//	{
//		DSC_RUN_LOG_ERROR("can not find file:%s", strFileName.c_str());
//		return -1;
//	}
//	else
//	{
//		char* pFileBuf = DSC_THREAD_SIZE_MALLOC(stat.st_size); //开辟缓冲区存放文件内容
//
//		ACE_HANDLE handle = ACE_OS::open(strFileName.c_str(), O_RDONLY | O_BINARY);
//
//		if (ACE_INVALID_HANDLE == handle)
//		{
//			int nLastError = ACE_OS::last_error();
//			DSC_RUN_LOG_ERROR("cann't open file:%s, last error code:%d, last error reason:%s.", strFileName.c_str(), nLastError, ACE_OS::strerror(nLastError));
//			DSC_THREAD_SIZE_FREE(pFileBuf, stat.st_size);
//
//			return -1;
//		}
//
//		if (ACE_OS::read(handle, pFileBuf, stat.st_size) < stat.st_size)
//		{
//			int nLastError = ACE_OS::last_error();
//			DSC_RUN_LOG_ERROR("read file:%s error, last error code:%d, last error reason:%s.", strFileName.c_str(), nLastError, ACE_OS::strerror(nLastError));
//			DSC_THREAD_SIZE_FREE(pFileBuf, stat.st_size);
//			ACE_OS::close(handle);
//
//			return -1;
//		}
//
//		ACE_OS::close(handle);		
//		strUserKey.assign(pFileBuf, stat.st_size);
//		DSC_THREAD_SIZE_FREE(pFileBuf, stat.st_size);		
//	}
//
//	return 0;
//}
//
//void CVbhPerfTestQueryBlockClientService::OnDscMsg(VBH::CQueryBlockInfoCltCltRsp& rsp, const CDscMsg::CDscMsgAddr& rSrcMsgAddr)
//{
//	VBH_MESSAGE_ENTER_TRACE(CVbhPerfTestQueryBlockClientService, VBH::CQueryBlockInfoCltCltRsp);
//	
//	++m_nTotalRsp;
//	if (rsp.m_nReturnCode == VBH::EN_OK_TYPE)
//	{
//		++m_nQuerySum;
//	}
//	else
//	{
//		++m_nTotalFailedRsp;
//		DSC_RUN_LOG_ERROR("CQueryBlockInfoCltCltRsp, session-id:%d, error-code:%d", rsp.m_nSrcSessionID, rsp.m_nReturnCode);
//	}
//	
//	CQueryBlockSession* pSession = m_mapQueryBlockSession.Find(rsp.m_nSrcSessionID);
//	
//	if (pSession)
//	{
//		if (m_bLimitTimes)
//		{
//			--pSession->m_nLimitTimes;
//			if (pSession->m_nLimitTimes > 0)
//			{
//				Query(pSession);
//				CDscReactor::ResetDscTimer(pSession, EN_SESSION_TIMEOUT_VALUE);
//			}
//		}
//		else
//		{
//			Query(pSession);
//			CDscReactor::ResetDscTimer(pSession, EN_SESSION_TIMEOUT_VALUE);
//		}
//	}
//	else
//	{
//		DSC_RUN_LOG_ERROR("can not find session, session-id:%d", rsp.m_nSrcSessionID);
//	}
//	
//	VBH_MESSAGE_LEAVE_TRACE(CVbhPerfTestQueryBlockClientService, VBH::CQueryBlockInfoCltCltRsp);
//}
//
//void CVbhPerfTestQueryBlockClientService::Query(CQueryBlockSession* pSession)
//{
//	const CDscMsg::CDscMsgAddr address(CDscAppManager::m_nNodeType, CDscAppManager::m_nNodeID, m_nClientServiceType, DSC::EN_INVALID_ID);
//	VBH::CQueryBlockInfoCltCltReq req;
//
//	VBH::Assign(req.m_userKey, pSession->m_user.m_strUserID);
//	VBH::Assign(req.m_userCryptKey, pSession->m_user.m_strUserCryptKey);
//
//	req.m_nBlockID = m_nBlockNum;
//
//	req.m_nSrcSessionID = pSession->m_nSessionID;
//	req.m_nChannelID = pSession->m_nChannelID;
//
//	this->SendDscMessage(req, address);
//	++m_nTotalReq;
//}
//
//
//CVbhPerfTestQueryBlockClientService::CQueryBlockSession::CQueryBlockSession(CVbhPerfTestQueryBlockClientService& rService)
//	:m_rService(rService)
//{
//
//}
//
//void CVbhPerfTestQueryBlockClientService::CQueryBlockSession::OnTimer(void)
//{
//	m_rService.OnTimeOut(this);
//}
