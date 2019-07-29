#pragma once
#include"ResTableType.h"
#include"ResTableEntry.h"

class ResTableTypeElement
{
public:

	ResTableTypeElement()
	{
	}

	//深拷贝构造函数
	ResTableTypeElement(const ResTableTypeElement& C)
	{
		resTableType = C.resTableType;
		entryOffsets = C.entryOffsets;
		for (std::vector<ResTableEntry *>::const_iterator it = C.resTableEntrys.begin(); it != C.resTableEntrys.end(); it++)
		{
			ResTableEntry *entry = new ResTableEntry();
			*entry = **it;
			resTableEntrys.push_back(entry);

		}
	}


	virtual ~ResTableTypeElement()
	{
		clear();
	}
public:
	ResTableType		resTableType;
	std::vector<unsigned int>	entryOffsets;
	std::vector<ResTableEntry*> resTableEntrys;
public:
	void add(ResTableEntry *p)
	{
		resTableEntrys.push_back(p);
	}
	void clear()
	{
		for (std::vector<ResTableEntry *>::iterator it = resTableEntrys.begin(); it != resTableEntrys.end(); it++)
		{
			if (NULL != *it)
			{
				delete *it;
				*it = NULL;
			}
		}
		resTableEntrys.clear();
	}
};

