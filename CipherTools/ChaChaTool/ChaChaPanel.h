#pragma once
#include "afxwin.h"
#include<string>


// ChaChaPanel 对话框

class ChaChaPanel : public CDialogEx
{
	DECLARE_DYNAMIC(ChaChaPanel)

public:
	ChaChaPanel(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~ChaChaPanel();

// 对话框数据
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnBnClickedButton2();
public:
	CWnd *pHexControl1;
	CString mKey;
	CString mNonce;
	CButton mCheckHex;
	BOOL bVerify;
public:
public:
	std::string HexToStr(BYTE *pbDest, int nLen);
	std::string StrToHex(BYTE *pbDest, int nLen);
	std::string MyWideCharToMultiByte(BYTE *pbDest, int nLen);
	std::string MyMultiByteToWideChar(BYTE *pbDest, int nLen);
	CString strSSLeay_version;
};
