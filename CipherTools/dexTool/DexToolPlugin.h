#pragma once
#include "stdafx.h"
class DexToolPlugin :
	public CPlugin
{
	DECLARE_PLUGIN(DexToolPlugin)
private:
	DexToolPlugin(){};
public:
	virtual void Init();

	virtual void Query(CPluginInfo& plugininfo);
};

