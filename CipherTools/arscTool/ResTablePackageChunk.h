#pragma once
#include<string>
#include "ResChunkBase.h"
#include"ResStringPoolChunk.h"
#include"ResResource.h"


class ResTablePackageChunk :
	public ResChunkBase
{
public:
	ResTablePackageChunk()
	{
	}
	virtual ~ResTablePackageChunk()
	{
	}
public:
	int id;
	std::wstring name;
	int typeStrings;
	int lastPublicType;
	int keyStrings;
	int lastPublicKey;
public:
	ResStringPoolChunk			resStringPoolType;
	ResStringPoolChunk			resStringPoolKey;
	std::vector<ResResource>	resResources;
};

