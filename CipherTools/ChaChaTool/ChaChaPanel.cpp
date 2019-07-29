// ChaChaPanel.cpp : 实现文件
//

#include "stdafx.h"
#include "ChaChaTool.h"
#include "ChaChaPanel.h"
#include "afxdialogex.h"
#include"ChaCha20.h"


// ChaChaPanel 对话框

IMPLEMENT_DYNAMIC(ChaChaPanel, CDialogEx)

ChaChaPanel::ChaChaPanel(CWnd* pParent /*=NULL*/)
	: CDialogEx(ChaChaPanel::IDD, pParent)
	, mKey(_T(""))
	, mNonce(_T(""))
	, bVerify(FALSE)
	, strSSLeay_version(_T(""))
{

}

ChaChaPanel::~ChaChaPanel()
{
}

void ChaChaPanel::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, mKey);
	DDX_Text(pDX, IDC_EDIT2, mNonce);
	DDX_Control(pDX, IDC_CHECK1, mCheckHex);
	DDX_Check(pDX, IDC_CHECK2, bVerify);
	DDX_Text(pDX, IDC_EDIT3, strSSLeay_version);
}


BEGIN_MESSAGE_MAP(ChaChaPanel, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &ChaChaPanel::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &ChaChaPanel::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_CHECK1, &ChaChaPanel::OnBnClickedCheck1)
END_MESSAGE_MAP()


// ChaChaPanel 消息处理程序

BOOL ChaChaPanel::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//加载sodium版本
	std::string str = ChaCha20::Getsodium_version();
	std::wstring strW(str.length(), L' ');
	std::copy(str.begin(), str.end(), strW.begin());
	strSSLeay_version = "";
	strSSLeay_version.Append(strW.c_str(), strW.size());

	//加载第一个HexControl
	pHexControl1 = HexEditControl::ShowHexControlEx(this->m_hWnd, 10, 120, 630, 350, FALSE, TRUE);
	pHexControl1->SetParent(this);
	BYTE buffer2[] = "1213123213213 dsdfsafsda";
	HexEditControl::SetData(pHexControl1, buffer2, sizeof(buffer2));

	mKey = "356BAA7CC713F17F0C0126A47C7FFFBB393B723CEF8B0065127D8678ADE9C53E";
	mNonce = "A65B988BAED0375A";
	mCheckHex.SetCheck(TRUE);
	bVerify = FALSE;
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

//加密
void ChaChaPanel::OnBnClickedButton1()
{
	UpdateData(TRUE);
	std::string mb_Key = MyWideCharToMultiByte((BYTE*)mKey.GetBuffer(0), mKey.GetLength());
	if (mCheckHex.GetCheck() == TRUE)
	{
		//16进制密钥数据
		mb_Key = StrToHex((BYTE*)mb_Key.c_str(), mb_Key.length());
	}
	//取出Nonce
	std::string mb_Nonce = MyWideCharToMultiByte((BYTE*)mNonce.GetBuffer(0), mNonce.GetLength());
	if (mCheckHex.GetCheck() == TRUE)
	{
		//16进制IV数据
		mb_Nonce = StrToHex((BYTE*)mb_Nonce.c_str(), mb_Nonce.length());
	}

	//取出源数据
	unsigned __int64 srclen = HexEditControl::GetDataLength(pHexControl1);
	if (srclen == 0)
	{
		AfxMessageBox(_T("数据为空"));
		return;
	}
	BYTE *srcBuffer = (BYTE*)malloc(srclen);
	memset(srcBuffer, 0, srclen);
	HexEditControl::GetData(pHexControl1, srcBuffer, srclen);

	ChaCha20 mChaCha20;
	mChaCha20.SetKey((unsigned char*)mb_Key.c_str(), crypto_aead_chacha20poly1305_KEYBYTES, (unsigned char*)mb_Nonce.c_str(), crypto_aead_chacha20poly1305_NPUBBYTES);
	unsigned long long dstlen =0;
	unsigned char *dstbuffer=NULL;
	mChaCha20.Encrypt(srcBuffer, srclen, &dstbuffer, dstlen);
	if (dstlen)
	{
		HexEditControl::SetData(pHexControl1, dstbuffer, dstlen);
	}
	mChaCha20.FreeBuffer(&dstbuffer);
}

//解密
void ChaChaPanel::OnBnClickedButton2()
{
	UpdateData(TRUE);
	std::string mb_Key = MyWideCharToMultiByte((BYTE*)mKey.GetBuffer(0), mKey.GetLength());
	if (mCheckHex.GetCheck() == TRUE)
	{
		//16进制密钥数据
		mb_Key = StrToHex((BYTE*)mb_Key.c_str(), mb_Key.length());
	}
	//取出Nonce
	std::string mb_Nonce = MyWideCharToMultiByte((BYTE*)mNonce.GetBuffer(0), mNonce.GetLength());
	if (mCheckHex.GetCheck() == TRUE)
	{
		//16进制IV数据
		mb_Nonce = StrToHex((BYTE*)mb_Nonce.c_str(), mb_Nonce.length());
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

	ChaCha20 mChaCha20;
	mChaCha20.SetKey((unsigned char*)mb_Key.c_str(), crypto_aead_chacha20poly1305_KEYBYTES, (unsigned char*)mb_Nonce.c_str(), crypto_aead_chacha20poly1305_NPUBBYTES);
	unsigned long long dstlen = 0;
	unsigned char *dstbuffer = NULL;
	if (bVerify)
		mChaCha20.Decrypt(srcBuffer, srclen, &dstbuffer, dstlen);
	else
		mChaCha20.DecryptAndNoVerify(srcBuffer, srclen, &dstbuffer, dstlen);
	if (dstlen)
	{
		HexEditControl::SetData(pHexControl1, dstbuffer, dstlen);
	}else
		HexEditControl::SetData(pHexControl1, 0, 0);
	mChaCha20.FreeBuffer(&dstbuffer);
}

//切换密码形态
void ChaChaPanel::OnBnClickedCheck1()
{
	// TODO:  在此添加控件通知处理程序代码
	UpdateData(TRUE);
	if (mCheckHex.GetCheck() == TRUE)
	{
		if (mKey.GetLength() != 0)
		{
			std::string strM = MyWideCharToMultiByte((BYTE*)mKey.GetBuffer(0), mKey.GetLength());
			std::string str = HexToStr((BYTE*)strM.c_str(), strM.length());
			std::string strW = MyMultiByteToWideChar((BYTE*)str.c_str(), str.length());
			mKey = "";
			mKey.Append((wchar_t*)strW.c_str(), strW.length() / 2);
		}
		if (mNonce.GetLength() != 0)
		{
			std::string strM = MyWideCharToMultiByte((BYTE*)mNonce.GetBuffer(0), mNonce.GetLength());
			std::string str = HexToStr((BYTE*)strM.c_str(), strM.length());
			std::string strW = MyMultiByteToWideChar((BYTE*)str.c_str(), str.length());
			mNonce = "";
			mNonce.Append((wchar_t*)strW.c_str(), strW.length() / 2);
		}
	}
	else
	{
		if (mKey.GetLength() != 0)
		{
			std::string strM = MyWideCharToMultiByte((BYTE*)mKey.GetBuffer(0), mKey.GetLength());
			std::string str = StrToHex((BYTE*)strM.c_str(), strM.length());
			std::string strW = MyMultiByteToWideChar((BYTE*)str.c_str(), str.length());
			mKey = "";
			mKey.Append((wchar_t*)strW.c_str(), strW.length() / 2);
		}
		if (mNonce.GetLength() != 0)
		{
			std::string strM = MyWideCharToMultiByte((BYTE*)mNonce.GetBuffer(0), mNonce.GetLength());
			std::string str = StrToHex((BYTE*)strM.c_str(), strM.length());
			std::string strW = MyMultiByteToWideChar((BYTE*)str.c_str(), str.length());
			mNonce = "";
			mNonce.Append((wchar_t*)strW.c_str(), strW.length() / 2);
		}
	}
	UpdateData(FALSE);
}

std::string ChaChaPanel::HexToStr(BYTE *pbSrc, int nLen)
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

std::string ChaChaPanel::StrToHex(BYTE *pbSrc, int nLen)
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

std::string ChaChaPanel::MyWideCharToMultiByte(BYTE *pbDest, int nLen)
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

std::string ChaChaPanel::MyMultiByteToWideChar(BYTE *pbDest, int nLen)
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