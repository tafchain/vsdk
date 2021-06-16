#pragma once

#include "afxdialogex.h"
#include "ace/Basic_Types.h"

// CVbhSyntheticUuIdPage 对话框

class CGenerateUserCryptKeyPage : public CMFCPropertyPage
{
	DECLARE_DYNAMIC(CGenerateUserCryptKeyPage)

public:
	CGenerateUserCryptKeyPage(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CGenerateUserCryptKeyPage();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_GENERATE_USER_CRYPT_KEY_PAGE};
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:

	virtual BOOL OnInitDialog();
	CString m_strPrivateKey;
	CString m_strVbhCryptKey;
	CString m_strUserCryptKey;
	ACE_UINT32 m_nChannelID;
	afx_msg void OnBnGenUserCryptKey();

	afx_msg void OnEnChangeEditSyntheticUuIdPageLongUuId();
	afx_msg void OnBnClickedButtonGuckPageClear();
};
