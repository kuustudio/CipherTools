#pragma once
#include<string>
#include"libdex\SysUtil.h"
#include"libdex\DexFile.h"
#include"libdex\DexClass.h"
#include"libdex\InstrUtils.h"


typedef struct {
	std::wstring path;
	MemMapping map;
	DexFile *pDexFile;
} dex_ctx_t;

/*
* Flag for use with createAccessFlagStr().
*/
typedef enum AccessFor {
	kAccessForClass = 0, kAccessForMethod = 1, kAccessForField = 2,
	kAccessForMAX
} AccessFor;

typedef enum OutputFormat {
	OUTPUT_PLAIN = 0,               /* default */
	OUTPUT_XML,                     /* fancy */
} OutputFormat;

/* command-line options */
typedef struct{
	bool checksumOnly;
	bool disassemble;
	bool showFileHeaders;
	bool showSectionHeaders;
	bool ignoreBadChecksum;
	bool dumpRegisterMaps;
	OutputFormat outputFormat;
	std::wstring tempFileName;
	bool exportsOnly;
	bool verbose;
} CmdOptions;

typedef struct FieldMethodInfo {
	const char* classDescriptor;
	const char* name;
	const char* signature;
} FieldMethodInfo;

class CDexAnalyse
{
public:
	CDexAnalyse();
	~CDexAnalyse();
public:
	dex_ctx_t	mCtx;
	CmdOptions gOptions;
	bool		isAnalysised;
	InstructionWidth* gInstrWidth;
	InstructionFormat* gInstrFormat;
public:
	//初始化
	bool dexLoadFile(const wchar_t *lpPath, const wchar_t *lpModel);
	bool dexUnload();
public://分析函数
	const char* primitiveTypeLabel(char typeChar);
	char* descriptorToDot(const char* str);
	char* descriptorClassToDot(const char* str);
	const char* quotedBool(bool val);
	const char* quotedVisibility(u4 accessFlags);
	int countOnes(u4 val);
	char* createAccessFlagStr(u4 flags, AccessFor forWhat);
	std::wstring dumpFileHeader(const DexFile* pDexFile);
	std::wstring dumpClassDef(DexFile* pDexFile, int idx);
	std::wstring dumpInterface(const DexFile* pDexFile, const DexTypeItem* pTypeItem,int i);
	void dumpCatches(DexFile* pDexFile, const DexCode* pCode);
	void dumpPositions(DexFile* pDexFile, const DexCode* pCode,const DexMethod *pDexMethod);
	void dumpLocals(DexFile* pDexFile, const DexCode* pCode,const DexMethod *pDexMethod);
	bool getMethodInfo(DexFile* pDexFile, u4 methodIdx, FieldMethodInfo* pMethInfo);
	bool getFieldInfo(DexFile* pDexFile, u4 fieldIdx, FieldMethodInfo* pFieldInfo);
	const char* getClassDescriptor(DexFile* pDexFile, u4 classIdx);
	void dumpInstruction(DexFile* pDexFile, const DexCode* pCode, int insnIdx,int insnWidth, const DecodedInstruction* pDecInsn);
	void dumpBytecodes(DexFile* pDexFile, const DexMethod* pDexMethod);
	std::wstring dumpCode(DexFile* pDexFile, const DexMethod* pDexMethod);
	std::wstring dumpMethod(DexFile* pDexFile, const DexMethod* pDexMethod, int i);
	std::wstring dumpSField(const DexFile* pDexFile, const DexField* pSField, int i);
	std::wstring dumpIField(const DexFile* pDexFile, const DexField* pIField, int i);
	std::wstring dumpClass(DexFile* pDexFile, int idx, char** pLastPackage);
	void dumpDifferentialCompressedMap(const u1** pData);
	void dumpMethodMap(DexFile* pDexFile, const DexMethod* pDexMethod, int idx,const u1** pData);
	void dumpRegisterMaps(DexFile* pDexFile);
public:
	bool	Analysis();
	void Utf8ToUnicode(const std::string &str, std::wstring &wstr);
	void UnicodeToUtf8(const std::wstring &wstr, std::string &str);
	std::wstring HexToStr(BYTE *pbDest, int nLen);
	std::wstring StrToHex(BYTE *pbDest, int nLen);
	std::wstring doReport();
public:
	inline u2 get2LE(unsigned char const* pSrc)
	{
		return pSrc[0] | (pSrc[1] << 8);
	}
	inline u4 get4LE(unsigned char const* pSrc)
	{
		return pSrc[0] | (pSrc[1] << 8) | (pSrc[2] << 16) | (pSrc[3] << 24);
	}
	inline const u1* align32(const u1* ptr)
	{
		return (u1*)(((int)ptr + 3) & ~0x03);
	}
};

