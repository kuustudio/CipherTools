#pragma once
#include<string>
#include "ChunkBase.h"
class EndNamespaceChunk :
	public ChunkBase
{
public:

	EndNamespaceChunk()
	{
	}

	virtual ~EndNamespaceChunk()
	{
	}
public:
	std::wstring strPrefix;
	std::wstring strUri;
};

