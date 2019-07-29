#include "stdafx.h"
#include "AesToolPlugin.h"


IMPLEMENT_PLUGIN(AesToolPlugin)

void AesToolPlugin::Init()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// merge menu
	CMenu append;
	append.LoadMenu(IDR_MENU1);
	MergeMenu(&append, TRUE);
}

void AesToolPlugin::Query(CPluginInfo& plugininfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	plugininfo.m_strName = _T("AES Cipher");
	plugininfo.m_strBlurb = _T("AES加密解密工具");
	plugininfo.m_strHelp = _T("暂无");
	plugininfo.m_strAuthor = _T("wangzha");
	plugininfo.m_strCopyRight = _T("Copyright wangzha");
	plugininfo.m_strDate = _T("2017.11.03");
	plugininfo.m_strMenuLabel = "插件";
}
