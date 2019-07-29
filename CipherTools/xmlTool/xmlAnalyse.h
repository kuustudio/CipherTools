#pragma once
#include"h_xml.h"
class CXmlAnalyse
{
public:
	CXmlAnalyse();
	~CXmlAnalyse();
public:
	xml_ctx_t	mCtx;
	bool		isAnalysised;
public:
	//初始化
	bool xmlLoadFile(const wchar_t *lpPath, const wchar_t *lpModel);
	bool xmlUnload();
public://分析函数
	bool	Analysis();
	std::wstring getAttrType(unsigned int);
	std::wstring getAttributeData(unsigned int,unsigned int);
	std::wstring createXml();
};

