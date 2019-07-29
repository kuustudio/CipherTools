#pragma once
#include"DexAnalyse.h"


// dexPanel 对话框

class dexPanel : public CDialogEx
{
	DECLARE_DYNAMIC(dexPanel)

public:
	dexPanel(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~dexPanel();

// 对话框数据
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	virtual BOOL OnInitDialog();
	CString strFile;
public:
	CDexAnalyse mDexAnalyse;
	CString strReport;
};
