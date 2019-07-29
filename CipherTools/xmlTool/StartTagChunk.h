#pragma once
#include"h_xml.h"
#include "ChunkBase.h"
class StartTagChunk :
	public ChunkBase
{
public:

	StartTagChunk()
	{
	}

	virtual ~StartTagChunk()
	{
	}
public:
	unsigned int dwNamespaceUri;
	std::wstring strName;
	unsigned int dwFlags;
	unsigned int dwAttributeCount;
	unsigned int dwClassAttribute;
	std::vector<AttributeEntry> Attributes;
};

