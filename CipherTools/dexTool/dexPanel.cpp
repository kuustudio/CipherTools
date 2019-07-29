// dexPanel.cpp : 实现文件
//

#include "stdafx.h"
#include "dexTool.h"
#include "dexPanel.h"
#include "afxdialogex.h"


// dexPanel 对话框

IMPLEMENT_DYNAMIC(dexPanel, CDialogEx)

dexPanel::dexPanel(CWnd* pParent /*=NULL*/)
	: CDialogEx(dexPanel::IDD, pParent)
	, strFile(_T(""))
	, strReport(_T(""))
{

}

dexPanel::~dexPanel()
{
}

void dexPanel::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_MFCEDITBROWSE1, strFile);
	DDX_Text(pDX, IDC_EDIT3, strReport);
}


BEGIN_MESSAGE_MAP(dexPanel, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &dexPanel::OnBnClickedButton1)
END_MESSAGE_MAP()


// dexPanel 消息处理程序

//分析dex文件
void dexPanel::OnBnClickedButton1()
{
	UpdateData(TRUE);
	//需要等待线程等待完成
	if (mDexAnalyse.isAnalysised)
	{
		mDexAnalyse.dexUnload();
	}

	if (strFile.GetLength() == 0)
	{
		::MessageBox(this->m_hWnd, L"请选择要分析的文件", L"警告", MB_OK);
		return;
	}

	//加载文件
	if (!mDexAnalyse.dexLoadFile(strFile.GetBuffer(0), L"r"))     //只读方式打开文件
	{
		::MessageBox(this->m_hWnd, L"加载dex文件失败", L"警告", MB_OK);
		return;
	}

	//开始分析
	if (mDexAnalyse.Analysis())
	{
		mDexAnalyse.isAnalysised = true;
		std::wstring str = mDexAnalyse.doReport();
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


BOOL dexPanel::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	strFile = L"C:\\Users\\Administrator\\Desktop\\1.dex";
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}
