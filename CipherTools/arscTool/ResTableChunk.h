#pragma once
#include "ResChunkBase.h"
class ResTableChunk :
	public ResChunkBase
{
public:

	ResTableChunk()
	{
	}

	virtual ~ResTableChunk()
	{
	}
public:
	unsigned int packageCount;   // 被编译的资源包的个数
};

