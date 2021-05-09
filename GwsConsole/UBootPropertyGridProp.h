#pragma once
#include "afxpropertygridctrl.h"

class CUBootPropertyGridProp : public CMFCPropertyGridProperty
{
public:
	CUBootPropertyGridProp(const CString& strGroupName, DWORD_PTR dwData = 0, BOOL bIsValueList = FALSE);
	CUBootPropertyGridProp(const CString& strName, const COleVariant& varValue, LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0,
	LPCTSTR lpszEditMask = NULL, LPCTSTR lpszEditTemplate = NULL, LPCTSTR lpszValidChars = NULL);
	~CUBootPropertyGridProp(void);
public:
	virtual void OnClickName(CPoint C);
	virtual void OnClickButton(CPoint point);
	virtual BOOL OnClickValue(UINT uiMsg, CPoint point);
	virtual BOOL OnDblClk(CPoint point);
	virtual void OnRClickName(CPoint /*point*/);
	virtual void OnRClickValue(CPoint /*point*/, BOOL /*bSelChanged*/);
};