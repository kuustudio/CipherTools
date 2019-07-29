// xmlPanel.cpp : 实现文件
//

#include "stdafx.h"
#include "xmlTool.h"
#include "xmlPanel.h"
#include "afxdialogex.h"


// xmlPanel 对话框

IMPLEMENT_DYNAMIC(xmlPanel, CDialogEx)

xmlPanel::xmlPanel(CWnd* pParent /*=NULL*/)
	: CDialogEx(xmlPanel::IDD, pParent)
	, xmlFilePath(_T(""))
	, strXml(_T(""))
	, strMagic(_T(""))
	, strFileSize(_T(""))
{

}

xmlPanel::~xmlPanel()
{
}

void xmlPanel::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_MFCEDITBROWSE1, xmlFilePath);
	DDX_Text(pDX, IDC_EDIT3, strXml);
	DDX_Text(pDX, IDC_EDIT1, strMagic);
	DDX_Text(pDX, IDC_EDIT2, strFileSize);
}


BEGIN_MESSAGE_MAP(xmlPanel, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &xmlPanel::OnBnClickedButton1)
END_MESSAGE_MAP()


// xmlPanel 消息处理程序


void xmlPanel::OnBnClickedButton1()
{
	UpdateData(TRUE);

	//需要等待线程等待完成
	if (mXmlAnalyse.isAnalysised)
	{
		mXmlAnalyse.xmlUnload();
	}

	if (xmlFilePath.GetLength() == 0)
	{
		::MessageBox(this->m_hWnd, L"请选择要分析的文件", L"警告", MB_OK);
		return;
	}

	//加载文件
	if (!mXmlAnalyse.xmlLoadFile(xmlFilePath.GetBuffer(0),L"r"))     //只读方式打开文件
	{
		::MessageBox(this->m_hWnd, L"加载xml文件失败", L"警告", MB_OK);
		return;
	}

	//开始分析
	if (mXmlAnalyse.Analysis())
	{
		mXmlAnalyse.isAnalysised=true;
	}
	else
	{
		::MessageBox(this->m_hWnd, L"分析失败", L"警告", MB_OK);
		return;
	}
	//开始分析xml
	strMagic.Format(L"%08XH", mXmlAnalyse.mCtx.xml.magic);
	strFileSize.Format(L"%d B", mXmlAnalyse.mCtx.size);
	std::wstring str = mXmlAnalyse.createXml();
	strXml.Append(str.c_str(), str.size());
	UpdateData(FALSE);
}
