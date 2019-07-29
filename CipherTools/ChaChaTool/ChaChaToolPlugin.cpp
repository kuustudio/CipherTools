#include "stdafx.h"
#include "ChaChaToolPlugin.h"


IMPLEMENT_PLUGIN(ChaChaToolPlugin)

void ChaChaToolPlugin::Init()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// merge menu
	CMenu append;
	append.LoadMenu(IDR_MENU1);
	MergeMenu(&append, TRUE);
}

void ChaChaToolPlugin::Query(CPluginInfo& plugininfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	plugininfo.m_strName = _T("ChaCha Cipher");
	plugininfo.m_strBlurb = _T("ChaCha加密解密工具");
	plugininfo.m_strHelp = _T("暂无");
	plugininfo.m_strAuthor = _T("wangzha");
	plugininfo.m_strCopyRight = _T("Copyright wangzha");
	plugininfo.m_strDate = _T("2017.12.11");
	plugininfo.m_strMenuLabel = "插件";
}