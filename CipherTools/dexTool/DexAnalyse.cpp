#include "stdafx.h"
#include <sstream>
#include "DexAnalyse.h"
#include"libdex\DexFile.h"
#include"libdex\CmdUtils.h"
#include"libdex\DexClass.h"
#include"OpCodeNames.h"
#include"libdex\DexCatch.h"
#include"libdex\DexProto.h"


CDexAnalyse::CDexAnalyse()
{
	isAnalysised = false;
	gOptions.checksumOnly = false;;
	gOptions.disassemble=false;
	gOptions.showFileHeaders = false;
	gOptions.showSectionHeaders = false;
	gOptions.ignoreBadChecksum = false;
	gOptions.dumpRegisterMaps = false;
	gOptions.outputFormat = OUTPUT_PLAIN;
	gOptions.tempFileName=L"";
	gOptions.exportsOnly = false;
	gOptions.verbose = false;
}

CDexAnalyse::~CDexAnalyse()
{
}

/*
* Converts a single-character primitive type into its human-readable
* equivalent.
*/
const char* CDexAnalyse::primitiveTypeLabel(char typeChar)
{
	switch (typeChar) {
	case 'B':   return "byte";
	case 'C':   return "char";
	case 'D':   return "double";
	case 'F':   return "float";
	case 'I':   return "int";
	case 'J':   return "long";
	case 'S':   return "short";
	case 'V':   return "void";
	case 'Z':   return "boolean";
	default:
		return "UNKNOWN";
	}
}

/*
* Converts a type descriptor to human-readable "dotted" form.  For
* example, "Ljava/lang/String;" becomes "java.lang.String", and
* "[I" becomes "int[]".  Also converts '$' to '.', which means this
* form can't be converted back to a descriptor.
*/
char* CDexAnalyse::descriptorToDot(const char* str)
{
	int targetLen = strlen(str);
	int offset = 0;
	int arrayDepth = 0;
	char* newStr;

	/* strip leading [s; will be added to end */
	while (targetLen > 1 && str[offset] == '[') {
		offset++;
		targetLen--;
	}
	arrayDepth = offset;

	if (targetLen == 1) {
		/* primitive type */
		str = primitiveTypeLabel(str[offset]);
		offset = 0;
		targetLen = strlen(str);
	}
	else {
		/* account for leading 'L' and trailing ';' */
		if (targetLen >= 2 && str[offset] == 'L' &&
			str[offset + targetLen - 1] == ';')
		{
			targetLen -= 2;
			offset++;
		}
	}

	newStr = (char*)malloc(targetLen + arrayDepth * 2 + 1);

	/* copy class name over */
	int i;
	for (i = 0; i < targetLen; i++) {
		char ch = str[offset + i];
		newStr[i] = (ch == '/' || ch == '$') ? '.' : ch;
	}

	/* add the appropriate number of brackets for arrays */
	while (arrayDepth-- > 0) {
		newStr[i++] = '[';
		newStr[i++] = ']';
	}
	newStr[i] = '\0';
	//    assert(i == targetLen + arrayDepth * 2);

	return newStr;
}

/*
* Converts the class name portion of a type descriptor to human-readable
* "dotted" form.
*
* Returns a newly-allocated string.
*/
char* CDexAnalyse::descriptorClassToDot(const char* str)
{
	const char* lastSlash;
	char* newStr;
	char* cp;

	/* reduce to just the class name, trimming trailing ';' */
	lastSlash = strrchr(str, '/');
	if (lastSlash == NULL)
		lastSlash = str + 1;        /* start past 'L' */
	else
		lastSlash++;                /* start past '/' */

	newStr = strdup(lastSlash);
	newStr[strlen(lastSlash) - 1] = '\0';
	for (cp = newStr; *cp != '\0'; cp++) {
		if (*cp == '$')
			*cp = '.';
	}

	return newStr;
}

/*
* Returns a quoted string representing the boolean value.
*/
const char* CDexAnalyse::quotedBool(bool val)
{
	if (val)
		return "\"true\"";
	else
		return "\"false\"";
}

const char* CDexAnalyse::quotedVisibility(u4 accessFlags)
{
	if ((accessFlags & ACC_PUBLIC) != 0)
		return "\"public\"";
	else if ((accessFlags & ACC_PROTECTED) != 0)
		return "\"protected\"";
	else if ((accessFlags & ACC_PRIVATE) != 0)
		return "\"private\"";
	else
		return "\"package\"";
}

/*
* Count the number of '1' bits in a word.
*/
int CDexAnalyse::countOnes(u4 val)
{
	int count = 0;

	val = val - ((val >> 1) & 0x55555555);
	val = (val & 0x33333333) + ((val >> 2) & 0x33333333);
	count = (((val + (val >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;

	return count;
}

/*
* Create a new string with human-readable access flags.
*
* In the base language the access_flags fields are type u2; in Dalvik
* they're u4.
*/
char* CDexAnalyse::createAccessFlagStr(u4 flags, AccessFor forWhat)
{
#define NUM_FLAGS   18
	static const char* kAccessStrings[kAccessForMAX][NUM_FLAGS] = {
		{
			/* class, inner class */
			"PUBLIC",           /* 0x0001 */
			"PRIVATE",          /* 0x0002 */
			"PROTECTED",        /* 0x0004 */
			"STATIC",           /* 0x0008 */
			"FINAL",            /* 0x0010 */
			"?",                /* 0x0020 */
			"?",                /* 0x0040 */
			"?",                /* 0x0080 */
			"?",                /* 0x0100 */
			"INTERFACE",        /* 0x0200 */
			"ABSTRACT",         /* 0x0400 */
			"?",                /* 0x0800 */
			"SYNTHETIC",        /* 0x1000 */
			"ANNOTATION",       /* 0x2000 */
			"ENUM",             /* 0x4000 */
			"?",                /* 0x8000 */
			"VERIFIED",         /* 0x10000 */
			"OPTIMIZED",        /* 0x20000 */
		},
		{
			/* method */
			"PUBLIC",           /* 0x0001 */
			"PRIVATE",          /* 0x0002 */
			"PROTECTED",        /* 0x0004 */
			"STATIC",           /* 0x0008 */
			"FINAL",            /* 0x0010 */
			"SYNCHRONIZED",     /* 0x0020 */
			"BRIDGE",           /* 0x0040 */
			"VARARGS",          /* 0x0080 */
			"NATIVE",           /* 0x0100 */
			"?",                /* 0x0200 */
			"ABSTRACT",         /* 0x0400 */
			"STRICT",           /* 0x0800 */
			"SYNTHETIC",        /* 0x1000 */
			"?",                /* 0x2000 */
			"?",                /* 0x4000 */
			"MIRANDA",          /* 0x8000 */
			"CONSTRUCTOR",      /* 0x10000 */
			"DECLARED_SYNCHRONIZED", /* 0x20000 */
		},
		{
			/* field */
			"PUBLIC",           /* 0x0001 */
			"PRIVATE",          /* 0x0002 */
			"PROTECTED",        /* 0x0004 */
			"STATIC",           /* 0x0008 */
			"FINAL",            /* 0x0010 */
			"?",                /* 0x0020 */
			"VOLATILE",         /* 0x0040 */
			"TRANSIENT",        /* 0x0080 */
			"?",                /* 0x0100 */
			"?",                /* 0x0200 */
			"?",                /* 0x0400 */
			"?",                /* 0x0800 */
			"SYNTHETIC",        /* 0x1000 */
			"?",                /* 0x2000 */
			"ENUM",             /* 0x4000 */
			"?",                /* 0x8000 */
			"?",                /* 0x10000 */
			"?",                /* 0x20000 */
		},
	};
	const int kLongest = 21;        /* strlen of longest string above */
	int i, count;
	char* str;
	char* cp;

	/*
	* Allocate enough storage to hold the expected number of strings,
	* plus a space between each.  We over-allocate, using the longest
	* string above as the base metric.
	*/
	count = countOnes(flags);
	cp = str = (char*)malloc(count * (kLongest + 1) + 1);

	for (i = 0; i < NUM_FLAGS; i++) {
		if (flags & 0x01) {
			const char* accessStr = kAccessStrings[forWhat][i];
			int len = strlen(accessStr);
			if (cp != str)
				*cp++ = ' ';

			memcpy(cp, accessStr, len);
			cp += len;
		}
		flags >>= 1;
	}
	*cp = '\0';

	return str;
}


/*
* Dump the file header.
*/
std::wstring CDexAnalyse::dumpFileHeader(const DexFile* pDexFile)
{
	std::wstring strR = L"";
	const DexHeader* pHeader = pDexFile->pHeader;

	std::wostringstream   ostr;
	ostr << L"****************************************************************************" << L"\r\n";
	ostr << L"DEX file header:" << L"\r\n";
	ostr << L"magic\t\t:" << (char*)pHeader->magic << "(" << HexToStr((BYTE*)pHeader->magic, sizeof(pHeader->magic)) << L")\r\n";
	ostr << L"checksum\t\t:0x" << std::hex << pHeader->checksum << L"\r\n";
	//ostr.unsetf(std::wostringstream::hex);
	ostr << L"signature\t\t:" << HexToStr((BYTE*)pHeader->signature, sizeof(pHeader->signature)) << L"\r\n";
	ostr << L"file_size\t\t:0x" << pHeader->fileSize << L"\r\n";
	ostr << L"header_size\t:0x" << pHeader->headerSize << L"\r\n";
	ostr << L"link_size\t\t:0x" << pHeader->linkSize << L"\r\n";
	ostr << L"link_off\t\t:0x" << pHeader->linkOff << L"\r\n";
	ostr << L"string_ids_size\t:0x" << pHeader->stringIdsSize << L"\r\n";
	ostr << L"string_ids_off\t:0x" << pHeader->stringIdsOff << L"\r\n";
	ostr << L"type_ids_size\t:0x" << pHeader->typeIdsSize << L"\r\n";
	ostr << L"type_ids_off\t:0x" << pHeader->typeIdsOff << L"\r\n";
	ostr << L"field_ids_size\t:0x" << pHeader->fieldIdsSize << L"\r\n";
	ostr << L"field_ids_off\t:0x" << pHeader->fieldIdsOff << L"\r\n";
	ostr << L"method_ids_size\t:0x" << pHeader->methodIdsSize << L"\r\n";
	ostr << L"method_ids_off\t:0x" << pHeader->methodIdsOff << L"\r\n";
	ostr << L"class_defs_size\t:0x" << pHeader->classDefsSize << L"\r\n";
	ostr << L"class_defs_off\t:0x" << pHeader->classDefsOff << L"\r\n";
	ostr << L"data_size\t\t:0x" << pHeader->dataSize << L"\r\n";
	ostr << L"data_off\t\t:0x" << pHeader->dataOff << L"\r\n";
	ostr.unsetf(std::wostringstream::hex);
	ostr << L"****************************************************************************" << L"\r\n";
	strR = ostr.str();
	return strR;
}

/*
* Dump a class_def_item.
*/
std::wstring CDexAnalyse::dumpClassDef(DexFile* pDexFile, int idx)
{
	std::wstring strR = L"";
	std::wostringstream   ostr;
	ostr << L"****************************************************************************" << L"\r\n";

	const DexClassDef* pClassDef;
	const u1* pEncodedData;
	DexClassData* pClassData;

	pClassDef = dexGetClassDef(pDexFile, idx);
	pEncodedData = dexGetClassData(pDexFile, pClassDef);
	pClassData = dexReadAndVerifyClassData(&pEncodedData, NULL);

	if (pClassData == NULL) {
		ostr << "Trouble reading class data" << L"\r\n";
		return L"";
	}
	ostr << "Class #" << idx << " header:" << L"\r\n";
	ostr << "class_idx\t\t:" << pClassDef->classIdx << L"\r\n";
	ostr << "access_flags\t\t:" << pClassDef->accessFlags << L"\r\n";
	ostr << "superclass_idx\t\t:" << pClassDef->superclassIdx << L"\r\n";
	ostr << "interfaces_off\t\t:" << pClassDef->interfacesOff << L"\r\n";
	ostr << "source_file_idx\t\t:" << pClassDef->sourceFileIdx << L"\r\n";
	ostr << "annotations_off\t\t:" << pClassDef->annotationsOff << L"\r\n";
	ostr << "class_data_off\t\t:" << pClassDef->classDataOff << L"\r\n";
	ostr << "static_fields_size\t\t:" << pClassData->header.staticFieldsSize << L"\r\n";
	ostr << "instance_fields_size\t\t:" << pClassData->header.instanceFieldsSize << L"\r\n";
	ostr << "direct_methods_size\t\t:" << pClassData->header.directMethodsSize << L"\r\n";
	ostr << "virtual_methods_size\t\t:" << pClassData->header.virtualMethodsSize << L"\r\n";
	ostr << L"****************************************************************************" << L"\r\n";

	free(pClassData);

	strR = ostr.str();
	return strR;
}

/*
* Dump an interface that a class declares to implement.
*/
std::wstring CDexAnalyse::dumpInterface(const DexFile* pDexFile, const DexTypeItem* pTypeItem,
	int i)
{
	std::wstring strR = L"";
	std::wostringstream   ostr;

	const char* interfaceName =
		dexStringByTypeIdx(pDexFile, pTypeItem->typeIdx);

	if (gOptions.outputFormat == OUTPUT_PLAIN) {
		ostr <<L"    #" << i <<L"              :" << interfaceName << L"\r\n";
	}
	else {
		char* dotted = descriptorToDot(interfaceName);
		ostr << "<implements name=\"" << dotted << "\">" << L"\r\n" << "</implements>" << L"\r\n";
		free(dotted);
	}
	strR = ostr.str();
	return strR;
}

/*
* Dump the catches table associated with the code.
*/
void CDexAnalyse::dumpCatches(DexFile* pDexFile, const DexCode* pCode)
{
	u4 triesSize = pCode->triesSize;

	if (triesSize == 0) {
		printf("      catches       : (none)\n");
		return;
	}

	printf("      catches       : %d\n", triesSize);

	const DexTry* pTries = dexGetTries(pCode);
	u4 i;

	for (i = 0; i < triesSize; i++) {
		const DexTry* pTry = &pTries[i];
		u4 start = pTry->startAddr;
		u4 end = start + pTry->insnCount;
		DexCatchIterator iterator;

		printf("        0x%04x - 0x%04x\n", start, end);

		dexCatchIteratorInit(&iterator, pCode, pTry->handlerOff);

		for (;;) {
			DexCatchHandler* handler = dexCatchIteratorNext(&iterator);
			const char* descriptor;

			if (handler == NULL) {
				break;
			}

			descriptor = (handler->typeIdx == kDexNoIndex) ? "<any>" :
				dexStringByTypeIdx(pDexFile, handler->typeIdx);

			printf("          %s -> 0x%04x\n", descriptor,
				handler->address);
		}
	}
}

static int dumpPositionsCb(void *cnxt, u4 address, u4 lineNum)
{
	printf("        0x%04x line=%d\n", address, lineNum);
	return 0;
}

/*
* Dump the positions list.
*/
void CDexAnalyse::dumpPositions(DexFile* pDexFile, const DexCode* pCode,
	const DexMethod *pDexMethod)
{
	printf("      positions     : \n");
	const DexMethodId *pMethodId
		= dexGetMethodId(pDexFile, pDexMethod->methodIdx);
	const char *classDescriptor
		= dexStringByTypeIdx(pDexFile, pMethodId->classIdx);

	dexDecodeDebugInfo(pDexFile, pCode, classDescriptor, pMethodId->protoIdx,
		pDexMethod->accessFlags, dumpPositionsCb, NULL, NULL);
}

static void dumpLocalsCb(void *cnxt, u2 reg, u4 startAddress,
	u4 endAddress, const char *name, const char *descriptor,
	const char *signature)
{
	printf("        0x%04x - 0x%04x reg=%d %s %s %s\n",
		startAddress, endAddress, reg, name, descriptor,
		signature);
}

/*
* Dump the locals list.
*/
void CDexAnalyse::dumpLocals(DexFile* pDexFile, const DexCode* pCode,
	const DexMethod *pDexMethod)
{
	printf("      locals        : \n");

	const DexMethodId *pMethodId
		= dexGetMethodId(pDexFile, pDexMethod->methodIdx);
	const char *classDescriptor
		= dexStringByTypeIdx(pDexFile, pMethodId->classIdx);

	dexDecodeDebugInfo(pDexFile, pCode, classDescriptor, pMethodId->protoIdx,
		pDexMethod->accessFlags, NULL, dumpLocalsCb, NULL);
}

/*
* Get information about a method.
*/
bool CDexAnalyse::getMethodInfo(DexFile* pDexFile, u4 methodIdx, FieldMethodInfo* pMethInfo)
{
	const DexMethodId* pMethodId;

	if (methodIdx >= pDexFile->pHeader->methodIdsSize)
		return false;

	pMethodId = dexGetMethodId(pDexFile, methodIdx);
	pMethInfo->name = dexStringById(pDexFile, pMethodId->nameIdx);
	pMethInfo->signature = dexCopyDescriptorFromMethodId(pDexFile, pMethodId);

	pMethInfo->classDescriptor =
		dexStringByTypeIdx(pDexFile, pMethodId->classIdx);
	return true;
}

/*
* Get information about a field.
*/
bool CDexAnalyse::getFieldInfo(DexFile* pDexFile, u4 fieldIdx, FieldMethodInfo* pFieldInfo)
{
	const DexFieldId* pFieldId;

	if (fieldIdx >= pDexFile->pHeader->fieldIdsSize)
		return false;

	pFieldId = dexGetFieldId(pDexFile, fieldIdx);
	pFieldInfo->name = dexStringById(pDexFile, pFieldId->nameIdx);
	pFieldInfo->signature = dexStringByTypeIdx(pDexFile, pFieldId->typeIdx);
	pFieldInfo->classDescriptor =
		dexStringByTypeIdx(pDexFile, pFieldId->classIdx);
	return true;
}


/*
* Look up a class' descriptor.
*/
const char* CDexAnalyse::getClassDescriptor(DexFile* pDexFile, u4 classIdx)
{
	return dexStringByTypeIdx(pDexFile, classIdx);
}

/*
* Dump a single instruction.
*/
void CDexAnalyse::dumpInstruction(DexFile* pDexFile, const DexCode* pCode, int insnIdx,
	int insnWidth, const DecodedInstruction* pDecInsn)
{
	const u2* insns = pCode->insns;
	int i;

	printf("%06x:", ((u1*)insns - pDexFile->baseAddr) + insnIdx * 2);
	for (i = 0; i < 8; i++) {
		if (i < insnWidth) {
			if (i == 7) {
				printf(" ... ");
			}
			else {
				/* print 16-bit value in little-endian order */
				const u1* bytePtr = (const u1*)&insns[insnIdx + i];
				printf(" %02x%02x", bytePtr[0], bytePtr[1]);
			}
		}
		else {
			fputs("     ", stdout);
		}
	}

	if (pDecInsn->opCode == OP_NOP) {
		u2 instr = get2LE((const u1*)&insns[insnIdx]);
		if (instr == kPackedSwitchSignature) {
			printf("|%04x: packed-switch-data (%d units)",
				insnIdx, insnWidth);
		}
		else if (instr == kSparseSwitchSignature) {
			printf("|%04x: sparse-switch-data (%d units)",
				insnIdx, insnWidth);
		}
		else if (instr == kArrayDataSignature) {
			printf("|%04x: array-data (%d units)",
				insnIdx, insnWidth);
		}
		else {
			printf("|%04x: nop // spacer", insnIdx);
		}
	}
	else {
		printf("|%04x: %s", insnIdx, getOpcodeName(pDecInsn->opCode));
	}

	switch (dexGetInstrFormat(gInstrFormat, pDecInsn->opCode)) {
	case kFmt10x:        // op
		break;
	case kFmt12x:        // op vA, vB
		printf(" v%d, v%d", pDecInsn->vA, pDecInsn->vB);
		break;
	case kFmt11n:        // op vA, #+B
		printf(" v%d, #int %d // #%x",
			pDecInsn->vA, (s4)pDecInsn->vB, (u1)pDecInsn->vB);
		break;
	case kFmt11x:        // op vAA
		printf(" v%d", pDecInsn->vA);
		break;
	case kFmt10t:        // op +AA
	case kFmt20t:        // op +AAAA
	{
							 s4 targ = (s4)pDecInsn->vA;
							 printf(" %04x // %c%04x",
								 insnIdx + targ,
								 (targ < 0) ? '-' : '+',
								 (targ < 0) ? -targ : targ);
	}
		break;
	case kFmt22x:        // op vAA, vBBBB
		printf(" v%d, v%d", pDecInsn->vA, pDecInsn->vB);
		break;
	case kFmt21t:        // op vAA, +BBBB
	{
							 s4 targ = (s4)pDecInsn->vB;
							 printf(" v%d, %04x // %c%04x", pDecInsn->vA,
								 insnIdx + targ,
								 (targ < 0) ? '-' : '+',
								 (targ < 0) ? -targ : targ);
	}
		break;
	case kFmt21s:        // op vAA, #+BBBB
		printf(" v%d, #int %d // #%x",
			pDecInsn->vA, (s4)pDecInsn->vB, (u2)pDecInsn->vB);
		break;
	case kFmt21h:        // op vAA, #+BBBB0000[00000000]
		// The printed format varies a bit based on the actual opcode.
		if (pDecInsn->opCode == OP_CONST_HIGH16) {
			s4 value = pDecInsn->vB << 16;
			printf(" v%d, #int %d // #%x",
				pDecInsn->vA, value, (u2)pDecInsn->vB);
		}
		else {
			s8 value = ((s8)pDecInsn->vB) << 48;
			printf(" v%d, #long %lld // #%x",
				pDecInsn->vA, value, (u2)pDecInsn->vB);
		}
		break;
	case kFmt21c:        // op vAA, thing@BBBB
		if (pDecInsn->opCode == OP_CONST_STRING) {
			printf(" v%d, \"%s\" // string@%04x", pDecInsn->vA,
				dexStringById(pDexFile, pDecInsn->vB), pDecInsn->vB);
		}
		else if (pDecInsn->opCode == OP_CHECK_CAST ||
			pDecInsn->opCode == OP_NEW_INSTANCE ||
			pDecInsn->opCode == OP_CONST_CLASS)
		{
			printf(" v%d, %s // class@%04x", pDecInsn->vA,
				getClassDescriptor(pDexFile, pDecInsn->vB), pDecInsn->vB);
		}
		else /* OP_SGET* */ {
			FieldMethodInfo fieldInfo;
			if (getFieldInfo(pDexFile, pDecInsn->vB, &fieldInfo)) {
				printf(" v%d, %s.%s:%s // field@%04x", pDecInsn->vA,
					fieldInfo.classDescriptor, fieldInfo.name,
					fieldInfo.signature, pDecInsn->vB);
			}
			else {
				printf(" v%d, ??? // field@%04x", pDecInsn->vA, pDecInsn->vB);
			}
		}
		break;
	case kFmt23x:        // op vAA, vBB, vCC
		printf(" v%d, v%d, v%d", pDecInsn->vA, pDecInsn->vB, pDecInsn->vC);
		break;
	case kFmt22b:        // op vAA, vBB, #+CC
		printf(" v%d, v%d, #int %d // #%02x",
			pDecInsn->vA, pDecInsn->vB, (s4)pDecInsn->vC, (u1)pDecInsn->vC);
		break;
	case kFmt22t:        // op vA, vB, +CCCC
	{
							 s4 targ = (s4)pDecInsn->vC;
							 printf(" v%d, v%d, %04x // %c%04x", pDecInsn->vA, pDecInsn->vB,
								 insnIdx + targ,
								 (targ < 0) ? '-' : '+',
								 (targ < 0) ? -targ : targ);
	}
		break;
	case kFmt22s:        // op vA, vB, #+CCCC
		printf(" v%d, v%d, #int %d // #%04x",
			pDecInsn->vA, pDecInsn->vB, (s4)pDecInsn->vC, (u2)pDecInsn->vC);
		break;
	case kFmt22c:        // op vA, vB, thing@CCCC
		if (pDecInsn->opCode >= OP_IGET && pDecInsn->opCode <= OP_IPUT_SHORT) {
			FieldMethodInfo fieldInfo;
			if (getFieldInfo(pDexFile, pDecInsn->vC, &fieldInfo)) {
				printf(" v%d, v%d, %s.%s:%s // field@%04x", pDecInsn->vA,
					pDecInsn->vB, fieldInfo.classDescriptor, fieldInfo.name,
					fieldInfo.signature, pDecInsn->vC);
			}
			else {
				printf(" v%d, v%d, ??? // field@%04x", pDecInsn->vA,
					pDecInsn->vB, pDecInsn->vC);
			}
		}
		else {
			printf(" v%d, v%d, %s // class@%04x",
				pDecInsn->vA, pDecInsn->vB,
				getClassDescriptor(pDexFile, pDecInsn->vC), pDecInsn->vC);
		}
		break;
	case kFmt22cs:       // [opt] op vA, vB, field offset CCCC
		printf(" v%d, v%d, [obj+%04x]",
			pDecInsn->vA, pDecInsn->vB, pDecInsn->vC);
		break;
	case kFmt30t:
		printf(" #%08x", pDecInsn->vA);
		break;
	case kFmt31i:        // op vAA, #+BBBBBBBB
	{
							 /* this is often, but not always, a float */
							 union {
								 float f;
								 u4 i;
							 } conv;
							 conv.i = pDecInsn->vB;
							 printf(" v%d, #float %f // #%08x",
								 pDecInsn->vA, conv.f, pDecInsn->vB);
	}
		break;
	case kFmt31c:        // op vAA, thing@BBBBBBBB
		printf(" v%d, \"%s\" // string@%08x", pDecInsn->vA,
			dexStringById(pDexFile, pDecInsn->vB), pDecInsn->vB);
		break;
	case kFmt31t:       // op vAA, offset +BBBBBBBB
		printf(" v%d, %08x // +%08x",
			pDecInsn->vA, insnIdx + pDecInsn->vB, pDecInsn->vB);
		break;
	case kFmt32x:        // op vAAAA, vBBBB
		printf(" v%d, v%d", pDecInsn->vA, pDecInsn->vB);
		break;
	case kFmt35c:        // op vB, {vD, vE, vF, vG, vA}, thing@CCCC
	{
							 /* NOTE: decoding of 35c doesn't quite match spec */
							 fputs(" {", stdout);
							 for (i = 0; i < (int)pDecInsn->vA; i++) {
								 if (i == 0)
									 printf("v%d", pDecInsn->arg[i]);
								 else
									 printf(", v%d", pDecInsn->arg[i]);
							 }
							 if (pDecInsn->opCode == OP_FILLED_NEW_ARRAY) {
								 printf("}, %s // class@%04x",
									 getClassDescriptor(pDexFile, pDecInsn->vB), pDecInsn->vB);
							 }
							 else {
								 FieldMethodInfo methInfo;
								 if (getMethodInfo(pDexFile, pDecInsn->vB, &methInfo)) {
									 printf("}, %s.%s:%s // method@%04x",
										 methInfo.classDescriptor, methInfo.name,
										 methInfo.signature, pDecInsn->vB);
								 }
								 else {
									 printf("}, ??? // method@%04x", pDecInsn->vB);
								 }
							 }
	}
		break;
	case kFmt35ms:       // [opt] invoke-virtual+super
	case kFmt35fs:       // [opt] invoke-interface
	{
							 fputs(" {", stdout);
							 for (i = 0; i < (int)pDecInsn->vA; i++) {
								 if (i == 0)
									 printf("v%d", pDecInsn->arg[i]);
								 else
									 printf(", v%d", pDecInsn->arg[i]);
							 }
							 printf("}, [%04x] // vtable #%04x", pDecInsn->vB, pDecInsn->vB);
	}
		break;
	case kFmt3rc:        // op {vCCCC .. v(CCCC+AA-1)}, meth@BBBB
	{
							 /*
							 * This doesn't match the "dx" output when some of the args are
							 * 64-bit values -- dx only shows the first register.
							 */
							 fputs(" {", stdout);
							 for (i = 0; i < (int)pDecInsn->vA; i++) {
								 if (i == 0)
									 printf("v%d", pDecInsn->vC + i);
								 else
									 printf(", v%d", pDecInsn->vC + i);
							 }
							 if (pDecInsn->opCode == OP_FILLED_NEW_ARRAY_RANGE) {
								 printf("}, %s // class@%04x",
									 getClassDescriptor(pDexFile, pDecInsn->vB), pDecInsn->vB);
							 }
							 else {
								 FieldMethodInfo methInfo;
								 if (getMethodInfo(pDexFile, pDecInsn->vB, &methInfo)) {
									 printf("}, %s.%s:%s // method@%04x",
										 methInfo.classDescriptor, methInfo.name,
										 methInfo.signature, pDecInsn->vB);
								 }
								 else {
									 printf("}, ??? // method@%04x", pDecInsn->vB);
								 }
							 }
	}
		break;
	case kFmt3rms:       // [opt] invoke-virtual+super/range
	case kFmt3rfs:       // [opt] invoke-interface/range
	{
							 /*
							 * This doesn't match the "dx" output when some of the args are
							 * 64-bit values -- dx only shows the first register.
							 */
							 fputs(" {", stdout);
							 for (i = 0; i < (int)pDecInsn->vA; i++) {
								 if (i == 0)
									 printf("v%d", pDecInsn->vC + i);
								 else
									 printf(", v%d", pDecInsn->vC + i);
							 }
							 printf("}, [%04x] // vtable #%04x", pDecInsn->vB, pDecInsn->vB);
	}
		break;
	case kFmt3rinline:   // [opt] execute-inline/range
	{
							 fputs(" {", stdout);
							 for (i = 0; i < (int)pDecInsn->vA; i++) {
								 if (i == 0)
									 printf("v%d", pDecInsn->vC + i);
								 else
									 printf(", v%d", pDecInsn->vC + i);
							 }
							 printf("}, [%04x] // inline #%04x", pDecInsn->vB, pDecInsn->vB);
	}
		break;
	case kFmt3inline:    // [opt] inline invoke
	{
#if 0
							 const InlineOperation* inlineOpsTable = dvmGetInlineOpsTable();
							 u4 tableLen = dvmGetInlineOpsTableLength();
#endif

							 fputs(" {", stdout);
							 for (i = 0; i < (int)pDecInsn->vA; i++) {
								 if (i == 0)
									 printf("v%d", pDecInsn->arg[i]);
								 else
									 printf(", v%d", pDecInsn->arg[i]);
							 }
#if 0
							 if (pDecInsn->vB < tableLen) {
								 printf("}, %s.%s:%s // inline #%04x",
									 inlineOpsTable[pDecInsn->vB].classDescriptor,
									 inlineOpsTable[pDecInsn->vB].methodName,
									 inlineOpsTable[pDecInsn->vB].methodSignature,
									 pDecInsn->vB);
							 }
							 else {
#endif
								 printf("}, [%04x] // inline #%04x", pDecInsn->vB, pDecInsn->vB);
#if 0
							 }
#endif
	}
		break;
	case kFmt51l:        // op vAA, #+BBBBBBBBBBBBBBBB
	{
							 /* this is often, but not always, a double */
							 union {
								 double d;
								 u8 j;
							 } conv;
							 conv.j = pDecInsn->vB_wide;
							 printf(" v%d, #double %f // #%016llx",
								 pDecInsn->vA, conv.d, pDecInsn->vB_wide);
	}
		break;
	case kFmtUnknown:
		break;
	default:
		printf(" ???");
		break;
	}


	putchar('\n');

}

/*
* Dump a bytecode disassembly.
*/
void CDexAnalyse::dumpBytecodes(DexFile* pDexFile, const DexMethod* pDexMethod)
{
	const DexCode* pCode = dexGetCode(pDexFile, pDexMethod);
	const u2* insns;
	int insnIdx;
	FieldMethodInfo methInfo;
	int startAddr;
	char* className = NULL;

	assert(pCode->insnsSize > 0);
	insns = pCode->insns;

	getMethodInfo(pDexFile, pDexMethod->methodIdx, &methInfo);
	startAddr = ((u1*)pCode - pDexFile->baseAddr);
	className = descriptorToDot(methInfo.classDescriptor);

	printf("%06x:                                        |[%06x] %s.%s:%s\n",
		startAddr, startAddr,
		className, methInfo.name, methInfo.signature);

	insnIdx = 0;
	while (insnIdx < (int)pCode->insnsSize) {
		int insnWidth;
		OpCode opCode;
		DecodedInstruction decInsn;
		u2 instr;

		instr = get2LE((const u1*)insns);
		if (instr == kPackedSwitchSignature) {
			insnWidth = 4 + get2LE((const u1*)(insns + 1)) * 2;
		}
		else if (instr == kSparseSwitchSignature) {
			insnWidth = 2 + get2LE((const u1*)(insns + 1)) * 4;
		}
		else if (instr == kArrayDataSignature) {
			int width = get2LE((const u1*)(insns + 1));
			int size = get2LE((const u1*)(insns + 2)) |
				(get2LE((const u1*)(insns + 3)) << 16);
			// The plus 1 is to round up for odd size and width 
			insnWidth = 4 + ((size * width) + 1) / 2;
		}
		else {
			opCode = (OpCode)(instr & 0xff);
			insnWidth = dexGetInstrWidthAbs(gInstrWidth, opCode);
			if (insnWidth == 0) {
				fprintf(stderr,
					"GLITCH: zero-width instruction at idx=0x%04x\n", insnIdx);
				break;
			}
		}

		dexDecodeInstruction(gInstrFormat, insns, &decInsn);
		dumpInstruction(pDexFile, pCode, insnIdx, insnWidth, &decInsn);

		insns += insnWidth;
		insnIdx += insnWidth;
	}

	free(className);
}

/*
* Dump a "code" struct.
*/
std::wstring CDexAnalyse::dumpCode(DexFile* pDexFile, const DexMethod* pDexMethod)
{
	std::wstring strR = L"";
	std::wostringstream   ostr;

	const DexCode* pCode = dexGetCode(pDexFile, pDexMethod);

	ostr << "      registers     : " << pCode->registersSize << L"\r\n";
	ostr << "      ins           : " << pCode->insSize << L"\r\n";
	ostr << "      outs          : " << pCode->outsSize << L"\r\n";
	ostr << "      insns size    : " << pCode->insnsSize << " 16-bit code units" << L"\r\n";

	if (gOptions.disassemble)
		dumpBytecodes(pDexFile, pDexMethod);

	dumpCatches(pDexFile, pCode);
	/* both of these are encoded in debug info */
	dumpPositions(pDexFile, pCode, pDexMethod);
	dumpLocals(pDexFile, pCode, pDexMethod);

	strR = ostr.str();
	return strR;
}

/*
* Dump a method.
*/
std::wstring CDexAnalyse::dumpMethod(DexFile* pDexFile, const DexMethod* pDexMethod, int i)
{
	std::wstring strR = L"";
	std::wostringstream   ostr;

	const DexMethodId* pMethodId;
	const char* backDescriptor;
	const char* name;
	char* typeDescriptor = NULL;
	char* accessStr = NULL;

	if (gOptions.exportsOnly &&
		(pDexMethod->accessFlags & (ACC_PUBLIC | ACC_PROTECTED)) == 0)
	{
		return L"";
	}

	pMethodId = dexGetMethodId(pDexFile, pDexMethod->methodIdx);
	name = dexStringById(pDexFile, pMethodId->nameIdx);
	typeDescriptor = dexCopyDescriptorFromMethodId(pDexFile, pMethodId);

	backDescriptor = dexStringByTypeIdx(pDexFile, pMethodId->classIdx);

	accessStr = createAccessFlagStr(pDexMethod->accessFlags,
		kAccessForMethod);

	if (gOptions.outputFormat == OUTPUT_PLAIN) {
		ostr << "    #" << i << "              : (in " << backDescriptor << ")" << L"\r\n";
		ostr << "      name          : " << name << L"\r\n";
		ostr << "      type          : " << typeDescriptor << L"\r\n";
		ostr << "      access        : " << pDexMethod->accessFlags << "(" << accessStr << ")" << L"\r\n";

		if (pDexMethod->codeOff == 0) {
			ostr << "      code          : (none)" << L"\r\n";
		}
		else {
			ostr << "      code          " << L"\r\n";
			ostr<<dumpCode(pDexFile, pDexMethod);
		}

		if (gOptions.disassemble)
			ostr << L"\r\n";
	}
	else if (gOptions.outputFormat == OUTPUT_XML) {
		bool constructor = (name[0] == '<');

		if (constructor) {
			char* tmp;

			tmp = descriptorClassToDot(backDescriptor);
			ostr << "<constructor name=" << tmp << L"\r\n";
			free(tmp);

			tmp = descriptorToDot(backDescriptor);
			ostr << "type=" << tmp << L"\r\n";
			free(tmp);
		}
		else {
			ostr << "<method name =" << name << L"\r\n";

			const char* returnType = strrchr(typeDescriptor, ')');
			if (returnType == NULL) {
				ostr << "bad method type descriptor " << typeDescriptor << L"\r\n";
				goto bail;
			}

			char* tmp = descriptorToDot(returnType + 1);
			ostr << " return=" << tmp << L"\r\n";
			free(tmp);

			ostr << " abstract=" << quotedBool((pDexMethod->accessFlags & ACC_ABSTRACT) != 0) << L"\r\n";
			ostr << " native=" << quotedBool((pDexMethod->accessFlags & ACC_NATIVE) != 0) << L"\r\n";

			bool isSync =
				(pDexMethod->accessFlags & ACC_SYNCHRONIZED) != 0 ||
				(pDexMethod->accessFlags & ACC_DECLARED_SYNCHRONIZED) != 0;
			ostr << " synchronized=" << quotedBool(isSync) << L"\r\n";
		}
		ostr << " static=" << quotedBool((pDexMethod->accessFlags & ACC_STATIC) != 0) << L"\r\n";
		ostr << " final=" << quotedBool((pDexMethod->accessFlags & ACC_FINAL) != 0) << L"\r\n";
		// "deprecated=" not knowable w/o parsing annotations
		ostr << " visibility = " << quotedVisibility(pDexMethod->accessFlags) <<">"<< L"\r\n";

		/*
		* Parameters.
		*/
		if (typeDescriptor[0] != '(') {
			ostr << "ERROR: bad descriptor " << typeDescriptor << L"\r\n";
			goto bail;
		}

		int argNum = 0;
		const char* base = typeDescriptor + 1;
		while (*base != ')') {
			char* tmpBuf = (char*)malloc(strlen(typeDescriptor) + 1);      /* more than big enough */
			char* cp = tmpBuf;

			while (*base == '[')
				*cp++ = *base++;

			if (*base == 'L') {
				/* copy through ';' */
				do {
					*cp = *base++;
				} while (*cp++ != ';');
			}
			else {
				/* primitive char, copy it */
				if (strchr("ZBCSIFJD", *base) == NULL) {
					ostr << "ERROR: bad method signature " << base << L"\r\n";
					goto bail;
				}
				*cp++ = *base++;
			}

			/* null terminate and display */
			*cp++ = '\0';

			char* tmp = descriptorToDot(tmpBuf);
			ostr << "<parameter name=arg" << argNum++ << " type=" << tmp << ">" << L"\r\n" << "</parameter>" << L"\r\n";
			free(tmp);
			free(tmpBuf);
		}

		if (constructor)
			ostr << "</constructor>" << L"\r\n";
		else
			ostr << "</method>" << L"\r\n";
	}

bail:
	free(typeDescriptor);
	free(accessStr);

	strR = ostr.str();
	return strR;
}

/*
* Dump a static (class) field.
*/
std::wstring CDexAnalyse::dumpSField(const DexFile* pDexFile, const DexField* pSField, int i)
{
	std::wstring strR = L"";
	std::wostringstream   ostr;

	const DexFieldId* pFieldId;
	const char* backDescriptor;
	const char* name;
	const char* typeDescriptor;
	char* accessStr;

	if (gOptions.exportsOnly &&
		(pSField->accessFlags & (ACC_PUBLIC | ACC_PROTECTED)) == 0)
	{
		return L"";
	}

	pFieldId = dexGetFieldId(pDexFile, pSField->fieldIdx);
	name = dexStringById(pDexFile, pFieldId->nameIdx);
	typeDescriptor = dexStringByTypeIdx(pDexFile, pFieldId->typeIdx);
	backDescriptor = dexStringByTypeIdx(pDexFile, pFieldId->classIdx);

	accessStr = createAccessFlagStr(pSField->accessFlags, kAccessForField);

	if (gOptions.outputFormat == OUTPUT_PLAIN) {
		ostr << "    #" << i << "              : (in " << backDescriptor << ")" << L"\r\n";
		ostr << "      name          : " << name << L"\r\n";
		ostr << "      type          : " << typeDescriptor << L"\r\n";
		ostr << "access        :" << pSField->accessFlags << "(" << accessStr << ")" << L"\r\n";
	}
	else if (gOptions.outputFormat == OUTPUT_XML) {
		char* tmp;

		ostr << "<field name = " << name << L"\r\n";

		tmp = descriptorToDot(typeDescriptor);
		ostr << "type =" << tmp << L"\r\n";
		free(tmp);

		ostr << "transient=" << quotedBool((pSField->accessFlags & ACC_TRANSIENT) != 0) << L"\r\n";
		ostr << "volatile=" << quotedBool((pSField->accessFlags & ACC_VOLATILE) != 0) << L"\r\n";
		// "value=" not knowable w/o parsing annotations
		ostr << "static=" << quotedBool((pSField->accessFlags & ACC_STATIC) != 0) << L"\r\n";
		ostr << "final=" << quotedBool((pSField->accessFlags & ACC_FINAL) != 0) << L"\r\n";
		// "deprecated=" not knowable w/o parsing annotations
		ostr << "visibility=" << quotedVisibility(pSField->accessFlags) << L"\r\n";
		ostr << ">" << L"\r\n" << "</field>" << L"\r\n";
	}
	free(accessStr);
	strR = ostr.str();
	return strR;
}

/*
* Dump an instance field.
*/
std::wstring CDexAnalyse::dumpIField(const DexFile* pDexFile, const DexField* pIField, int i)
{
	return dumpSField(pDexFile, pIField, i);
}

/*
* Dump the class.
*
* Note "idx" is a DexClassDef index, not a DexTypeId index.
*
* If "*pLastPackage" is NULL or does not match the current class' package,
* the value will be replaced with a newly-allocated string.
*/
std::wstring CDexAnalyse::dumpClass(DexFile* pDexFile, int idx, char** pLastPackage)
{
	std::wstring strR = L"";
	std::wostringstream   ostr;
	ostr << L"****************************************************************************" << L"\r\n";

	const DexTypeList* pInterfaces;
	const DexClassDef* pClassDef;
	DexClassData* pClassData = NULL;
	const u1* pEncodedData;
	const char* fileName;
	const char* classDescriptor;
	const char* superclassDescriptor;
	char* accessStr = NULL;
	int i;

	pClassDef = dexGetClassDef(pDexFile, idx);

	if (gOptions.exportsOnly && (pClassDef->accessFlags & ACC_PUBLIC) == 0) {
		//printf("<!-- omitting non-public class %s -->\n",
		//    classDescriptor);
		goto bail;
	}

	pEncodedData = dexGetClassData(pDexFile, pClassDef);
	pClassData = dexReadAndVerifyClassData(&pEncodedData, NULL);

	if (pClassData == NULL) {
		ostr << "Trouble reading class data " << idx << L"\r\n";
		goto bail;
	}

	classDescriptor = dexStringByTypeIdx(pDexFile, pClassDef->classIdx);

	/*
	* For the XML output, show the package name.  Ideally we'd gather
	* up the classes, sort them, and dump them alphabetically so the
	* package name wouldn't jump around, but that's not a great plan
	* for something that needs to run on the device.
	*/
	if (!(classDescriptor[0] == 'L' &&
		classDescriptor[strlen(classDescriptor) - 1] == ';'))
	{
		/* arrays and primitives should not be defined explicitly */
		ostr << "Malformed class name " << classDescriptor << L"\r\n";
		/* keep going? */
	}
	else if (gOptions.outputFormat == OUTPUT_XML) {
		char* mangle;
		char* lastSlash;
		char* cp;

		mangle = strdup(classDescriptor + 1);
		mangle[strlen(mangle) - 1] = '\0';

		/* reduce to just the package name */
		lastSlash = strrchr(mangle, '/');
		if (lastSlash != NULL) {
			*lastSlash = '\0';
		}
		else {
			*mangle = '\0';
		}

		for (cp = mangle; *cp != '\0'; cp++) {
			if (*cp == '/')
				*cp = '.';
		}

		if (*pLastPackage == NULL || strcmp(mangle, *pLastPackage) != 0) {
			/* start of a new package */
			if (*pLastPackage != NULL)
				ostr << "</package>" << L"\r\n";
			ostr << "<package name=" << mangle << L"\r\n";
			free(*pLastPackage);
			*pLastPackage = mangle;
		}
		else {
			free(mangle);
		}
	}

	accessStr = createAccessFlagStr(pClassDef->accessFlags, kAccessForClass);

	if (pClassDef->superclassIdx == kDexNoIndex) {
		superclassDescriptor = NULL;
	}
	else {
		superclassDescriptor =
			dexStringByTypeIdx(pDexFile, pClassDef->superclassIdx);
	}

	if (gOptions.outputFormat == OUTPUT_PLAIN) {
		ostr << "Class #" << idx << L"\r\n";
		ostr << "Class descriptor  :" << classDescriptor << L"\r\n";
		ostr << "Access flags      :" << pClassDef->accessFlags << "(" << accessStr << ")" << L"\r\n";
		if (superclassDescriptor != NULL)
			ostr << "Superclass        :" << superclassDescriptor << L"\r\n";
		ostr << "Interfaces" << L"\r\n";
	}
	else {
		char* tmp;
		tmp = descriptorClassToDot(classDescriptor);
		ostr << "<class name=" << tmp << L"\r\n";
		free(tmp);

		if (superclassDescriptor != NULL) {
			tmp = descriptorToDot(superclassDescriptor);
			ostr << "extends=" << tmp << L"\r\n";
			free(tmp);
		}
		ostr << "abstract=" << quotedBool((pClassDef->accessFlags & ACC_ABSTRACT) != 0) << L"\r\n";
		ostr << "static=" << quotedBool((pClassDef->accessFlags & ACC_STATIC) != 0) << L"\r\n";
		ostr << "final=" << quotedBool((pClassDef->accessFlags & ACC_FINAL) != 0) << L"\r\n";
		// "deprecated=" not knowable w/o parsing annotations
		ostr << "visibility=" << quotedVisibility(pClassDef->accessFlags) << L"\r\n";
		ostr << ">" << L"\r\n";
	}
	pInterfaces = dexGetInterfacesList(pDexFile, pClassDef);
	if (pInterfaces != NULL) {
		for (i = 0; i < (int)pInterfaces->size; i++)
			ostr<<dumpInterface(pDexFile, dexGetTypeItem(pInterfaces, i), i);
	}

	if (gOptions.outputFormat == OUTPUT_PLAIN)
		ostr << "Static fields" << L"\r\n";
	for (i = 0; i < (int)pClassData->header.staticFieldsSize; i++) {
		ostr<<dumpSField(pDexFile, &pClassData->staticFields[i], i);
	}

	if (gOptions.outputFormat == OUTPUT_PLAIN)
		ostr << "Instance fields" << L"\r\n";
	for (i = 0; i < (int)pClassData->header.instanceFieldsSize; i++) {
		ostr<<dumpIField(pDexFile, &pClassData->instanceFields[i], i);
	}

	if (gOptions.outputFormat == OUTPUT_PLAIN)
		ostr << "Direct methods" << L"\r\n";
	for (i = 0; i < (int)pClassData->header.directMethodsSize; i++) {
		ostr<<dumpMethod(pDexFile, &pClassData->directMethods[i], i);
	}

	if (gOptions.outputFormat == OUTPUT_PLAIN)
		ostr << "Virtual methods" << L"\r\n";
	for (i = 0; i < (int)pClassData->header.virtualMethodsSize; i++) {
		ostr<<dumpMethod(pDexFile, &pClassData->virtualMethods[i], i);
	}

	// TODO: Annotations.

	if (pClassDef->sourceFileIdx != kDexNoIndex)
		fileName = dexStringById(pDexFile, pClassDef->sourceFileIdx);
	else
		fileName = "unknown";

	if (gOptions.outputFormat == OUTPUT_PLAIN) {
		ostr << "source_file_idx" << pClassDef->sourceFileIdx << "(" << fileName <<")"<< L"\r\n";
	}

	if (gOptions.outputFormat == OUTPUT_XML) {
		ostr << "</class>" << L"\r\n";
	}

bail:
	free(pClassData);
	free(accessStr);

	strR = ostr.str();
	return strR;
}


/*
* Advance "ptr" to ensure 32-bit alignment.
*/
static inline const u1* align32(const u1* ptr)
{
	return (u1*)(((int)ptr + 3) & ~0x03);
}


/*
* Dump a map in the "differential" format.
*
* TODO: show a hex dump of the compressed data.  (We can show the
* uncompressed data if we move the compression code to libdex; otherwise
* it's too complex to merit a fast & fragile implementation here.)
*/
void CDexAnalyse::dumpDifferentialCompressedMap(const u1** pData)
{
	const u1* data = *pData;
	const u1* dataStart = data - 1;      // format byte already removed
	u1 regWidth;
	u2 numEntries;

	/* standard header */
	regWidth = *data++;
	numEntries = *data++;
	numEntries |= (*data++) << 8;

	/* compressed data begins with the compressed data length */
	int compressedLen = readUnsignedLeb128(&data);
	int addrWidth = 1;
	if ((*data & 0x80) != 0)
		addrWidth++;

	int origLen = 4 + (addrWidth + regWidth) * numEntries;
	int compLen = (data - dataStart) + compressedLen;

	printf("        (differential compression %d -> %d [%d -> %d])\n",
		origLen, compLen,
		(addrWidth + regWidth) * numEntries, compressedLen);

	/* skip past end of entry */
	data += compressedLen;

	*pData = data;
}

/*
* Dump register map contents of the current method.
*
* "*pData" should point to the start of the register map data.  Advances
* "*pData" to the start of the next map.
*/
void CDexAnalyse::dumpMethodMap(DexFile* pDexFile, const DexMethod* pDexMethod, int idx,
	const u1** pData)
{
	const u1* data = *pData;
	const DexMethodId* pMethodId;
	const char* name;
	int offset = data - (u1*)pDexFile->pOptHeader;

	pMethodId = dexGetMethodId(pDexFile, pDexMethod->methodIdx);
	name = dexStringById(pDexFile, pMethodId->nameIdx);
	printf("      #%d: 0x%08x %s\n", idx, offset, name);

	u1 format;
	int addrWidth;

	format = *data++;
	if (format == 1) {              /* kRegMapFormatNone */
		/* no map */
		printf("        (no map)\n");
		addrWidth = 0;
	}
	else if (format == 2) {       /* kRegMapFormatCompact8 */
		addrWidth = 1;
	}
	else if (format == 3) {       /* kRegMapFormatCompact16 */
		addrWidth = 2;
	}
	else if (format == 4) {       /* kRegMapFormatDifferential */
		dumpDifferentialCompressedMap(&data);
		goto bail;
	}
	else {
		printf("        (unknown format %d!)\n", format);
		/* don't know how to skip data; failure will cascade to end of class */
		goto bail;
	}

	if (addrWidth > 0) {
		u1 regWidth;
		u2 numEntries;
		int idx, addr, byte;

		regWidth = *data++;
		numEntries = *data++;
		numEntries |= (*data++) << 8;

		for (idx = 0; idx < numEntries; idx++) {
			addr = *data++;
			if (addrWidth > 1)
				addr |= (*data++) << 8;

			printf("        %4x:", addr);
			for (byte = 0; byte < regWidth; byte++) {
				printf(" %02x", *data++);
			}
			printf("\n");
		}
	}

bail:
	//if (addrWidth >= 0)
	//    *pData = align32(data);
	*pData = data;
}

/*
* Dump the contents of the register map area.
*
* These are only present in optimized DEX files, and the structure is
* not really exposed to other parts of the VM itself.  We're going to
* dig through them here, but this is pretty fragile.  DO NOT rely on
* this or derive other code from it.
*/
void CDexAnalyse::dumpRegisterMaps(DexFile* pDexFile)
{
	const u1* pClassPool = (const u1*)pDexFile->pRegisterMapPool;
	const u4* classOffsets;
	const u1* ptr;
	u4 numClasses;
	int baseFileOffset = (u1*)pClassPool - (u1*)pDexFile->pOptHeader;
	int idx;

	if (pClassPool == NULL) {
		printf("No register maps found\n");
		return;
	}

	ptr = pClassPool;
	numClasses = get4LE(ptr);
	ptr += sizeof(u4);
	classOffsets = (const u4*)ptr;

	printf("RMAP begins at offset 0x%07x\n", baseFileOffset);
	printf("Maps for %d classes\n", numClasses);
	for (idx = 0; idx < (int)numClasses; idx++) {
		const DexClassDef* pClassDef;
		const char* classDescriptor;

		pClassDef = dexGetClassDef(pDexFile, idx);
		classDescriptor = dexStringByTypeIdx(pDexFile, pClassDef->classIdx);

		printf("%4d: +%d (0x%08x) %s\n", idx, classOffsets[idx],
			baseFileOffset + classOffsets[idx], classDescriptor);

		if (classOffsets[idx] == 0)
			continue;

		/*
		* What follows is a series of RegisterMap entries, one for every
		* direct method, then one for every virtual method.
		*/
		DexClassData* pClassData;
		const u1* pEncodedData;
		const u1* data = (u1*)pClassPool + classOffsets[idx];
		u2 methodCount;
		int i;

		pEncodedData = dexGetClassData(pDexFile, pClassDef);
		pClassData = dexReadAndVerifyClassData(&pEncodedData, NULL);
		if (pClassData == NULL) {
			fprintf(stderr, "Trouble reading class data\n");
			continue;
		}

		methodCount = *data++;
		methodCount |= (*data++) << 8;
		data += 2;      /* two pad bytes follow methodCount */
		if (methodCount != pClassData->header.directMethodsSize
			+ pClassData->header.virtualMethodsSize)
		{
			printf("NOTE: method count discrepancy (%d != %d + %d)\n",
				methodCount, pClassData->header.directMethodsSize,
				pClassData->header.virtualMethodsSize);
			/* this is bad, but keep going anyway */
		}

		printf("    direct methods: %d\n",
			pClassData->header.directMethodsSize);
		for (i = 0; i < (int)pClassData->header.directMethodsSize; i++) {
			dumpMethodMap(pDexFile, &pClassData->directMethods[i], i, &data);
		}

		printf("    virtual methods: %d\n",
			pClassData->header.virtualMethodsSize);
		for (i = 0; i < (int)pClassData->header.virtualMethodsSize; i++) {
			dumpMethodMap(pDexFile, &pClassData->virtualMethods[i], i, &data);
		}

		free(pClassData);
	}
}


bool CDexAnalyse::dexLoadFile(const wchar_t *lpPath, const wchar_t *lpModel)
{
	if (!PathFileExists(lpPath))
	{
		return false;
	}

	gOptions.verbose = true;
	gOptions.checksumOnly = true;
	gOptions.disassemble = true;
	gOptions.showFileHeaders = true;
	gOptions.showSectionHeaders = true;
	gOptions.ignoreBadChecksum = true;
	gOptions.outputFormat = OUTPUT_PLAIN;
	gOptions.dumpRegisterMaps = true;
	gOptions.tempFileName = lpPath;
	if (gOptions.tempFileName.find_last_of('/') != -1)
	{
		int o = gOptions.tempFileName.find_last_of('/');
		gOptions.tempFileName.erase(o+1,-1);
		gOptions.tempFileName.append(L"zip-temp.dex");
	}
	else if (gOptions.tempFileName.find_last_of('\\')!=-1)
	{
		int o = gOptions.tempFileName.find_last_of('\\');
		gOptions.tempFileName.erase(o+1,-1);
		gOptions.tempFileName.append(L"zip-temp.dex");
	}
	

	//static InstructionWidth* gInstrWidth;
	//static InstructionFormat* gInstrFormat;
	///* initialize some VM tables */
	gInstrWidth = dexCreateInstrWidthTable();
	gInstrFormat = dexCreateInstrFormatTable();

	//free(gInstrWidth);
	//free(gInstrFormat);

	std::wstring wstr = lpPath;
	std::string ansistr1 = "";
	UnicodeToUtf8(wstr, ansistr1);
	std::string ansistr2;
	UnicodeToUtf8(gOptions.tempFileName, ansistr2);
	if (dexOpenAndMap(ansistr1.c_str(), ansistr2.c_str(), &mCtx.map, false) != 0)
	{
		return false;
	}
	mCtx.pDexFile = dexFileParse((const u1*)mCtx.map.addr, mCtx.map.length, 0);
	if (mCtx.pDexFile == NULL) {
		return false;
	}
	return true;
}

bool CDexAnalyse::Analysis()
{
	std::wstring strR;
	//dumpRegisterMaps(mCtx.pDexFile);
	//strR.append(dumpFileHeader(mCtx.pDexFile));

	//char* package = NULL;
	//for (int i = 0; i < (int)mCtx.pDexFile->pHeader->classDefsSize; i++) {
	//	dumpClassDef(mCtx.pDexFile, i);
	//	dumpClass(mCtx.pDexFile, i, &package);
	//}

	///* free the last one allocated */
	//if (package != NULL) {
	//	free(package);
	//}
	return true;
}

void CDexAnalyse::UnicodeToUtf8(const std::wstring &wstr, std::string &str)
{
	int len;
	len = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
	char *szUtf8 = (char*)malloc(len + 1);
	memset(szUtf8, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, szUtf8, len, NULL, NULL);

	str.clear();
	str.append(szUtf8, len);
}

void CDexAnalyse::Utf8ToUnicode(const std::string &szU8, std::wstring &wstr)
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

std::wstring CDexAnalyse::HexToStr(BYTE *pbSrc, int nLen)
{
	std::wstring wstr;
	char ddl, ddh;
	int i;

	for (i = 0; i<nLen; i++)
	{
		ddh = 48 + pbSrc[i] / 16;
		ddl = 48 + pbSrc[i] % 16;
		if (ddh > 57) ddh = ddh + 7;
		if (ddl > 57) ddl = ddl + 7;
		wstr.push_back(ddh);
		wstr.push_back(ddl);
	}
	return wstr;
}

std::wstring CDexAnalyse::StrToHex(BYTE *pbSrc, int nLen)
{
	std::wstring str;
	char h1, h2;
	BYTE s1, s2;
	int i;

	for (i = 0; i<nLen / 2; i++)
	{
		h1 = pbSrc[2 * i];
		h2 = pbSrc[2 * i + 1];

		s1 = toupper(h1) - 0x30;
		if (s1 > 9)
			s1 -= 7;

		s2 = toupper(h2) - 0x30;
		if (s2 > 9)
			s2 -= 7;

		str.push_back(s1 * 16 + s2);
	}
	return str;
}

std::wstring CDexAnalyse::doReport()
{
	std::wstring str;
	if (gOptions.dumpRegisterMaps) {
		dumpRegisterMaps(mCtx.pDexFile);
	}
	if (gOptions.showFileHeaders)
		str.append(dumpFileHeader(mCtx.pDexFile));

	if (gOptions.outputFormat == OUTPUT_XML)
		str.append(L"<api>\r\n");

	char* package = NULL;
	for (int i = 0; i < (int)mCtx.pDexFile->pHeader->classDefsSize; i++) {
		if (gOptions.showSectionHeaders)
			str.append(dumpClassDef(mCtx.pDexFile, i));
		str.append(dumpClass(mCtx.pDexFile, i, &package));
	}

	/* free the last one allocated */
	if (package != NULL) {
		str.append(L"</package>\r\n");
		free(package);
	}

	if (gOptions.outputFormat == OUTPUT_XML)
		str.append(L"</api>\r\n");

	return str;
}

bool CDexAnalyse::dexUnload()
{
	if (mCtx.pDexFile != NULL)
	{
		free(gInstrWidth);
		free(gInstrFormat);
		dexFileFree(mCtx.pDexFile);
		sysReleaseShmem(&mCtx.map);
	}
	return true;
}