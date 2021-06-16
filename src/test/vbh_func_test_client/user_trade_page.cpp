// vbh_user_op_page.cpp: 实现文件
//

#include "pch.h"
#include "vbh_func_test.h"
#include "user_trade_page.h"

#include "client_sdk/client_api.h"
//#include "common/test_def.h"

#include "ace/OS_NS_sys_stat.h"
#include "ace/OS_NS_unistd.h"
#include "ace/OS_NS_fcntl.h"

#include "dsc/codec/dsc_codec/dsc_codec.h"
#include "vbh_comm/vbh_comm_error_code.h"
// CVbhUserTradePage 对话框

IMPLEMENT_DYNAMIC(CUserTradePage, CMFCPropertyPage)

CUserTradePage::CUserTradePage(CWnd* pParent /*=nullptr*/)
	: CMFCPropertyPage(IDD_USER_TRADE_PAGE, IDS_USER_TRADE_PAGE)
	, m_strFromUserKey(_T(""))
	, m_strToUserKey(_T(""))
	, m_nTradeAssetNum(0)
	, m_strFromUserCryptKey(_T(""))
	, m_nChannelID(0)
	, m_strTransKey(_T(""))
	, m_strCcReceipt(_T(""))
{

}

CUserTradePage::~CUserTradePage()
{
}

void CUserTradePage::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_UTRD_PAGE_FROM_USER_ID, m_strFromUserKey);
	DDX_Text(pDX, IDC_EDIT_UTRD_PAGE_FROM_USER_CRYPT_KEY, m_strFromUserCryptKey);
	DDX_Text(pDX, IDC_EDIT_UTRD_PAGE_TO_USER_ID, m_strToUserKey);
	DDX_Text(pDX, IDC_EDIT_UTRD_PAGE_ASSET_NUMBER, m_nTradeAssetNum);
	DDX_Text(pDX, IDC_EDIT_UTRD_PAGE_TRANS_ID, m_strTransKey);
	DDX_Text(pDX, IDC_EDIT_UTRD_PAGE_CC_RECEIPT, m_strCcReceipt);
}


BEGIN_MESSAGE_MAP(CUserTradePage, CMFCPropertyPage)
	ON_MESSAGE(CUserTradePage::WM_ON_USER_TRADE_RSP_MSG, CUserTradePage::OnUserTradeRsp)
	ON_MESSAGE(CUserTradePage::WM_ON_USER_TRADE_NOTIFY_MSG, CUserTradePage::OnUserTradeNotify)
	ON_BN_CLICKED(IDC_BUTTON_UTRD_PAGE_DO_TRADE, &CUserTradePage::OnUserTrade)
END_MESSAGE_MAP()


// CVbhUserTradePage 消息处理程序
BOOL CUserTradePage::OnInitDialog()
{
	CMFCPropertyPage::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_strFromUserCryptKey = "";

	m_nChannelID = 201;

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CUserTradePage::OnUserTrade()
{
	UpdateData(TRUE);

	VBH_SDK::CProposeReq req;
	TEST_CC::CTradeAction info;

	info.m_nAsset = m_nTradeAssetNum;
	info.m_fromUserKey.Set(m_strFromUserKey.GetBuffer(), m_strFromUserKey.GetLength());
	info.m_toUserKey.Set(m_strToUserKey.GetBuffer(), m_strToUserKey.GetLength());

	req.m_nActionID = TEST_CC::CTradeAction::EN_ACTION_ID;
	req.m_nSessionID = ++m_nRequestID;//暂时不用

	size_t nProposalLen;
	DSC::Encode(info, req.m_pProposal, nProposalLen);
	req.m_nProposalLen = nProposalLen;
	req.m_pUserCryptKey = m_strFromUserCryptKey.GetBuffer();
	req.m_nUserCryptKeyLen = m_strFromUserCryptKey.GetLength();

	if (VBH_SDK::Propose(req))
	{
		AfxMessageBox("propose failed");
	}
	
	DSC_THREAD_SIZE_FREE(req.m_pProposal, req.m_nProposalLen);
}

LRESULT CUserTradePage::OnUserTradeRsp(WPARAM wParam, LPARAM lParam)
{
	CUserTradeRsp* pUserTradeRsp = (CUserTradeRsp*)wParam;

	if (pUserTradeRsp->m_nReturnCode)
	{
		CString strMessage;

		strMessage.Format("trade failed, record code:%d,%s", pUserTradeRsp->m_nReturnCode, VBH::GetErrorString(pUserTradeRsp->m_nReturnCode));
		MessageBox(strMessage);
	}
	else
	{
		m_strTransKey = pUserTradeRsp->m_strTransKey.c_str();
		m_strCcReceipt = pUserTradeRsp->m_strCcReceipt.c_str();
		UpdateData(FALSE);
	}

	delete pUserTradeRsp;
	return LRESULT(0);
}

LRESULT CUserTradePage::OnUserTradeNotify(WPARAM wParam, LPARAM lParam)
{
	CUserTradeNotify* pUserTradeNotify = (CUserTradeNotify*)wParam;

	if (pUserTradeNotify->m_nReturnCode)
	{
		CString strMessage;

		strMessage.Format("trade failed, record code:%d,%s", pUserTradeNotify->m_nReturnCode, VBH::GetErrorString(pUserTradeNotify->m_nReturnCode));
		MessageBox(strMessage);
	}
	else
	{
		AfxMessageBox("trade noitfy succeed!");
	}

	delete pUserTradeNotify;
	return LRESULT(0);
}

void CUserTradePage::OnProposeRsp(VBH_SDK::CProposeRsp& rProposeRsp)
{
	CUserTradeRsp* pUserTradeRsp = new CUserTradeRsp;

	pUserTradeRsp->m_nReturnCode = rProposeRsp.m_nReturnCode;
	pUserTradeRsp->m_strTransKey.assign(rProposeRsp.m_pTransKey, rProposeRsp.m_nTransKeyLen);
	pUserTradeRsp->m_strCcReceipt.assign(rProposeRsp.m_pReceipt, rProposeRsp.m_nReceiptLen);
	PostMessage(WM_ON_USER_TRADE_RSP_MSG, (WPARAM)pUserTradeRsp, NULL);
}
