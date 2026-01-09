
// HOUGHView.cpp : implementation of the CHOUGHView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "HOUGH.h"
#endif

#include "HOUGHDoc.h"
#include "HOUGHView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CHOUGHView

IMPLEMENT_DYNCREATE(CHOUGHView, CFormView)

BEGIN_MESSAGE_MAP(CHOUGHView, CFormView)
END_MESSAGE_MAP()

// CHOUGHView construction/destruction

CHOUGHView::CHOUGHView() noexcept
	: CFormView(IDD_HOUGH_FORM)
{
	// TODO: add construction code here

}

CHOUGHView::~CHOUGHView()
{
}

void CHOUGHView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BOOL CHOUGHView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

void CHOUGHView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

}


// CHOUGHView diagnostics

#ifdef _DEBUG
void CHOUGHView::AssertValid() const
{
	CFormView::AssertValid();
}

void CHOUGHView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CHOUGHDoc* CHOUGHView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CHOUGHDoc)));
	return (CHOUGHDoc*)m_pDocument;
}
#endif //_DEBUG


// CHOUGHView message handlers
