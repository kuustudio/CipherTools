#pragma once
#include<map>
class elf_class
{
public:

	elf_class()
	{
	}

	virtual ~elf_class()
	{
	}
public:
	BOOL				is64Bit;
	BOOL				isNoSPARC;
	std::map<CString, Elf32_Shdr>Map_Shdr32;
	std::map<CString, Elf64_Shdr>Map_Shdr64;
	std::map<CString, Elf32_Phdr>Map_Phdr32;
	std::map<CString, Elf64_Phdr>Map_Phdr64;
};

