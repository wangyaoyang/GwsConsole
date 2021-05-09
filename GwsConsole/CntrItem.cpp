
// CntrItem.cpp : implementation of the CGwsConsoleCntrItem class
//

#include "stdafx.h"
#include "GwsConsole.h"

#include "GwsConsoleDoc.h"
#include "GwsConsoleView.h"
#include "CntrItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CGwsConsoleCntrItem implementation

IMPLEMENT_SERIAL(CGwsConsoleCntrItem, CRichEditCntrItem, 0)

CGwsConsoleCntrItem::CGwsConsoleCntrItem(REOBJECT* preo, CGwsConsoleDoc* pContainer)
	: CRichEditCntrItem(preo, pContainer)
{
	// TODO: add one-time construction code here
}

CGwsConsoleCntrItem::~CGwsConsoleCntrItem()
{
	// TODO: add cleanup code here
}


// CGwsConsoleCntrItem diagnostics

#ifdef _DEBUG
void CGwsConsoleCntrItem::AssertValid() const
{
	CRichEditCntrItem::AssertValid();
}

void CGwsConsoleCntrItem::Dump(CDumpContext& dc) const
{
	CRichEditCntrItem::Dump(dc);
}
#endif

