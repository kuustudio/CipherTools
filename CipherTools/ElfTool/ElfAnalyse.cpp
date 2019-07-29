#include "stdafx.h"
#include<sstream>
#include "ElfAnalyse.h"


CElfAnalyse::CElfAnalyse()
{
}


CElfAnalyse::~CElfAnalyse()
{
}

bool CElfAnalyse::Analysis()
{
	if (((char*)mCtx.pVirMem)[EI_CLASS] == ELFCLASS64)
	{
		mCtx.elf.is64Bit = TRUE;
	}
	else if (((char*)mCtx.pVirMem)[EI_CLASS] == ELFCLASS32)
	{
		mCtx.elf.is64Bit = FALSE;
	}
	else if (((char*)mCtx.pVirMem)[EI_CLASS] == ELFCLASSNONE)
	{
		AfxMessageBox(L"无效类");
		return false;
	}

	SetElfHeaderInfo();  //解析ELF_HEAD
	return true;
}

void CElfAnalyse::SetElfHeaderInfo()
{
	if (mCtx.elf.is64Bit == TRUE)
	{
		Elf64_Ehdr* Elf_Ehdr = (Elf64_Ehdr*)(mCtx.pVirMem);
		if (Elf_Ehdr->e_machine == EM_SPARCV9)
		{
			mCtx.elf.isNoSPARC = TRUE;
		}
		else
		{
			mCtx.elf.isNoSPARC = FALSE;
		}

		Elf64_Phdr* Phdr = (Elf64_Phdr*)(mCtx.pVirMem + Elf_Ehdr->e_phoff);
		for (int i = 0; i<Elf_Ehdr->e_phnum; i++, Phdr++)
		{
			CString item;
			item.Format(L"Program Header %d", i);
			mCtx.elf.Map_Phdr64.insert(std::map<CString, Elf64_Phdr>::value_type(item, *(Elf64_Phdr*)Phdr));
		}



		char * pStrTable;	// 用以取得每个 section 的名字
		Elf64_Shdr * ShdrStringTable = (Elf64_Shdr *)(mCtx.pVirMem + Elf_Ehdr->e_shoff) + Elf_Ehdr->e_shstrndx;
		pStrTable = (char *)(mCtx.pVirMem + ShdrStringTable->sh_offset);

		Elf64_Shdr* Shdr = (Elf64_Shdr*)(mCtx.pVirMem + Elf_Ehdr->e_shoff);
		for (int i = 0; i< Elf_Ehdr->e_shnum; i++, Shdr++)
		{
			std::wstring strw;
			Utf8ToUnicode(pStrTable + Shdr->sh_name, strw);
			CString item;
			item.Format(L"%s", strw.c_str());
			mCtx.elf.Map_Shdr64.insert(std::map<CString, Elf64_Shdr>::value_type(item, *(Elf64_Shdr*)Shdr));
		}
	}
	else
	{
		Elf32_Ehdr* Elf_Ehdr = (Elf32_Ehdr*)(mCtx.pVirMem);
		if (Elf_Ehdr->e_machine == EM_SPARCV9)
		{
			mCtx.elf.isNoSPARC = TRUE;
		}
		else
		{
			mCtx.elf.isNoSPARC = FALSE;
		}

		Elf32_Phdr* Phdr = (Elf32_Phdr*)(mCtx.pVirMem + Elf_Ehdr->e_phoff);
		for (int i = 0; i<Elf_Ehdr->e_phnum; i++, Phdr++)
		{
			CString item;
			item.Format(L"Program Header %d", i);
			mCtx.elf.Map_Phdr32.insert(std::map<CString, Elf32_Phdr>::value_type(item, *(Elf32_Phdr*)Phdr));
		}
		char * pStrTable;	// 用以取得每个 section 的名字
		Elf32_Shdr * ShdrStringTable = (Elf32_Shdr *)(mCtx.pVirMem + Elf_Ehdr->e_shoff) + Elf_Ehdr->e_shstrndx;
		pStrTable = (char *)(mCtx.pVirMem + ShdrStringTable->sh_offset);

		Elf32_Shdr* Shdr = (Elf32_Shdr*)(mCtx.pVirMem + Elf_Ehdr->e_shoff);
		for (int i = 0; i< Elf_Ehdr->e_shnum; i++, Shdr++)
		{
			std::wstring strw;
			Utf8ToUnicode(pStrTable + Shdr->sh_name,strw);
			CString item;
			item.Format(L"%s", strw.c_str());
			mCtx.elf.Map_Shdr32.insert(std::map<CString, Elf32_Shdr>::value_type(item, *(Elf32_Shdr*)Shdr));
		}
	}
}

bool CElfAnalyse::elfLoadFile(const wchar_t *lpPath, const wchar_t *lpModel)
{
	if (!PathFileExists(lpPath))
	{
		return false;
	}
	mCtx.path.clear();
	mCtx.path.append(lpPath);
	HANDLE mHandle = CreateFile(mCtx.path.c_str(), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == mHandle)
	{
		return false;
	}

	DWORD dwSizeHigh = 0, dwSizeLow = 0;
	dwSizeLow = GetFileSize(mHandle, &dwSizeHigh);
	if (dwSizeLow == INVALID_FILE_SIZE || dwSizeHigh != 0)
	{
		CloseHandle(mHandle);
		return false;
	}

	mCtx.size = dwSizeLow;

	mCtx.pVirMem = (char*)VirtualAlloc(NULL, mCtx.size, MEM_RESERVE | MEM_COMMIT | MEM_TOP_DOWN, PAGE_EXECUTE_READWRITE);
	::VirtualLock(mCtx.pVirMem, mCtx.size);

	if (mCtx.pVirMem == NULL)
	{
		CloseHandle(mHandle);
		return false;
	}

	DWORD readsize;
	if (!ReadFile(mHandle, mCtx.pVirMem, mCtx.size, &readsize, NULL))
	{
		CloseHandle(mHandle);
		return false;
	}
	CloseHandle(mHandle);

	if (!(
		(((char*)mCtx.pVirMem)[EI_MAG0] == 0x7F) &&
		(((char*)mCtx.pVirMem)[EI_MAG1] == 'E') &&
		(((char*)mCtx.pVirMem)[EI_MAG2] == 'L') &&
		(((char*)mCtx.pVirMem)[EI_MAG3] == 'F'))) {
		return false;
	}

	return true;
}

void CElfAnalyse::UnicodeToUtf8(const std::wstring &wstr, std::string &str)
{
	int len;
	len = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
	char *szUtf8 = (char*)malloc(len + 1);
	memset(szUtf8, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, szUtf8, len, NULL, NULL);

	str.clear();
	str.append(szUtf8, len);
}

void CElfAnalyse::Utf8ToUnicode(const std::string &szU8, std::wstring &wstr)
{
	//预转换，得到所需空间的大小;
	int wcsLen = ::MultiByteToWideChar(CP_UTF8, NULL, szU8.c_str(), szU8.size(), NULL, 0);

	//分配空间要给'\0'留个空间，MultiByteToWideChar不会给'\0'空间
	wchar_t* wszString = new wchar_t[wcsLen + 1];

	//转换
	::MultiByteToWideChar(CP_UTF8, NULL, szU8.c_str(), szU8.size(), wszString, wcsLen);

	//最后加上'\0'
	wszString[wcsLen] = '\0';

	wstr.clear();
	wstr.append(wszString, wcsLen);

	delete[] wszString;
	wszString = NULL;
}

std::wstring CElfAnalyse::doReport()
{
	std::wstring strReport = L"";
	if (!isAnalysised) return strReport;

	std::wostringstream   ostr;
	

	strReport = ostr.str();
	return strReport;
}

bool CElfAnalyse::elfUnload()
{
	VirtualUnlock(mCtx.pVirMem, mCtx.size);
	VirtualFree(mCtx.pVirMem, mCtx.size, MEM_RELEASE);
	return true;
}