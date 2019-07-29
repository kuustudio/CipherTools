#include "stdafx.h"
#include "EcdhToolPlugin.h"


IMPLEMENT_PLUGIN(EcdhToolPlugin)

void EcdhToolPlugin::Init()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// merge menu
	CMenu append;
	append.LoadMenu(IDR_MENU1);
	MergeMenu(&append, TRUE);
}

void EcdhToolPlugin::Query(CPluginInfo& plugininfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	plugininfo.m_strName = _T("Ecdh Cipher");
	plugininfo.m_strBlurb = _T("Ecdh密钥交互协议");
	plugininfo.m_strHelp = _T("暂无");
	plugininfo.m_strAuthor = _T("wangzha");
	plugininfo.m_strCopyRight = _T("Copyright wangzha");
	plugininfo.m_strDate = _T("2017.11.27");
	plugininfo.m_strMenuLabel = "插件";
}