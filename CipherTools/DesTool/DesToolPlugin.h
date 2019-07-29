#pragma once
#include"stdafx.h"

class DesToolPlugin : public CPlugin
{
	DECLARE_PLUGIN(DesToolPlugin)

private:
	DesToolPlugin(){}

public:
	virtual void Init();

	virtual void Query(CPluginInfo& plugininfo);
};

