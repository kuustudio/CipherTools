#pragma once
#include<vector>
#include"ResTableChunk.h"
#include"ResChunkBase.h"
#include"ResTablePackageChunk.h"


class arsc_class
{
public:

	arsc_class()
	{
	}

	virtual ~arsc_class()
	{
		clear();
	}
public:
	ResTableChunk			resTableChunk;
	std::vector<ResChunkBase*>	chunkData;
public:
	void add(ResChunkBase *p)
	{
		chunkData.push_back(p);
	}

	std::wstring findResStringKey(unsigned int index)
	{
		std::vector<ResChunkBase*>::iterator itor;
		for (itor = chunkData.begin(); itor != chunkData.end(); itor++)
		{
			if ((*itor)->wChunkType == RES_TABLE_PACKAGE_TYPE)
			{
				ResTablePackageChunk *p = static_cast<ResTablePackageChunk*>(*itor);
				return p->resStringPoolKey.strings[index];
			}
		}
	}

	std::wstring findResStringType(unsigned int index)
	{
		std::vector<ResChunkBase*>::iterator itor;
		for (itor = chunkData.begin(); itor != chunkData.end(); itor++)
		{
			if ((*itor)->wChunkType == RES_TABLE_PACKAGE_TYPE)
			{
				ResTablePackageChunk *p = static_cast<ResTablePackageChunk*>(*itor);
				return p->resStringPoolType.strings[index];
			}
		}
	}


	void clear()
	{
		for (std::vector<ResChunkBase *>::iterator it = chunkData.begin(); it != chunkData.end(); it++)
		{
			if (NULL != *it)
			{
				delete *it;
				*it = NULL;
			}
		}
		chunkData.clear();
	}
};

