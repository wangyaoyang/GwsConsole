// GWSPropertyGridCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "GwsConsole.h"
#include "GWSPropertyGridCtrl.h"
#include "UBootPropertyGridProp.h"

// CGWSPropertyGridCtrl

IMPLEMENT_DYNAMIC(CGWSPropertyGridCtrl, CMFCPropertyGridCtrl)

CGWSPropertyGridCtrl::CGWSPropertyGridCtrl()
{

}

CGWSPropertyGridCtrl::~CGWSPropertyGridCtrl()
{
}


BEGIN_MESSAGE_MAP(CGWSPropertyGridCtrl, CMFCPropertyGridCtrl)
END_MESSAGE_MAP()



// CGWSPropertyGridCtrl 消息处理程序
void CGWSPropertyGridCtrl::m_InitList() {
	CGwsConsoleApp*	pApp = (CGwsConsoleApp*) AfxGetApp();
	CGwsProperties&	props = pApp->m_props;

	CUBootPropertyGridProp* pGroup3 = new CUBootPropertyGridProp(_T("Configuration"));
	/* Color Configuration */
	CMFCPropertyGridColorProperty* pColorProp = new CMFCPropertyGridColorProperty(
		PROPERTY_BACKGROUND_COLOR, props.m_oColor.V(), NULL, _T("Specifies the window color"));
	pColorProp->EnableOtherButton(_T("Other..."));
	pColorProp->EnableAutomaticButton(_T("Default"), ::GetSysColor(COLOR_3DFACE));
	pGroup3->AddSubItem(pColorProp);
	/* Font Configuration */
	LOGFONT lf;
	CFont* font = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	font->GetLogFont(&lf);
	lstrcpy(lf.lfFaceName, props.m_oFont.V());

	CMFCPropertyGridFontProperty* pFontProp = new CMFCPropertyGridFontProperty(
		PROPERTY_TEXT_FONT, lf, CF_EFFECTS|CF_SCREENFONTS, _T("Specifies the font for the window"));
	pGroup3->AddSubItem(pFontProp);
	/* IP Address Configuration */
	CMFCPropertyGridProperty* pTargetIP = new CMFCPropertyGridProperty(
		PROPERTY_TARGET_IP, (_variant_t) props.m_oTargetIP.V(), _T("Specifies the target device IP address"));
	pGroup3->AddSubItem(pTargetIP);
	props.m_oTargetIP.m_pProp = pTargetIP;

	this->AddProperty(pGroup3);
	///////////////////////////////////////////////////////////////////////////////////////////
	CUBootPropertyGridProp* pGroupScripts = new CUBootPropertyGridProp(PROPERTY_GROUP_SCRIPTS);

	for (int i = 0; i < MAX_SCRIPTS; i ++) {
		CGwsScript&	script = props.m_scripts[i];
		CString		szScript = script.m_szScript;
		CObList&	list = script.m_oCommands;
		if (!szScript.IsEmpty()) {
			CUBootPropertyGridProp* pGroupScript	= new CUBootPropertyGridProp(szScript);

			for (POSITION pos = list.GetHeadPosition(); pos; list.GetNext(pos)) {
				CGwsCommand* command = (CGwsCommand*) list.GetAt(pos);
				pGroupScript->AddSubItem(new CUBootPropertyGridProp(command->m_szIndex,
					(_variant_t)command->m_szValue, szScript + _T(" - ") + command->m_szIndex +
																_T(" - ") + command->m_szValue));
				if (command->m_szExpect.IsEmpty()) continue;
				pGroupScript->AddSubItem(new CUBootPropertyGridProp(PROPERTY_EXPECT	,
					(_variant_t) command->m_szExpect, _T("Expected echo string to move on next step")));
			}
			pGroupScript->Expand(FALSE);
			pGroupScripts->AddSubItem(pGroupScript);
		}
	}
	for (int i = 0; i < MAX_USER_SCRIPTS; i ++) {
		CGwsScript&	script = props.m_sc_user[i];
		CString		szScript = script.m_szScript;
		CObList&	list = script.m_oCommands;
		if (!szScript.IsEmpty()) {
			CUBootPropertyGridProp* pGroupScript	= new CUBootPropertyGridProp(szScript);

			for (POSITION pos = list.GetHeadPosition(); pos; list.GetNext(pos)) {
				CGwsCommand* command = (CGwsCommand*) list.GetAt(pos);
				pGroupScript->AddSubItem(new CUBootPropertyGridProp(command->m_szIndex,
					(_variant_t)command->m_szValue, szScript + _T(" - ") + command->m_szIndex +
																_T(" - ") + command->m_szValue));
				if (command->m_szExpect.IsEmpty()) continue;
				pGroupScript->AddSubItem(new CUBootPropertyGridProp(PROPERTY_EXPECT	,
					(_variant_t) command->m_szExpect, _T("Expected echo string to move on next step")));
			}
			pGroupScript->Expand(FALSE);
			pGroupScripts->AddSubItem(pGroupScript);
		}
	}
	pGroupScripts->Expand(TRUE);
	this->AddProperty(pGroupScripts);
}

void CGWSPropertyGridCtrl::OnPropertyChanged(CMFCPropertyGridProperty* pProp) const
{
	CGwsConsoleApp*	pApp = (CGwsConsoleApp*) AfxGetApp();
	const CString	szName = pProp->GetName();
	if (szName == PROPERTY_BACKGROUND_COLOR) {
		pApp->m_props.m_oColor = pProp->GetValue().intVal;
	} else if (szName == PROPERTY_TEXT_FONT) {
		pApp->m_props.m_oFont = pProp->GetValue();
	} else if (szName == PROPERTY_TARGET_IP) {
		pApp->m_props.m_oTargetIP = pProp->GetValue();
	} else {
		CMFCPropertyGridProperty*	pParent = pProp->GetParent();
		if (pParent) {
			CString	szParent = pParent->GetName();
			for (int i = 0; i < MAX_SCRIPTS; i ++) {
				CGwsScript&	script = pApp->m_props.m_scripts[i];
				if (script.m_szScript == szParent) {
					CObList&	list = script.m_oCommands;
					for (POSITION pos = list.GetHeadPosition(); pos; list.GetNext(pos)) {
						CGwsCommand* command = (CGwsCommand*) list.GetAt(pos);
						if (command->m_szIndex == pProp->GetName()) {
							command->m_szValue = pProp->GetValue();
							return CMFCPropertyGridCtrl::OnPropertyChanged(pProp);
						}
					}
				}
			}
			for (int i = 0; i < MAX_USER_SCRIPTS; i ++) {
				CGwsScript&	script = pApp->m_props.m_sc_user[i];
				if (script.m_szScript == szParent) {
					CObList&	list = script.m_oCommands;
					for (POSITION pos = list.GetHeadPosition(); pos; list.GetNext(pos)) {
						CGwsCommand* command = (CGwsCommand*) list.GetAt(pos);
						if (command->m_szIndex == pProp->GetName()) {
							command->m_szValue = pProp->GetValue();
							return CMFCPropertyGridCtrl::OnPropertyChanged(pProp);
						}
					}
				}
			}
		}
	}
	return CMFCPropertyGridCtrl::OnPropertyChanged(pProp);
}

