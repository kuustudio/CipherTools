#pragma once
#include "afxwin.h"
#include<string>


// TeaPanel 对话框

class TeaPanel : public CDialogEx
{
	DECLARE_DYNAMIC(TeaPanel)

public:
	TeaPanel(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~TeaPanel();

// 对话框数据
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CWnd *pHexControl1;
	virtual BOOL OnInitDialog();
	CComboBox mMode;
	CEdit mKey;
	afx_msg void OnBnClickedCheck1();
	CButton mCheckHex;
public:
public:
	std::string HexToStr(BYTE *pbDest, int nLen);
	std::string StrToHex(BYTE *pbDest, int nLen);
	std::string MyWideCharToMultiByte(BYTE *pbDest, int nLen);
	std::string MyMultiByteToWideChar(BYTE *pbDest, int nLen);
	afx_msg void OnBnClickedButton1();
	CComboBox mRound;
	CComboBox mDelta;
	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnBnClickedButton2();
	CComboBox mComType;
	afx_msg void OnCbnSelchangeCombo4();
};
