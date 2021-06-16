#pragma once

#include <string>
#include "afxpropertypage.h"

#include "ace/Basic_Types.h"

#include "client_sdk/client_api_msg.h"
// CVbhUserOpPage 对话框

class CEditInformationPage : public CMFCPropertyPage
{
private:
	enum
	{
		WM_ON_GET_INFO_KEY_RSP_MSG = WM_USER + 200,
		WM_ON_UPDATE_USR_NOTIFY_MSG,
		WM_ON_COMMIT_INFO_RSP_MSG
	};
	class CInfoRsp
	{
	public:
		ACE_INT32 m_nReturnCode;
		std::string m_strTransKey;
	};

	class CUpdateUserNotify
	{
	public:
		ACE_INT32 m_nReturnCode;
	};

	class CGetUserInfoRsp
	{
	public:
		ACE_INT32 m_nReturnCode;
		char* m_pUserKey;
		size_t m_nUserKeyLen;
	};
	DECLARE_DYNAMIC(CEditInformationPage)

public:
	CEditInformationPage(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CEditInformationPage();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EDIT_INFORMATION_PAGE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();

public:
	afx_msg LRESULT OnProposeRsp(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetInfoKeyRsp(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCommitInfoRsp(WPARAM wParam, LPARAM lParam);


public:
	afx_msg void OnCommitInfo();
	afx_msg void OnGetInfoKey();
	afx_msg void OnClearUserInfo();

public:
	void OnProposeRsp(VBH_SDK::CProposeRsp& rProposeRsp);

public:
	CString m_clientCryptKey;

	CString m_strInitInformation;
	ACE_UINT32 m_nRequestID;
	ACE_UINT32 m_nChannelID;
	CString m_strInformationKey;
	CString m_strDepositInformation;
	CString m_strTransId;
	afx_msg void OnStnClickedStaticRegistUserPageUserKey();
	afx_msg void OnBnClickedButtonQryDoQuery();
};
