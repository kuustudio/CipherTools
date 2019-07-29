#pragma once
#include "stdafx.h"
class xmlToolPlugin :
	public CPlugin
{
	DECLARE_PLUGIN(xmlToolPlugin)
private:
	xmlToolPlugin(){};
public:
	virtual void Init();

	virtual void Query(CPluginInfo& plugininfo);
};

