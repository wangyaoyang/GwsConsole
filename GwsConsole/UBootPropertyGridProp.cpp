// GWSPropertyGridCtrl.cpp : 实现文件
//

#include "StdAfx.h"
#include "GwsConsole.h"
#include "GWSPropertyGridCtrl.h"
#include "UBootPropertyGridProp.h"

CUBootPropertyGridProp::CUBootPropertyGridProp(const CString& strGroupName, DWORD_PTR dwData, BOOL bIsValueList)
	: CMFCPropertyGridProperty(strGroupName, dwData, bIsValueList)
{
}

CUBootPropertyGridProp::CUBootPropertyGridProp(const CString& strName, const COleVariant& varValue, LPCTSTR lpszDescr,
							DWORD_PTR dwData, LPCTSTR lpszEditMask, LPCTSTR lpszEditTemplate, LPCTSTR lpszValidChars)
	: CMFCPropertyGridProperty(strName, varValue, lpszDescr, dwData, lpszEditMask, lpszEditTemplate, lpszValidChars)
{
}

CUBootPropertyGridProp::~CUBootPropertyGridProp(void)
{
}

static int static_nClicks;
void CUBootPropertyGridProp::OnClickName(CPoint C)
{
	if (this->IsExpanded() && this->GetSubItemsCount() > 0 && static_nClicks ++ > 1) {
		CMFCPropertyGridProperty*	pParent = this->GetParent();
		CString						szGrandName,szParentName;

		static_nClicks = 0;
		if (pParent && CString(pParent->GetName()) == PROPERTY_GROUP_SCRIPTS) {
			CGwsConsoleApp*	pApp = (CGwsConsoleApp*) AfxGetApp();
			//pApp->m_RunScript(this->GetName(),_T(""));
		}	//this is a script
	}
}

void CUBootPropertyGridProp::OnClickButton(CPoint point)
{
}

BOOL CUBootPropertyGridProp::OnClickValue(UINT uiMsg, CPoint point)
{
	return TRUE;
}

void CUBootPropertyGridProp::OnRClickName(CPoint /*point*/)
{
}

void CUBootPropertyGridProp::OnRClickValue(CPoint /*point*/, BOOL /*bSelChanged*/)
{
}

BOOL CUBootPropertyGridProp::OnDblClk(CPoint point)
{
	CMFCPropertyGridProperty*	pParent = this->GetParent();

	static_nClicks = 0;
	if (pParent && !CString(pParent->GetName()).IsEmpty()) {
		CGwsConsoleApp*	pApp = (CGwsConsoleApp*) AfxGetApp();
		pApp->m_RunScript(pParent->GetName(),this->GetName());
	}	//this is a entry
	return FALSE;
}
