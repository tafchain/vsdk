#pragma once

#include "afxpropertypage.h"

// vbh_gen_crypt_key_page 对话框

class CGenerateCryptKeyPairPage : public CMFCPropertyPage
{
	DECLARE_DYNAMIC(CGenerateCryptKeyPairPage)

public:
	CGenerateCryptKeyPairPage(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CGenerateCryptKeyPairPage();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_GENERATE_CRYPT_KEY_PAIR_PAGE };
#endif

protected:

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnGenerateCryptKey();
	virtual void DoDataExchange(CDataExchange* pDX);
	CString m_strPrivateKey;
	CString m_strPublicKey;
	afx_msg void OnBnClickedButtonGckpClear();
};
