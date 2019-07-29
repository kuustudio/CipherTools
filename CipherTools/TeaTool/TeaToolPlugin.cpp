#include "stdafx.h"
#include "TeaToolPlugin.h"


IMPLEMENT_PLUGIN(TeaToolPlugin)

void TeaToolPlugin::Init()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// merge menu
	CMenu append;
	append.LoadMenu(IDR_MENU1);
	//append.ModifyMenu(0, MF_BYPOSITION,-1, L"插件");
	MergeMenu(&append, TRUE);
}

void TeaToolPlugin::Query(CPluginInfo& plugininfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	plugininfo.m_strName = _T("TEA Cipher");
	plugininfo.m_strBlurb = _T("TEA加密解密工具");
	plugininfo.m_strHelp = _T("暂无");
	plugininfo.m_strAuthor = _T("wangzha");
	plugininfo.m_strCopyRight = _T("Copyright wangzha");
	plugininfo.m_strDate = _T("2017.11.10");
	plugininfo.m_strMenuLabel = "插件";
}