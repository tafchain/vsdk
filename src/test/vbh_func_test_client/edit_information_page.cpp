// vbh_user_op_page.cpp: 实现文件
//

#include "pch.h"
#include "vbh_func_test.h"
#include "edit_information_page.h"
#include "afxdialogex.h"
#include "client_sdk/client_api.h"
//#include "common/test_def.h"

#include "ace/OS_NS_sys_stat.h"
#include "ace/OS_NS_unistd.h"
#include "ace/OS_NS_fcntl.h"

#include "dsc/codec/dsc_codec/dsc_codec.h"
#include "vbh_comm/vbh_comm_error_code.h"
#include "vbh_sdk/client_c_sdk/client_c_api.h"

// CVbhUserOpPage 对话框

IMPLEMENT_DYNAMIC(CEditInformationPage, CMFCPropertyPage)


void ANSItoUTF8(CString& strAnsi)
{
	//获取转换为宽字节后需要的缓冲区大小，创建宽字节缓冲区，936为简体中文GB2312代码页
	UINT nLen = MultiByteToWideChar(936, NULL, strAnsi, -1, NULL, NULL);
	WCHAR* wszBuffer = new WCHAR[nLen + 1];
	nLen = MultiByteToWideChar(936, NULL, strAnsi, -1, wszBuffer, nLen);
	wszBuffer[nLen] = 0;
	//获取转为UTF8多字节后需要的缓冲区大小，创建多字节缓冲区
	nLen = WideCharToMultiByte(CP_UTF8, NULL, wszBuffer, -1, NULL, NULL, NULL, NULL);
	CHAR* szBuffer = new CHAR[nLen + 1];
	nLen = WideCharToMultiByte(CP_UTF8, NULL, wszBuffer, -1, szBuffer, nLen, NULL, NULL);
	szBuffer[nLen] = 0;
	strAnsi = szBuffer;
	//内存清理
	delete[]wszBuffer;
	delete[]szBuffer;
}

CEditInformationPage::CEditInformationPage(CWnd* pParent /*=nullptr*/)
	: CMFCPropertyPage(IDD_EDIT_INFORMATION_PAGE, IDS_VBH_USER_OP_PAGE)
	, m_clientCryptKey(_T(""))
	, m_strInitInformation(_T(""))
	, m_strTransId(_T(""))
	, m_strDepositInformation(_T(""))
	, m_nRequestID(0)
	, m_nChannelID(0)
	, m_strInformationKey(_T(""))
{
}

CEditInformationPage::~CEditInformationPage()
{
}

void CEditInformationPage::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_USER_OP_PAGE_USER_KEY, m_clientCryptKey);
	DDX_Text(pDX, IDC_EDIT_USER_OP_PAGE_COMMIT_INFO_TRANS_ID, m_strTransId);
	DDX_Text(pDX, IDC_EDIT_USER_OP_PAGE_INIT_INFORMATION, m_strInitInformation);
	DDX_Text(pDX, IDC_EDIT_USER_OP_PAGE_DEPOSIT_INFORMATION, m_strDepositInformation);
	DDX_Text(pDX, IDC_EDIT_USER_OP_PAGE_CHANNEL_ID, m_nChannelID);
	DDX_Text(pDX, IDC_EDIT_USER_OP_PAGE_TXID, m_strInformationKey);
}


BEGIN_MESSAGE_MAP(CEditInformationPage, CMFCPropertyPage)

	ON_MESSAGE(CEditInformationPage::WM_ON_COMMIT_INFO_RSP_MSG, CEditInformationPage::OnCommitInfoRsp)
	ON_MESSAGE(CEditInformationPage::WM_ON_GET_INFO_KEY_RSP_MSG, CEditInformationPage::OnGetInfoKeyRsp)
	ON_BN_CLICKED(IDC_BUTTON_USER_OP_PAGE_COMMIT_INFO, &CEditInformationPage::OnCommitInfo)
	ON_BN_CLICKED(IDC_BUTTON_USER_OP_PAGE_GET_INFO_KEY, &CEditInformationPage::OnGetInfoKey)
	ON_BN_CLICKED(IDC_BUTTON_USER_OP_PAGE_CLEAR_USER_INFO, &CEditInformationPage::OnClearUserInfo)
	ON_STN_CLICKED(IDC_STATIC_REGIST_USER_PAGE_USER_KEY, &CEditInformationPage::OnStnClickedStaticRegistUserPageUserKey)
	ON_BN_CLICKED(IDC_BUTTON_QRY_DO_QUERY, &CEditInformationPage::OnBnClickedButtonQryDoQuery)
END_MESSAGE_MAP()


// CVbhUserOpPage 消息处理程序
BOOL CEditInformationPage::OnInitDialog()
{
	CMFCPropertyPage::OnInitDialog();

	m_nChannelID = 201;

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

LRESULT CEditInformationPage::OnGetInfoKeyRsp(WPARAM wParam, LPARAM lParam)
{
	CInfoRsp* pGetInfoKeyRsp = (CInfoRsp*)wParam;

	if (pGetInfoKeyRsp->m_nReturnCode)
	{
		CString strMessage;

		strMessage.Format("update user failed, record code:%d,%s", pGetInfoKeyRsp->m_nReturnCode, VBH::GetErrorString(pGetInfoKeyRsp->m_nReturnCode));
		MessageBox(strMessage);
	}
	else
	{
		m_strInformationKey = pGetInfoKeyRsp->m_strTransKey.c_str();
		UpdateData(FALSE);
		std::string msg = "update user succeed! Info id is:\n" + pGetInfoKeyRsp->m_strTransKey;
		AfxMessageBox(msg.c_str());
	}

	delete pGetInfoKeyRsp;

	return LRESULT(0);
}

LRESULT CEditInformationPage::OnCommitInfoRsp(WPARAM wParam, LPARAM lParam)
{
	CInfoRsp* pCommitInfoRsp = (CInfoRsp*)wParam;

	if (pCommitInfoRsp->m_nReturnCode)
	{
		CString strMessage;

		strMessage.Format("update user failed, record code:%d,%s", pCommitInfoRsp->m_nReturnCode, VBH::GetErrorString(pCommitInfoRsp->m_nReturnCode));
		MessageBox(strMessage);
	}
	else
	{
		m_strTransId = pCommitInfoRsp->m_strTransKey.c_str();
		UpdateData(FALSE);
		std::string msg = "update user succeed! Transaction id is:\n" + pCommitInfoRsp->m_strTransKey;
		AfxMessageBox(msg.c_str());
	}

	delete pCommitInfoRsp;

	return LRESULT(0);
}


void CEditInformationPage::OnProposeRsp(VBH_SDK::CProposeRsp& rProposeRsp)
{
	CInfoRsp* pInfoRsp = new CInfoRsp;
	if (TEST_CC::EN_CREATE_INFORMATION_ACTION_ID == rProposeRsp.m_nActionID)
	{
		pInfoRsp->m_nReturnCode = rProposeRsp.m_nReturnCode;
		pInfoRsp->m_strTransKey.assign(rProposeRsp.m_pInfoID, rProposeRsp.m_nInfoIdLen);
		PostMessage(WM_ON_GET_INFO_KEY_RSP_MSG, (WPARAM)pInfoRsp, NULL);
	}
	else if (TEST_CC::EN_COMMIT_INFORMATION_ACTION_ID == rProposeRsp.m_nActionID)
	{
		pInfoRsp->m_nReturnCode = rProposeRsp.m_nReturnCode;
		pInfoRsp->m_strTransKey.assign(rProposeRsp.m_pTransKey, rProposeRsp.m_nTransKeyLen);
		PostMessage(WM_ON_COMMIT_INFO_RSP_MSG, (WPARAM)pInfoRsp, NULL);
	}
}


void CEditInformationPage::OnCommitInfo()
{
	UpdateData(TRUE);

	++m_nRequestID;
	if (m_nRequestID >= 0X0FFFFFFF)
	{
		m_nRequestID = 2;
	}

	if (vbh_propose_commit_information(true, m_nRequestID,
		m_strInformationKey.GetBuffer(), m_strInformationKey.GetLength(), 
		m_strDepositInformation.GetBuffer(), m_strDepositInformation.GetLength(), /*存证内容*/
		m_clientCryptKey.GetBuffer(), m_clientCryptKey.GetLength()))
	{
		AfxMessageBox("propose failed");
	}

}

void CEditInformationPage::OnGetInfoKey()
{
	UpdateData(TRUE);
	
	++m_nRequestID;
	if (m_nRequestID >= 0X0FFFFFFF)
	{
		m_nRequestID = 2;
	}

	if (vbh_propose_creat_information_id(m_nRequestID, m_strInitInformation.GetBuffer(),
		m_strInitInformation.GetLength(), m_clientCryptKey.GetBuffer(), m_clientCryptKey.GetLength()))
	{
		AfxMessageBox("propose failed");
	}
}


void CEditInformationPage::OnClearUserInfo()
{
	// TODO: 在此添加控件通知处理程序代码

	m_strInitInformation = "";
    m_strInformationKey = "";
	m_strDepositInformation = "";
	m_strTransId = "";
	UpdateData(FALSE);
}


void CEditInformationPage::OnBnClickedButtonQryDoQuery()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CEditInformationPage::OnStnClickedStaticRegistUserPageUserKey()
{

}



