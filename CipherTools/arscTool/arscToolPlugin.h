#pragma once
#include"stdafx.h"
class arscToolPlugin :
	public CPlugin
{
	DECLARE_PLUGIN(arscToolPlugin)
private:
	arscToolPlugin(){};
public:
	virtual void Init();

	virtual void Query(CPluginInfo& plugininfo);
};

