#pragma once
#include <afxpropertysheet.h>

#include "generate_crypt_key_pair_page.h"
#include "regist_user_page.h"
#include "generate_user_crypt_key_page.h"

#include "edit_information_page.h"
#include "user_trade_page.h"
#include "explorer_test_page.h"
#include "query_page.h"

#include "client_sdk/client_api.h"
#include "client_sdk/client_api_msg.h"

class CVbhFuncTestCallback : public VBH_SDK::IClientSdkMsgCallback
{
public:
	virtual void OnRegistUserRsp(VBH_SDK::CRegistUserRsp& rRegistUserRsp) override;//m_pUserKey存放的是base64转码后的字符串
	virtual void OnProposeRsp(VBH_SDK::CProposeRsp& rProposeRsp)  override;
	virtual void OnQueryRsp(VBH_SDK::CQueryRsp& rQueryRsp) override;
	virtual void OnQueryTransInfoRsp(VBH_SDK::CGetTransactionInfoRsp& rGetTransInfoRsp);//  override;

	virtual void OnExplorerQueryBlockRsp(VBH_SDK::CExplorerQueryBlockInfoRsp& rBlockInfoRsp);
	virtual void OnExplorerQueryBlockCountRsp(VBH_SDK::CExplorerQueryBlockCountRsp& rBlockCountRsp);
	virtual void OnExplorerQueryUserRsp(VBH_SDK::CExplorerQueryUserRsp& rUserInfo);
	virtual void OnExplorerQueryTransRsp(VBH_SDK::CExplorerQueryTransRsp& rUserInfo);
	virtual void OnExplorerQueryBlockTransListRsp(VBH_SDK::CExplorerQueryBlockTransListRsp& rBlockTransList);

public:
	CRegistUserPage* m_pRegistUserPage;
	CUserTradePage* m_pUserTradePage;
	CQueryPage* m_pQueryPage;
	CExplorerTestPage* m_pExplorerTestPage;
	CEditInformationPage* m_pVbhUserOpPage;

};

class CVbhFuncTestSheet : public CMFCPropertySheet
{
public:
	CVbhFuncTestSheet();
	virtual BOOL OnInitDialog();

private:
	CGenerateCryptKeyPairPage m_genCryptKeyPairPage;
	CRegistUserPage m_registUserPage;
	CGenerateUserCryptKeyPage m_genUserCryptKeyPage;
	CUserTradePage m_userTradePage;
	CQueryPage m_queryPage;
	CExplorerTestPage m_explorerTestPage;


	CEditInformationPage m_editInformationPage;

private:
	CVbhFuncTestCallback m_vbhFuncTestCallback;
};

