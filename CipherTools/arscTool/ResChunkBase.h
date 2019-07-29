#pragma once
class ResChunkBase
{
public:

	ResChunkBase()
	{
	}

	virtual ~ResChunkBase()
	{
	}
public:
	unsigned short wChunkType;	 // CHUNK_TYPE
	unsigned short wHeaderSize;			// 当前Chunk的head大小
	unsigned int dwChunkSize;	// 当前Chunk大小
};

