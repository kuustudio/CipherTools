#pragma once
#include"stdafx.h"
class ProtobufToolPlugin :public CPlugin
{
	DECLARE_PLUGIN(ProtobufToolPlugin)

private:
	ProtobufToolPlugin(){}

public:
	virtual void Init();

	virtual void Query(CPluginInfo& plugininfo);
};

