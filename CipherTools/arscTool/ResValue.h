#pragma once
#include<string>
#include"ResourceTypes.h"

class ResValue
{
public:

	ResValue()
	{
	}

	virtual ~ResValue()
	{
	}
public:
	short size;
	char res0;
	char dataType;
	int data;

	enum
	{
		FORMAT_REFERENCE = 0x01,
		FORMAT_STRING = 0x02,
		FORMAT_INT = 0x04,
		FORMAT_BOOL = 0x08,
		FORMAT_COLOR = 0x10,
		FORMAT_FLOAT = 0x20,
		FORMAT_DIMEN = 0x40,
		FORMAT_FRACTION = 0x80,
		FORMAT_ANY_STRING = 0xee
	};

	void getTypeAsString(char* lpszTypeStr) {
		char szTypeStr[128] = { 0x0 };
		szTypeStr[0] = '|';
		if ((data & FORMAT_REFERENCE) != 0) {
			strcat_s(szTypeStr, "|reference");
		}
		if ((data & FORMAT_STRING) != 0) {
			strcat_s(szTypeStr, "|string");
		}
		if ((data & FORMAT_INT) != 0) {
			strcat_s(szTypeStr, "|integer");
		}
		if ((data & FORMAT_BOOL) != 0) {
			strcat_s(szTypeStr, "|boolean");
		}
		if ((data & FORMAT_COLOR) != 0) {
			strcat_s(szTypeStr, "|color");
		}
		if ((data & FORMAT_FLOAT) != 0) {
			strcat_s(szTypeStr, "|float");
		}
		if ((data & FORMAT_DIMEN) != 0) {
			strcat_s(szTypeStr, "|dimension");
		}
		if ((data & FORMAT_FRACTION) != 0) {
			strcat_s(szTypeStr, "|fraction");
		}

		if (szTypeStr[1] == 0x0){
			lpszTypeStr = NULL;
		}
		else{
			//strcpy_s(lpszTypeStr, szTypeStr + 2);
		}
	}

	std::wstring getTypeStr()
	{
		switch (dataType){
		case TYPE_NULL:
			return L"TYPE_NULL";
		case TYPE_REFERENCE:
			return L"TYPE_REFERENCE";
		case TYPE_ATTRIBUTE:
			return L"TYPE_ATTRIBUTE";
		case TYPE_STRING:
			return L"TYPE_STRING";
		case TYPE_FLOAT:
			return L"TYPE_FLOAT";
		case TYPE_DIMENSION:
			return L"TYPE_DIMENSION";
		case TYPE_FRACTION:
			return L"TYPE_FRACTION";
		case TYPE_FIRST_INT:
			return L"TYPE_FIRST_INT";
		case TYPE_INT_HEX:
			return L"TYPE_INT_HEX";
		case TYPE_INT_BOOLEAN:
			return L"TYPE_INT_BOOLEAN";
		case TYPE_FIRST_COLOR_INT:
			return L"TYPE_FIRST_COLOR_INT";
		case TYPE_INT_COLOR_RGB8:
			return L"TYPE_INT_COLOR_RGB8";
		case TYPE_INT_COLOR_ARGB4:
			return L"TYPE_INT_COLOR_ARGB4";
		case TYPE_INT_COLOR_RGB4:
			return L"TYPE_INT_COLOR_RGB4";
		}
		return L"0";
	}
};

