#pragma once
#include "afxwin.h"
#include<string>


// CDesPanel 对话框

class CDesPanel : public CDialogEx
{
	DECLARE_DYNAMIC(CDesPanel)

public:
	CDesPanel(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDesPanel();

// 对话框数据
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
public:
	CWnd *pHexControl1;
	CComboBox EncryptMode;
	CComboBox mPadding;
	
	CEdit mKey;
	CEdit mIv;
	CButton mCheckHex;
public:
	std::string HexToStr(BYTE *pbDest, int nLen);
	std::string StrToHex(BYTE *pbDest, int nLen);
	std::string MyWideCharToMultiByte(BYTE *pbDest, int nLen);
	std::string MyMultiByteToWideChar(BYTE *pbDest, int nLen);
	CString strSSLeay_version;
};
