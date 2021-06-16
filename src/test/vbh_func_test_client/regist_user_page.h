#pragma once
#include <string>

#include "afxpropertypage.h"

#include "ace/Basic_Types.h"

#include "client_sdk/client_api_msg.h"
// CVbhRegistUserPage 对话框

class CRegistUserPage : public CMFCPropertyPage
{
private:
	enum
	{
		WM_ON_REG_USR_RSP_MSG = WM_USER + 100,
		WM_ON_REG_USR_NOTIFY_MSG,
	};

	class CRegistUserRsp
	{
	public:
		ACE_INT32 m_nReturnCode;
		ACE_UINT32 m_nSrcSessionID; //本次请求的ID，可以是上层业务的sessionID
		std::string m_strVbhCryptKey;
		std::string m_strUserID ;
	};

	class CRegistUserNotify
	{
	public:
		ACE_INT32 m_nReturnCode;
	};

private:
	DECLARE_DYNAMIC(CRegistUserPage)

public:
	CRegistUserPage(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CRegistUserPage();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_REGIST_USER_PAGE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();

public:
	afx_msg LRESULT OnRegistUsrRsp(WPARAM wParam, LPARAM lParam);

public:
	afx_msg void OnRegistUser();

public:
	void OnRegistUserRsp(VBH_SDK::CRegistUserRsp& rRegistUserRsp);

public:
	CString m_strPublicCryptKey;
	CString m_strVbhCryptKey;
	CString m_strUserID;
	ACE_UINT32 m_nAsset;
	CString m_strIdCardNo;
	CString m_strName;
	ACE_UINT32 m_nRequestID;
	ACE_UINT32 m_nChannelID;
	afx_msg void OnStnClickedStaticRegistUserPageUserBtc();
	
	afx_msg void OnBnClickedButtonReguPageClear();
};
