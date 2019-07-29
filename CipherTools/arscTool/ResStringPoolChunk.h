#pragma once
#include<map>
#include "ResChunkBase.h"

typedef struct
{
	int index;
}ResStringPoolRef;

typedef struct
{
	ResStringPoolRef name;
	int firstChar;
	int lastChar;
}ResStringPoolSpan;

typedef struct{
	std::wstring str;
	ResStringPoolSpan	resStringPoolSpan;

}ResStringStyles;

class ResStringPoolChunk :
	public ResChunkBase
{
public:

	ResStringPoolChunk()
	{
	}

	virtual ~ResStringPoolChunk()
	{
	}
public:
	int stringCount;        // 字符串的个数
	int styleCount;         // 字符串样式的个数
	int flags;              // 字符串的属性,可取值包括0x000(UTF-16),0x001(字符串经过排序)、0X100(UTF-8)和他们的组合值
	int stringsStart;       // 字符串内容块相对于其头部的距离
	int stylesStart;        // 字符串样式块相对于其头部的距离
	std::map<unsigned int, std::wstring> strings;
};

