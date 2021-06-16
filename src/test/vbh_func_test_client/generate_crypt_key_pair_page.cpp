// vbh_gen_crypt_key_page.cpp: 实现文件
//

#include "pch.h"
#include "vbh_func_test.h"
#include "generate_crypt_key_pair_page.h"

#include "client_sdk/client_api.h"

#include "ace/OS_NS_sys_stat.h"
#include "ace/OS_NS_unistd.h"
#include "ace/OS_NS_fcntl.h"

#include "dsc/dsc_comm_func.h"
//#include "vbh_regist_user_page.h"


// CVbhGenCryptKeyPage 对话框

IMPLEMENT_DYNAMIC(CGenerateCryptKeyPairPage, CMFCPropertyPage)

CGenerateCryptKeyPairPage::CGenerateCryptKeyPairPage(CWnd* pParent /*=nullptr*/)
	: CMFCPropertyPage(IDD_GENERATE_CRYPT_KEY_PAIR_PAGE, IDS_GENERATE_CRYPT_KEY_PAIR_PAGE)
	, m_strPrivateKey(_T(""))
	, m_strPublicKey(_T(""))
{

}

CGenerateCryptKeyPairPage::~CGenerateCryptKeyPairPage()
{
}


BEGIN_MESSAGE_MAP(CGenerateCryptKeyPairPage, CMFCPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_GEN_CRYPT_KEY_PAGE_GENERATE_CRYPT_KEY, &CGenerateCryptKeyPairPage::OnGenerateCryptKey)
	ON_BN_CLICKED(IDC_BUTTON_GCKP_CLEAR, &CGenerateCryptKeyPairPage::OnBnClickedButtonGckpClear)
END_MESSAGE_MAP()


void CGenerateCryptKeyPairPage::OnGenerateCryptKey()
{
	std::string strPriKey;
	std::string strPubKey;

	if (!VBH_SDK::CreateCryptKeyPair(strPubKey, strPriKey))
	{
		m_strPrivateKey = strPriKey.c_str();
		m_strPublicKey = strPubKey.c_str();

		UpdateData(FALSE);
	}
}


void CGenerateCryptKeyPairPage::DoDataExchange(CDataExchange* pDX)
{
	// TODO: 在此添加专用代码和/或调用基类

	CMFCPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_GCKP_PAGE_PRIVATE_KEY, m_strPrivateKey);
	DDX_Text(pDX, IDC_EDIT_GCKP_PAGE_PUBLIC_KEY, m_strPublicKey);
}


void CGenerateCryptKeyPairPage::OnBnClickedButtonGckpClear()
{
	m_strPrivateKey = "";
	m_strPublicKey = "";

	UpdateData(FALSE);
}
