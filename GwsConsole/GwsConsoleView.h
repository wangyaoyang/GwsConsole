
// GwsConsoleView.h : interface of the CGwsConsoleView class
//

#pragma once

class CGwsConsoleCntrItem;
class CGwsConsoleDoc;
class CGwsConsoleView : public CRichEditView
{
	friend class CGwsConsoleDoc;
protected: // create from serialization only
	CGwsConsoleView();
	DECLARE_DYNCREATE(CGwsConsoleView)

// Attributes
private:
	int		m_key_pressed;
private:
	bool	m_bWaiting;
	long	m_nLastCaret;
	long	m_nExpecting;
	bool	m_bExpecting;
	bool	m_bExpected;
	CString	m_szExpecting;
	CString	m_szExpected;
public:
	CGwsConsoleDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CGwsConsoleView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

private:
	void	m_ClearExpect();
	void	m_SetupExpect(CString szExpecting);
	bool	m_ParseExpect(CString& szCommand);
	void	m_AppendText(CString szText);
	void	m_AppendRecv(CString szRecv);
	long	m_SetCaretBack();
	long	m_GetCaretPos();
	long	m_SetCaretPos(long nIndex);
	long	m_SetCaretPos(int x,int y);
	long	m_SetCaretBOL();
	long	m_GetCaretEnd();
	long	m_SetCaretEnd();
	long	m_GetCaretEOF();
	void	m_SetCaretEOF();
	void	m_GetCaretLineRange(long& nLineHead,long& nLineTail);
	int		m_ParseFont(CString& szFont);
	CString	m_PickupCommand();
	CString	m_GetSelectedText(long& nStartChar,long& nTextLen);

protected:
	void	m_NotifyNetwork(int result);
// Generated message map functions
protected:
	afx_msg void OnDestroy();
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnServerCancel();
public:
};

#ifndef _DEBUG  // debug version in GwsConsoleView.cpp
inline CGwsConsoleDoc* CGwsConsoleView::GetDocument() const
   { return reinterpret_cast<CGwsConsoleDoc*>(m_pDocument); }
#endif

