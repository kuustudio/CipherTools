#pragma once
#include<string>
#include "ChunkBase.h"
class EndTagChunk :
	public ChunkBase
{
public:

	EndTagChunk()
	{
	}

	virtual ~EndTagChunk()
	{
	}
public:
	unsigned int dwNamespaceUri;
	std::wstring strName;
};

