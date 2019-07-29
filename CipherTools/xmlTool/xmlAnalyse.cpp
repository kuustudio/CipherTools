#include "stdafx.h"
#include "xmlAnalyse.h"
#include"StartNamespaceChunk.h"
#include"EndNamespaceChunk.h"
#include"StartTagChunk.h"
#include"EndTagChunk.h"
#include"TextChunk.h"


CXmlAnalyse::CXmlAnalyse()
{
	isAnalysised = false;
}


CXmlAnalyse::~CXmlAnalyse()
{
}

bool CXmlAnalyse::xmlLoadFile(const wchar_t *lpPath, const wchar_t *lpModel)
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
	//读取头部位
	CopyMemory(&mCtx.xml.magic, mCtx.pVirMem + offset, sizeof(mCtx.xml.magic));
	offset += sizeof(mCtx.xml.magic);
	CopyMemory(&mCtx.xml.filesize, mCtx.pVirMem + offset, sizeof(mCtx.xml.filesize));

	//根据大小判断文件合法性
	if (mCtx.xml.filesize != mCtx.size)
		return false;

	return true;
}

bool CXmlAnalyse::Analysis()
{
	//开始解析thunk
	mCtx.xml.chunkData.clear();

	unsigned int dwStartChunkOffset = sizeof(mCtx.xml.magic) + sizeof(mCtx.xml.filesize);
	unsigned int dwOffset = dwStartChunkOffset;
	
	//读取StringChunk
	CopyMemory(&mCtx.xml.stringChunk, mCtx.pVirMem + dwOffset, 4 * 7);
	dwOffset += 4 * 7;
	unsigned int stringContentStart = dwStartChunkOffset + mCtx.xml.stringChunk.dwStringPoolOffset;
	for (int i = 0; i < mCtx.xml.stringChunk.dwStringCount; i++)
	{
		unsigned int key = 0;
		CopyMemory(&key, mCtx.pVirMem + dwOffset, 4);
		dwOffset += 4;
		unsigned int stringlen = 0;
		CopyMemory(&stringlen, mCtx.pVirMem + stringContentStart + key, 2);
		wchar_t *buffer = new wchar_t[stringlen];
		CopyMemory(buffer, mCtx.pVirMem + stringContentStart + key + 2, stringlen * 2);//复制utf-16
		std::wstring value = L"";
		value.append(buffer, stringlen);
		delete[]buffer;
		buffer = NULL;
		mCtx.xml.stringChunk.StringOffsets.insert(std::pair<unsigned int, std::wstring>(i, value));
	}
	dwStartChunkOffset += mCtx.xml.stringChunk.dwChunkSize;
	dwOffset = dwStartChunkOffset;

	//读取resourceIdChunk
	CopyMemory(&mCtx.xml.resourceIdChunk, mCtx.pVirMem + dwOffset, 4 * 2);
	dwOffset += 4 * 2;
	unsigned int count = (mCtx.xml.resourceIdChunk.dwChunkSize - 8) / 4;
	for (int i = 0; i < count; i++)
	{
		unsigned int id = 0;
		CopyMemory(&id, mCtx.pVirMem + dwOffset, sizeof(id));
		dwOffset += 4;
		mCtx.xml.resourceIdChunk.ResourceIds.push_back(id);
	}
	dwStartChunkOffset += mCtx.xml.resourceIdChunk.dwChunkSize;
	dwOffset = dwStartChunkOffset;

	//读取xmlContentChunk
	while (dwStartChunkOffset<mCtx.size)
	{
		unsigned int dwChunkType = 0;
		unsigned int dwChunkSize = 0;
		dwOffset = dwStartChunkOffset;
		CopyMemory(&dwChunkType, mCtx.pVirMem + dwOffset, sizeof(dwChunkType));
		CopyMemory(&dwChunkSize, mCtx.pVirMem + dwOffset + 4, sizeof(dwChunkType));
		switch (dwChunkType)
		{
			case StartNamespaceChunkType:
			{
				StartNamespaceChunk	*startNamespaceChunk = new StartNamespaceChunk();
				CopyMemory(&startNamespaceChunk->dwChunkType, mCtx.pVirMem + dwOffset, 4);
				dwOffset += 4;
				CopyMemory(&startNamespaceChunk->dwChunkSize, mCtx.pVirMem + dwOffset, 4);
				dwOffset += 4;
				CopyMemory(&startNamespaceChunk->dwLineNumber, mCtx.pVirMem + dwOffset, 4);
				dwOffset += 4;
				CopyMemory(&startNamespaceChunk->dwUnknown1, mCtx.pVirMem + dwOffset, 4);
				dwOffset += 4;
				unsigned int dwPrefix = 0;
				CopyMemory(&dwPrefix, mCtx.pVirMem + dwOffset, sizeof(dwPrefix));
				dwOffset += 4;
				unsigned int dwUri = 0;
				CopyMemory(&dwUri, mCtx.pVirMem + dwOffset, sizeof(dwUri));
				startNamespaceChunk->strPrefix = mCtx.xml.stringChunk.StringOffsets[dwPrefix];
				startNamespaceChunk->strUri = mCtx.xml.stringChunk.StringOffsets[dwUri];
				mCtx.xml.chunkData.add(startNamespaceChunk);
			}break;
			case EndNamespaceChunkType:
			{
				EndNamespaceChunk *endNamespaceChunk = new EndNamespaceChunk();
				CopyMemory(&endNamespaceChunk->dwChunkType, mCtx.pVirMem + dwOffset, 4);
				dwOffset += 4;
				CopyMemory(&endNamespaceChunk->dwChunkSize, mCtx.pVirMem + dwOffset, 4);
				dwOffset += 4;
				CopyMemory(&endNamespaceChunk->dwLineNumber, mCtx.pVirMem + dwOffset, 4);
				dwOffset += 4;
				CopyMemory(&endNamespaceChunk->dwUnknown1, mCtx.pVirMem + dwOffset, 4);
				dwOffset += 4;
				unsigned int dwPrefix = 0;
				CopyMemory(&dwPrefix, mCtx.pVirMem + dwOffset, sizeof(dwPrefix));
				dwOffset += 4;
				unsigned int dwUri = 0;
				CopyMemory(&dwUri, mCtx.pVirMem + dwOffset, sizeof(dwUri));
				endNamespaceChunk->strPrefix = mCtx.xml.stringChunk.StringOffsets[dwPrefix];
				endNamespaceChunk->strUri = mCtx.xml.stringChunk.StringOffsets[dwUri];
				mCtx.xml.chunkData.add(endNamespaceChunk);
			}break;
			case StartTagChunkType:
			{
				StartTagChunk *startTagChunk = new StartTagChunk();
				CopyMemory(&startTagChunk->dwChunkType, mCtx.pVirMem + dwOffset, 4);
				dwOffset += 4;
				CopyMemory(&startTagChunk->dwChunkSize, mCtx.pVirMem + dwOffset, 4);
				dwOffset += 4;
				CopyMemory(&startTagChunk->dwLineNumber, mCtx.pVirMem + dwOffset, 4);
				dwOffset += 4;
				CopyMemory(&startTagChunk->dwUnknown1, mCtx.pVirMem + dwOffset, 4);
				dwOffset += 4;
				CopyMemory(&startTagChunk->dwNamespaceUri, mCtx.pVirMem + dwOffset, 4);
				dwOffset += 4;
				unsigned int dwName = 0;
				CopyMemory(&dwName, mCtx.pVirMem + dwOffset, sizeof(dwName));
				dwOffset += 4;
				startTagChunk->strName = mCtx.xml.stringChunk.StringOffsets[dwName];
				CopyMemory(&startTagChunk->dwFlags, mCtx.pVirMem + dwOffset, 4 );
				dwOffset += 4;
				CopyMemory(&startTagChunk->dwAttributeCount, mCtx.pVirMem + dwOffset, 4);
				dwOffset += 4;
				CopyMemory(&startTagChunk->dwClassAttribute, mCtx.pVirMem + dwOffset, 4);
				dwOffset += 4;
				//解析Entry
				for (int i = 0; i < startTagChunk->dwAttributeCount; i++)
				{
					unsigned int attributes[5] = { 0 };
					CopyMemory(attributes, mCtx.pVirMem + dwOffset, 4 * 5);
					dwOffset += 4 * 5;
					AttributeEntry entry;
					if (attributes[0] != -1)
						entry.namespaceUri = mCtx.xml.stringChunk.StringOffsets[attributes[0]];
					else
						entry.namespaceUri = L"null";
					if (attributes[1] != -1)
						entry.name = mCtx.xml.stringChunk.StringOffsets[attributes[1]];
					else
						entry.name = L"null";
					if (attributes[2] != -1)
						entry.valueString = mCtx.xml.stringChunk.StringOffsets[attributes[2]];
					else
						entry.valueString = L"null";
					attributes[3] >>= 24;
					entry.type = getAttrType(attributes[3]);
					entry.data = getAttributeData(attributes[3], attributes[4]);
					startTagChunk->Attributes.push_back(entry);
				}
				mCtx.xml.chunkData.add(startTagChunk);
			}break;
			case EndTagChunkType:
			{
				EndTagChunk *endTagChunk = new EndTagChunk();
				CopyMemory(&endTagChunk->dwChunkType, mCtx.pVirMem + dwOffset, 4);
				dwOffset += 4;
				CopyMemory(&endTagChunk->dwChunkSize, mCtx.pVirMem + dwOffset, 4);
				dwOffset += 4;
				CopyMemory(&endTagChunk->dwLineNumber, mCtx.pVirMem + dwOffset, 4);
				dwOffset += 4;
				CopyMemory(&endTagChunk->dwUnknown1, mCtx.pVirMem + dwOffset, 4);
				dwOffset += 4;
				CopyMemory(&endTagChunk->dwNamespaceUri, mCtx.pVirMem + dwOffset, 4);
				dwOffset += 4;
				unsigned int dwName = 0;
				CopyMemory(&dwName, mCtx.pVirMem + dwOffset, sizeof(dwName));
				dwOffset += 4;
				endTagChunk->strName = mCtx.xml.stringChunk.StringOffsets[dwName];
				mCtx.xml.chunkData.add(endTagChunk);
			}break;
			case TextChunkType:
			{
				TextChunk *textChunk = new TextChunk();
				CopyMemory(&textChunk->dwChunkType, mCtx.pVirMem + dwOffset, 4);
				dwOffset += 4;
				CopyMemory(&textChunk->dwChunkSize, mCtx.pVirMem + dwOffset, 4);
				dwOffset += 4;
				CopyMemory(&textChunk->dwLineNumber, mCtx.pVirMem + dwOffset, 4);
				dwOffset += 4;
				CopyMemory(&textChunk->dwUnknown1, mCtx.pVirMem + dwOffset, 4);
				dwOffset += 4;
				unsigned int dwName = 0;
				CopyMemory(&dwName, mCtx.pVirMem + dwOffset, sizeof(dwName));
				dwOffset += 4;
				textChunk->strName = mCtx.xml.stringChunk.StringOffsets[dwName];
				CopyMemory(&textChunk->dwUnknown2, mCtx.pVirMem + dwOffset, 4);
				dwOffset += 4;
				CopyMemory(&textChunk->dwUnknown3, mCtx.pVirMem + dwOffset, 4);
				dwOffset += 4;
				mCtx.xml.chunkData.add(textChunk);
			}break;
		}
		//修正数据
		dwStartChunkOffset += dwChunkSize;
	}
	return true;
}

std::wstring CXmlAnalyse::getAttrType(unsigned int type)
{
	std::wstring strResult = L"";
	switch (type)
	{
	case ATTR_NULL:
		strResult = L"ATTR_NULL";
		break;
	case ATTR_REFERENCE:
		strResult = L"ATTR_REFERENCE";
		break;
	case ATTR_ATTRIBUTE:
		strResult = L"ATTR_ATTRIBUTE";
		break;
	case ATTR_STRING:
		strResult = L"ATTR_STRING";
		break;
	case ATTR_FLOAT:
		strResult = L"ATTR_FLOAT";
		break;
	case ATTR_DIMENSION:
		strResult = L"ATTR_DIMENSION";
		break;
	case ATTR_FRACTION:
		strResult = L"ATTR_FRACTION";
		break;
	case ATTR_FIRSTINT:
		strResult = L"ATTR_FIRSTINT";
		break;
	//case ATTR_DEC:
	//	strResult = L"ATTR_DEC";
	//	break;
	case ATTR_HEX:
		strResult = L"ATTR_HEX";
		break;
	case ATTR_BOOLEAN:
		strResult = L"ATTR_BOOLEAN";
		break;
	case ATTR_FIRSTCOLOR:
		strResult = L"ATTR_FIRSTCOLOR";
		break;
	//case ATTR_ARGB8:
	//	strResult = L"ATTR_ARGB8";
	//	break;
	case ATTR_RGB8:
		strResult = L"ATTR_RGB8";
		break;
	case ATTR_ARGB4:
		strResult = L"ATTR_ARGB4";
		break;
	case ATTR_RGB4:
		strResult = L"ATTR_RGB4";
		break;
	//case ATTR_LASTCOLOR:
	//	strResult = L"ATTR_LASTCOLOR";
	//	break;
	//case ATTR_LASTINT:
	//	strResult = L"ATTR_LASTINT";
	//	break;
	default:
		strResult = L"unknown";
		break;
	}

	return strResult;
}

std::wstring CXmlAnalyse::getAttributeData(unsigned int type,unsigned int data)
{
	float RadixTable[] = { 0.00390625f, 3.051758E-005f, 1.192093E-007f, 4.656613E-010f };
	char *DimemsionTable[] = { "px", "dip", "sp", "pt", "in", "mm", "", "" };
	char *FractionTable[] = { "%", "%p", "", "", "", "", "", "" };

	std::wstring strResult = L"";
	wchar_t *buf = (wchar_t *)malloc(32);

	if (type >= ATTR_FIRSTCOLOR && type <= ATTR_LASTCOLOR)
	{
		memset(buf, 0, 32);
		_stprintf(buf, L"#%08x", data);
		strResult.append(buf, lstrlenW(buf));
		return strResult;
	}
	else if (type >= ATTR_FIRSTINT && type <= ATTR_LASTINT)
	{
		memset(buf, 0, 32);
		_stprintf(buf, L"%d", data);
		strResult.append(buf, lstrlenW(buf));
		return strResult;
	}

	switch (type)
	{
		case ATTR_STRING:
		{
			strResult = mCtx.xml.stringChunk.StringOffsets[data];
		}break;
		case ATTR_NULL:
			break;
		case ATTR_REFERENCE:
		{
			memset(buf, 0, 32);
			if (data >> 24 == 1)
				_stprintf(buf, L"@android:%08X", data);
			else
				_stprintf(buf, L"@%08X", data);
			strResult.append(buf, lstrlenW(buf));

		}break;
		case ATTR_FLOAT:
		{
			memset(buf, 0, 32);
			_stprintf(buf, L"%g", *(float *)&data);
			strResult.append(buf, lstrlenW(buf));
		}break;
		case ATTR_DIMENSION:
		{
			memset(buf, 0, 32);
			_stprintf(buf,L"%f%s",
				(float)(data & 0xffffff00) * RadixTable[(data >> 4) & 0x03],
				DimemsionTable[data & 0x0f]);
			strResult.append(buf, lstrlenW(buf));

		}break;
		case ATTR_FRACTION:
		{
			memset(buf, 0, 32);
			_stprintf(buf,L"%f%s",
				(float)(data & 0xffffff00) * RadixTable[(data >> 4) & 0x03],
				FractionTable[data & 0x0f]);
			strResult.append(buf, lstrlenW(buf));
		}break;
		case ATTR_HEX:
		{
			memset(buf, 0, 32);
			_stprintf(buf,L"0x%08x", data);
			strResult.append(buf, lstrlenW(buf));
		}break;
		case ATTR_BOOLEAN:
		{
			memset(buf, 0, 32);
			if (data == 0)
				wcscpy(buf, L"false");
			else
				wcscpy(buf, L"true");
			strResult.append(buf, lstrlenW(buf));

		}break;
		default:
		{
			memset(buf, 0, 32);
			_stprintf(buf, L"<0x%x, type 0x%02x>", data, type);
			strResult.append(buf, lstrlenW(buf));
		}break;
	}
	return strResult;
}

std::wstring CXmlAnalyse::createXml()
{
	std::wstring xmlSb = L"";
	xmlSb.append(L"<?xml version=\"1.0\" encoding=\"utf-8\"?>");
	xmlSb.append(L"\r\n");

	std::wstring strPrefix = L"";
	std::wstring strUri = L"";
	for (std::vector<ChunkBase *>::iterator it = mCtx.xml.chunkData.chunks.begin(); it != mCtx.xml.chunkData.chunks.end(); it++)
	{
		if (NULL != *it)
		{
			switch ((*it)->dwChunkType)
			{
				case StartNamespaceChunkType:
				{
					StartNamespaceChunk *p = static_cast<StartNamespaceChunk *>(*it);
					strPrefix = p->strPrefix;
					strUri = p->strUri;
				}break;
				case EndNamespaceChunkType:
				{
					strUri=L"";
				}break;
				case StartTagChunkType:
				{	
					StartTagChunk *p = static_cast<StartTagChunk *>(*it);
					xmlSb.append(L"\r\n<");
					xmlSb.append(p->strName);
					for (std::vector<AttributeEntry>::iterator itor = p->Attributes.begin(); itor != p->Attributes.end(); itor++)
					{
						xmlSb.append(L" ");
						xmlSb.append(strPrefix);
						xmlSb.append(L":");
						xmlSb.append(itor->name);
						xmlSb.append(L"=\"");
						xmlSb.append(itor->data);
						xmlSb.append(L"\"");
					}
					if (p->strName == L"manifest")
					{
						xmlSb.append(L" ");
						xmlSb.append(L"xmlns:");
						xmlSb.append(strPrefix);
						xmlSb.append(L"=\"");
						xmlSb.append(strUri);
						xmlSb.append(L"\"");
					}
					xmlSb.append(L">");
				}break;
				case EndTagChunkType:
				{
					xmlSb.append(L"</");
					EndTagChunk *p = static_cast<EndTagChunk *>(*it);
					xmlSb.append(p->strName);
					xmlSb.append(L">");
				}break;
				case TextChunkType:
				{
					strUri = L"";
				}break;
			}
		}
	}
	return xmlSb;
}

bool CXmlAnalyse::xmlUnload()
{
	VirtualUnlock(mCtx.pVirMem, mCtx.size);
	VirtualFree(mCtx.pVirMem, mCtx.size, MEM_RELEASE);
	return true;
}