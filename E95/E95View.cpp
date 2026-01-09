// E95View.cpp : implementation of the CE95View class
//

#include "stdafx.h"
#include "E95.h"

#include "E95Doc.h"
#include "E95View.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CE95View

IMPLEMENT_DYNCREATE(CE95View, CFormView)

BEGIN_MESSAGE_MAP(CE95View, CFormView)
	//	ON_COMMAND(ID_TOOL_FULLSCREEN, OnToolFullscreen)
END_MESSAGE_MAP()

// CE95View construction/destruction

CE95View::CE95View()
	: CFormView(CE95View::IDD) {}

CE95View::~CE95View() {}

void CE95View::DoDataExchange(CDataExchange* pDX) {
	CFormView::DoDataExchange(pDX);
}

BOOL CE95View::PreCreateWindow(CREATESTRUCT& cs) {
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

void CE95View::OnInitialUpdate() {
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();
}

// CE95View diagnostics

#ifdef _DEBUG
void CE95View::AssertValid() const {
	CFormView::AssertValid();
}

void CE95View::Dump(CDumpContext& dc) const {
	CFormView::Dump(dc);
}

CE95Doc* CE95View::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CE95Doc)));
	return (CE95Doc*)m_pDocument;
}
#endif //_DEBUG

// CE95View message handlers