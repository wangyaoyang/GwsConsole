
// GwsConsoleView.cpp : implementation of the CGwsConsoleView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "GwsConsole.h"
#endif

#include "GwsConsoleDoc.h"
#include "CntrItem.h"
#include "resource.h"
#include "MainFrm.h"
#include "GwsConsoleView.h"
#include "GwsWebServerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define		COMMAND_TIMEOUT		60
#define		TIMER_INTERVAL		100
#define		TIMER_TICKS_PER_SEC	(1000 / TIMER_INTERVAL)

// CGwsConsoleView

IMPLEMENT_DYNCREATE(CGwsConsoleView, CRichEditView)

BEGIN_MESSAGE_MAP(CGwsConsoleView, CRichEditView)
	ON_WM_DESTROY()
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CRichEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CRichEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CGwsConsoleView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_TIMER()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_KEYUP()
	ON_WM_CHAR()
	ON_COMMAND(ID_SERVER_CANCEL, &CGwsConsoleView::OnServerCancel)
END_MESSAGE_MAP()

// CGwsConsoleView construction/destruction

CGwsConsoleView::CGwsConsoleView()
{
	m_bWaiting		= false;
	m_nLastCaret	= -1;
	m_key_pressed	= 0;
	this->m_ClearExpect();
}

CGwsConsoleView::~CGwsConsoleView()
{
}

BOOL CGwsConsoleView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CRichEditView::PreCreateWindow(cs);
}

int CGwsConsoleView::m_ParseFont(CString& szFont) {
	int left_brackets = szFont.Find('(');
	int	right_brackets = szFont.Find(')');
	if (0 < left_brackets && left_brackets < right_brackets) {
		CString	szRight = szFont.Mid(left_brackets+1,right_brackets-left_brackets-1);
		szFont = szFont.Left(left_brackets);
		int size = 20 * STRTOINT(szRight.GetBuffer());
		return (size == 0) ? 180 : size;
	}
	return 180;
}

void CGwsConsoleView::OnInitialUpdate()
{
	CRichEditView::OnInitialUpdate();

	CHARFORMAT		cf;
	CGwsConsoleApp*	pApp = (CGwsConsoleApp*) AfxGetApp();
	CGwsProperties&	props = pApp->m_props;
	CString			szFont = props.m_oFont.V();

	CMainFrame*	pMainFrame = (CMainFrame*) AfxGetMainWnd();
	pMainFrame->m_wndProperties.m_AttachConsoleView(*this);
	this->GetDocument()->m_pEditView = this;
	this->SetTimer(0x20170606,TIMER_INTERVAL,NULL);
	this->GetRichEditCtrl().GetDefaultCharFormat(cf);
	cf.dwEffects = 0;
	cf.crTextColor = RGB(	~GetRValue(props.m_oColor.V()),
							~GetGValue(props.m_oColor.V()),
							~GetBValue(props.m_oColor.V()));
	cf.dwMask |= CFM_SIZE;	//设置字体大小
	cf.dwMask |= CFM_FACE;	//设置字体
	cf.yHeight = m_ParseFont(szFont);		//字体大小
	STR_CPY((CHAR_T*)(cf.szFaceName), 32, szFont.GetBuffer()); //设置字体名称
	this->GetRichEditCtrl().SetDefaultCharFormat(cf);
	this->GetRichEditCtrl().SetBackgroundColor(FALSE,props.m_oColor.V());

	// Set the printing margins (720 twips = 1/2 inch)
	SetMargins(CRect(720, 720, 720, 720));
	this->m_AppendText(this->GetDocument()->m_szContent);
}


// CGwsConsoleView printing


void CGwsConsoleView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CGwsConsoleView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}


void CGwsConsoleView::OnDestroy()
{
	// Deactivate the item on destruction; this is important
	// when a splitter view is being used
   COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
   if (pActiveItem != NULL && pActiveItem->GetActiveView() == this)
   {
      pActiveItem->Deactivate();
      ASSERT(GetDocument()->GetInPlaceActiveItem(this) == NULL);
   }
   CRichEditView::OnDestroy();
}


void CGwsConsoleView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CGwsConsoleView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CGwsConsoleView diagnostics

#ifdef _DEBUG
void CGwsConsoleView::AssertValid() const
{
	CRichEditView::AssertValid();
}

void CGwsConsoleView::Dump(CDumpContext& dc) const
{
	CRichEditView::Dump(dc);
}

CGwsConsoleDoc* CGwsConsoleView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGwsConsoleDoc)));
	return (CGwsConsoleDoc*)m_pDocument;
}
#endif //_DEBUG


// CGwsConsoleView message handlers
void CGwsConsoleView::m_AppendText(CString szText) {
	if (!szText.IsEmpty()) {
		CRichEditCtrl&	ctrl = GetRichEditCtrl();
		long			nLength = ctrl.GetTextLength();
		long			nStart = 0, nEnd = 0;
	
		ctrl.GetSel (nStart,nEnd);
		ctrl.SetSel (nLength,nLength );
		ctrl.ReplaceSel (szText);
		ctrl.SetSel (nStart,nEnd);
		this->m_SetCaretEOF();
	}
}

void CGwsConsoleView::m_AppendRecv(CString szRecv) {
	CString	szText;
	CHAR_T c = 0;
	CHAR_T* sRecv = szRecv.GetBuffer();
	int nRecv = szRecv.GetLength();
	for (CHAR_T* p = sRecv; (c = *p); p ++) {
		if (c < ' ' && c != 0x0d && c != 0x0a) {	//is a control code
			*p = 0;
			this->m_AppendText(sRecv);
			this->m_SetCaretBack();
			sRecv = p + 1;
		}
	}
	this->m_AppendText(sRecv);
}

void CGwsConsoleView::m_ClearExpect() {
	this->m_nExpecting	= 0;
	this->m_bExpecting	= false;
	this->m_bExpected	= false;
	if (!this->m_szExpecting.IsEmpty()) m_szExpecting.Empty();
	if (!this->m_szExpected.IsEmpty()) m_szExpected.Empty();
}

void CGwsConsoleView::m_SetupExpect(CString szExpecting) {
	this->m_ClearExpect();
	this->m_bExpecting = true;
	this->m_szExpecting = szExpecting;
}

bool CGwsConsoleView::m_ParseExpect(CString& szCommand) {
	int	left = szCommand.Find('[');
	int right = szCommand.Find(']');
	if (0 <= left && left < right) {	//expecting
		this->m_SetupExpect(szCommand.Mid(left + 1,right-left-1));
		return true;
	}
	return false;
}

void CGwsConsoleView::OnTimer(UINT_PTR nIDEvent)
{
	if (0x20170606 == nIDEvent) {
		CGwsConsoleApp*	pApp = (CGwsConsoleApp*) AfxGetApp();

		if (pApp->m_IsNetworkReady()) {
			CString	szRecv = pApp->m_UdpRecv();
			if (!szRecv.IsEmpty()) {
				CGwsConsoleDoc*	pDoc = GetDocument();
				if (pDoc->m_pHtmlView->IsWindowVisible() && szRecv.Find('>') >= 0) {
					pDoc->m_pHtmlView->ShowWindow(SW_HIDE);
					pDoc->m_pEditView->ShowWindow(SW_SHOWMAXIMIZED);
					pDoc->m_pEditView->SetFocus();
				}
				if (this->m_bWaiting) {
					this->m_bWaiting = false;
					this->GetRichEditCtrl().SetWindowText(_T(""));
				}
				this->m_AppendRecv(szRecv);
			}
			if (this->m_bWaiting) {
				static int nCounter;
				const int COUNTER = (1000 / TIMER_INTERVAL);
				CString	szCounter;
				nCounter ++;
				nCounter %= 10 * COUNTER;
				if (0 == (nCounter % 10)) {
					if (0 == nCounter) {
						this->GetRichEditCtrl().SetWindowText(_T(""));
					}
					szCounter.Format(_T("%d "),nCounter / 10);
					this->GetRichEditCtrl().ReplaceSel(szCounter);
				}
			} else {
				CGwsConsoleApp*	pApp = (CGwsConsoleApp*) AfxGetApp();
				if (this->m_bExpecting) {
					this->m_nExpecting ++;
					if (!szRecv.IsEmpty()) {
						this->m_szExpected += szRecv;
						if (this->m_szExpected.Find(this->m_szExpecting) >= 0) {
							this->m_ClearExpect();	//we got what we want
							this->m_bExpected = true;
						}
					}
					if (this->m_nExpecting > COMMAND_TIMEOUT * TIMER_TICKS_PER_SEC) {	//time out
						this->m_AppendText(_T("Did not get expected echo \"") +
										this->m_szExpecting + _T("\". Script has been canceled.\n"));
						this->m_ClearExpect();			//giving up
						pApp->m_szScripts.RemoveAll();	//cancel all buffered commands
						pApp->m_UdpSend(_T("\n"));
					}
				} else {
					this->m_ClearExpect();
					if (!pApp->m_szScripts.IsEmpty()) {
						CString	szCommand = pApp->m_szScripts.RemoveHead();
						CString	szExpected = pApp->m_szScripts.IsEmpty() ? _T("") : pApp->m_szScripts.GetHead();

						if (!this->m_ParseExpect(szCommand)) {	//this is a command
							if (this->m_ParseExpect(szExpected)) {	//this is the expected string
								pApp->m_szScripts.RemoveHead();
							}
							//this->m_AppendText(szCommand + _T("\n"));
							pApp->m_UdpSend(szCommand);
						}	//otherwise this is the expected string, do nothing
						if (pApp->m_szScripts.IsEmpty())
							this->m_ClearExpect();
					}
				}
			}
		}
	}

	CRichEditView::OnTimer(nIDEvent);
}

long CGwsConsoleView::m_GetCaretPos()
{
	long	nStart = 0,nEnd = 0;
	GetRichEditCtrl().GetSel( nStart,nEnd );
	return nStart;
}

long CGwsConsoleView::m_SetCaretPos(long nIndex)
{
	GetRichEditCtrl().SetSel( nIndex,nIndex );
	return nIndex;
}

long CGwsConsoleView::m_SetCaretPos(int x,int y)
{	//x,y 是行号、列号, 1 基索引
	CRichEditCtrl&	ctrl = GetRichEditCtrl();
	int				nLineCount = ctrl.GetLineCount();
	if (y > nLineCount)
		y = nLineCount;
	int	nMaxColumn = ctrl.LineLength( ctrl.LineIndex(y-1) );
	if (x > nMaxColumn)
		x = nMaxColumn;
	int	nPosition = ctrl.LineIndex(y-1) + (x-1);
	if( nPosition < 0 ) nPosition = 0;
	m_SetCaretPos( nPosition );
	return m_GetCaretPos();
}

long CGwsConsoleView::m_SetCaretBOL()
{	//x,y 是行号、列号, 1 基索引
	CRichEditCtrl&	ctrl = GetRichEditCtrl();
	int	nPosition = ctrl.LineIndex(-1);
	if( nPosition < 0 ) nPosition = 0;
	m_SetCaretPos( nPosition );
	return m_GetCaretPos();
}

long CGwsConsoleView::m_GetCaretEnd()
{
	long	nCurrent = m_GetCaretPos();
	long	nLastPos = m_SetCaretEnd();
	m_SetCaretPos( nCurrent );
	return nLastPos;
}

long CGwsConsoleView::m_GetCaretEOF() {	return this->m_nLastCaret;	}
void CGwsConsoleView::m_SetCaretEOF() {
	this->m_SetCaretEnd();
	this->m_nLastCaret = this->m_GetCaretEnd();
}

long CGwsConsoleView::m_SetCaretBack()
{
	CRichEditCtrl&	ctrl = GetRichEditCtrl();
	long			nLength = ctrl.GetTextLength();
	long			nStart = 0, nEnd = 0;

	m_GetCaretLineRange(nStart,nEnd);
	ctrl.SetSel( nStart,nEnd);
	ctrl.ReplaceSel(_T(""));
	return (nLength - 1);
}

long CGwsConsoleView::m_SetCaretEnd()
{
	CRichEditCtrl&	ctrl = GetRichEditCtrl();
	long			nLength = ctrl.GetTextLength();
	long			nStart = 0, nEnd = 0;

	ctrl.SetSel( nLength,nLength );
	ctrl.GetSel( nStart,nEnd );
	return nEnd;
}

void CGwsConsoleView::m_GetCaretLineRange(long& nLineHead,long& nLineTail)
{
	nLineHead = GetRichEditCtrl().LineIndex(-1);
	nLineTail = nLineHead + GetRichEditCtrl().LineLength(nLineHead);
}

CString CGwsConsoleView::m_GetSelectedText(long& nStartChar,long& nTextLen)
{
	CRichEditCtrl&	ctrl = GetRichEditCtrl();
	long		nEndChar = 0;
	//CString		szText;
	//wchar_t*	sLineBuff = NULL;

	ctrl.GetSel( nStartChar, nEndChar );
	nTextLen = nEndChar - nStartChar;
	//sLineBuff = new wchar_t[nTextLen+1];
	//wmemset( sLineBuff,0x00,nTextLen+1 );
	//ctrl.GetSelText((LPSTR)sLineBuff);
	//szText = sLineBuff;
	//delete [] sLineBuff;
	//return szText;
	return ctrl.GetSelText();
}

CString CGwsConsoleView::m_PickupCommand()
{
	CString	szCommand;
	long	nLineHead = 0, nLineTail = 0;
	long	nPosition = this->m_GetCaretEOF();

	this->m_SetCaretEOF();								//光标设置在命令行结束的位置
	this->m_GetCaretLineRange( nLineHead,nLineTail );	//取当前行的行首、行尾索引
	if( nLineHead <= nPosition && nPosition < nLineTail ) {
		if( GetRichEditCtrl().GetTextRange( nPosition,nLineTail,szCommand ) > 0 )
			return szCommand;
	}
	return szCommand;
}

void CGwsConsoleView::OnServerCancel()
{
	CGwsConsoleApp*	pApp = (CGwsConsoleApp*) AfxGetApp();
	pApp->m_UdpSend(_T("\03"));
}

void CGwsConsoleView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CGwsConsoleApp*	pApp = (CGwsConsoleApp*) AfxGetApp();
	long	nStartChar = 0, nTextLen = 0;
	CString	szSel;

	m_key_pressed = nChar;
	switch (nChar) {
	case VK_RETURN:
		szSel = m_GetSelectedText(nStartChar,nTextLen);
		if (szSel.IsEmpty()) {
			szSel = m_PickupCommand();
		}
		pApp->m_UdpSend(szSel);
		break;
	case VK_CONTROL:
		break;
	default:;
	}

	CRichEditView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CGwsConsoleView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	m_key_pressed = nChar;
	m_key_pressed *= (-1);

	CRichEditView::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CGwsConsoleView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CRichEditView::OnChar(nChar, nRepCnt, nFlags);
}

void CGwsConsoleView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CRichEditView::OnLButtonDown(nFlags, point);
}

void CGwsConsoleView::OnLButtonUp(UINT nFlags, CPoint point)
{
	CRichEditCtrl&	ctrl = GetRichEditCtrl();
	long			nStart,nEnd;

	CRichEditView::OnLButtonUp(nFlags, point);

	ctrl.GetSel(nStart,nEnd);
	if (nStart >= nEnd)
		this->m_SetCaretEnd();
}

void CGwsConsoleView::m_NotifyNetwork(int result) {
	switch (result) {
	case 0:
		//this->MessageBox(_T("Failed on listening UDP"),_T("Error!"),MB_ICONSTOP);
		break;
	case 1:
		this->m_bWaiting = true;
		//this->MessageBox(_T("Ready, waiting for connection ..."),_T("OK"),MB_OK);
		break;
	default:;
	}
}

