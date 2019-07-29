#pragma once
#include "stdafx.h"
class CKeyToolPlugin :
	public CPlugin
{
	DECLARE_PLUGIN(CKeyToolPlugin)

private:
	CKeyToolPlugin(){}

public:
	virtual void Init();

	virtual void Query(CPluginInfo& plugininfo);
};

