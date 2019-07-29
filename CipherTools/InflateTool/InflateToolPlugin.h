#pragma once
class InflateToolPlugin : public CPlugin
{
	DECLARE_PLUGIN(InflateToolPlugin)
private:
	InflateToolPlugin(){};
public:
	virtual void Init();

	virtual void Query(CPluginInfo& plugininfo);
};

