// stdafx.cpp : 只包括标准包含文件的源文件
// ProtobufTool.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"


#ifdef _DEBUG
#pragma comment(lib,"..\\proto_2_6_0_vs2013_LIB\\Debug\\libprotobuf.lib")
#pragma comment(lib,"..\\proto_2_6_0_vs2013_LIB\\Debug\\libprotobuf-lite.lib")
#pragma comment(lib,"..\\proto_2_6_0_vs2013_LIB\\Debug\\libprotoc.lib")
#else
#pragma comment(lib,"..\\proto_2_6_0_vs2013_LIB\\Release\\libprotobuf.lib")
#pragma comment(lib,"..\\proto_2_6_0_vs2013_LIB\\Release\\libprotobuf-lite.lib")
#pragma comment(lib,"..\\proto_2_6_0_vs2013_LIB\\Release\\libprotoc.lib")
#endif