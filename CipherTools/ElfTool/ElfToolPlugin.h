#pragma once
#include "stdafx.h"
class CElfToolPlugin :
	public CPlugin
{
	DECLARE_PLUGIN(CElfToolPlugin)
private:
	CElfToolPlugin(){};
public:
	virtual void Init();

	virtual void Query(CPluginInfo& plugininfo);
};

