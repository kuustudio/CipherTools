#pragma once
#include <google/protobuf/descriptor.h>
#include <google/protobuf/dynamic_message.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/tokenizer.h>
#include <google/protobuf/compiler/parser.h>
#include<google/protobuf/compiler/importer.h>
#include<fstream>
#include<string>
using namespace google::protobuf;


class ParseProto
{
public:
	ParseProto();
	virtual ~ParseProto();
private:
	std::vector<std::string> messagelist;
	std::string strProtoFile;
public:
	void Clear(){ messagelist.clear(); };
	bool LoadProtoFromFile(const std::string& data_filename);
	std::string PrintDataFile(const std::string& message_name, const unsigned char *buffer, const unsigned int size);
	std::vector<std::string> &GetMessageList();
};

