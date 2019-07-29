// DesPanel.cpp : 实现文件
//

#include "stdafx.h"
#include "DesTool.h"
#include "DesPanel.h"
#include "afxdialogex.h"
#include"DesCrypt.h"
using namespace CipherDes;


// CDesPanel 对话框

IMPLEMENT_DYNAMIC(CDesPanel, CDialogEx)

CDesPanel::CDesPanel(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDesPanel::IDD, pParent)
	, strSSLeay_version(_T(""))
{

}

CDesPanel::~CDesPanel()
{
}

void CDesPanel::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, EncryptMode);
	DDX_Control(pDX, IDC_COMBO2, mPadding);
	DDX_Control(pDX, IDC_EDIT1, mKey);
	DDX_Control(pDX, IDC_EDIT2, mIv);
	DDX_Control(pDX, IDC_CHECK1, mCheckHex);
	DDX_Text(pDX, IDC_EDIT3, strSSLeay_version);
}


BEGIN_MESSAGE_MAP(CDesPanel, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CDesPanel::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CDesPanel::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_CHECK1, &CDesPanel::OnBnClickedCheck1)
END_MESSAGE_MAP()


// CDesPanel 消息处理程序


BOOL CDesPanel::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//加载openssl版本
	std::string str = CDesCrypt::GetSSLeay_version();
	std::wstring strW(str.length(), L' ');
	std::copy(str.begin(), str.end(), strW.begin());
	strSSLeay_version = "";
	strSSLeay_version.Append(strW.c_str(), strW.size());

	EncryptMode.AddString(_T("ECB"));
	EncryptMode.AddString(_T("CBC"));
	EncryptMode.AddString(_T("CTR"));
	EncryptMode.AddString(_T("OFB"));
	EncryptMode.AddString(_T("CFB"));
	EncryptMode.SetCurSel(0);

	mPadding.AddString(_T("zero"));
	mPadding.AddString(_T("pkcs5/pkcs7"));
	mPadding.AddString(_T("iso10126"));
	mPadding.AddString(_T("ansix923"));
	mPadding.SetCurSel(0);

	//加载HexControl
	pHexControl1 = HexEditControl::ShowHexControlEx(m_hWnd, 10, 120, 630, 350, FALSE, TRUE);
	pHexControl1->SetParent(this);
	BYTE buffer2[] = "1213123213213 dsdfsafsda";
	HexEditControl::SetData(pHexControl1, buffer2, sizeof(buffer2));

	mCheckHex.SetCheck(FALSE);
	mKey.SetWindowTextW(_T("12345678"));
	mIv.SetWindowTextW(_T("12345678"));

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

//加密
void CDesPanel::OnBnClickedButton1()
{
	//取出密钥
	CString strKey = _T("");
	mKey.GetWindowTextW(strKey);
	std::string mb_Key = MyWideCharToMultiByte((BYTE*)strKey.GetBuffer(0), strKey.GetLength());
	if (mCheckHex.GetCheck() == TRUE)
	{
		//16进制密钥数据
		mb_Key=StrToHex((BYTE*)mb_Key.c_str(), mb_Key.length());
	}
	//取出IV
	CString strIV = _T("");
	mIv.GetWindowTextW(strIV);
	std::string mb_Iv = MyWideCharToMultiByte((BYTE*)strIV.GetBuffer(0), strIV.GetLength());
	if (mCheckHex.GetCheck() == TRUE)
	{
		//16进制IV数据
		mb_Iv = StrToHex((BYTE*)mb_Iv.c_str(), mb_Iv.length());
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

	
	CDesCrypt mCDesCrypt;
	//填充
	CString strPaddingType = _T("");
	mPadding.GetLBText(mPadding.GetCurSel(), strPaddingType);
	if (strPaddingType == "zero")
	{
		mCDesCrypt.GetPadding_zero(&srcBuffer, &srclen);
		HexEditControl::SetData(pHexControl1, srcBuffer, srclen - mCDesCrypt.GetPaddingLen());
	}
	else if (strPaddingType == "pkcs5/pkcs7")
	{
		mCDesCrypt.GetPadding_pkcs5(&srcBuffer, &srclen);
		HexEditControl::SetData(pHexControl1, srcBuffer, srclen - mCDesCrypt.GetPaddingLen());
	}
	else if (strPaddingType == "iso10126")
	{
		mCDesCrypt.GetPadding_iso10126(&srcBuffer, &srclen);
		HexEditControl::SetData(pHexControl1, srcBuffer, srclen - mCDesCrypt.GetPaddingLen());
	}
	else if (strPaddingType=="ansix923")
	{
		mCDesCrypt.GetPadding_ansix923(&srcBuffer, &srclen);
		HexEditControl::SetData(pHexControl1, srcBuffer, srclen - mCDesCrypt.GetPaddingLen());
	}

	//开始加密
	mCDesCrypt.SetKey((unsigned char*)mb_Key.c_str(), mb_Key.length());
	CString strEncryptMode = _T("");
	EncryptMode.GetWindowTextW(strEncryptMode);
	if (strEncryptMode == "ECB")
	{
		mCDesCrypt.SetMode(MODE_ECB);
	}
	else if (strEncryptMode == "CBC")
	{
		mCDesCrypt.SetMode(MODE_CBC);
		mCDesCrypt.SetIv((unsigned char*)mb_Iv.c_str(), mb_Iv.length());
	}
	else if (strEncryptMode == "CTR")
	{
		mCDesCrypt.SetMode(MODE_CTR);
	}
	else if (strEncryptMode == "OFB")
	{
		mCDesCrypt.SetMode(MODE_OFB);
	}
	else if (strEncryptMode == "CFB")
	{
		mCDesCrypt.SetMode(MODE_CFB);
	}
	unsigned int dstlen = srclen;
	BYTE *dstBuffer = (BYTE*)malloc(dstlen);
	memset(dstBuffer, 0, dstlen);
	mCDesCrypt.Encrypt(srcBuffer, dstlen, (unsigned char*)dstBuffer);
	HexEditControl::SetData(pHexControl1, dstBuffer, dstlen);

	free(srcBuffer);
	free(dstBuffer);
}

//解密
void CDesPanel::OnBnClickedButton2()
{
	//取出密钥
	CString strKey = _T("");
	mKey.GetWindowTextW(strKey);
	std::string mb_Key = MyWideCharToMultiByte((BYTE*)strKey.GetBuffer(0), strKey.GetLength());
	if (mCheckHex.GetCheck() == TRUE)
	{
		//16进制密钥数据
		mb_Key = StrToHex((BYTE*)mb_Key.c_str(), mb_Key.length());
	}
	//取出IV
	CString strIV = _T("");
	mIv.GetWindowTextW(strIV);
	std::string mb_Iv = MyWideCharToMultiByte((BYTE*)strIV.GetBuffer(0), strIV.GetLength());
	if (mCheckHex.GetCheck() == TRUE)
	{
		//16进制IV数据
		mb_Iv = StrToHex((BYTE*)mb_Iv.c_str(), mb_Iv.length());
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

	//开始解密
	CDesCrypt mCDesCrypt;
	mCDesCrypt.SetKey((unsigned char*)mb_Key.c_str(), mb_Key.length());
	CString strEncryptMode = _T("");
	EncryptMode.GetWindowTextW(strEncryptMode);
	if (strEncryptMode == "ECB")
	{
		mCDesCrypt.SetMode(MODE_ECB);
	}
	else if (strEncryptMode == "CBC")
	{
		mCDesCrypt.SetMode(MODE_CBC);
		mCDesCrypt.SetIv((unsigned char*)mb_Iv.c_str(), mb_Iv.length());
	}
	else if (strEncryptMode == "CTR")
	{
		mCDesCrypt.SetMode(MODE_CTR);
	}
	else if (strEncryptMode == "OFB")
	{
		mCDesCrypt.SetMode(MODE_OFB);
	}
	else if (strEncryptMode == "CFB")
	{
		mCDesCrypt.SetMode(MODE_CFB);
	}

	BYTE *dstBuffer = (BYTE*)malloc(srclen);
	memset(dstBuffer, 0, srclen);
	mCDesCrypt.Decrypt(srcBuffer, srclen, (unsigned char*)dstBuffer);
	HexEditControl::SetData(pHexControl1, dstBuffer, srclen);

	free(srcBuffer);
	free(dstBuffer);
}

//进制变化
void CDesPanel::OnBnClickedCheck1()
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
			strText.Append((wchar_t*)strW.c_str(), strW.length()/2);
			mKey.SetWindowTextW(strText);
		}
		mIv.GetWindowTextW(strText);
		if (strText.GetLength() != 0)
		{
			std::string strM = MyWideCharToMultiByte((BYTE*)strText.GetBuffer(0), strText.GetLength());
			std::string str = HexToStr((BYTE*)strM.c_str(), strM.length());
			std::string strW = MyMultiByteToWideChar((BYTE*)str.c_str(), str.length());
			strText = "";
			strText.Append((wchar_t*)strW.c_str(), strW.length() / 2);
			mIv.SetWindowTextW(strText);
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
		mIv.GetWindowTextW(strText);
		if (strText.GetLength() != 0)
		{
			std::string strM = MyWideCharToMultiByte((BYTE*)strText.GetBuffer(0), strText.GetLength());
			std::string str = StrToHex((BYTE*)strM.c_str(), strM.length());
			std::string strW = MyMultiByteToWideChar((BYTE*)str.c_str(), str.length());
			strText = "";
			strText.Append((wchar_t*)strW.c_str(), strW.length() / 2);
			mIv.SetWindowTextW(strText);
		}
	}
}


std::string CDesPanel::HexToStr(BYTE *pbSrc, int nLen)
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

std::string CDesPanel::StrToHex(BYTE *pbSrc, int nLen)
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

std::string CDesPanel::MyWideCharToMultiByte(BYTE *pbDest, int nLen)
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

std::string CDesPanel::MyMultiByteToWideChar(BYTE *pbDest, int nLen)
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