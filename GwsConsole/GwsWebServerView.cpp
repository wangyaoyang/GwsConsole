// GwsWebServerView.cpp : 实现文件
//

#include "stdafx.h"
#include "GwsConsole.h"
#include "GwsConsoleDoc.h"
#include "GwsWebServerView.h"

// CGwsWebServerView
IMPLEMENT_DYNCREATE(CGwsWebServerView, CHtmlView)

CGwsWebServerView::CGwsWebServerView()
{

}

CGwsWebServerView::~CGwsWebServerView()
{
}

void CGwsWebServerView::DoDataExchange(CDataExchange* pDX)
{
	CHtmlView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CGwsWebServerView, CHtmlView)
END_MESSAGE_MAP()


// CGwsWebServerView 诊断

#ifdef _DEBUG
void CGwsWebServerView::AssertValid() const
{
	CHtmlView::AssertValid();
}

void CGwsWebServerView::Dump(CDumpContext& dc) const
{
	CHtmlView::Dump(dc);
}

CGwsConsoleDoc* CGwsWebServerView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGwsConsoleDoc)));
	return (CGwsConsoleDoc*)m_pDocument;
}
#endif //_DEBUG

// CGwsWebServerView 消息处理程序

void CGwsWebServerView::OnInitialUpdate()
{
	CHtmlView::OnInitialUpdate();

	this->GetDocument()->m_pHtmlView = this;
	this->Navigate2(WEB_SERVER_URL);
	this->ShowWindow(SW_HIDE);
}
