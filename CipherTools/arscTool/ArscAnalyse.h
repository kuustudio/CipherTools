#pragma once
#include"ResourceTypes.h"
#include"ResStringPoolChunk.h"
#include"ResTablePackageChunk.h"
#include"arsc_class.h"


typedef struct {
	std::wstring path;
	char *pVirMem;
	size_t size;
	arsc_class arsc;
} arsc_ctx_t;

class CArscAnalyse
{
public:
	CArscAnalyse();
	~CArscAnalyse();
public:
	arsc_ctx_t	mCtx;
	bool		isAnalysised;
public:
	//初始化
	bool arscLoadFile(const wchar_t *lpPath, const wchar_t *lpModel);
	bool arscUnload();
public://分析函数
	bool	Analysis();
	void doResStringPool(const unsigned char*, ResStringPoolChunk*);
	void doResResource(unsigned int&, ResTablePackageChunk*);
	unsigned int decodeLength(const unsigned char * str, unsigned int &LenWide, unsigned int flag);
	void Utf8ToUnicode(const std::string &str, std::wstring &wstr);
	void UnicodeToUtf8(const std::wstring &wstr, std::string &str);
	std::wstring doReport();
};

