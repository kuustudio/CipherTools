#pragma once
#include"ElfAnalyse.h"
#include "afxcmn.h"


// ElfPanel 对话框

typedef struct
{
	WCHAR*     szTitle;           //列表的名称
	int		  nWidth;            //列表的宽度

}COLUMNSTRUCT;

class ElfPanel : public CDialogEx
{
	DECLARE_DYNAMIC(ElfPanel)

public:
	ElfPanel(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~ElfPanel();

// 对话框数据
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString strFile;
	CTreeCtrl m_Tree;
	CListCtrl m_List;
	CString m_ident;
	CString m_type;
	CString m_machine;
	CString m_version;
	LONGLONG m_entry;
	LONGLONG m_phoff;
	LONGLONG m_shoff;
	LONGLONG m_flags;
	LONGLONG m_ehsize;
	LONGLONG m_phentsize;
	LONGLONG m_phnum;
	LONGLONG m_shentsize;
	LONGLONG m_shnum;
	LONGLONG m_shstrndx;
	LONGLONG m_total;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnNMClickTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkTree(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL OnInitDialog();
public:
	CElfAnalyse mElfAnalyse;
public:
	void ToTree();
	void ToHead();
	WCHAR*  CharToWchar(char* szSour);
	VOID InitListControl(int number, COLUMNSTRUCT* Item);
	VOID InsertPhdr64(Elf64_Phdr Phdr);
	VOID InsertPhdr32(Elf32_Phdr Phdr);
	VOID InsertString32(Elf32_Shdr Shdr);
	VOID InsertString64(Elf64_Shdr Shdr);
	VOID InsertSym32(Elf32_Shdr Shdr, Elf32_Shdr DynString);
	VOID InsertSym64(Elf64_Shdr Shdr, Elf64_Shdr DynString);
	VOID InsertShdr32(Elf32_Shdr Shdr);
	VOID InsertShdr64(Elf64_Shdr Shdr);
	VOID InsertRel32(Elf32_Shdr Shdr);
	VOID InsertRel64(Elf64_Shdr Shdr);
};
