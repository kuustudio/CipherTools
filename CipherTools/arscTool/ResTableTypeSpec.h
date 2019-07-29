#pragma once
#include<vector>
#include "ResChunkBase.h"
class ResTableTypeSpec :
	public ResChunkBase
{
public:

	ResTableTypeSpec()
	{
	}

	virtual ~ResTableTypeSpec()
	{
	}
public:
	unsigned char id;
	unsigned char res0;
	short res1;
	int entryCount;
	std::vector<unsigned int> ResConfigs;
};

