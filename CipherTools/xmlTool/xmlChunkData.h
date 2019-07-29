#pragma once
#include"ChunkBase.h"
class xmlChunkData
{
public:

	xmlChunkData()
	{
	}

	virtual ~xmlChunkData()
	{
		clear();
	}
public:
	std::vector<ChunkBase*>	chunks;
public:
	void add(ChunkBase *p)
	{
		chunks.push_back(p);
	}
	void clear()
	{
		for (std::vector<ChunkBase *>::iterator it = chunks.begin(); it != chunks.end(); it++)
		if (NULL != *it)
		{
			delete *it;
			*it = NULL;
		}
		chunks.clear();
	}
};

