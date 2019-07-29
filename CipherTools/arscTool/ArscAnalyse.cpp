#include "stdafx.h"
#include <sstream>
#include "ArscAnalyse.h"
#include"ResTablePackageChunk.h"
#include"ResTableMapEntry.h"

CArscAnalyse::CArscAnalyse()
{
}


CArscAnalyse::~CArscAnalyse()
{
}

bool CArscAnalyse::arscLoadFile(const wchar_t *lpPath, const wchar_t *lpModel)
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

	unsigned int offset = 0;
	////读取头部位
	CopyMemory(&mCtx.arsc.resTableChunk.wChunkType, mCtx.pVirMem + offset, sizeof(mCtx.arsc.resTableChunk.wChunkType));
	offset += sizeof(mCtx.arsc.resTableChunk.wChunkType);
	CopyMemory(&mCtx.arsc.resTableChunk.wHeaderSize, mCtx.pVirMem + offset, sizeof(mCtx.arsc.resTableChunk.wHeaderSize));
	offset += sizeof(mCtx.arsc.resTableChunk.wHeaderSize);
	CopyMemory(&mCtx.arsc.resTableChunk.dwChunkSize, mCtx.pVirMem + offset, sizeof(mCtx.arsc.resTableChunk.dwChunkSize));
	offset += sizeof(mCtx.arsc.resTableChunk.dwChunkSize);
	CopyMemory(&mCtx.arsc.resTableChunk.packageCount, mCtx.pVirMem + offset, sizeof(mCtx.arsc.resTableChunk.packageCount));
	offset += sizeof(mCtx.arsc.resTableChunk.packageCount);

	//根据类型判断文件合法性
	if (mCtx.arsc.resTableChunk.wChunkType != RES_TABLE_TYPE)
		return false;

	return true;
}

bool CArscAnalyse::Analysis()
{
	//开始解析thunk
	mCtx.arsc.chunkData.clear();

	unsigned int dwStartChunkOffset = 2+2+4+4;
	unsigned int dwOffset = dwStartChunkOffset;
	
	
	//读取xmlContentChunk
	while (dwStartChunkOffset<mCtx.size)
	{
		unsigned short wChunkType = 0;
		unsigned short wHeadSize = 0;
		unsigned int dwChunkSize = 0;
		dwOffset = dwStartChunkOffset;
		
		CopyMemory(&wChunkType, mCtx.pVirMem + dwOffset, sizeof(wChunkType));
		CopyMemory(&wHeadSize, mCtx.pVirMem + dwOffset + 2, sizeof(wHeadSize));
		CopyMemory(&dwChunkSize, mCtx.pVirMem + dwOffset + 4, sizeof(dwChunkSize));
		switch (wChunkType)
		{
			case RES_NULL_TYPE:
			{
			}break;
			case RES_STRING_POOL_TYPE:
			{
				ResStringPoolChunk *resStringPoolChunk = new ResStringPoolChunk();
				doResStringPool((unsigned char*)(mCtx.pVirMem + dwOffset), resStringPoolChunk);
				mCtx.arsc.add(resStringPoolChunk);
			}break;
			case RES_TABLE_PACKAGE_TYPE:
			{
				ResTablePackageChunk *resTablePackageChunk = new ResTablePackageChunk();
				CopyMemory(&resTablePackageChunk->wChunkType, mCtx.pVirMem + dwOffset, 2);
				dwOffset += 2;
				CopyMemory(&resTablePackageChunk->wHeaderSize, mCtx.pVirMem + dwOffset, 2);
				dwOffset += 2;
				CopyMemory(&resTablePackageChunk->dwChunkSize, mCtx.pVirMem + dwOffset, 4);
				dwOffset += 4;
				CopyMemory(&resTablePackageChunk->id, mCtx.pVirMem + dwOffset, 4);
				dwOffset += 4;
				wchar_t name[0x80] = {0};
				CopyMemory(name, mCtx.pVirMem + dwOffset, sizeof(name));
				dwOffset += sizeof(name);
				resTablePackageChunk->name.clear();
				resTablePackageChunk->name.append(name, wcslen(name));
				CopyMemory(&resTablePackageChunk->typeStrings, mCtx.pVirMem + dwOffset, 4);
				dwOffset += 4;
				CopyMemory(&resTablePackageChunk->lastPublicType, mCtx.pVirMem + dwOffset, 4);
				dwOffset += 4;
				CopyMemory(&resTablePackageChunk->keyStrings, mCtx.pVirMem + dwOffset, 4);
				dwOffset += 4;
				CopyMemory(&resTablePackageChunk->lastPublicKey, mCtx.pVirMem + dwOffset, 4);
				dwOffset += 4;
				
				//开始解析资源类型字符串池
				doResStringPool((unsigned char*)(mCtx.pVirMem + dwOffset), &resTablePackageChunk->resStringPoolType);
				dwOffset += resTablePackageChunk->resStringPoolType.dwChunkSize;
				
				//开始解析资源项名称字符串池
				doResStringPool((unsigned char*)(mCtx.pVirMem + dwOffset), &resTablePackageChunk->resStringPoolKey);
				dwOffset += resTablePackageChunk->resStringPoolKey.dwChunkSize;
		
				doResResource( dwOffset, resTablePackageChunk);
				
				mCtx.arsc.add(resTablePackageChunk);
			}break;
			case RES_TABLE_TYPE:
			{
			}break;

		}
		//修正数据
		dwStartChunkOffset += dwChunkSize;
	}
	return true;
}

void CArscAnalyse::doResStringPool(const unsigned char *mem, ResStringPoolChunk *resStringPoolChunk)
{
	unsigned int dwOffset = 0;

	CopyMemory(&resStringPoolChunk->wChunkType, mem + dwOffset, 2);
	dwOffset += 2;
	CopyMemory(&resStringPoolChunk->wHeaderSize, mem + dwOffset, 2);
	dwOffset += 2;
	CopyMemory(&resStringPoolChunk->dwChunkSize, mem + dwOffset, 4);
	dwOffset += 4;
	CopyMemory(&resStringPoolChunk->stringCount, mem + dwOffset, 4);
	dwOffset += 4;
	CopyMemory(&resStringPoolChunk->styleCount, mem + dwOffset, 4);
	dwOffset += 4;
	CopyMemory(&resStringPoolChunk->flags, mem + dwOffset, 4);
	dwOffset += 4;
	CopyMemory(&resStringPoolChunk->stringsStart, mem + dwOffset, 4);
	dwOffset += 4;
	CopyMemory(&resStringPoolChunk->stylesStart, mem + dwOffset, 4);
	dwOffset += 4;

	//解析字符串数组
	for (int i = 0; i < resStringPoolChunk->stringCount; i++)
	{
		int value = 0;
		CopyMemory(&value, mem + dwOffset, 4);
		dwOffset += 4;
		std::wstring strValue;
		unsigned char size[4] = { 0 };
		CopyMemory(&size, mem + resStringPoolChunk->stringsStart + value, 4);
		unsigned int lenwide = 0;
		int stringlen = decodeLength(size, lenwide, resStringPoolChunk->flags);
		std::wstring str = L"";
		if (resStringPoolChunk->flags == 0x000)
		{
		}
		else if (resStringPoolChunk->flags == 0x001)
		{

		}
		else if (resStringPoolChunk->flags == 0x100)
		{
			char *buffer = new char[stringlen + 1];
			CopyMemory(buffer, mem + resStringPoolChunk->stringsStart + value + lenwide, stringlen + 1);//复制utf-16
			std::string strUtf8 = "";
			strUtf8.append(buffer, stringlen);
			Utf8ToUnicode(strUtf8, str);
			delete[]buffer;
			buffer = NULL;
		}
		else
		{

		}
		resStringPoolChunk->strings.insert(std::pair<unsigned int, std::wstring>(i, str));
	}
	////解析样式数组
	////暂不解析
}

void CArscAnalyse::doResResource(unsigned int &dwOffset, ResTablePackageChunk *resTablePackageChunk)
{
	for (int i = 0; i < resTablePackageChunk->resStringPoolType.stringCount; i++)
	{
		//开始解析类型规范数据块ResTableTypeSpec
		ResResource resResource;
		CopyMemory(&resResource.resTableTypeSpec.wChunkType, mCtx.pVirMem + dwOffset, 2);
		dwOffset += 2;
		CopyMemory(&resResource.resTableTypeSpec.wHeaderSize, mCtx.pVirMem + dwOffset, 2);
		dwOffset += 2;
		CopyMemory(&resResource.resTableTypeSpec.dwChunkSize, mCtx.pVirMem + dwOffset, 4);
		dwOffset += 4;
		CopyMemory(&resResource.resTableTypeSpec.id, mCtx.pVirMem + dwOffset, 1);
		dwOffset += 1;
		CopyMemory(&resResource.resTableTypeSpec.res0, mCtx.pVirMem + dwOffset, 1);
		dwOffset += 1;
		CopyMemory(&resResource.resTableTypeSpec.res1, mCtx.pVirMem + dwOffset, 2);
		dwOffset += 2;
		CopyMemory(&resResource.resTableTypeSpec.entryCount, mCtx.pVirMem + dwOffset, 4);
		dwOffset += 4;
		for (int i = 0; i < resResource.resTableTypeSpec.entryCount; i++)
		{
			unsigned int value = 0;
			CopyMemory(&value, mCtx.pVirMem + dwOffset, 4);
			dwOffset += 4;
			resResource.resTableTypeSpec.ResConfigs.push_back(value);
		}
		//开始解析资源类型项数据块ResTableType
		while (dwOffset<mCtx.size&&mCtx.pVirMem[dwOffset] == 0x01)
		{
			unsigned int dwResTableTypeOffset = dwOffset;
			ResTableTypeElement resTableTypeElement;
			CopyMemory(&resTableTypeElement.resTableType.wChunkType, mCtx.pVirMem + dwResTableTypeOffset, 2);
			dwResTableTypeOffset += 2;
			CopyMemory(&resTableTypeElement.resTableType.wHeaderSize, mCtx.pVirMem + dwResTableTypeOffset, 2);
			dwResTableTypeOffset += 2;
			CopyMemory(&resTableTypeElement.resTableType.dwChunkSize, mCtx.pVirMem + dwResTableTypeOffset, 4);
			dwResTableTypeOffset += 4;
			CopyMemory(&resTableTypeElement.resTableType.id, mCtx.pVirMem + dwResTableTypeOffset, 1);
			dwResTableTypeOffset += 1;
			CopyMemory(&resTableTypeElement.resTableType.res0, mCtx.pVirMem + dwResTableTypeOffset, 1);
			dwResTableTypeOffset += 1;
			CopyMemory(&resTableTypeElement.resTableType.res1, mCtx.pVirMem + dwResTableTypeOffset, 2);
			dwResTableTypeOffset += 2;
			CopyMemory(&resTableTypeElement.resTableType.entryCount, mCtx.pVirMem + dwResTableTypeOffset, 4);
			dwResTableTypeOffset += 4;
			CopyMemory(&resTableTypeElement.resTableType.entriesStart, mCtx.pVirMem + dwResTableTypeOffset, 4);
			dwResTableTypeOffset += 4;
			CopyMemory(&resTableTypeElement.resTableType.resConfig.size, mCtx.pVirMem + dwResTableTypeOffset, 4);
			CopyMemory(&resTableTypeElement.resTableType.resConfig.imsi, mCtx.pVirMem + dwResTableTypeOffset+4, 4);
			CopyMemory(&resTableTypeElement.resTableType.resConfig.locale, mCtx.pVirMem + dwResTableTypeOffset+8, 4);
			CopyMemory(&resTableTypeElement.resTableType.resConfig.screenType, mCtx.pVirMem + dwResTableTypeOffset+0xc, 4);
			CopyMemory(&resTableTypeElement.resTableType.resConfig.input, mCtx.pVirMem + dwResTableTypeOffset+0x10, 4);
			CopyMemory(&resTableTypeElement.resTableType.resConfig.screenSize, mCtx.pVirMem + dwResTableTypeOffset+0x14, 4);
			CopyMemory(&resTableTypeElement.resTableType.resConfig.version, mCtx.pVirMem + dwResTableTypeOffset+0x18, 4);
			CopyMemory(&resTableTypeElement.resTableType.resConfig.screenConfig, mCtx.pVirMem + dwResTableTypeOffset+0x1c, 4);
			CopyMemory(&resTableTypeElement.resTableType.resConfig.screenSizeDp, mCtx.pVirMem + dwResTableTypeOffset+0x20, 4);
			//CopyMemory(&resTableTypeElement.resTableType.resConfig.localeScript, mCtx.pVirMem + dwResTableTypeOffset+0x24, 4);
			//CopyMemory(&resTableTypeElement.resTableType.resConfig.localeVariant, mCtx.pVirMem + dwResTableTypeOffset, 8);
			dwResTableTypeOffset += resTableTypeElement.resTableType.resConfig.size;

			//未知数组
			resTableTypeElement.entryOffsets.clear();
			for (int i = 0; i < resTableTypeElement.resTableType.entryCount; i++)
			{
				int value = 0;
				CopyMemory(&value, mCtx.pVirMem + dwResTableTypeOffset, 4);
				dwResTableTypeOffset += 4;
				resTableTypeElement.entryOffsets.push_back(value);
			}
			//解析entrys
			resTableTypeElement.clear();
			
			for (int i = 0; i < resTableTypeElement.resTableType.entryCount; i++)
			{
				if (resTableTypeElement.entryOffsets[i] == 0xFFFFFFFF) continue;
				unsigned int dwEntryStart = dwResTableTypeOffset + resTableTypeElement.entryOffsets[i];
				short flag = 0;
				CopyMemory(&flag, mCtx.pVirMem + dwEntryStart + 2, 2);
				if (flag == 0)
				{
					ResTableEntry *entry = new ResTableEntry();
					entry->entryId = i;
					CopyMemory(&entry->size, mCtx.pVirMem + dwEntryStart, 2);
					dwEntryStart += 2;
					CopyMemory(&entry->flags, mCtx.pVirMem + dwEntryStart, 2);
					dwEntryStart += 2;
					CopyMemory(&entry->key.index, mCtx.pVirMem + dwEntryStart, 4);
					dwEntryStart += 4;
					//开始解析ResValue
					CopyMemory(&entry->resValue.size, mCtx.pVirMem + dwEntryStart, 2);
					dwEntryStart += 2;
					CopyMemory(&entry->resValue.res0, mCtx.pVirMem + dwEntryStart, 1);
					dwEntryStart += 1;
					CopyMemory(&entry->resValue.dataType, mCtx.pVirMem + dwEntryStart, 1);
					dwEntryStart += 1;
					CopyMemory(&entry->resValue.data, mCtx.pVirMem + dwEntryStart, 4);
					dwEntryStart += 4;

					resTableTypeElement.add(entry);
				}
				else if (flag == 1)
				{
					ResTableMapEntry *entry = new ResTableMapEntry();
					entry->entryId = i;
					CopyMemory(&entry->size, mCtx.pVirMem + dwEntryStart, 2);
					dwEntryStart += 2;
					CopyMemory(&entry->flags, mCtx.pVirMem + dwEntryStart, 2);
					dwEntryStart += 2;
					CopyMemory(&entry->key.index, mCtx.pVirMem + dwEntryStart, 4);
					dwEntryStart += 4;
					CopyMemory(&entry->parent.ident, mCtx.pVirMem + dwEntryStart, 4);
					dwEntryStart += 4;
					CopyMemory(&entry->count, mCtx.pVirMem + dwEntryStart, 4);
					dwEntryStart += 4;
					//解析ResTableMap
					entry->resTableMaps.clear();
					for (int k = 0; k < entry->count; k++)
					{
						ResTableMap resTableMap;
						CopyMemory(&resTableMap.index, mCtx.pVirMem + dwEntryStart, 2);
						dwEntryStart += 2;
						CopyMemory(&resTableMap.res0, mCtx.pVirMem + dwEntryStart, 2);
						dwEntryStart += 2;
						CopyMemory(&resTableMap.value.size, mCtx.pVirMem + dwEntryStart, 2);
						dwEntryStart += 2;
						CopyMemory(&resTableMap.value.res0, mCtx.pVirMem + dwEntryStart, 1);
						dwEntryStart += 1;
						CopyMemory(&resTableMap.value.dataType, mCtx.pVirMem + dwEntryStart, 1);
						dwEntryStart += 1;
						CopyMemory(&resTableMap.value.data, mCtx.pVirMem + dwEntryStart, 4);
						dwEntryStart += 4;
						entry->resTableMaps.push_back(resTableMap);
					}
					resTableTypeElement.add(entry);
				}
				else
				{
					ResTableEntry *entry = new ResTableEntry();
					entry->entryId = i;
					CopyMemory(&entry->size, mCtx.pVirMem + dwEntryStart, 2);
					dwEntryStart += 2;
					CopyMemory(&entry->flags, mCtx.pVirMem + dwEntryStart, 2);
					dwEntryStart += 2;
					CopyMemory(&entry->key.index, mCtx.pVirMem + dwEntryStart, 4);
					dwEntryStart += 4;
					//开始解析ResValue
					CopyMemory(&entry->resValue.size, mCtx.pVirMem + dwEntryStart, 2);
					dwEntryStart += 2;
					CopyMemory(&entry->resValue.res0, mCtx.pVirMem + dwEntryStart, 1);
					dwEntryStart += 1;
					CopyMemory(&entry->resValue.dataType, mCtx.pVirMem + dwEntryStart, 1);
					dwEntryStart += 1;
					CopyMemory(&entry->resValue.data, mCtx.pVirMem + dwEntryStart, 4);
					dwEntryStart += 4;

					resTableTypeElement.add(entry);
				}
			}
			dwOffset += resTableTypeElement.resTableType.dwChunkSize;
			resResource.resTableTypes.push_back(resTableTypeElement);
		}
		resTablePackageChunk->resResources.push_back(resResource);
	}
}

unsigned int CArscAnalyse::decodeLength(const unsigned char * str, unsigned int &LenWide, unsigned int flag)
{
	unsigned int len = str[0];

	if (flag == 0x100)
	{
		if ((len & 0x80) != 0) {
			//双
			len = str[2];
			if ((len & 0x80) != 0){
				//双双
				len = ((len & 0x7F) << 8) | str[3];
				LenWide = 4;
			}
			else
			{
				//双单
				LenWide = 3;
			}
		}
		else
		{
			//单
			len = str[1];
			if ((len & 0x80) != 0){
				//单双
				len = ((len & 0x7F) << 8) | str[2];
				LenWide = 3;
			}
			else
			{
				//单单
				LenWide = 2;
			}

		}
	}
	else if (flag == 0x000)
	{
		//if ((len & 0x8000) != 0) {
		//	len = ((len & 0x7FFF) << 16) | str[1];
		//}
		AfxMessageBox(L"unicode string");
	}
	return len;
}

void CArscAnalyse::UnicodeToUtf8(const std::wstring &wstr, std::string &str)
{
	int len;
	len = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
	char *szUtf8 = (char*)malloc(len + 1);
	memset(szUtf8, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, szUtf8, len, NULL, NULL);
	
	str.clear();
	str.append(szUtf8, len);
}

void CArscAnalyse::Utf8ToUnicode(const std::string &szU8, std::wstring &wstr)
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

std::wstring CArscAnalyse::doReport()
{
	std::wstring strReport = L"";
	if (!isAnalysised) return strReport;

	std::wostringstream   ostr;
	ostr << L"Package Count:" << mCtx.arsc.resTableChunk.packageCount << L"\r\n";

	for (int i = 0; i < mCtx.arsc.resTableChunk.packageCount;i++)
	{
		ostr << L"Package " << i << L"\r\n";
		for (int j = 0; j < mCtx.arsc.chunkData.size(); j++)
		{
			ResChunkBase *p = mCtx.arsc.chunkData[j];
			switch (p->wChunkType)
			{
				case RES_NULL_TYPE:
				{

				}break;
				case RES_STRING_POOL_TYPE:
				{
					ostr << L"RES_STRING_POOL_TYPE" << L"\r\n";
					ResStringPoolChunk *resStringPoolChunk = static_cast<ResStringPoolChunk*>(p);
					ostr << L"stringCount=" << resStringPoolChunk->stringCount << L"\r\n";
					//for (int k = 0; k < resStringPoolChunk->stringCount; k++)
					//{
					//	ostr << resStringPoolChunk->strings[k] << L"\r\n";
					//}
				}break;
				case RES_TABLE_PACKAGE_TYPE:
				{
					ostr << L"RES_TABLE_TYPE" << L"\r\n";
					ResTablePackageChunk *resTablePackageChunk = static_cast<ResTablePackageChunk*>(p);
					ostr << L"Package id=" << resTablePackageChunk->id;
					ostr << L" name=" << resTablePackageChunk->name;
					ostr << L" typeCount=" << resTablePackageChunk->resStringPoolType.stringCount;
					ostr << L" keyCount=" << resTablePackageChunk->resStringPoolKey.stringCount << L"\r\n";
					for (int k = 0; k < resTablePackageChunk->resResources.size(); k++)
					{
						ostr << L"  ";
						ostr << L"type " << resTablePackageChunk->resResources[k].resTableTypeSpec.id-1;
						ostr << L"(" << resTablePackageChunk->resStringPoolType.strings[resTablePackageChunk->resResources[k].resTableTypeSpec.id-1] << ")";
						ostr << L" configCount=" << resTablePackageChunk->resResources[k].resTableTypes.size();
						ostr << L" entryCount " << resTablePackageChunk->resResources[k].resTableTypeSpec.entryCount<<L"\r\n";
						for (int m = 0; m < resTablePackageChunk->resResources[k].resTableTypes.size(); m++)
						{
							ostr << L"    ";
							ostr << L"config=" << m;
							ostr << L" count=" <<resTablePackageChunk->resResources[k].resTableTypes[m].resTableEntrys.size();
							ostr << L" density=" << resTablePackageChunk->resResources[k].resTableTypes[m].resTableType.resConfig.density;
							ostr << L" version=" << resTablePackageChunk->resResources[k].resTableTypes[m].resTableType.resConfig.sdkVersion;
							ostr << L"\r\n";
							for (int n = 0; n < resTablePackageChunk->resResources[k].resTableTypes[m].resTableEntrys.size(); n++)
							{
								ostr << L"      ";
								unsigned int packId = resTablePackageChunk->id;
								unsigned int resTypeId = resTablePackageChunk->resResources[k].resTableTypeSpec.id;
								unsigned int entryId = resTablePackageChunk->resResources[k].resTableTypes[m].resTableEntrys[n]->entryId;
								unsigned int resid = (packId << 0x18) | ((resTypeId & 0xFF) << 0x10) | entryId&0xFFFF;
								ostr << L"resource 0x" << std::hex << resid;
								ostr.unsetf(std::wostringstream::hex);
								ostr << L" key =" << resTablePackageChunk->resStringPoolKey.strings[resTablePackageChunk->resResources[k].resTableTypes[m].resTableEntrys[n]->key.index];
								ostr << L" datatype =" << resTablePackageChunk->resResources[k].resTableTypes[m].resTableEntrys[n]->resValue.getTypeStr();
								ostr << L" data =" << resTablePackageChunk->resResources[k].resTableTypes[m].resTableEntrys[n]->resValue.data;
								ostr << L"\r\n";
							}
						}
					}

				}break;
				case RES_XML_TYPE:
				{

				}break;
			}

		}
	}

	strReport = ostr.str();
	return strReport;
}

bool CArscAnalyse::arscUnload()
{
	VirtualUnlock(mCtx.pVirMem, mCtx.size);
	VirtualFree(mCtx.pVirMem, mCtx.size, MEM_RELEASE);
	return true;
}