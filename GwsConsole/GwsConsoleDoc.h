
// GwsConsoleDoc.h : interface of the CGwsConsoleDoc class
//


#pragma once

class CGwsConsoleView;
class CGwsWebServerView;
class CGwsConsoleDoc : public CRichEditDoc
{
	friend class CGwsConsoleView;
	friend class CGwsWebServerView;
protected: // create from serialization only
	CGwsConsoleDoc();
	DECLARE_DYNCREATE(CGwsConsoleDoc)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual CRichEditCntrItem* CreateClientItem(REOBJECT* preo) const;
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// Implementation
public:
	virtual ~CGwsConsoleDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
private:
	CString				m_szLastCommand;
protected:
	CString				m_szContent;
	CGwsConsoleView*	m_pEditView;
	CGwsWebServerView*	m_pHtmlView;
public:
	afx_msg void OnServerStart();
	afx_msg void OnUpdateServerStart(CCmdUI *pCmdUI);
	afx_msg void OnServerWeb();
	afx_msg void OnUpdateServerWeb(CCmdUI *pCmdUI);
};
