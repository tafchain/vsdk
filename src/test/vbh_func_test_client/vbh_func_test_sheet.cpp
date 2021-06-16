#include "pch.h"
#include "vbh_func_test_sheet.h"

void CVbhFuncTestCallback::OnRegistUserRsp(VBH_SDK::CRegistUserRsp& rRegistUserRsp)
{
	m_pRegistUserPage->OnRegistUserRsp(rRegistUserRsp);
}

void CVbhFuncTestCallback::OnProposeRsp(VBH_SDK::CProposeRsp& rProposeRsp)
{
	if (3 == rProposeRsp.m_nActionID)
	{
		m_pUserTradePage->OnProposeRsp(rProposeRsp);
	}
	else
	{
		m_pVbhUserOpPage->OnProposeRsp(rProposeRsp);
	}
}

void CVbhFuncTestCallback::OnQueryRsp(VBH_SDK::CQueryRsp& rQueryRsp)
{

	m_pQueryPage->OnQueryRsp(rQueryRsp);

}

void CVbhFuncTestCallback::OnQueryTransInfoRsp(VBH_SDK::CGetTransactionInfoRsp& rGetTransInfoRsp)
{
}

void CVbhFuncTestCallback::OnExplorerQueryBlockRsp(VBH_SDK::CExplorerQueryBlockInfoRsp& rBlockInfoRsp)
{
	m_pExplorerTestPage->OnExplorerQueryBlockRsp(rBlockInfoRsp);
}

void CVbhFuncTestCallback::OnExplorerQueryBlockCountRsp(VBH_SDK::CExplorerQueryBlockCountRsp& rBlockCountRsp)
{
	m_pExplorerTestPage->OnExplorerQueryBlockCountRsp(rBlockCountRsp);
}

void CVbhFuncTestCallback::OnExplorerQueryUserRsp(VBH_SDK::CExplorerQueryUserRsp& rUserInfo)
{
}

void CVbhFuncTestCallback::OnExplorerQueryTransRsp(VBH_SDK::CExplorerQueryTransRsp& rTransInfo)
{
	m_pExplorerTestPage->OnExplorerQueryTransRsp(rTransInfo);
}

void CVbhFuncTestCallback::OnExplorerQueryBlockTransListRsp(VBH_SDK::CExplorerQueryBlockTransListRsp& rBlockTransList)
{
	m_pExplorerTestPage->OnExplorerQueryBlockTransListRsp(rBlockTransList);
}
CVbhFuncTestSheet::CVbhFuncTestSheet()
: CMFCPropertySheet(_T("VBH功能测试"))
{
	m_psh.dwFlags &= ~PSH_HASHELP;
	m_psh.dwFlags |= PSH_NOAPPLYNOW;

	//如果要去掉帮助按钮，一定要每个属性页都去掉该按钮
	m_genCryptKeyPairPage.m_psp.dwFlags &= ~PSP_HASHELP;
	m_registUserPage.m_psp.dwFlags &= ~PSP_HASHELP;
	m_userTradePage.m_psp.dwFlags &= ~PSP_HASHELP;
	m_queryPage.m_psp.dwFlags &= ~PSP_HASHELP;
	m_explorerTestPage.m_psp.dwFlags &= ~PSP_HASHELP;
	m_genUserCryptKeyPage.m_psp.dwFlags &= ~PSP_HASHELP;
	m_editInformationPage.m_psp.dwFlags &= ~PSP_HASHELP;


	AddPage(&m_genCryptKeyPairPage);
	AddPage(&m_registUserPage);
	AddPage(&m_genUserCryptKeyPage);
	AddPage(&m_userTradePage);
	AddPage(&m_queryPage);
	AddPage(&m_explorerTestPage);

	AddPage(&m_editInformationPage);
	
	m_vbhFuncTestCallback.m_pRegistUserPage = &m_registUserPage;
	m_vbhFuncTestCallback.m_pUserTradePage = &m_userTradePage;
	m_vbhFuncTestCallback.m_pQueryPage = &m_queryPage;
	m_vbhFuncTestCallback.m_pExplorerTestPage = &m_explorerTestPage;
	m_vbhFuncTestCallback.m_pVbhUserOpPage = &m_editInformationPage;
}


BOOL CVbhFuncTestSheet::OnInitDialog()
{
	BOOL bResult = CMFCPropertySheet::OnInitDialog();

	// TODO:  在此添加您的专用代码
	CWnd* pWnd = GetDlgItem(IDCANCEL);
	if (pWnd)
	{
		pWnd->ShowWindow(FALSE);
	}

	if (VBH_SDK::InitClientSdk(999, &m_vbhFuncTestCallback))
	{
		bResult = FALSE;
	}

	return bResult;
}
