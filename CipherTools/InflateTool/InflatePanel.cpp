// InflatePanel.cpp : 实现文件
//

#include "stdafx.h"
#include "InflateTool.h"
#include "InflatePanel.h"
#include "afxdialogex.h"
#include"Inflate.h"


// InflatePanel 对话框

IMPLEMENT_DYNAMIC(InflatePanel, CDialogEx)

InflatePanel::InflatePanel(CWnd* pParent /*=NULL*/)
	: CDialogEx(InflatePanel::IDD, pParent)
{

}

InflatePanel::~InflatePanel()
{
}

void InflatePanel::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(InflatePanel, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &InflatePanel::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &InflatePanel::OnBnClickedButton2)
END_MESSAGE_MAP()


// InflatePanel 消息处理程序


BOOL InflatePanel::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(1);

	//((CButton*)GetDlgItem(IDC_BUTTON1))->EnableWindow(FALSE);

	//加载HexControl
	pHexControl1 = HexEditControl::ShowHexControlEx(m_hWnd, 10, 70, 560, 350, FALSE, TRUE);
	pHexControl1->SetParent(this);
	BYTE buffer2[] = { 0x1f,0x8b,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x45, 0x50, 0xcb, 0x6e, 0xdb, 0x30, 0x10, 0xfc, 0x1b, 0xde, 0x62, 0xec, 0x92, 0x5c, 0x3e, 0x4a, 0xec, 0x41, 0x95, 0xac, 0x53, 0x7b, 0x09, 0xd0, 0x43, 0x4f,
		0x06, 0x45, 0x4a, 0xa9, 0x12, 0xdb, 0x4a, 0x2d, 0xc7, 0x46, 0x01, 0x7e, 0x7c, 0xa9, 0xa4, 0x8f, 0xe1, 0x81, 0xbb, 0x33, 0x20, 0x77, 0x76, 0xc4, 0xf9, 0xce, 0x49, 0xa4, 0x23, 0x6b, 0x03, 0x05, 0x35, 0x6a, 0x5d, 0x08,
		0x7c, 0x51, 0x56, 0xa2, 0xac, 0xf4, 0x61, 0xe5, 0x07, 0x4f, 0x5b, 0x71, 0x65, 0x24, 0x70, 0x4e, 0x4b, 0xef, 0x88, 0xd0, 0x56, 0xea, 0xca, 0xa5, 0x42, 0x4a, 0x03, 0x9a, 0x82, 0x48, 0x2b, 0x83, 0xb8, 0x4f, 0x8c, 0xc9,
		0x40, 0x1e, 0xd1, 0x0c, 0x03, 0x8e, 0xc1, 0xb8, 0x50, 0x5c, 0x9a, 0xa4, 0x74, 0x79, 0x30, 0x6a, 0x18, 0x83, 0x95, 0xa1, 0x98, 0x44, 0x5e, 0x83, 0xd4, 0x32, 0x4e, 0x3a, 0x58, 0x15, 0xbe, 0xee, 0x1f, 0xdb, 0x6f, 0x8f,
		0xdf, 0x0f, 0xb2, 0xe9, 0xf5, 0x1f, 0xd1, 0xa8, 0x64, 0xb4, 0x0c, 0xd6, 0xfd, 0x13, 0xdb, 0xda, 0x97, 0xff, 0x7f, 0x9b, 0x2a, 0xfa, 0x50, 0x92, 0x53, 0x51, 0x11, 0x52, 0x35, 0x0f, 0xc1, 0xeb, 0x6d, 0x58, 0x34, 0x79,
		0x72, 0x08, 0x40, 0x18, 0xbc, 0x0d, 0xd5, 0xd1, 0xe1, 0xcc, 0x6a, 0xbb, 0x2e, 0x2b, 0x5b, 0xbd, 0x15, 0x6f, 0x57, 0x96, 0xb6, 0x7c, 0x60, 0xeb, 0xd7, 0xeb, 0xbb, 0xf3, 0xc3, 0xf8, 0xbe, 0xa2, 0x07, 0xe3, 0x41, 0xa1,
		0x06, 0xbb, 0x71, 0xb7, 0xbf, 0xda, 0x99, 0x51, 0xac, 0xf9, 0x85, 0xcd, 0x4e, 0x29, 0x11, 0x73, 0xbe, 0x70, 0x3c, 0x1e, 0x45, 0x5a, 0x96, 0x0b, 0x3f, 0xa5, 0x67, 0xa8, 0x59, 0xbd, 0x71, 0x4d, 0xa7, 0x03, 0xda, 0xf7,
		0xd0, 0xf4, 0xaa, 0xd3, 0xb6, 0x33, 0x64, 0xd0, 0x76, 0x9f, 0x69, 0xaf, 0x5a, 0xdf, 0x83, 0xa1, 0x42, 0x7d, 0x23, 0xfb, 0x8e, 0x08, 0x36, 0x34, 0xa2, 0x99, 0x33, 0xa3, 0x82, 0x34, 0xfa, 0x0c, 0x93, 0x89, 0x29, 0x67,
		0xe7, 0xc4, 0x74, 0xff, 0x98, 0x51, 0xe3, 0x45, 0x71, 0x1a, 0xb8, 0xfd, 0x31, 0xe2, 0x43, 0xfb, 0x45, 0x82, 0x38, 0xc6, 0xf9, 0x95, 0x13, 0x44, 0x07, 0x68, 0x92, 0xb8, 0x8c, 0xeb, 0xf6, 0x5c, 0x8a, 0x65, 0xe5, 0x06,
		0xbd, 0x58, 0x6f, 0xac, 0x77, 0xf5, 0x88, 0x54, 0xf7, 0x95, 0xe2, 0x14, 0x13, 0x4b, 0x8d, 0x53, 0x84, 0x89, 0x06, 0x8a, 0x5e, 0xbc, 0x5e, 0x96, 0xcc, 0x9f, 0xd2, 0x72, 0xda, 0xfd, 0x9c, 0x7f, 0xcd, 0xbb, 0xdb, 0x9c,
		0xc7, 0xa5, 0x88, 0x21, 0xb1, 0xfa, 0x0d, 0x10, 0x6d, 0x4d, 0x42, 0x03, 0x02 };
	HexEditControl::SetData(pHexControl1, buffer2, sizeof(buffer2));

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

//压缩
void InflatePanel::OnBnClickedButton1()
{
	if (IsDlgButtonChecked(IDC_RADIO1))
	{
		//Gzip
		UINT srclen = HexEditControl::GetDataLength(pHexControl1);
		if (srclen == 0)
		{
			AfxMessageBox(_T("数据为空"));
			return;
		}
		BYTE *srcBuffer = (BYTE*)malloc(srclen);
		memset(srcBuffer, 0, srclen);
		HexEditControl::GetData(pHexControl1, srcBuffer, srclen);

		CInflate mInflate;
		//压缩测试
		if (mInflate.gzcompress(srcBuffer, srclen) == 0)
		{
			//压缩成功
			HexEditControl::SetData(pHexControl1, mInflate.odata, mInflate.nodata);
		}
		else
		{
			HexEditControl::SetData(pHexControl1, mInflate.odata, 1);
		}
		free(srcBuffer);

	}
	else if (IsDlgButtonChecked(IDC_RADIO2))
	{
		//ZLIB:头部特征:
		//78 01 - No Compression / low
		//78 5E
		//78 9C - Default Compression
		//78 DA - Best Compression

		UINT srclen = HexEditControl::GetDataLength(pHexControl1);
		if (srclen == 0)
		{
			AfxMessageBox(_T("数据为空"));
			return;
		}
		BYTE *srcBuffer = (BYTE*)malloc(srclen);
		memset(srcBuffer, 0, srclen);
		HexEditControl::GetData(pHexControl1, srcBuffer, srclen);

		CInflate mInflate;
		if (mInflate.zcompress(srcBuffer, srclen) == 0)
		{
			HexEditControl::SetData(pHexControl1, mInflate.odata, mInflate.nodata);
		}
		else
			HexEditControl::SetData(pHexControl1, mInflate.odata, 1);

		free(srcBuffer);
	}
	else if (IsDlgButtonChecked(IDC_RADIO3))
	{
		//HttpGzip头部特征:1f 8b 08
		UINT srclen = HexEditControl::GetDataLength(pHexControl1);
		if (srclen == 0)
		{
			AfxMessageBox(_T("数据为空"));
			return;
		}
		BYTE *srcBuffer = (BYTE*)malloc(srclen);
		memset(srcBuffer, 0, srclen);
		HexEditControl::GetData(pHexControl1, srcBuffer, srclen);

		CInflate mInflate;
		//压缩测试
		if (mInflate.gzcompress(srcBuffer, srclen) == 0)
		{
			//压缩成功
			HexEditControl::SetData(pHexControl1, mInflate.odata, mInflate.nodata);
		}
		else
		{
			HexEditControl::SetData(pHexControl1, mInflate.odata, 1);
		}
		free(srcBuffer);
	}
	else if (IsDlgButtonChecked(IDC_RADIO4))
	{
		//lzma头部特征:6c 00
		UINT srclen = HexEditControl::GetDataLength(pHexControl1);
		if (srclen == 0)
		{
			AfxMessageBox(_T("数据为空"));
			return;
		}
		BYTE *srcBuffer = (BYTE*)malloc(srclen);
		memset(srcBuffer, 0, srclen);
		HexEditControl::GetData(pHexControl1, srcBuffer, srclen);

		CInflate mInflate;
		//压缩测试
		if (mInflate.lzmacompress(srcBuffer, srclen) == 0)
		{
			//压缩成功
			HexEditControl::SetData(pHexControl1, mInflate.odata, mInflate.nodata);
		}
		else
		{
			HexEditControl::SetData(pHexControl1, mInflate.odata, 1);
		}
		free(srcBuffer);
	}
}

//解压
void InflatePanel::OnBnClickedButton2()
{
	if (IsDlgButtonChecked(IDC_RADIO1))
	{
		//GZIP
		UINT srclen = HexEditControl::GetDataLength(pHexControl1);
		if (srclen == 0)
		{
			AfxMessageBox(_T("数据为空"));
			return;
		}
		BYTE *srcBuffer = (BYTE*)malloc(srclen);
		memset(srcBuffer, 0, srclen);
		HexEditControl::GetData(pHexControl1, srcBuffer, srclen);

		CInflate mInflate;
		//解压测试
		if (mInflate.gzdecompress(srcBuffer, srclen) == 0)
		{
			//解压成功
			HexEditControl::SetData(pHexControl1, mInflate.odata, mInflate.nodata);
		}
		else
		{
			HexEditControl::SetData(pHexControl1, mInflate.odata, 1);
		}
		free(srcBuffer);
	}
	else if (IsDlgButtonChecked(IDC_RADIO2))
	{
		//ZLIB
		UINT srclen = HexEditControl::GetDataLength(pHexControl1);
		if (srclen == 0)
		{
			AfxMessageBox(_T("数据为空"));
			return;
		}
		BYTE *srcBuffer = (BYTE*)malloc(srclen);
		memset(srcBuffer, 0, srclen);
		HexEditControl::GetData(pHexControl1, srcBuffer, srclen);

		CInflate mInflate;
		//解压测试
		if (mInflate.zdecompress(srcBuffer, srclen) == 0)
		{
			//解压成功
			HexEditControl::SetData(pHexControl1, mInflate.odata, mInflate.nodata);
		}else
			HexEditControl::SetData(pHexControl1, mInflate.odata, 1);
		
		free(srcBuffer);
	}
	else if (IsDlgButtonChecked(IDC_RADIO3))
	{
		//HttpGzip
		UINT srclen = HexEditControl::GetDataLength(pHexControl1);
		if (srclen == 0)
		{
			AfxMessageBox(_T("数据为空"));
			return;
		}
		BYTE *srcBuffer = (BYTE*)malloc(srclen);
		memset(srcBuffer, 0, srclen);
		HexEditControl::GetData(pHexControl1, srcBuffer, srclen);

		CInflate mInflate;
		//解压测试
		if (mInflate.httpgzdecompress(srcBuffer, srclen) == 0)
		{
			//解压成功
			HexEditControl::SetData(pHexControl1, mInflate.odata, mInflate.nodata);
		}
		else
		{
			HexEditControl::SetData(pHexControl1, mInflate.odata, 1);
		}
		free(srcBuffer);
	}
	else if (IsDlgButtonChecked(IDC_RADIO4))
	{
		//lzma
		UINT srclen = HexEditControl::GetDataLength(pHexControl1);
		if (srclen == 0)
		{
			AfxMessageBox(_T("数据为空"));
			return;
		}
		BYTE *srcBuffer = (BYTE*)malloc(srclen);
		memset(srcBuffer, 0, srclen);
		HexEditControl::GetData(pHexControl1, srcBuffer, srclen);

		CInflate mInflate;
		//解压测试
		if (mInflate.lzmauncompress(srcBuffer, srclen) == 0)
		{
			//解压成功
			HexEditControl::SetData(pHexControl1, mInflate.odata, mInflate.nodata);
		}
		else
		{
			HexEditControl::SetData(pHexControl1, mInflate.odata, 1);
		}
		free(srcBuffer);
	}


}
