#pragma once
#include "ResChunkBase.h"
#include"ResTableConfig.h"
class ResTableType :
	public ResChunkBase
{
public:

	ResTableType()
	{
	}

	virtual ~ResTableType()
	{
	}
public:
	unsigned char id;
	unsigned char res0;
	unsigned short res1;
	unsigned int entryCount;
	unsigned int entriesStart;
	ResTableConfig resConfig;
};

