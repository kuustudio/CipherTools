#pragma once
#include"ResStringPoolChunk.h"
#include"ResValue.h"

typedef struct
{
	int ident;
}ResTableRef;

class ResTableEntry
{
public:

	ResTableEntry()
	{
	}

	virtual ~ResTableEntry()
	{
	}
public:
	unsigned int entryId;//自己添加的标记entry在当前的类型中的偏移
public:
	short size;
	short flags;
	ResStringPoolRef key;
	ResValue	resValue;
};

