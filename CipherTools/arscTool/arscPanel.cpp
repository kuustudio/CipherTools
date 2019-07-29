// arscPanel.cpp : 实现文件
//

#include "stdafx.h"
#include "arscTool.h"
#include "arscPanel.h"
#include "afxdialogex.h"


// arscPanel 对话框

IMPLEMENT_DYNAMIC(arscPanel, CDialogEx)

arscPanel::arscPanel(CWnd* pParent /*=NULL*/)
	: CDialogEx(arscPanel::IDD, pParent)
	, strFile(_T(""))
	, strReport(_T(""))
{

}

arscPanel::~arscPanel()
{
}

void arscPanel::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_MFCEDITBROWSE1, strFile);
	DDX_Text(pDX, IDC_EDIT3, strReport);
}


BEGIN_MESSAGE_MAP(arscPanel, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &arscPanel::OnBnClickedButton1)
END_MESSAGE_MAP()


// arscPanel 消息处理程序

//分析
void arscPanel::OnBnClickedButton1()
{
	UpdateData(TRUE);

	//需要等待线程等待完成
	if (mArscAnalyse.isAnalysised)
	{
		mArscAnalyse.arscUnload();
	}

	if (strFile.GetLength() == 0)
	{
		::MessageBox(this->m_hWnd, L"请选择要分析的文件", L"警告", MB_OK);
		return;
	}

	//加载文件
	if (!mArscAnalyse.arscLoadFile(strFile.GetBuffer(0), L"r"))     //只读方式打开文件
	{
		::MessageBox(this->m_hWnd, L"加载arsc文件失败", L"警告", MB_OK);
		return;
	}

	//开始分析
	if (mArscAnalyse.Analysis())
	{
		mArscAnalyse.isAnalysised = true;
		std::wstring str = mArscAnalyse.doReport();
		strReport = L"";
		strReport.Append(str.c_str(), str.size());
		UpdateData(FALSE);
	}
	else
	{
		::MessageBox(this->m_hWnd, L"分析失败", L"警告", MB_OK);
		return;
	}
}


BOOL arscPanel::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	strFile = L"C:\\Users\\Administrator\\Desktop\\resources.arsc";
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}
