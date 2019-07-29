#include "stdafx.h"
#include "KeyToolPlugin.h"


IMPLEMENT_PLUGIN(CKeyToolPlugin)

void CKeyToolPlugin::Init()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// merge menu
	CMenu append;
	append.LoadMenu(IDR_MENU1);
	MergeMenu(&append, TRUE);
}

void CKeyToolPlugin::Query(CPluginInfo& plugininfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	plugininfo.m_strName = _T("Cipher Expand Test");
	plugininfo.m_strBlurb = _T("√‹‘ø¿©…¢À„∑®");
	plugininfo.m_strHelp = _T("‘›Œﬁ");
	plugininfo.m_strAuthor = _T("wangzha");
	plugininfo.m_strCopyRight = _T("Copyright wangzha");
	plugininfo.m_strDate = _T("2018.5.22");
	plugininfo.m_strMenuLabel = "≤Âº˛";
}