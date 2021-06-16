// query_page.cpp: 实现文件
//

#include "pch.h"
#include "vbh_func_test.h"
#include "query_page.h"

#include "client_sdk/client_api.h"
//#include "common/test_def.h"

#include "ace/OS_NS_sys_stat.h"
#include "ace/OS_NS_unistd.h"
#include "ace/OS_NS_fcntl.h"

#include "dsc/codec/dsc_codec/dsc_codec.h"
#include "vbh_comm/vbh_comm_error_code.h"
// CQueryPage 对话框
extern void UTF8toANSI(CString& strUTF8);
IMPLEMENT_DYNAMIC(CQueryPage, CMFCPropertyPage)

CQueryPage::CQueryPage(CWnd* pParent /*=nullptr*/)
	: CMFCPropertyPage(IDD_QUERY_PAGE, IDS_QUERY_PAGE)
	, m_strUserCryptKey(_T(""))
	, m_strUserID(_T(""))
	, m_strInformationID(_T(""))
	, m_strTransID(_T(""))
	, m_strResult(_T(""))
	, m_nQueryType(FALSE)
{

}

CQueryPage::~CQueryPage()
{
}

void CQueryPage::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_QRY_USER_CRYPT_KEY, m_strUserCryptKey);
	DDX_Text(pDX, IDC_EDIT_QRY_USER_ID, m_strUserID);
	DDX_Text(pDX, IDC_EDIT_INFORMATION_ID, m_strInformationID);
	DDX_Text(pDX, IDC_EDIT_QRY_TRANS_ID, m_strTransID);
	DDX_Text(pDX, IDC_EDIT_QRY_RESULT, m_strResult);
	DDX_Radio(pDX, IDC_RADIO_QRY_USER, m_nQueryType);
}


BEGIN_MESSAGE_MAP(CQueryPage, CMFCPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_QRY_DO_QUERY, &CQueryPage::OnBnClickedButtonQryDoQuery)
	ON_MESSAGE(WM_ON_QUERY_RSP_MSG, &CQueryPage::OnQueryRspMsg)
END_MESSAGE_MAP()


// CQueryPage 消息处理程序



void CQueryPage::OnQueryRsp(VBH_SDK::CQueryRsp& rQueryRsp)
{
	CQueryPage::CQueryRsp* pRsp = new CQueryPage::CQueryRsp;

	pRsp->m_nReturnCode = rQueryRsp.m_nReturnCode;
	pRsp->m_nActionID = rQueryRsp.m_nActionID;

	pRsp->m_strInfo.assign(rQueryRsp.m_pInfo, rQueryRsp.m_nInfoLen);

	PostMessage(WM_ON_QUERY_RSP_MSG, (WPARAM)pRsp, NULL);
}

void CQueryPage::OnBnClickedButtonQryDoQuery()
{
	UpdateData(TRUE);

	VBH_SDK::CQueryReq req;

	if (m_nQueryType == 0) //查询用户
	{
		size_t nParamLen;
		TEST_CC::CQueryUserAction action;

		action.m_userID.Set(m_strUserID.GetBuffer(), m_strUserID.GetLength());

		DSC::Encode(action, req.m_pParam, nParamLen);
		req.m_nParamLen = nParamLen;
		req.m_nDstPeerID = 1; //暂时先固定为1， 可以在界面上加peer-id输入框
		req.m_nActionID = TEST_CC::CQueryUserAction::EN_ACTION_ID;
		req.m_pUserCryptKey = m_strUserCryptKey.GetBuffer();
		req.m_nUserCryptKeyLen = m_strUserCryptKey.GetLength();

		if (VBH_SDK::Query(req))
		{
			AfxMessageBox("query failed");
		}

		DSC_THREAD_SIZE_FREE(req.m_pParam, req.m_nParamLen);
	}
	else if (m_nQueryType == 2) //查询交易
	{
		size_t nParamLen;
		TEST_CC::CQueryTransAction action;

		action.m_transID.Set(m_strTransID.GetBuffer(), m_strTransID.GetLength());

		DSC::Encode(action, req.m_pParam, nParamLen);
		req.m_nParamLen = nParamLen;
		req.m_nDstPeerID = 1;
		req.m_nActionID = TEST_CC::CQueryTransAction::EN_ACTION_ID;
		req.m_pUserCryptKey = m_strUserCryptKey.GetBuffer();
		req.m_nUserCryptKeyLen = m_strUserCryptKey.GetLength();

		if (VBH_SDK::Query(req))
		{
			AfxMessageBox("query failed");
		}

		DSC_THREAD_SIZE_FREE(req.m_pParam, req.m_nParamLen);
	}
	else if (m_nQueryType == 1) //查询信息溯源
	{
		size_t nParamLen;
		TEST_CC::CQueryInfoHistoryListAction action;

		action.m_informationID.Set(m_strInformationID.GetBuffer(), m_strInformationID.GetLength());

		DSC::Encode(action, req.m_pParam, nParamLen);
		req.m_nParamLen = nParamLen;
		req.m_nDstPeerID = 1;
		req.m_nActionID = TEST_CC::CQueryInfoHistoryListAction::EN_ACTION_ID;
		req.m_pUserCryptKey = m_strUserCryptKey.GetBuffer();
		req.m_nUserCryptKeyLen = m_strUserCryptKey.GetLength();

		if (VBH_SDK::Query(req))
		{
			AfxMessageBox("query failed");
		}

		DSC_THREAD_SIZE_FREE(req.m_pParam, req.m_nParamLen);
	}

}


afx_msg LRESULT CQueryPage::OnQueryRspMsg(WPARAM wParam, LPARAM lParam)
{
	CQueryPage::CQueryRsp* pQueryRsp = (CQueryPage::CQueryRsp*)wParam;

	if (pQueryRsp->m_nReturnCode)
	{
		CString strMessage;

		strMessage.Format("query failed, record code:%d,%s", pQueryRsp->m_nReturnCode, VBH::GetErrorString(pQueryRsp->m_nReturnCode));
		MessageBox(strMessage);
	}
	else
	{
		std::string strMsg;
		if (!VBH_SDK::SDKDecode(pQueryRsp->m_strInfo.c_str(), pQueryRsp->m_strInfo.size(), pQueryRsp->m_nActionID, strMsg))
		{
			m_strResult = strMsg.c_str();
		}
		else
		{
			CString strMessage;

			strMessage.Format("error action-id:%d", pQueryRsp->m_nActionID);
			MessageBox(strMessage);
		}
		
		//switch (pQueryRsp->m_nActionID)
		//{
		//case TEST_CC::CQueryUserAction::EN_ACTION_ID:
		//{
		//	TEST_CC::CQueryUserRsp rsp;

		//	if (DSC::Decode(rsp, pQueryRsp->m_strInfo.c_str(), pQueryRsp->m_strInfo.size()))
		//	{
		//		MessageBox(_T("Decode response info failed."));
		//	}
		//	else
		//	{
		//		CDscString c1;

		//		FormatUser(c1, rsp.m_nVersion, rsp.m_key, rsp.m_value);
		//		m_strResult = c1.c_str();
		//	}
		//}
		//break;
		//case TEST_CC::CQueryTransAction::EN_ACTION_ID:
		//{
		//	TEST_CC::CQueryTransRsp rsp;
		//	


		//	if (DSC::Decode(rsp, pQueryRsp->m_strInfo.c_str(), pQueryRsp->m_strInfo.size()))
		//	{
		//		MessageBox(_T("Decode response info failed."));
		//	}
		//	else
		//	{
		//		CDscString c1;
		//		CDscString c2;
		//		CDscString signature;
		//		char* pHexBuf;
		//		size_t hexBufSize;
		//		pHexBuf = DSC_THREAD_SIZE_MALLOC(rsp.m_signature.size() << 1);
		//		::memset(pHexBuf, 0, (rsp.m_signature.size() << 1));
		//		hexBufSize = DSC::ToHex(pHexBuf, rsp.m_signature.c_str(), rsp.m_signature.size());
		//		signature.assign(pHexBuf, hexBufSize);

		//		c1 += "trans-key:";
		//		c1 += rsp.m_transKey;
		//		c1 += "\r\n";

		//		c1 += "user:";
		//		c1 += rsp.m_userKey;
		//		c1 += "\r\n";

		//		c1 += "signature:";
		//		c1 += signature;
		//		c1 += "\r\n";

		//		c1 += "propose:";
		//		c1 += rsp.m_proposal;
		//		c1 += "\r\n";

		//		c1 += "-----------------------\r\n";

		//		for (ACE_UINT16 idx = 0; idx < rsp.m_vecWs.Size(); ++idx)
		//		{
		//			FormatUser(c2, rsp.m_vecWs[idx].m_nVersion, rsp.m_vecWs[idx].m_key, rsp.m_vecWs[idx].m_value);

		//			c1 += c2;
		//			c1 += "\r\n-----------------------\r\n";
		//		}

		//		m_strResult = c1.c_str();
		//		UTF8toANSI(m_strResult);
		//	}
		//}
		//break;
		//default:
		//{
		//	CString strMessage;

		//	strMessage.Format("error action-id:%d", pQueryRsp->m_nActionID);
		//	MessageBox(strMessage);
		//}
		//}

		UpdateData(FALSE);
	}

	delete pQueryRsp;

	return 0;
}

void CQueryPage::FormatUser(CDscString& strOut, const ACE_UINT32 nVersion, const CDscString& key, const CDscString& value)
{
	strOut.clear();

	strOut += "key:";
	strOut += key;
	strOut += "\r\n";

	strOut += "value:";
	strOut += value;
	strOut += "\r\n";

	strOut += "version:";
	strOut += nVersion;
}
