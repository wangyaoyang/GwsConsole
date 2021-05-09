
// CntrItem.h : interface of the CGwsConsoleCntrItem class
//

#pragma once

class CGwsConsoleDoc;
class CGwsConsoleView;

class CGwsConsoleCntrItem : public CRichEditCntrItem
{
	DECLARE_SERIAL(CGwsConsoleCntrItem)

// Constructors
public:
	CGwsConsoleCntrItem(REOBJECT* preo = NULL, CGwsConsoleDoc* pContainer = NULL);
		// Note: pContainer is allowed to be NULL to enable IMPLEMENT_SERIALIZE
		//  IMPLEMENT_SERIALIZE requires the class have a constructor with
		//  zero arguments.  Normally, OLE items are constructed with a
		//  non-NULL document pointer

// Attributes
public:
	CGwsConsoleDoc* GetDocument()
		{ return reinterpret_cast<CGwsConsoleDoc*>(CRichEditCntrItem::GetDocument()); }
	CGwsConsoleView* GetActiveView()
		{ return reinterpret_cast<CGwsConsoleView*>(CRichEditCntrItem::GetActiveView()); }

// Implementation
public:
	~CGwsConsoleCntrItem();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};

