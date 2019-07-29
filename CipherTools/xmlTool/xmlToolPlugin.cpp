#include "stdafx.h"
#include "xmlToolPlugin.h"


IMPLEMENT_PLUGIN(xmlToolPlugin)

void xmlToolPlugin::Init()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// merge menu
	CMenu append;
	append.LoadMenu(IDR_MENU1);
	MergeMenu(&append, TRUE);
}

void xmlToolPlugin::Query(CPluginInfo& plugininfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	plugininfo.m_strName = _T("xml Tool");
	plugininfo.m_strBlurb = _T("xml解析工具");
	plugininfo.m_strHelp = _T("暂无");
	plugininfo.m_strAuthor = _T("wangzha");
	plugininfo.m_strCopyRight = _T("Copyright wangzha");
	plugininfo.m_strDate = _T("2018.03.07");
	plugininfo.m_strMenuLabel = "apk";
}
