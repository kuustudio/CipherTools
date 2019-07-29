#pragma once
#include"ResTableTypeSpec.h"
#include"ResTableTypeElement.h"

class ResResource
{
public:

	ResResource()
	{
	}

	virtual ~ResResource()
	{
	}
public:
	ResTableTypeSpec	resTableTypeSpec;
	std::vector<ResTableTypeElement> resTableTypes;
};

