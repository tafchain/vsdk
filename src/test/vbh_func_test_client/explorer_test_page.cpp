// vbh_explorer_test_page.cpp: 实现文件
//

#include "pch.h"
#include "vbh_func_test.h"
#include "explorer_test_page.h"

#include "client_sdk/client_api.h"
//#include "common/test_def.h"

#include "ace/OS_NS_sys_stat.h"
#include "ace/OS_NS_unistd.h"
#include "ace/OS_NS_fcntl.h"

#include "dsc/codec/dsc_codec/dsc_codec.h"

// CVbhExplorerTestPage 对话框

IMPLEMENT_DYNAMIC(CExplorerTestPage, CMFCPropertyPage)
void UTF8toANSI(CString& strUTF8)
{
	//获取转换为多字节后需要的缓冲区大小，创建多字节缓冲区
	UINT nLen = MultiByteToWideChar(CP_UTF8, NULL, strUTF8, -1, NULL, NULL);
	WCHAR* wszBuffer = new WCHAR[nLen + 1];
	nLen = MultiByteToWideChar(CP_UTF8, NULL, strUTF8, -1, wszBuffer, nLen);
	wszBuffer[nLen] = 0;

	nLen = WideCharToMultiByte(936, NULL, wszBuffer, -1, NULL, NULL, NULL, NULL);
	CHAR* szBuffer = new CHAR[nLen + 1];
	nLen = WideCharToMultiByte(936, NULL, wszBuffer, -1, szBuffer, nLen, NULL, NULL);
	szBuffer[nLen] = 0;
	strUTF8 = szBuffer;
	//清理内存
	delete[]szBuffer;
	delete[]wszBuffer;
}

CExplorerTestPage::CExplorerTestPage(CWnd* pParent /*=nullptr*/)
	: CMFCPropertyPage(IDD_EXPLOERE_TEST_PAGE, IDS_EXPLOERE_TEST_PAGE)
	, m_nPeerID(1)
	, m_strUserCryptKey(_T(""))
	, m_nRequestID(0)
	, m_strQueryResult(_T(""))
	, m_nChannelID(0)
	, m_strQueryKey(_T(""))
	, m_nBlockNum(1)
{

}

CExplorerTestPage::~CExplorerTestPage()
{
}

void CExplorerTestPage::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_EXPLORER_TEST_PAGE_USER_KEY, m_nPeerID);
	DDX_Text(pDX, IDC_EDIT_EXPLORER_TEST_PAGE_USER_CRYPT_KEY, m_strUserCryptKey);
	DDX_Text(pDX, IDC_EDIT_EXPLORER_TEST_PAGE_QUERY_RESULT, m_strQueryResult);
	DDX_Text(pDX, IDC_EDIT_EXPLORER_TEST_PAGE_CHANNEL_ID, m_nChannelID);
	DDX_Text(pDX, IDC_EDIT_EXPLORER_TEST_PAGE_QUERY_INPUT_KEY, m_strQueryKey);
	DDX_Text(pDX, IDC_EDIT_EXPLORER_TEST_PAGE_QUERY_INPUT_BLOCK_NUM, m_nBlockNum);
}


BEGIN_MESSAGE_MAP(CExplorerTestPage, CMFCPropertyPage)
	ON_MESSAGE(CExplorerTestPage::WM_ON_QUERY_BLOCK_COUNT_RSP_MSG, CExplorerTestPage::OnExplorerQueryBlockCountRsp)
	ON_MESSAGE(CExplorerTestPage::WM_ON_QUERY_BLOCK_RSP_MSG, CExplorerTestPage::OnExplorerQueryBlockRsp)
	ON_MESSAGE(CExplorerTestPage::WM_ON_EXPLORER_QUERY_USER_RSP_MSG, CExplorerTestPage::OnExplorerQueryUserRsp)
	ON_MESSAGE(CExplorerTestPage::WM_ON_EXPLORER_QUERY_TRANS_RSP_MSG, CExplorerTestPage::OnExplorerQueryTransRsp)
	ON_BN_CLICKED(IDC_BUTTON_EXPLORER_TEST_PAGE_QUERY_BLOCK_COUNT, &CExplorerTestPage::OnQueryBlockCount)
	ON_BN_CLICKED(IDC_BUTTON_EXPLORER_TEST_PAGE_QUERY_BLOCK, &CExplorerTestPage::OnQueryBlock)
	ON_BN_CLICKED(IDC_BUTTON_EXPLORER_TEST_PAGE_QUERY_TRANS, &CExplorerTestPage::OnQueryTrans)
	ON_STN_CLICKED(IDC_STATIC_EXPLORER_TEST_PAGE_USER_KEY, &CExplorerTestPage::OnStnClickedStaticExplorerTestPageUserKey)
END_MESSAGE_MAP()


// CVbhExplorerTestPage 消息处理程序

BOOL CExplorerTestPage::OnInitDialog()
{
	CMFCPropertyPage::OnInitDialog();

	m_nChannelID = 201;

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CExplorerTestPage::OnQueryBlockCount()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	ACE_stat stat;

	++m_nRequestID;
	if (m_nRequestID >= 0X0FFFFFFF)
	{
		m_nRequestID = 1;
	}

	m_nRequestID |= 0X80000000;

	if (VBH_SDK::ExplorerQueryBlockCount(m_nChannelID, m_nRequestID,
		m_strUserCryptKey.GetBuffer(), m_strUserCryptKey.GetLength(), m_nPeerID))
	{
		AfxMessageBox("Query Block Count Failed");
	}
}


void CExplorerTestPage::OnQueryBlock()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);


	++m_nRequestID;
	if (m_nRequestID >= 0X0FFFFFFF)
	{
		m_nRequestID = 1;
	}
	m_nRequestID |= 0X80000000;


	if (VBH_SDK::ExplorerQueryBlock(m_nChannelID, m_nRequestID, m_nBlockNum,
		m_strUserCryptKey.GetBuffer(), m_strUserCryptKey.GetLength(), m_nPeerID))
	{
		AfxMessageBox("Query Block Information Failed");
	}

}

void CExplorerTestPage::OnQueryTrans()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	++m_nRequestID;
	if (m_nRequestID >= 0X0FFFFFFF)
	{
		m_nRequestID = 1;
	}
	m_nRequestID |= 0X80000000;

	if (0 == m_strQueryKey.GetLength())
	{
		if (VBH_SDK::ExplorerQueryBlockTranList(m_nChannelID, m_nRequestID, m_nBlockNum, 1, 2,
			m_strUserCryptKey.GetBuffer(), m_strUserCryptKey.GetLength(), m_nPeerID))
		{
			AfxMessageBox("Query Transaction Information Failed");
		}
	}
	else
	{
		if (VBH_SDK::ExplorerQueryTrans(m_nChannelID, m_nRequestID, m_strQueryKey.GetBuffer(), m_strQueryKey.GetLength(),
			m_strUserCryptKey.GetBuffer(), m_strUserCryptKey.GetLength(), m_nPeerID))
		{
			AfxMessageBox("Query Transaction Information Failed");
		}

	}

}


LRESULT CExplorerTestPage::OnExplorerQueryBlockRsp(WPARAM wParam, LPARAM lParam)
{
	CExplorerQueryBlockInfoRsp* pExplorerQueryBlockInfoRsp = (CExplorerQueryBlockInfoRsp*)wParam;

	if (pExplorerQueryBlockInfoRsp->m_nReturnCode)
	{
		CString strMessage;

		strMessage.Format("Query Block Info Failed, Return Code: %d", pExplorerQueryBlockInfoRsp->m_nReturnCode);
		MessageBox(strMessage);

		m_strQueryResult = "";
		UpdateData(FALSE);
	}
	else
	{

		m_strQueryResult = pExplorerQueryBlockInfoRsp->m_strBlockInfo.c_str();
		UpdateData(FALSE);
	}

	delete pExplorerQueryBlockInfoRsp;
	return LRESULT(0);
}
LRESULT CExplorerTestPage::OnExplorerQueryBlockCountRsp(WPARAM wParam, LPARAM lParam)
{
	CExplorerQueryBlockCountRsp* pExplorerQueryBlockCountRsp = (CExplorerQueryBlockCountRsp*)wParam;

	if (pExplorerQueryBlockCountRsp->m_nReturnCode)
	{
		CString strMessage;

		strMessage.Format("Query Block Count Failed, Return Code: %d!", pExplorerQueryBlockCountRsp->m_nReturnCode);
		MessageBox(strMessage);

		m_strQueryResult = "";
		UpdateData(FALSE);
	}
	else
	{
		m_strQueryResult.Format(_T("%lld"), pExplorerQueryBlockCountRsp->m_nBlockCount);
		UpdateData(FALSE);
	}

	delete pExplorerQueryBlockCountRsp;
	return LRESULT(0);
}
LRESULT CExplorerTestPage::OnExplorerQueryUserRsp(WPARAM wParam, LPARAM lParam)
{
	return LRESULT(0);
}

LRESULT CExplorerTestPage::OnExplorerQueryTransRsp(WPARAM wParam, LPARAM lParam)
{
	CExplorerQueryTransRsp* pExplorerQueryTransRsp = (CExplorerQueryTransRsp*)wParam;

	if (pExplorerQueryTransRsp->m_nReturnCode)
	{
		CString strMessage;

		strMessage.Format("Query Transaction Info Failed, Return Code: %d", pExplorerQueryTransRsp->m_nReturnCode);
		MessageBox(strMessage);

		m_strQueryResult = "";
		UpdateData(FALSE);
	}
	else
	{
		CString strQueryResult = pExplorerQueryTransRsp->m_strTransInfo.c_str();
		UTF8toANSI(strQueryResult);

		m_strQueryResult = strQueryResult;
		UpdateData(FALSE);
	}

	//DSC::Decode(rsp, (const char*)pExplorerQueryTransRsp->m_strTransInfo.c_str, (size_t)pExplorerQueryTransRsp->m_strTransInfo.size());

	delete pExplorerQueryTransRsp;
	return LRESULT(0);
}


void CExplorerTestPage::OnExplorerQueryBlockRsp(VBH_SDK::CExplorerQueryBlockInfoRsp& rBlockInfoRsp)
{
	CExplorerQueryBlockInfoRsp* pExplorerQueryBlockInfoRsp = new CExplorerQueryBlockInfoRsp;

	pExplorerQueryBlockInfoRsp->m_strBlockInfo.assign(rBlockInfoRsp.m_pJsonBlockInfo, rBlockInfoRsp.m_nJsonBlockInfo);
	pExplorerQueryBlockInfoRsp->m_nReturnCode = rBlockInfoRsp.m_nReturnCode;
	PostMessage(WM_ON_QUERY_BLOCK_RSP_MSG, (WPARAM)pExplorerQueryBlockInfoRsp, NULL);
}

void CExplorerTestPage::OnExplorerQueryBlockCountRsp(VBH_SDK::CExplorerQueryBlockCountRsp& rBlockCountRsp)
{
	CExplorerQueryBlockCountRsp* pExplorerQueryBlockCountRsp = new CExplorerQueryBlockCountRsp;

	pExplorerQueryBlockCountRsp->m_nReturnCode = rBlockCountRsp.m_nReturnCode;
	pExplorerQueryBlockCountRsp->m_nBlockCount = rBlockCountRsp.m_nBlockCount;
	PostMessage(WM_ON_QUERY_BLOCK_COUNT_RSP_MSG, (WPARAM)pExplorerQueryBlockCountRsp, NULL);
}

void CExplorerTestPage::OnExplorerQueryUserRsp(VBH_SDK::CExplorerQueryUserRsp& rUserInfo)
{
}

void CExplorerTestPage::OnExplorerQueryTransRsp(VBH_SDK::CExplorerQueryTransRsp& rTransInfo)
{
	CExplorerQueryTransRsp* pExplorerQueryTransRsp = new CExplorerQueryTransRsp;

	pExplorerQueryTransRsp->m_strTransInfo.assign(rTransInfo.m_pTransInfo, rTransInfo.m_nTransInfoLen);
	pExplorerQueryTransRsp->m_nReturnCode = rTransInfo.m_nReturnCode;
	PostMessage(WM_ON_EXPLORER_QUERY_TRANS_RSP_MSG, (WPARAM)pExplorerQueryTransRsp, NULL);
}

void CExplorerTestPage::OnExplorerQueryBlockTransListRsp(VBH_SDK::CExplorerQueryBlockTransListRsp& rTransInfo)
{
	CExplorerQueryTransRsp* pExplorerQueryTransRsp = new CExplorerQueryTransRsp;

	pExplorerQueryTransRsp->m_strTransInfo.assign(rTransInfo.m_pJsonBlockTransList, rTransInfo.m_nJsonBlockTransListLen);
	pExplorerQueryTransRsp->m_nReturnCode = rTransInfo.m_nReturnCode;
	PostMessage(WM_ON_EXPLORER_QUERY_TRANS_RSP_MSG, (WPARAM)pExplorerQueryTransRsp, NULL);

}


void CExplorerTestPage::OnStnClickedStaticExplorerTestPageUserKey()
{
	// TODO: 在此添加控件通知处理程序代码
}
