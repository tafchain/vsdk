// vbh_regist_user_page.cpp: 实现文件
//

#include "pch.h"
#include "vbh_func_test.h"
#include "regist_user_page.h"
#include "client_sdk/client_api.h"
//#include "common/test_def.h"

#include "ace/OS_NS_sys_stat.h"
#include "ace/OS_NS_unistd.h"
#include "ace/OS_NS_fcntl.h"

#include "dsc/codec/dsc_codec/dsc_codec.h"
#include "vbh_comm/vbh_comm_error_code.h"
#include "client_c_sdk/client_c_api.h"
// CVbhRegistUserPage 对话框


IMPLEMENT_DYNAMIC(CRegistUserPage, CMFCPropertyPage)

CRegistUserPage::CRegistUserPage(CWnd* pParent /*=nullptr*/)
	: CMFCPropertyPage(IDD_REGIST_USER_PAGE, IDS_REGIST_USER_PAGE)
	, m_strPublicCryptKey(_T(""))
	, m_strVbhCryptKey(_T(""))
	, m_nAsset(0)
	, m_strIdCardNo(_T("7879465487961564878949189"))
	, m_strName(_T("张三"))
	, m_nRequestID(0)
	, m_nChannelID(0)
	, m_strUserID(_T(""))
{
}

CRegistUserPage::~CRegistUserPage()
{
}

void CRegistUserPage::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_REGU_PAGE_PUBLIC_CRYPT_KEY, m_strPublicCryptKey);
	DDX_Text(pDX, IDC_EDIT_REGU_PAGE_VBH_CRYPT_KEY, m_strVbhCryptKey);
	DDX_Text(pDX, IDC_EDIT_REGU_PAGE_ASSET, m_nAsset);
	DDX_Text(pDX, IDC_EDIT_REGU_PAGE_ID_CARD_NO, m_strIdCardNo);
	DDX_Text(pDX, IDC_EDIT_REGU_PAGE_NAME, m_strName);
	DDX_Text(pDX, IDC_EDIT_REGU_PAGE_CHANNEL_ID, m_nChannelID);
	DDX_Text(pDX, IDC_EDIT_REGU_PAGE_USER_ID, m_strUserID);
}


BEGIN_MESSAGE_MAP(CRegistUserPage, CMFCPropertyPage)
	ON_MESSAGE(CRegistUserPage::WM_ON_REG_USR_RSP_MSG, CRegistUserPage::OnRegistUsrRsp)
	ON_BN_CLICKED(IDC_BUTTON_REGIST_USER_PAGE_REGIST, &CRegistUserPage::OnRegistUser)
	ON_BN_CLICKED(IDC_BUTTON_REGU_PAGE_CLEAR, &CRegistUserPage::OnBnClickedButtonReguPageClear)
END_MESSAGE_MAP()


// CVbhRegistUserPage 消息处理程序



BOOL CRegistUserPage::OnInitDialog()
{
	CMFCPropertyPage::OnInitDialog();

	m_nChannelID = 201;
	m_nAsset = 1024;

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CRegistUserPage::OnRegistUser()
{
	UpdateData(TRUE);

	//// TODO: 在此添加控件通知处理程序代码
	//VBH_SDK::CRegistUserReq req;
	//TEST_CC::CCcCheckInfoOnUserRegist info;


	//req.m_nChannelID = m_nChannelID;
	//req.m_nSessionID = ++m_nRequestID;//暂时不用
	//req.m_pCltPubKey = m_strPublicCryptKey.GetBuffer();
	//req.m_nCltPubKeyLen = m_strPublicCryptKey.GetLength();

	//info.m_nAsset = m_nAsset;
	//info.m_userName.Set(m_strName.GetBuffer(), m_strName.GetLength());
	//info.m_idCardNo.Set(m_strIdCardNo.GetBuffer(), m_strIdCardNo.GetLength());

	//size_t nUserInfoLen;
	//DSC::Encode(info, req.m_pUserInfo, nUserInfoLen);
	//req.m_nUserInfoLen = nUserInfoLen;


	reg_vbh_user(m_nChannelID, m_nRequestID,
		m_nAsset, m_strName.GetBuffer(), m_strName.GetLength(),
		m_strIdCardNo.GetBuffer(), m_strIdCardNo.GetLength(),
		m_strPublicCryptKey.GetBuffer(), m_strPublicCryptKey.GetLength());

}

LRESULT CRegistUserPage::OnRegistUsrRsp(WPARAM wParam, LPARAM lParam)
{
	CRegistUserRsp* pRegistUserRsp = (CRegistUserRsp*) wParam;

	if (pRegistUserRsp->m_nReturnCode)
	{
		CString strMessage;

		strMessage.Format("regist user failed, record code:%d,%s", pRegistUserRsp->m_nReturnCode, VBH::GetErrorString(pRegistUserRsp->m_nReturnCode));
		MessageBox(strMessage);
	}
	else
	{
		m_strVbhCryptKey = pRegistUserRsp->m_strVbhCryptKey.c_str();
		m_strUserID = pRegistUserRsp->m_strUserID.c_str();
		UpdateData(FALSE);
	}

	delete pRegistUserRsp;

	return LRESULT(0);
}

void CRegistUserPage::OnRegistUserRsp(VBH_SDK::CRegistUserRsp& rRegistUserRsp)
{
	CRegistUserRsp* pRegistUserRsp = new CRegistUserRsp;

	pRegistUserRsp->m_nReturnCode = rRegistUserRsp.m_nReturnCode;
	pRegistUserRsp->m_nSrcSessionID = rRegistUserRsp.m_nSessionID;
	pRegistUserRsp->m_strUserID.assign(rRegistUserRsp.m_pVbhUserID, rRegistUserRsp.m_nVbhUserIDLen);
	pRegistUserRsp->m_strVbhCryptKey.assign(rRegistUserRsp.m_pVbhCryptKey, rRegistUserRsp.m_nVbhCryptKeyLen);
	
	PostMessage(WM_ON_REG_USR_RSP_MSG, (WPARAM)pRegistUserRsp, NULL);
}



void CRegistUserPage::OnBnClickedButtonReguPageClear()
{
	m_strPublicCryptKey = "";
	m_strVbhCryptKey = "";
	m_strUserID = "";

	UpdateData(FALSE);
}
