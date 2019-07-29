#pragma once
#include"stdafx.h"
class AesToolPlugin :
	public CPlugin
{
	DECLARE_PLUGIN(AesToolPlugin)

private:
	AesToolPlugin(){}

public:
	virtual void Init();

	virtual void Query(CPluginInfo& plugininfo);
};

