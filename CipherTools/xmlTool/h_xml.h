#pragma once
#include<Windows.h>
#include<string>
#include<vector>
#include<map>
#include <memory>
#include"xmlChunkData.h"


#define StringChunkType			0x001c0001
#define ResourceIdChunkType		0x00080180
#define StartNamespaceChunkType	0x00100100
#define EndNamespaceChunkType	0x00100101
#define StartTagChunkType		0x00100102
#define EndTagChunkType			0x00100103
#define TextChunkType			0x00100104


typedef struct {
	unsigned int dwChunkType;
	unsigned int dwChunkSize;
	unsigned int dwStringCount;
	unsigned int dwStyleCount;
	unsigned int dwUnknown;
	unsigned int dwStringPoolOffset;
	unsigned int dwStylePoolOffset;
	std::map<unsigned int, std::wstring> StringOffsets;
	std::map<unsigned int, std::wstring> StyleOffsets;
	//String Pool
	//Style Pool
}StringChunk;

typedef struct {
	unsigned int dwChunkType;
	unsigned int dwChunkSize;
	std::vector<unsigned int> ResourceIds;
}ResourceIdChunk;

enum ATTR_TYPE
{
	ATTR_NULL = 0,
	ATTR_REFERENCE = 1,
	ATTR_ATTRIBUTE = 2,
	ATTR_STRING = 3,
	ATTR_FLOAT = 4,
	ATTR_DIMENSION = 5,
	ATTR_FRACTION = 6,
	ATTR_FIRSTINT = 16,
	ATTR_DEC = 16,
	ATTR_HEX = 17,
	ATTR_BOOLEAN = 18,
	ATTR_FIRSTCOLOR = 28,
	ATTR_ARGB8 = 28,
	ATTR_RGB8 = 29,
	ATTR_ARGB4 = 30,
	ATTR_RGB4 = 31,
	ATTR_LASTCOLOR = 31,
	ATTR_LASTINT = 31,
};

typedef struct {
	std::wstring namespaceUri;
	std::wstring name;
	std::wstring valueString;
	std::wstring type;
	std::wstring data;
}AttributeEntry;


typedef struct{
	unsigned int magic;
	unsigned int filesize;
	StringChunk  stringChunk;
	ResourceIdChunk resourceIdChunk;
	xmlChunkData	chunkData;
} xml_struct;

typedef struct {
	std::wstring path;
	char *pVirMem;
	size_t size;
	xml_struct xml;
} xml_ctx_t;