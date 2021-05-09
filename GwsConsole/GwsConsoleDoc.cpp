
// GwsConsoleDoc.cpp : implementation of the CGwsConsoleDoc class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "GwsConsole.h"
#endif

#include "GwsConsoleDoc.h"
#include "GwsConsoleView.h"
#include "GwsWebServerView.h"
#include "CntrItem.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CGwsConsoleDoc

IMPLEMENT_DYNCREATE(CGwsConsoleDoc, CRichEditDoc)

BEGIN_MESSAGE_MAP(CGwsConsoleDoc, CRichEditDoc)
	// Enable default OLE container implementation
	ON_UPDATE_COMMAND_UI(ID_OLE_EDIT_LINKS, &CRichEditDoc::OnUpdateEditLinksMenu)
	ON_COMMAND(ID_OLE_EDIT_LINKS, &CRichEditDoc::OnEditLinks)
	ON_UPDATE_COMMAND_UI_RANGE(ID_OLE_VERB_FIRST, ID_OLE_VERB_LAST, &CRichEditDoc::OnUpdateObjectVerbMenu)
	ON_COMMAND(ID_SERVER_START, &CGwsConsoleDoc::OnServerStart)
	ON_UPDATE_COMMAND_UI(ID_SERVER_START, &CGwsConsoleDoc::OnUpdateServerStart)
	ON_COMMAND(ID_SERVER_WEB, &CGwsConsoleDoc::OnServerWeb)
	ON_UPDATE_COMMAND_UI(ID_SERVER_WEB, &CGwsConsoleDoc::OnUpdateServerWeb)
END_MESSAGE_MAP()


// CGwsConsoleDoc construction/destruction

CGwsConsoleDoc::CGwsConsoleDoc()
{
	m_pEditView		= NULL;
	m_pHtmlView		= NULL;
}

CGwsConsoleDoc::~CGwsConsoleDoc()
{
}

BOOL CGwsConsoleDoc::OnNewDocument()
{
	if (!CRichEditDoc::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

CRichEditCntrItem* CGwsConsoleDoc::CreateClientItem(REOBJECT* preo) const
{
	return new CGwsConsoleCntrItem(preo, const_cast<CGwsConsoleDoc*>(this));
}




// CGwsConsoleDoc serialization

void CGwsConsoleDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		if (this->m_pEditView) {
			this->m_pEditView->GetRichEditCtrl().GetWindowText(this->m_szContent);
			ar << this->m_szContent;
		}
	}
	else
	{
		ar >> this->m_szContent;
	}

	// Calling the base class CRichEditDoc enables serialization
	//  of the container document's COleClientItem objects.
	// TODO: set CRichEditDoc::m_bRTF = FALSE if you are serializing as text
	CRichEditDoc::Serialize(ar);
}

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CGwsConsoleDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CGwsConsoleDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data. 
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CGwsConsoleDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CGwsConsoleDoc diagnostics

#ifdef _DEBUG
void CGwsConsoleDoc::AssertValid() const
{
	CRichEditDoc::AssertValid();
}

void CGwsConsoleDoc::Dump(CDumpContext& dc) const
{
	CRichEditDoc::Dump(dc);
}
#endif //_DEBUG


// CGwsConsoleDoc commands
void CGwsConsoleDoc::OnServerStart()
{
	if (this->m_pEditView && this->m_pHtmlView) {
		this->m_pHtmlView->ShowWindow(SW_HIDE);
		this->m_pEditView->ShowWindow(SW_SHOWMAXIMIZED);
		CGwsConsoleApp*	pApp = (CGwsConsoleApp*) AfxGetApp();
		if (pApp->m_IsNetworkReady()) {
			pApp->m_CloseNetwork();
			this->m_pEditView->m_NotifyNetwork(0);
		} else {
			bool bWaiting = pApp->m_InitNetwork(6666,6666);
			this->m_pEditView->m_NotifyNetwork(bWaiting);
		}
	}
}

void CGwsConsoleDoc::OnUpdateServerStart(CCmdUI *pCmdUI)
{
	CGwsConsoleApp*	pApp = (CGwsConsoleApp*) AfxGetApp();
	pCmdUI->SetCheck(pApp->m_IsNetworkReady());
}

void CGwsConsoleDoc::OnServerWeb()
{
	if (this->m_pHtmlView && this->m_pEditView) {
		if (this->m_pHtmlView->IsWindowVisible()) {
			this->m_pHtmlView->ShowWindow(SW_HIDE);
			this->m_pEditView->ShowWindow(SW_SHOWMAXIMIZED);
			return;
		}
		if (this->m_pEditView->IsWindowVisible()) {
			this->m_pEditView->ShowWindow(SW_HIDE);
			this->m_pHtmlView->ShowWindow(SW_SHOWMAXIMIZED);
			this->m_pHtmlView->Navigate2(WEB_SERVER_URL);
		}
	}
}

void CGwsConsoleDoc::OnUpdateServerWeb(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(this->m_pHtmlView->IsWindowVisible());
}
