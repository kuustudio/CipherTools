#pragma once
#include<string>
#include "ChunkBase.h"
class TextChunk :
	public ChunkBase
{
public:

	TextChunk()
	{
	}

	virtual ~TextChunk()
	{
	}
public:
	std::wstring strName;
	unsigned int dwUnknown2;
	unsigned int dwUnknown3;
};

