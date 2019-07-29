#pragma once
#include "afxwin.h"
#include "MyEditBrowseCtrl.h"
#include<string>
#include"ParseProto.h"


// CProtobufPanel 对话框

class CProtobufPanel : public CDialogEx
{
	DECLARE_DYNAMIC(CProtobufPanel)

public:
	CProtobufPanel(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CProtobufPanel();

// 对话框数据
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	virtual BOOL OnInitDialog();
public:
	CWnd *pHexControl1;
	CWnd *pHexControl2;
	CString mEdtResult;
	CString mValue;
	CEdit mProtoEdit;
	CString mProtoFile;
	CMyEditBrowseCtrl mProtoFileEdit;
	CComboBox mMessageList;
public:
	std::wstring StringToWString(const std::string &str);
	std::string WStringToString(const std::wstring &wstr);
	ParseProto mParseProto;
};
