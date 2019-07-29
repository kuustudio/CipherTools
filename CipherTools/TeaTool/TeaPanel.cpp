// TeaPanel.cpp : 实现文件
//

#include "stdafx.h"
#include "TeaTool.h"
#include "TeaPanel.h"
#include "afxdialogex.h"
#include "TeaCrypt.h"


// TeaPanel 对话框

IMPLEMENT_DYNAMIC(TeaPanel, CDialogEx)

TeaPanel::TeaPanel(CWnd* pParent /*=NULL*/)
	: CDialogEx(TeaPanel::IDD, pParent)
{

}

TeaPanel::~TeaPanel()
{
}

void TeaPanel::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, mMode);
	DDX_Control(pDX, IDC_EDIT1, mKey);
	DDX_Control(pDX, IDC_CHECK1, mCheckHex);
	DDX_Control(pDX, IDC_COMBO2, mRound);
	DDX_Control(pDX, IDC_COMBO3, mDelta);
	DDX_Control(pDX, IDC_COMBO4, mComType);
}


BEGIN_MESSAGE_MAP(TeaPanel, CDialogEx)
	ON_BN_CLICKED(IDC_CHECK1, &TeaPanel::OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_BUTTON1, &TeaPanel::OnBnClickedButton1)
	ON_CBN_SELCHANGE(IDC_COMBO1, &TeaPanel::OnCbnSelchangeCombo1)
	ON_BN_CLICKED(IDC_BUTTON2, &TeaPanel::OnBnClickedButton2)
	ON_CBN_SELCHANGE(IDC_COMBO4, &TeaPanel::OnCbnSelchangeCombo4)
END_MESSAGE_MAP()


// TeaPanel 消息处理程序


BOOL TeaPanel::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	this->mMode.AddString(_T("TEA"));
	this->mMode.AddString(_T("xTEA"));
	this->mMode.AddString(_T("xxTEA"));
	this->mMode.AddString(_T("TencentxxTEA"));
	this->mMode.SetCurSel(0);
	this->mRound.AddString(_T("16"));
	this->mRound.AddString(_T("32"));
	this->mRound.AddString(_T("64"));
	this->mRound.SetCurSel(0);
	this->mDelta.AddString(_T("9e3779b9"));
	this->mDelta.AddString(_T("57E89147"));
	this->mDelta.SetCurSel(0);
	this->mComType.AddString(_T("QQ one"));
	this->mComType.AddString(_T("QQ two"));
	this->mComType.AddString(_T("QQ pim"));

	this->mKey.SetWindowTextW(_T("f15e78d23033c2c4bac8a925677e863a"));
	mCheckHex.SetCheck(TRUE);

	//加载HexControl
	pHexControl1 = HexEditControl::ShowHexControlEx(m_hWnd, 10, 90, 630, 350, FALSE, TRUE);
	pHexControl1->SetParent(this);
	BYTE buffer2[] ="12345678901234567890test";
	HexEditControl::SetData(pHexControl1, buffer2, sizeof(buffer2));

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

//变换进制
void TeaPanel::OnBnClickedCheck1()
{
	// TODO:  在此添加控件通知处理程序代码
	if (mCheckHex.GetCheck() == TRUE)
	{
		CString strText;
		mKey.GetWindowTextW(strText);
		if (strText.GetLength() != 0)
		{
			std::string strM = MyWideCharToMultiByte((BYTE*)strText.GetBuffer(0), strText.GetLength());
			std::string str = HexToStr((BYTE*)strM.c_str(), strM.length());
			std::string strW = MyMultiByteToWideChar((BYTE*)str.c_str(), str.length());
			strText = "";
			strText.Append((wchar_t*)strW.c_str(), strW.length() / 2);
			mKey.SetWindowTextW(strText);
		}
	}
	else
	{
		CString strText;
		mKey.GetWindowTextW(strText);
		if (strText.GetLength() != 0)
		{
			std::string strM = MyWideCharToMultiByte((BYTE*)strText.GetBuffer(0), strText.GetLength());
			std::string str = StrToHex((BYTE*)strM.c_str(), strM.length());
			std::string strW = MyMultiByteToWideChar((BYTE*)str.c_str(), str.length());
			strText = "";
			strText.Append((wchar_t*)strW.c_str(), strW.length() / 2);
			mKey.SetWindowTextW(strText);
		}
	}
}

std::string TeaPanel::HexToStr(BYTE *pbSrc, int nLen)
{
	std::string str;
	char ddl, ddh;
	int i;

	for (i = 0; i<nLen; i++)
	{
		ddh = 48 + pbSrc[i] / 16;
		ddl = 48 + pbSrc[i] % 16;
		if (ddh > 57) ddh = ddh + 7;
		if (ddl > 57) ddl = ddl + 7;
		str.push_back(ddh);
		str.push_back(ddl);
	}
	return str;
}

std::string TeaPanel::StrToHex(BYTE *pbSrc, int nLen)
{
	std::string str;
	char h1, h2;
	BYTE s1, s2;
	int i;

	for (i = 0; i<nLen / 2; i++)
	{
		h1 = pbSrc[2 * i];
		h2 = pbSrc[2 * i + 1];

		s1 = toupper(h1) - 0x30;
		if (s1 > 9)
			s1 -= 7;

		s2 = toupper(h2) - 0x30;
		if (s2 > 9)
			s2 -= 7;

		str.push_back(s1 * 16 + s2);
	}
	return str;
}

std::string TeaPanel::MyWideCharToMultiByte(BYTE *pbDest, int nLen)
{
	//计算需要多少个字节才能表示对应的多字节字符串
	DWORD num = WideCharToMultiByte(CP_ACP, 0, (LPCWCH)pbDest, nLen, NULL, 0, NULL, 0);
	//开辟空间
	char *pChar = NULL;
	pChar = (char*)malloc(num*sizeof(char));
	if (pChar == NULL)
	{
		free(pChar);
	}
	memset(pChar, 0, num*sizeof(char));
	//将宽字节字符串转换为多字节字符串
	WideCharToMultiByte(CP_ACP, 0, (LPCWCH)pbDest, nLen, pChar, num, NULL, 0);
	std::string retString;
	retString.append(pChar, num);
	free(pChar);
	return retString;
}

std::string TeaPanel::MyMultiByteToWideChar(BYTE *pbDest, int nLen)
{
	char* pWCHAR = NULL;

	//计算pChar所指向的多字节字符串相当于多少个宽字节
	DWORD num = MultiByteToWideChar(CP_ACP, 0, (LPCCH)pbDest, nLen, NULL, 0);

	num = num*sizeof(wchar_t);
	pWCHAR = (char*)malloc(num);

	if (pWCHAR == NULL)
	{
		free(pWCHAR);
	}

	memset(pWCHAR, 0, num);

	//多字节转换为宽字节
	MultiByteToWideChar(CP_ACP, 0, (LPCCH)pbDest, nLen, (LPWSTR)pWCHAR, num);
	std::string retString;
	retString.append(pWCHAR, num);
	free(pWCHAR);
	return retString;

}

//变换加密模式
void TeaPanel::OnCbnSelchangeCombo1()
{
	// TODO:  在此添加控件通知处理程序代码
	CString strMode = _T("");
	if (strMode == "TEA")
	{
	}
	else if (strMode == "xTEA")
	{
	}
	else if (strMode == "xxTEA")
	{
	}
	else if (strMode == "TencentxxTEA")
	{
	}
}

//加密
void TeaPanel::OnBnClickedButton1()
{
	//取出循环次数
	CString strRound = _T("");
	mRound.GetWindowTextW(strRound);
	int nRound = StrToInt(strRound);

	//取出魔数
	CString strDelta = _T("");
	mDelta.GetWindowTextW(strDelta);
	strDelta.Insert(0, L"0x");
	int nDelta = 0;
	StrToIntEx(strDelta, STIF_SUPPORT_HEX, &nDelta);

	//取出密钥
	CString strKey = _T("");
	mKey.GetWindowTextW(strKey);
	std::string mb_Key = MyWideCharToMultiByte((BYTE*)strKey.GetBuffer(0), strKey.GetLength());
	if (mCheckHex.GetCheck() == TRUE)
	{
		//16进制密钥数据
		mb_Key = StrToHex((BYTE*)mb_Key.c_str(), mb_Key.length());
	}
	if (mb_Key == "0" || mb_Key.length() == 0)
	{
		mb_Key.clear();
		mb_Key.assign(0x10, 0);
	}

	//取出源数据
	UINT srclen = HexEditControl::GetDataLength(pHexControl1);
	if (srclen == 0)
	{
		AfxMessageBox(_T("数据为空"));
		return;
	}
	BYTE *srcBuffer = (BYTE*)malloc(srclen);
	memset(srcBuffer, 0, srclen);
	HexEditControl::GetData(pHexControl1, srcBuffer, srclen);

	CString strMode = _T("");
	mMode.GetWindowTextW(strMode);
	if (strMode == "TEA")
	{
		CTeaCrypt mCTeaCrypt;
		mCTeaCrypt.CommonInitTea(nRound, nDelta);
		mCTeaCrypt.SetKey((unsigned char*)mb_Key.c_str(), mb_Key.length());
		mCTeaCrypt.tea_encrypt(srcBuffer, srclen);
		HexEditControl::SetData(pHexControl1, srcBuffer, srclen - srclen%8);
	}
	else if (strMode == "xTEA")
	{
		CTeaCrypt mCTeaCrypt;
		mCTeaCrypt.CommonInitTea(nRound, nDelta);
		mCTeaCrypt.SetKey((unsigned char*)mb_Key.c_str(), mb_Key.length());
		mCTeaCrypt.xtea_encrypt(srcBuffer, srclen);
		HexEditControl::SetData(pHexControl1, srcBuffer, srclen - srclen % 8);
	}
	else if (strMode == "xxTEA")
	{
		CTeaCrypt mCTeaCrypt;
		mCTeaCrypt.CommonInitTea(nRound, nDelta);
		mCTeaCrypt.SetKey((unsigned char*)mb_Key.c_str(), mb_Key.length());
		mCTeaCrypt.xxtea_encrypt(srcBuffer, srclen);
		HexEditControl::SetData(pHexControl1, srcBuffer, srclen - srclen % 8);
	}
	else if (strMode == "TencentxxTEA")
	{

		CTeaCrypt mCTeaCrypt;
		mCTeaCrypt.CommonInitTea(nRound, nDelta);
		mCTeaCrypt.SetKey((unsigned char*)mb_Key.c_str(), mb_Key.length());
		//目的数据
		unsigned char *dstBuffer=NULL;
		unsigned int dstlen = 0;
		mCTeaCrypt.Tencent_xxTea_encrypt(srcBuffer, srclen, &dstBuffer, &dstlen);
		HexEditControl::SetData(pHexControl1, dstBuffer, dstlen);
		free(dstBuffer);
	}
	free(srcBuffer);
}

//解密
void TeaPanel::OnBnClickedButton2()
{
	//取出循环次数
	CString strRound = _T("");
	mRound.GetWindowTextW(strRound);
	int nRound = StrToInt(strRound);

	//取出魔数
	CString strDelta = _T("");
	mDelta.GetWindowTextW(strDelta);
	strDelta.Insert(0, L"0x");
	int nDelta = 0;
	StrToIntEx(strDelta, STIF_SUPPORT_HEX, &nDelta);

	//取出密钥
	CString strKey = _T("");
	mKey.GetWindowTextW(strKey);
	std::string mb_Key = MyWideCharToMultiByte((BYTE*)strKey.GetBuffer(0), strKey.GetLength());
	if (mCheckHex.GetCheck() == TRUE)
	{
		//16进制密钥数据
		mb_Key = StrToHex((BYTE*)mb_Key.c_str(), mb_Key.length());
	}
	if (mb_Key == "0" || mb_Key.length()==0)
	{
		mb_Key.clear();
		mb_Key.assign(0x10, 0);
	}

	//取出源数据
	UINT srclen = HexEditControl::GetDataLength(pHexControl1);
	if (srclen == 0)
	{
		AfxMessageBox(_T("数据为空"));
		return;
	}
	BYTE *srcBuffer = (BYTE*)malloc(srclen);
	memset(srcBuffer, 0, srclen);
	HexEditControl::GetData(pHexControl1, srcBuffer, srclen);



	CString strMode = _T("");
	mMode.GetWindowTextW(strMode);
	if (strMode == "TEA")
	{
		CTeaCrypt mCTeaCrypt;
		mCTeaCrypt.CommonInitTea(nRound, nDelta);
		mCTeaCrypt.SetKey((unsigned char*)mb_Key.c_str(), mb_Key.length());
		mCTeaCrypt.tea_decrypt(srcBuffer, srclen);
		HexEditControl::SetData(pHexControl1, srcBuffer, srclen);
	}
	else if (strMode == "xTEA")
	{
		CTeaCrypt mCTeaCrypt;
		mCTeaCrypt.CommonInitTea(nRound, nDelta);
		mCTeaCrypt.SetKey((unsigned char*)mb_Key.c_str(), mb_Key.length());
		mCTeaCrypt.xtea_decrypt(srcBuffer, srclen);
		HexEditControl::SetData(pHexControl1, srcBuffer, srclen);
	}
	else if (strMode == "xxTEA")
	{
		CTeaCrypt mCTeaCrypt;
		mCTeaCrypt.CommonInitTea(nRound, nDelta);
		mCTeaCrypt.SetKey((unsigned char*)mb_Key.c_str(), mb_Key.length());
		mCTeaCrypt.xxtea_decrypt(srcBuffer,srclen);
		HexEditControl::SetData(pHexControl1, srcBuffer, srclen);
	}
	else if (strMode == "TencentxxTEA")
	{
		CTeaCrypt mCTeaCrypt;
		mCTeaCrypt.CommonInitTea(nRound, nDelta);
		mCTeaCrypt.SetKey((unsigned char*)mb_Key.c_str(), mb_Key.length());
		//目的数据
		unsigned char *dstBuffer = 0;
		unsigned int dstlen = 0;
		mCTeaCrypt.Tencent_xxTea_decrypt(srcBuffer, srclen, &dstBuffer, &dstlen);
		HexEditControl::SetData(pHexControl1, dstBuffer, dstlen);
		free(dstBuffer);
	}
	free(srcBuffer);
}

//固定组合
void TeaPanel::OnCbnSelchangeCombo4()
{
	CString strType = _T("");
	mComType.GetWindowTextW(strType);
	if (strType == "QQ one")
	{
		mMode.SetCurSel(mMode.FindString(0,_T("TencentxxTEA")));
		mRound.SetCurSel(mRound.FindString(0, _T("16")));
		mDelta.SetCurSel(mDelta.FindString(0, _T("9e3779b9")));

		mKey.SetWindowTextW(_T("0"));

		unsigned char buffer[] =
		{
			0x35, 0xd1, 0x20, 0x2b, 0x41,
			0x5e, 0xca, 0xce, 0x3b, 0xb8, 0x2d, 0x0b, 0x02,
			0xb9, 0xda, 0x56, 0xf0, 0xdc, 0xe9, 0x05, 0x50,
			0xb2, 0xb7, 0xd1, 0xb3, 0xf7, 0x67, 0xe1, 0x9e,
			0x08, 0xe1, 0xdb, 0x4b, 0xa3, 0xec, 0xaa, 0x5b,
			0xe1, 0x1e, 0x68, 0x6e, 0x50, 0xe5, 0xf2, 0x88,
			0x6b, 0x0f, 0x14, 0xa9, 0x67, 0xb5, 0xff, 0xb5,
			0xd2, 0xb0, 0xfa, 0x49, 0xb2, 0x94, 0xeb, 0x6b,
			0x13, 0xf7, 0x2d, 0x97, 0x79, 0x20, 0x52, 0xa6,
			0xd7, 0x6a, 0x47, 0x84, 0xce, 0x9f, 0x79, 0x4a,
			0xce, 0x9f, 0xe3, 0x65, 0xd2, 0x5c, 0x57, 0xad,
			0x2c, 0x6a, 0x2c, 0x08, 0xaf, 0xd3, 0xfe, 0x8f,
			0x13, 0x9d, 0xbe, 0x76, 0x3b, 0x82, 0x7d, 0xa8,
			0x23, 0x59, 0x4b, 0x0f, 0xf6, 0x45, 0xd4, 0xec,
			0x5e, 0x64, 0xde, 0xfe, 0xa8, 0x9d, 0x5e, 0xe1,
			0xb5, 0xc7, 0x65, 0xb3, 0x56, 0xf3, 0x38, 0x1c,
			0x26, 0xdf, 0x82, 0xb4, 0xa7, 0x21, 0x7e, 0xf7,
			0x96, 0xd8, 0x61, 0x63, 0x74, 0x99, 0xb2, 0xad,
			0xd3, 0x7c, 0x57, 0x6b, 0x8f, 0x18, 0xc8, 0xc4,
			0x84, 0x98, 0x1e, 0x9d, 0x3c, 0x31, 0x5d, 0xd5,
			0xcd, 0x7f, 0x53, 0x73, 0x44, 0x33, 0xe8, 0x10,
			0x06, 0x95, 0x16, 0x31, 0x81, 0x61, 0x34, 0x39,
			0x91, 0x24, 0xb6, 0xa9, 0xe6, 0x85, 0xde, 0x5b,
			0x4e, 0x9c, 0x33, 0x89, 0x10, 0xf2, 0x21, 0xde,
			0xa0, 0x74, 0xc2, 0x16, 0xb6, 0x66, 0xd5, 0x5b,
			0x19, 0x5c, 0xe4, 0x0e, 0x73, 0x6d, 0x7e, 0xc0,
			0x59, 0x41, 0xd7, 0x71, 0xa4, 0x54, 0x76, 0xce,
			0xac, 0x55, 0x09, 0x87, 0x64, 0xb4, 0x34, 0x46,
			0xff, 0xc3, 0x74, 0x20, 0x3c, 0xe2, 0xa3, 0xc5,
			0xa1, 0x7b, 0xe1, 0xcf, 0xb2, 0x3b, 0x1d, 0x07,
			0x04, 0xbb, 0xb0, 0x98, 0x5f, 0x9b, 0xb2, 0x26,
			0x96, 0x5b, 0x9a, 0x23, 0x28, 0x7a, 0xb9, 0x6b,
			0xb3, 0xfe, 0x47, 0xa2, 0x25, 0xf6, 0x3f, 0xf4,
			0x37, 0x1d, 0xee, 0x00, 0xd2, 0x38, 0x29, 0x69,
			0xd9, 0x45, 0x78, 0xbc, 0x12, 0x53, 0x53, 0x4b,
			0x55, 0x57, 0x07, 0x37, 0x1c, 0x63, 0xd6, 0x82,
			0x54, 0x57, 0x83, 0x77, 0xc7, 0x2f, 0xe5, 0xf6,
			0xd6, 0x59, 0x93, 0x2f, 0x15, 0x37, 0x51, 0x52,
			0x2a, 0x5f, 0x96, 0x66, 0x5d, 0x89, 0x70, 0xef,
			0xe8, 0x59, 0x56, 0x1f, 0xca, 0x75, 0x41, 0x07,
			0xd9, 0x09, 0x2b, 0xdd, 0x3d, 0x92, 0x3b, 0xd7,
			0xbc, 0x49, 0xe9, 0x7a, 0x41, 0x5a, 0x1e, 0xfb,
			0x32, 0xaf, 0xe3, 0x19, 0x39, 0x97, 0x86, 0x04,
			0xe5, 0xda, 0x4f, 0x7e, 0x16, 0xcc, 0x79, 0x60,
			0x9c, 0x58, 0x50, 0xc2, 0x8c, 0x75, 0x82, 0x41,
			0x55, 0x12, 0xd2, 0xe1, 0xfb, 0xa1, 0xf2, 0xd8,
			0xe0, 0xe3, 0x12, 0x5d, 0xf0, 0x0d, 0x41, 0x85,
			0x08, 0xdd, 0xce, 0x8d, 0xcb, 0xb3, 0x4b, 0x4a,
			0x2d, 0x39, 0x5d, 0x76, 0xba, 0xd4, 0x60, 0x20,
			0xd9, 0x31, 0x11, 0xfd, 0x4f, 0xce, 0xe7, 0x36,
			0x65, 0x4b, 0x8e, 0x3c, 0x45, 0xd4, 0x4a, 0x36,
			0xb4, 0x8f, 0x8f, 0x44, 0xad, 0x9b, 0x3a, 0xaa,
			0x83, 0xc9, 0x98, 0xac, 0x64, 0x05, 0xe6, 0x62,
			0xb5, 0x70, 0x8b, 0x6d, 0x5b, 0x93, 0x2b, 0xea,
			0x7e, 0x9b, 0xff, 0x24, 0x6d, 0x3a, 0x03, 0x86,
			0x4e, 0xda, 0x87, 0xe8, 0x68, 0x03, 0xf7, 0xa6,
			0xf8, 0x3d, 0xa7, 0xd5, 0xdc, 0xc6, 0xd3, 0x86,
			0x0c, 0x68, 0x73, 0x24, 0xc8, 0x2f, 0x92, 0x44,
			0x92, 0x0d, 0xd8, 0x63, 0x4a, 0x0b, 0x0c, 0x9d,
			0x8f, 0x47, 0x1c, 0xb7, 0x1e, 0x18, 0x8e, 0x54,
			0x20, 0xe3, 0xf9, 0x86, 0xc7, 0x67, 0xd6, 0x75,
			0x5e, 0x7d, 0xc4, 0x3c, 0x44, 0x20, 0xe2, 0xad,
			0xc4, 0x56, 0x46, 0xd3, 0x34, 0x5b, 0xc8, 0x61,
			0x56, 0xe2, 0x73, 0xba, 0xe2, 0xe6, 0xbb, 0x76,
			0x57, 0x95, 0xb1, 0x78, 0xc2, 0xde, 0x1a, 0xd1,
			0xf5, 0xae, 0x64, 0x92, 0xce, 0x35, 0x44, 0xcf,
			0x7e, 0x0f, 0xe4, 0x3f, 0x89, 0x72, 0x13, 0x5f,
			0xc0, 0x7e, 0x15, 0x5a, 0x9f, 0xb9, 0x74, 0xa4,
			0x23, 0xdf, 0x65, 0x79, 0x1b, 0xf2, 0xbf, 0xaa,
			0x4f, 0xac, 0x27, 0xd5, 0x88, 0xa0, 0x12, 0x26,
			0x53, 0x12, 0xe2, 0x3c, 0xf1, 0x95, 0xf5, 0xf5,
			0xe7, 0xe1, 0xf2, 0xce, 0x56, 0x66, 0x3d, 0xed,
			0x43, 0x1b, 0x3f, 0xc1, 0x45, 0x2f, 0x0b, 0x04,
			0xad, 0x70, 0x74, 0xa5, 0x8d, 0x69, 0x69, 0x0a,
			0x3a, 0x5b, 0x1b, 0xfe, 0x7b, 0x62, 0x27, 0xc7,
			0x1d, 0xe6, 0x20, 0x86, 0x72, 0x53, 0x32, 0x2a,
			0x71, 0xfc, 0xb3, 0xae, 0x86, 0xbc, 0xd8, 0x72,
			0xf3, 0x06, 0x41, 0x27, 0xd4, 0x58, 0x5d, 0x6c,
			0xae, 0x9f, 0xa6, 0xe8, 0xbb, 0x07, 0x3e, 0x1d,
			0x99, 0xb8, 0x3b, 0x04, 0x88, 0x86, 0x8d, 0xf9,
			0x07, 0xb1, 0x8d, 0x21, 0x2b, 0x84, 0x08, 0x0c,
			0xbc, 0x49, 0x25, 0x47, 0x16, 0xa7, 0x45, 0x73,
			0x98, 0x1a, 0x96, 0x87, 0xa0, 0xee, 0x1a, 0xed,
			0x94, 0x36, 0x78, 0x5a, 0xbd, 0x23, 0x64, 0x17,
			0xed, 0x3d, 0xff, 0xea, 0xe7, 0x97, 0x46, 0x67,
			0xe3, 0xaf, 0xdb, 0xbe, 0xa7, 0xae, 0xe8, 0x79,
			0x37, 0x58, 0x97, 0x8c, 0x8c, 0xd0, 0xbb, 0x49,
			0xb5, 0xf5, 0x89, 0xd2, 0x76, 0xa3, 0x12, 0x60,
			0x2d, 0xff, 0x24, 0xa0, 0xbf, 0x55, 0x1f, 0x0a,
			0xa1, 0x35, 0xbf, 0xc6, 0x06, 0xae, 0x77, 0x71,
			0x64, 0x5d, 0x01, 0xfe, 0xa7, 0x3c, 0xad, 0xa1,
			0x0a, 0xb9, 0x90, 0x45, 0xfa, 0x14, 0x11, 0x98,
			0x32, 0xed, 0xe9, 0xa4, 0xb2, 0xd0, 0xd4, 0x2a,
			0x43, 0x3d, 0xe0, 0x5f, 0x25, 0x39, 0x2a, 0xf1,
			0xf7, 0x88, 0xae, 0x7d, 0x3a, 0xe4, 0xfd, 0x42,
			0x70, 0x4b, 0x37, 0x79, 0x02, 0x34, 0x84, 0x34,
			0xc6, 0x42, 0x12, 0x63, 0x3a, 0x07, 0x03, 0xbd,
			0x13, 0xab, 0xe7, 0x8c, 0xd7, 0x2a, 0x88, 0xb8,
			0x45, 0x5b, 0xe1, 0xf0, 0x26, 0xee, 0xd3, 0x22,
			0xc4, 0x3e, 0x23, 0x80, 0x4d, 0x2a, 0x17, 0x9d,
			0x81, 0xa7, 0x4d, 0x4f, 0xb5, 0x18, 0x1c, 0xd8,
			0x5e, 0xda, 0x2a, 0xa2, 0x8c, 0xff, 0x19, 0x6b,
			0x98, 0x02, 0x21, 0x6c, 0x89, 0xcb, 0xfc, 0xca,
			0x0a, 0xb0, 0xf5, 0xfa, 0x9b, 0xbc, 0x5d, 0x32,
			0xf9, 0x9a, 0xe9, 0x87, 0xa7, 0x82, 0x6c, 0xba,
			0xbc, 0xce, 0x9c, 0xd9, 0x2f, 0x00, 0x52, 0x29,
			0xd2, 0x76, 0x70, 0x28, 0x19, 0x43, 0xd1, 0xba,
			0x9d, 0xa2, 0xd4, 0xae, 0x7f, 0xf4, 0xed, 0x49,
			0xc7, 0x0f, 0x24, 0x16, 0xd3, 0x6d, 0xf4, 0x90,
			0xe3, 0xdc, 0xbd, 0x7b, 0x6d, 0x05, 0xcb, 0xcc,
			0x77, 0xd4, 0x18, 0xff, 0x49, 0x5d, 0xcd, 0xef,
			0x29, 0xd1, 0xd5, 0xa1, 0x1f, 0x86, 0x45, 0x92,
			0x5a, 0x44, 0xb5, 0x6c, 0x57, 0xf4, 0xea, 0x47,
			0x9a, 0x57, 0x72, 0x7b, 0x77, 0x34, 0x8f, 0x40,
			0x0c, 0xfe, 0x9b, 0x7c, 0x0a, 0x34, 0x86, 0x55,
			0x85, 0x22, 0x46, 0xaf, 0x22, 0x6a, 0x4e, 0x58,
			0xbd, 0x36, 0xa1, 0xc0, 0x88, 0xb3, 0x21, 0xdd,
			0xfb, 0x3e, 0xa7, 0x02, 0x58, 0xc1, 0xae, 0x0f,
			0xb4, 0xcf, 0x28, 0xee, 0xd6, 0x58, 0xef, 0x54,
			0x36, 0x65, 0x76, 0xe2, 0x37, 0xe8, 0x24, 0xfa,
			0x78, 0xa1, 0x5d, 0xae, 0x44, 0xd8, 0xb5, 0xdf,
			0xa0, 0xbb, 0x7f, 0x3c, 0x3a, 0xf6, 0x32, 0x70,
			0x71, 0x7b, 0xba, 0xc4, 0xc4, 0xa4, 0x48, 0x9f,
			0x6b, 0x6a, 0x48, 0xf9, 0x7c, 0xe8, 0x4f, 0x99,
			0x26, 0x89, 0x27, 0x81, 0x72, 0x42, 0x24, 0xcf,
			0x2f, 0xe2, 0x5f, 0xaa, 0xad, 0x51, 0xae, 0x05,
			0x67, 0x9e, 0x7e, 0xbf, 0x61, 0xcd, 0x10, 0x3b,
			0xd8, 0xc0, 0xbd, 0x48, 0x42, 0xc9, 0x6b, 0xc3,
			0xd7, 0x8d, 0xdc, 0x2c, 0x90, 0x0d, 0xd9, 0xcb,
			0x9a, 0x54, 0xf0, 0xec, 0x8f, 0xb3, 0x7c, 0x00,
			0xb6, 0x58, 0xcb, 0xd1, 0x68, 0x45, 0xdc, 0xea,
			0x3a, 0xa7, 0xda, 0x73, 0x16, 0x30, 0x5d, 0xcd,
			0xe0, 0x03, 0xbd, 0x83, 0x42, 0x97, 0x1a, 0x21,
			0xbd, 0x4c, 0xd2, 0x2c, 0xca, 0x51, 0x1b, 0x7c,
			0x54, 0x3d, 0x5f, 0x6f, 0x9e, 0x00, 0x50, 0x88,
			0xcb, 0xb7, 0xcb, 0xba, 0x1c, 0x96, 0x53, 0x22,
			0x56, 0x8f, 0x3c, 0x38, 0x6b, 0xf7, 0xbe, 0x80,
			0x97, 0xe9, 0xae, 0xb1, 0xea, 0x39, 0x05, 0x52,
			0x54, 0x9e, 0xb5, 0xd7, 0xb5, 0xeb, 0xb5, 0xfa,
			0xd6, 0x1d, 0x82, 0x72, 0xee, 0xe4, 0xc3, 0x98,
			0x77, 0xaa, 0x5b, 0xba, 0xc3, 0x9f, 0x9a, 0x54,
			0x49, 0x98, 0x3d, 0x38, 0xd8, 0x59, 0xc7, 0x93,
			0xce, 0x84, 0x72, 0x42, 0x7e, 0xc9, 0x85, 0x44,
			0xde, 0xb9, 0x49, 0x1a, 0x1b, 0xb8, 0xb3, 0xee,
			0xcf, 0x1d, 0xfc, 0xdd, 0xd9, 0xb3, 0x9d, 0xf5,
			0x1d, 0x9a, 0xe7, 0x7c, 0x24, 0x43, 0x56, 0xf0,
			0x27, 0x21, 0xf6
		};
		HexEditControl::SetData(pHexControl1, buffer, sizeof(buffer));
	}
	else if (strType == "QQ two")
	{
		mMode.SetCurSel(mMode.FindString(0, _T("TencentxxTEA")));
		mRound.SetCurSel(mRound.FindString(0, _T("32")));
		mDelta.SetCurSel(mDelta.FindString(0, _T("57E89147")));
	}
	else if (strType == "QQ pim")
	{
		mMode.SetCurSel(mMode.FindString(0, _T("xxTEA")));
		mRound.SetCurSel(mRound.FindString(0, _T("32")));
		mDelta.SetCurSel(mDelta.FindString(0, _T("9e3779b9")));

		mKey.SetWindowTextW(_T("f15e78d23033c2c4bac8a925677e863a"));

		BYTE buffer[] = {
			0x83, 0x50, 0xac, 0x8e,
			0x7d, 0x28, 0xec, 0xcb, 0x5b, 0x3b, 0x4d, 0xdc,
			0xab, 0x5d, 0x9e, 0xb6, 0x7e, 0x44, 0x68, 0x66,
			0x77, 0x2c, 0x5c, 0x09, 0xbc, 0xcf, 0xac, 0xbd,
			0xd9, 0x50, 0xd9, 0xf9, 0x4b, 0x6d, 0xf1, 0xd2,
			0x13, 0x7f, 0x9b, 0x69, 0x19, 0x21, 0xa8, 0x9e,
			0xfa, 0xb7, 0x19, 0xc1, 0xfa, 0xd3, 0xcc, 0xe4,
			0xb8, 0x0b, 0x57, 0x74, 0x6f, 0xd1, 0x24, 0x3c,
			0xf2, 0xda, 0x27, 0x52, 0x35, 0x0b, 0xd1, 0xe6,
			0x9a, 0x1d, 0x5b, 0xdc, 0xe5, 0x84, 0x5f, 0x45,
			0xec, 0xdc, 0xb4, 0x6e, 0xaa, 0x10, 0x85, 0xa6,
			0xbc, 0x2a, 0x9e, 0xa9, 0xd4, 0x71, 0x1a, 0x14,
			0xaa, 0xcc, 0x91, 0x2e, 0x4f, 0x00, 0x2c, 0x7c,
			0x0e, 0xc0, 0x73, 0x3d, 0xdd, 0x51, 0xe3, 0xa1,
			0x5c, 0x37, 0x61, 0x64, 0xe1, 0x62, 0x3f, 0x7c,
			0xed, 0x4a, 0xc9, 0xed, 0xbb, 0x88, 0x61, 0xe0,
			0xdc, 0xa3, 0xea, 0x18, 0x46, 0x96, 0xbc, 0xd5,
			0xae, 0x29, 0x76, 0xc9
		};
		HexEditControl::SetData(pHexControl1, buffer, sizeof(buffer));
	}
}
