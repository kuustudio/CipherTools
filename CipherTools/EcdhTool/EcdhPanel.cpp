// EcdhPanel.cpp : 实现文件
//

#include "stdafx.h"
#include "EcdhTool.h"
#include "EcdhPanel.h"
#include "afxdialogex.h"
#include"Ecdh.h"


// CEcdhPanel 对话框

IMPLEMENT_DYNAMIC(CEcdhPanel, CDialogEx)

CEcdhPanel::CEcdhPanel(CWnd* pParent /*=NULL*/)
	: CDialogEx(CEcdhPanel::IDD, pParent)
{

}

CEcdhPanel::~CEcdhPanel()
{
}

void CEcdhPanel::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CEcdhPanel, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CEcdhPanel::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CEcdhPanel::OnBnClickedButton2)
END_MESSAGE_MAP()


// CEcdhPanel 消息处理程序


BOOL CEcdhPanel::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//加载第一个HexControl
	pHexControl1 = HexEditControl::ShowHexControl(this->m_hWnd, 10, 40, 600, 35);
	pHexControl1->SetParent(this);

	//加载第二个HexControl
	pHexControl2 = HexEditControl::ShowHexControl(this->m_hWnd, 10, 100, 600, 70);
	pHexControl2->SetParent(this);

	pHexControl3 = HexEditControl::ShowHexControl(this->m_hWnd, 10, 245, 600, 35);
	pHexControl3->SetParent(this);

	pHexControl4 = HexEditControl::ShowHexControl(this->m_hWnd, 10, 305, 600, 70);
	pHexControl4->SetParent(this);

	pHexControl5 = HexEditControl::ShowHexControl(this->m_hWnd, 10, 395, 600, 35);
	pHexControl5->SetParent(this);


	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

//生成密钥
void CEcdhPanel::OnBnClickedButton1()
{
	CEcdh mEcdh;
	unsigned char privateKey1[0x50] = { 0 };
	unsigned int	len1 = sizeof(privateKey1);
	unsigned char publicKey1[0x50] = { 0 };
	unsigned int	len2 = sizeof(publicKey1);
	mEcdh.GenerateECKey(privateKey1, &len1, publicKey1, &len2,false);

	HexEditControl::SetData(pHexControl1, privateKey1, len1);
	HexEditControl::SetData(pHexControl2, publicKey1, len2);

}

//计算共享密钥
void CEcdhPanel::OnBnClickedButton2()
{
	unsigned int pri_key_len=HexEditControl::GetDataLength(pHexControl3);
	unsigned int pub_key_len = HexEditControl::GetDataLength(pHexControl4);
	if (pri_key_len != 0 && pub_key_len != 0)
	{
		unsigned char *pri_key = (unsigned char*)malloc(pri_key_len);
		unsigned char *pub_key = (unsigned char*)malloc(pub_key_len);
		HexEditControl::GetData(pHexControl3, pri_key, pri_key_len);
		HexEditControl::GetData(pHexControl4, pub_key, pub_key_len);
		unsigned char sharekey[0x50] = { 0 };
		unsigned int sharekeylen = 0x50;
		CEcdh mEcdh;
		if (!mEcdh.ComputeShareKey(pri_key, pri_key_len, pub_key, pub_key_len, sharekey, &sharekeylen))
		{
			AfxMessageBox(L"生成失败");
			return;
		}
		HexEditControl::SetData(pHexControl5, sharekey, sharekeylen);
	}
}
