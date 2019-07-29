#pragma once
class TeaToolPlugin:public CPlugin
{
	DECLARE_PLUGIN(TeaToolPlugin)
private:
	TeaToolPlugin(){};
public:
	virtual void Init();

	virtual void Query(CPluginInfo& plugininfo);
};

