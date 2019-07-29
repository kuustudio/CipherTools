#include "stdafx.h"
#include "ParseProto.h"


ParseProto::ParseProto()
{
}


ParseProto::~ParseProto()
{
}

bool ParseProto::LoadProtoFromFile(const std::string& proto_filename)
{
	strProtoFile = proto_filename;
	int ipos = proto_filename.find_last_of('\\');
	if (ipos == -1)
	{
		ipos = proto_filename.find_last_of('/');
	}
	std::string path = proto_filename.substr(0,ipos);
	std::string file = proto_filename.substr(ipos+1);
	google::protobuf::compiler::DiskSourceTree sourceTree;
	sourceTree.MapPath("", path);
	google::protobuf::compiler::Importer mImport((google::protobuf::compiler::SourceTree*)(&sourceTree), NULL);
	const FileDescriptor* pFileDescriptor = mImport.Import(file);
	Clear();
	for (int i = 0; i < pFileDescriptor->message_type_count(); i++)
	{
		std::string name = pFileDescriptor->message_type(i)->full_name();
		messagelist.push_back(name);
	}
	return true;
}

std::vector<std::string>& ParseProto::GetMessageList()
{
	return messagelist;
}

std::string ParseProto::PrintDataFile(const std::string& message_name,const unsigned char *buffer,const unsigned int size) {
	std::string strdebug = "";
	if (size) {
		int ipos = strProtoFile.find_last_of('\\');
		if (ipos == -1)
		{
			ipos = strProtoFile.find_last_of('/');
		}
		std::string path = strProtoFile.substr(0, ipos);
		std::string file = strProtoFile.substr(ipos + 1);
		google::protobuf::compiler::DiskSourceTree sourceTree;
		sourceTree.MapPath("", path);
		google::protobuf::compiler::Importer mImport((google::protobuf::compiler::SourceTree*)(&sourceTree), NULL);
		const FileDescriptor* pFileDescriptor = mImport.Import(file);
		if (pFileDescriptor)
		{
			const google::protobuf::Descriptor *p=mImport.pool()->FindMessageTypeByName(message_name);
			google::protobuf::DynamicMessageFactory factory;
			google::protobuf::Message *msg = factory.GetPrototype(p)->New();
			msg->ParseFromArray(buffer, size);
			strdebug = msg->DebugString();
			delete msg;

		}
	}
	return strdebug;
}