#pragma once

#ifdef _WIN32_WCE
#error "Windows CE ��֧�� CHtmlView��"
#endif 

// CGwsWebServerView Html ��ͼ

class CGwsConsoleDoc;
class CGwsWebServerView : public CHtmlView
{
	DECLARE_DYNCREATE(CGwsWebServerView)

protected:
	CGwsWebServerView();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual ~CGwsWebServerView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	CGwsConsoleDoc* GetDocument() const;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
};

#define	WEB_SERVER_URL	_T("http://192.168.1.11")

#ifndef _DEBUG  // debug version in GwsConsoleView.cpp
inline CGwsConsoleDoc* CGwsWebServerView::GetDocument() const
   { return reinterpret_cast<CGwsConsoleDoc*>(m_pDocument); }
#endif
