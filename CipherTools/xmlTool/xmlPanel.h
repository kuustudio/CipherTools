#pragma once
#include"xmlAnalyse.h"

// xmlPanel 对话框

class xmlPanel : public CDialogEx
{
	DECLARE_DYNAMIC(xmlPanel)

public:
	xmlPanel(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~xmlPanel();

// 对话框数据
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CXmlAnalyse mXmlAnalyse;
	CString xmlFilePath;
	CString strXml;
	CString strMagic;
	CString strFileSize;
public:
	afx_msg void OnBnClickedButton1();
};
