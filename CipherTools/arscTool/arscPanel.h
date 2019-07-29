#pragma once
#include"ArscAnalyse.h"


// arscPanel 对话框

class arscPanel : public CDialogEx
{
	DECLARE_DYNAMIC(arscPanel)

public:
	arscPanel(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~arscPanel();

// 对话框数据
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString strFile;
	CArscAnalyse mArscAnalyse;
public:
	afx_msg void OnBnClickedButton1();
	CString strReport;
	virtual BOOL OnInitDialog();
};
