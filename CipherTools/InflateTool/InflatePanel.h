#pragma once


// InflatePanel 对话框

class InflatePanel : public CDialogEx
{
	DECLARE_DYNAMIC(InflatePanel)

public:
	InflatePanel(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~InflatePanel();

// 对话框数据
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CWnd *pHexControl1;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
};
