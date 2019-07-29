#include "stdafx.h"
#include "ElfToolPlugin.h"


IMPLEMENT_PLUGIN(CElfToolPlugin)

void CElfToolPlugin::Init()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// merge menu
	CMenu append;
	append.LoadMenu(IDR_MENU1);
	MergeMenu(&append, TRUE);
}

void CElfToolPlugin::Query(CPluginInfo& plugininfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	plugininfo.m_strName = _T("elf Tool");
	plugininfo.m_strBlurb = _T("elf解析工具");
	plugininfo.m_strHelp = _T("暂无");
	plugininfo.m_strAuthor = _T("wangzha");
	plugininfo.m_strCopyRight = _T("Copyright wangzha");
	plugininfo.m_strDate = _T("2018.04.09");
	plugininfo.m_strMenuLabel = "apk";
}
