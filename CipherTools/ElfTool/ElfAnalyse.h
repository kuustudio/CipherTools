#pragma once
#include<string>
#include"elf.h"
#include"elf_class.h"

typedef struct {
	std::wstring path;
	char *pVirMem;
	size_t size;
	elf_class elf;
} elf_ctx_t;

class CElfAnalyse
{
public:
	CElfAnalyse();
	~CElfAnalyse();
public:
	elf_ctx_t	mCtx;
	bool		isAnalysised;
public:
	//初始化
	bool elfLoadFile(const wchar_t *lpPath, const wchar_t *lpModel);
	bool elfUnload();
public:
	void SetElfHeaderInfo();
public://分析函数
	bool	Analysis();
	void Utf8ToUnicode(const std::string &str, std::wstring &wstr);
	void UnicodeToUtf8(const std::wstring &wstr, std::string &str);
	std::wstring doReport();
};

