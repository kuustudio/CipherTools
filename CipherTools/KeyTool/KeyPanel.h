#pragma once
#include "afxwin.h"
#include<string>
#include"KeyExpand.h"

using namespace CipherKey;
// KeyPanel 对话框

class KeyPanel : public CDialogEx
{
	DECLARE_DYNAMIC(KeyPanel)

public:
	KeyPanel(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~KeyPanel();

// 对话框数据
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CKeyExpand mKeyExpand;
	CWnd *pHexControl1;
	CComboBox mKeyType;
	CString mKeyText;
	CString mOpensslVer;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCheck1();
	CButton mCheckHex;
	std::string HexToStr(BYTE *pbDest, int nLen);
	std::string StrToHex(BYTE *pbDest, int nLen);
	std::string MyWideCharToMultiByte(BYTE *pbDest, int nLen);
	std::string MyMultiByteToWideChar(BYTE *pbDest, int nLen);
};
