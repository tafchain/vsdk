// CVbhSyntheticUuIdPage.cpp: 实现文件
//

#include "pch.h"
#include "vbh_func_test.h"
#include "generate_user_crypt_key_page.h"
#include "afxdialogex.h"
#include "client_sdk/client_api.h"


// CVbhSyntheticUuIdPage 对话框

IMPLEMENT_DYNAMIC(CGenerateUserCryptKeyPage, CMFCPropertyPage)

CGenerateUserCryptKeyPage::CGenerateUserCryptKeyPage(CWnd* pParent /*=nullptr*/)
	: CMFCPropertyPage(IDD_GENERATE_USER_CRYPT_KEY_PAGE, IDS_GENERATE_USER_CRYPT_KEY_PAGE)
	, m_strPrivateKey(_T(""))
	, m_strVbhCryptKey(_T(""))
	, m_strUserCryptKey(_T(""))
	, m_nChannelID(0)
{

}

CGenerateUserCryptKeyPage::~CGenerateUserCryptKeyPage()
{
}

void CGenerateUserCryptKeyPage::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_GUCK_PAGE_PRIVATE_CRYPT_KEY, m_strPrivateKey);
	DDX_Text(pDX, IDC_EDIT_GUCK_PAGE_VBH_CRYPT_KEY, m_strVbhCryptKey);
	DDX_Text(pDX, IDC_EDIT_GUCK_PAGE_USER_CRYPT_KEY, m_strUserCryptKey);
	DDX_Text(pDX, IDC_EDIT_GUCK_PAGE_CHANEL_ID, m_nChannelID);
}

BOOL CGenerateUserCryptKeyPage::OnInitDialog()
{
	CMFCPropertyPage::OnInitDialog();

	m_nChannelID = 201;

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


BEGIN_MESSAGE_MAP(CGenerateUserCryptKeyPage, CMFCPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_GUCK_PAGE_GEN_USER_CRYPT_KEY, &CGenerateUserCryptKeyPage::OnBnGenUserCryptKey)
	ON_BN_CLICKED(IDC_BUTTON_GUCK_PAGE_CLEAR, &CGenerateUserCryptKeyPage::OnBnClickedButtonGuckPageClear)
END_MESSAGE_MAP()


// CVbhSyntheticUuIdPage 消息处理程序


void CGenerateUserCryptKeyPage::OnBnGenUserCryptKey()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	std::string strUserCryptKey;

	if (!VBH_SDK::GenerateUserCryptKey(strUserCryptKey, 
		m_strPrivateKey.GetBuffer(), m_strPrivateKey.GetLength(),
		m_strVbhCryptKey.GetBuffer(), m_strVbhCryptKey.GetLength(), m_nChannelID))
	{
		m_strUserCryptKey = strUserCryptKey.c_str();
	}
	UpdateData(FALSE);
}



void CGenerateUserCryptKeyPage::OnEnChangeEditSyntheticUuIdPageLongUuId()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CMFCPropertyPage::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}


void CGenerateUserCryptKeyPage::OnBnClickedButtonGuckPageClear()
{
	m_strPrivateKey = "";
	m_strVbhCryptKey = "";
	m_strUserCryptKey = "";

	UpdateData(FALSE);
}
