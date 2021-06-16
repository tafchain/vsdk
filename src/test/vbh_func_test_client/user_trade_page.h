#pragma once
#include "afxpropertypage.h"

#include "ace/Basic_Types.h"

#include "client_sdk/client_api_msg.h"
#include <string>
// CVbhUserTradePage 对话框

class CUserTradePage : public CMFCPropertyPage
{
private:
	enum
	{
		WM_ON_USER_TRADE_RSP_MSG = WM_USER + 200,
		WM_ON_USER_TRADE_NOTIFY_MSG
	};
	class CUserTradeRsp
	{
	public:
		ACE_INT32 m_nReturnCode;
		std::string m_strCcReceipt;
		std::string m_strTransKey;
	};

	class CUserTradeNotify
	{
	public:
		ACE_INT32 m_nReturnCode;
	};

	DECLARE_DYNAMIC(CUserTradePage)

public:
	CUserTradePage(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CUserTradePage();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_USER_TRADE_PAGE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();

public:
	afx_msg LRESULT OnUserTradeRsp(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUserTradeNotify(WPARAM wParam, LPARAM lParam);

public:
	afx_msg void OnUserTrade();

public:
	void OnProposeRsp(VBH_SDK::CProposeRsp& rProposeRsp);

public:
	ACE_UINT32 m_nTradeAssetNum;
	CString m_strFromUserKey;
	CString m_strToUserKey;
	CString m_strFromUserCryptKey;
	ACE_UINT32 m_nRequestID = 0;
	ACE_UINT32 m_nChannelID;
	CString m_strTransKey;
	CString m_strCcReceipt;
};
