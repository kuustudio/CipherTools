#pragma once
class ChunkBase
{
public:

	ChunkBase()
	{
	}

	virtual ~ChunkBase()
	{
	}
public:
	unsigned int dwChunkType;
	unsigned int dwChunkSize;
	unsigned int dwLineNumber;
	unsigned int dwUnknown1;
};

