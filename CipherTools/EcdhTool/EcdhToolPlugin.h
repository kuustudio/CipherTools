#pragma once
#include"stdafx.h"

class EcdhToolPlugin :
	public CPlugin
{
	DECLARE_PLUGIN(EcdhToolPlugin)

private:
	EcdhToolPlugin(){}

public:
	virtual void Init();

	virtual void Query(CPluginInfo& plugininfo);
};

