#pragma once
#include"ResTableEntry.h"
#include"ResTableMap.h"
class ResTableMapEntry
	:public ResTableEntry
{
public:

	ResTableMapEntry()
	{
	}

	virtual ~ResTableMapEntry()
	{
	}
public:
	ResTableRef parent;
	int count;
	std::vector<ResTableMap> resTableMaps;
};

