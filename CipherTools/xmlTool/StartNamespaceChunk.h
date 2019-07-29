#pragma once
#include<string>
#include "ChunkBase.h"
class StartNamespaceChunk :
	public ChunkBase
{
public:

	StartNamespaceChunk()
	{
	}

	virtual ~StartNamespaceChunk()
	{
	}
public:
	std::wstring strPrefix;
	std::wstring strUri;
};

