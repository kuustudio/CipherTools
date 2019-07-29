#pragma once
class ChaChaToolPlugin :public CPlugin
{
	DECLARE_PLUGIN(ChaChaToolPlugin)
private:
	ChaChaToolPlugin(){};
public:
	virtual void Init();

	virtual void Query(CPluginInfo& plugininfo);
};

