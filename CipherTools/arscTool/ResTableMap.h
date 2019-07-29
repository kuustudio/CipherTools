#pragma once
#include"ResValue.h"
class ResTableMap
{
public:

	ResTableMap()
	{
	}

	virtual ~ResTableMap()
	{
	}
public:
	//ResTableRef name;
	short index;
	short res0;
	ResValue value;
};

