#include "stdafx.h"
#include "InflateToolPlugin.h"


IMPLEMENT_PLUGIN(InflateToolPlugin)

void InflateToolPlugin::Init()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// merge menu
	CMenu append;
	append.LoadMenu(IDR_MENU1);
	MergeMenu(&append, TRUE);
}

void InflateToolPlugin::Query(CPluginInfo& plugininfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	plugininfo.m_strName = _T("Inflate/Deflate");
	plugininfo.m_strBlurb = _T("解压缩工具");
	plugininfo.m_strHelp = _T("暂无");
	plugininfo.m_strAuthor = _T("wangzha");
	plugininfo.m_strCopyRight = _T("Copyright wangzha");
	plugininfo.m_strDate = _T("2017.11.09");
	plugininfo.m_strMenuLabel = "插件";
}