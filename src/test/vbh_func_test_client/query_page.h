#pragma once
#include "afxpropertypage.h"

#include "ace/Basic_Types.h"
#include "dsc/container/dsc_string.h"

#include "client_sdk/client_api_msg.h"
#include <string>
// CQueryPage 对话框

class CQueryPage : public CMFCPropertyPage
{
	DECLARE_DYNAMIC(CQueryPage)
private:
	enum
	{
		WM_ON_QUERY_RSP_MSG = WM_USER + 400,
	};

	class CQueryRsp
	{
	public:
		ACE_INT32 m_nReturnCode;
		ACE_UINT32 m_nActionID;
		std::string m_strInfo;
	};

public:
	CQueryPage(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CQueryPage();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_QUERY_PAGE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	void OnQueryRsp(VBH_SDK::CQueryRsp& rQueryRsp);

public:
	CString m_strUserCryptKey;
	CString m_strUserID;
	CString m_strInformationID;
	CString m_strTransID;
	CString m_strResult;
	BOOL m_nQueryType;
	afx_msg void OnBnClickedButtonQryDoQuery();

protected:
	afx_msg LRESULT OnQueryRspMsg(WPARAM wParam, LPARAM lParam);
	;
	;
	;
protected:
	void FormatUser(CDscString& strOut, const ACE_UINT32 nVersion, const CDscString& key, const CDscString& value);
};
