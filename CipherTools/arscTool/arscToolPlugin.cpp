#include "stdafx.h"
#include "arscToolPlugin.h"


IMPLEMENT_PLUGIN(arscToolPlugin)

void arscToolPlugin::Init()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// merge menu
	CMenu append;
	append.LoadMenu(IDR_MENU1);
	MergeMenu(&append, TRUE);
}

void arscToolPlugin::Query(CPluginInfo& plugininfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	plugininfo.m_strName = _T("arsc Tool");
	plugininfo.m_strBlurb = _T("arsc解析工具");
	plugininfo.m_strHelp = _T("暂无");
	plugininfo.m_strAuthor = _T("wangzha");
	plugininfo.m_strCopyRight = _T("Copyright wangzha");
	plugininfo.m_strDate = _T("2018.03.12");
	plugininfo.m_strMenuLabel = "apk";
}
