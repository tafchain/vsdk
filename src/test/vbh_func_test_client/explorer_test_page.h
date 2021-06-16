#pragma once

#include <string>
#include "afxdialogex.h"

#include "ace/Basic_Types.h"

#include "client_sdk/client_api_msg.h"

// CVbhExplorerTestPage 对话框

class CExplorerTestPage : public CMFCPropertyPage
{
private:
	enum
	{
		WM_ON_QUERY_BLOCK_COUNT_RSP_MSG = WM_USER + 500,
		WM_ON_QUERY_BLOCK_RSP_MSG,
		WM_ON_EXPLORER_QUERY_USER_RSP_MSG,
		WM_ON_EXPLORER_QUERY_TRANS_RSP_MSG
	};
	class CExplorerQueryBlockCountRsp
	{
	public:
		int32_t m_nReturnCode;
		uint64_t m_nBlockCount;
	};

	class CExplorerQueryBlockInfoRsp
	{
	public:
		int32_t m_nReturnCode;
		std::string m_strBlockInfo;
	};

	class CExplorerQueryTransRsp
	{
	public:
		int32_t m_nReturnCode;
		std::string m_strTransInfo;
	};

	DECLARE_DYNAMIC(CExplorerTestPage)

public:
	CExplorerTestPage(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CExplorerTestPage();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EXPLOERE_TEST_PAGE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();

public:
	afx_msg void OnQueryBlockCount();
	afx_msg void OnQueryBlock();
	afx_msg void OnQueryTrans();

public:
	afx_msg LRESULT OnExplorerQueryBlockRsp(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnExplorerQueryBlockCountRsp(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnExplorerQueryUserRsp(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnExplorerQueryTransRsp(WPARAM wParam, LPARAM lParam);


public:
	void OnExplorerQueryBlockRsp(VBH_SDK::CExplorerQueryBlockInfoRsp& rBlockInfoRsp);
	void OnExplorerQueryBlockCountRsp(VBH_SDK::CExplorerQueryBlockCountRsp& rBlockCountRsp);
	void OnExplorerQueryUserRsp(VBH_SDK::CExplorerQueryUserRsp& rUserInfo);
	void OnExplorerQueryTransRsp(VBH_SDK::CExplorerQueryTransRsp& rTransInfo);
	void OnExplorerQueryBlockTransListRsp(VBH_SDK::CExplorerQueryBlockTransListRsp& rTransInfo);

public:
	ACE_UINT32 m_nPeerID;
	CString m_strUserCryptKey;
	ACE_UINT32 m_nRequestID;
	CString m_strQueryResult;
	ACE_UINT32 m_nChannelID;
	CString m_strQueryKey;
	ACE_UINT64 m_nBlockNum;
	afx_msg void OnStnClickedStaticExplorerTestPageUserKey();
};
