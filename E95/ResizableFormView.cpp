// ResizableFormView.cpp : implementation file
//

#include "stdafx.h"
#include "ResizableFormView.h"
//#include "XTabCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CResizableFormView

IMPLEMENT_DYNCREATE(CResizableFormView, CFormView)

inline void CResizableFormView::Construct() {
	bWnd = FALSE;
	m_bInitDone = FALSE;

	m_bUseMinTrack = TRUE;
	m_bUseMaxTrack = FALSE;
	m_bUseMaxRect = FALSE;

	m_bShowGrip = TRUE;

	m_bEnableSaveRestore = FALSE;

	m_szGripSize.cx = GetSystemMetrics(SM_CXVSCROLL);
	m_szGripSize.cy = GetSystemMetrics(SM_CYHSCROLL);

	// jms - 11/28/00 - new code starts
	// initialize the typed pointer array
	m_tpaControls.SetSize(0, 25);
}

CResizableFormView::CResizableFormView()
	: CFormView(CResizableFormView::IDD) {
	Construct();
}

CResizableFormView::CResizableFormView(UINT nIDTemplate)
	: CFormView(nIDTemplate) {
	Construct();
}

CResizableFormView::~CResizableFormView() {
	// free memory used by the layout
	Layout* pl;

	POSITION pos = m_plLayoutList.GetHeadPosition();

	while (pos != NULL) {
		pl = (Layout*)m_plLayoutList.GetNext(pos);
		delete pl;
	}

	// jms - 11/28/00 - new code starts
	// Delete the array items we created.
	int nCount = m_tpaControls.GetSize();
	for (int i = 0; i < nCount; i++) {
		FORMCONTROL* pCtrl = m_tpaControls.GetAt(i);
		delete pCtrl;
		pCtrl = NULL;
		m_tpaControls.SetAt(i, NULL);
	}
	m_tpaControls.RemoveAll();
	m_tpaControls.FreeExtra();
	// jms - 11/28/00 - new code stops
}

void CResizableFormView::DoDataExchange(CDataExchange* pDX) {
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CResizableFormView)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CResizableFormView, CFormView)
	//{{AFX_MSG_MAP(CResizableFormView)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResizableFormView diagnostics

#ifdef _DEBUG
void CResizableFormView::AssertValid() const {
	CFormView::AssertValid();
}

void CResizableFormView::Dump(CDumpContext& dc) const {
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CResizableFormView message handlers

void CResizableFormView::OnSize(UINT nType, int cx, int cy) {
	CFormView::OnSize(nType, cx, cy);

	if (nType == SIZE_MAXHIDE || nType == SIZE_MAXSHOW) {
		return;		// arrangement not needed
	}

	if (m_bInitDone) {
		ArrangeLayout();
	}
}

void CResizableFormView::OnInitialUpdate() {
	CFormView::OnInitialUpdate();

	CRect rc;
	GetWindowRect(&rc);
	m_ptMinTrackSize.x = rc.Width();
	m_ptMinTrackSize.y = rc.Height();
	m_bInitDone = TRUE;
}

// layout functions
void CResizableFormView::AddAnchor(HWND wnd, CSize tl_type, CSize br_type) {
	ASSERT(wnd != NULL && ::IsWindow(wnd));
	ASSERT(::IsChild(*this, wnd));
	ASSERT(tl_type != NOANCHOR);

	// get control's window class

	CString st;
	GetClassName(wnd, st.GetBufferSetLength(MAX_PATH), MAX_PATH);
	st.ReleaseBuffer();
	st.MakeUpper();

	// add the style 'clipsiblings' to a GroupBox
	// to avoid unnecessary repainting of controls inside
	if (st == "BUTTON") {
		DWORD style = GetWindowLong(wnd, GWL_STYLE);
		if (style & BS_GROUPBOX) {
			SetWindowLong(wnd, GWL_STYLE, style | WS_CLIPSIBLINGS);
		}
	}

	// wnd classes that don't redraw client area correctly
	// when the hor scroll pos changes due to a resizing
	BOOL hscroll = FALSE;
	if (st == "LISTBOX") {
		hscroll = TRUE;
	}

	// wnd classes that need refresh when resized
	BOOL refresh = FALSE;
	if (st == "STATIC") {
		DWORD style = GetWindowLong(wnd, GWL_STYLE);

		switch (style & SS_TYPEMASK) {
		case SS_LEFT:
		case SS_CENTER:
		case SS_RIGHT:
			// word-wrapped text needs refresh
			refresh = TRUE;
			break;
		}

		// centered images or text need refresh
		if (style & SS_CENTERIMAGE) {
			refresh = TRUE;
		}

		// simple text never needs refresh
		if (style & (SS_TYPEMASK == SS_SIMPLE)) {
			refresh = FALSE;
		}
	}

	// get dialog's and control's rect
	CRect wndrc, objrc;

	GetClientRect(&wndrc);
	if (bWnd) {
		wndrc = iniRC;
	}
	::GetWindowRect(wnd, &objrc);
	ScreenToClient(&objrc);

	CSize tl_margin, br_margin;

	if (br_type == NOANCHOR) {
		br_type = tl_type;
	}

	// calculate margin for the top-left corner

	tl_margin.cx = objrc.left - wndrc.Width() * tl_type.cx / 100;
	tl_margin.cy = objrc.top - wndrc.Height() * tl_type.cy / 100;

	// calculate margin for the bottom-right corner

	br_margin.cx = objrc.right - wndrc.Width() * br_type.cx / 100;
	br_margin.cy = objrc.bottom - wndrc.Height() * br_type.cy / 100;

	if (br_margin.cx > -8) {
		br_margin.cx = -8;
	}
	if (br_margin.cy > -8) {
		br_margin.cy = -8;
	}
	// add to the list
	m_plLayoutList.AddTail(new Layout(wnd, tl_type, tl_margin, br_type, br_margin, hscroll, refresh));
}

void CResizableFormView::ArrangeLayout() {
	// init some vars
	CRect wndrc;
	GetClientRect(&wndrc);

	Layout* pl;
	POSITION pos = m_plLayoutList.GetHeadPosition();

	HDWP hdwp = BeginDeferWindowPos(m_plLayoutList.GetCount());

	while (pos != NULL) {
		pl = (Layout*)m_plLayoutList.GetNext(pos);

		CRect objrc, newrc;
		CWnd* wnd = CWnd::FromHandle(pl->hwnd); // temporary solution

		wnd->GetWindowRect(&objrc);
		ScreenToClient(&objrc);

		// calculate new top-left corner
		newrc.left = pl->tl_margin.cx + wndrc.Width() * pl->tl_type.cx / 100;
		newrc.top = pl->tl_margin.cy + wndrc.Height() * pl->tl_type.cy / 100;
		// calculate new bottom-right corner
		newrc.right = pl->br_margin.cx + wndrc.Width() * pl->br_type.cx / 100;
		newrc.bottom = pl->br_margin.cy + wndrc.Height() * pl->br_type.cy / 100;

		// jms - 11/28/00 - new code starts
		// Restrict controls from being sized smaller than their size on
		// the dialog template. I could have mingled this code with the original
		// four lines above, but I wanted Paolo to be abe to easily find what I
		// did.
		FORMCONTROL* pCtrl = GetFormControl(pl->hwnd);
		newrc.left = max(newrc.left, pCtrl->rect.left);
		newrc.top = max(newrc.top, pCtrl->rect.top);
		newrc.right = max(newrc.right, pCtrl->rect.right);
		newrc.bottom = max(newrc.bottom, pCtrl->rect.bottom);
		// jms - 11/28/00 - new code stops

		if (!newrc.EqualRect(&objrc)) {
			BOOL add = TRUE;

			if (pl->adj_hscroll) {
				// needs repainting, due to horiz scrolling
				int diff = newrc.Width() - objrc.Width();
				int max = wnd->GetScrollLimit(SB_HORZ);

				if (max > 0 && wnd->GetScrollPos(SB_HORZ) > max - diff) {
					wnd->MoveWindow(&newrc);
					wnd->Invalidate();
					wnd->UpdateWindow();

					add = FALSE;
				}
			}

			if (pl->need_refresh) {
				wnd->MoveWindow(&newrc);
				wnd->Invalidate();
				wnd->UpdateWindow();

				add = FALSE;
			}

			if (add)
				DeferWindowPos(hdwp, pl->hwnd, NULL, newrc.left, newrc.top,
					newrc.Width(), newrc.Height(), SWP_NOZORDER | SWP_NOACTIVATE);
		}
	}

	// go re-arrange child windows
	EndDeferWindowPos(hdwp);
}

//------------------------------------------------------------------------------
// NAME:		CResizableFormView::AddResizedControl()
//
// PURPOSE:		Allows the class to determine and retain a control's *original*
//				size and position on the dialog template used by this formview.
//				This info needs to be ratined because the user could potentially
//				resize the view in sych a way as to obscure part of or all of a
//				resized/positioned control.
//
// PARAMETERS:	UINT nID		The ID of the control.
//				CSize tl_type	The top/left corner anchor for the control.
//				CSize br_type	The bottom/left corner anchor for the control.
//
// RETURNS:		void
//------------------------------------------------------------------------------
void CResizableFormView::AddResizedControl(UINT nID, CSize tl_type, CSize br_type) {
	CWnd* pWnd = GetDlgItem(nID);
	if (pWnd) {
		FORMCONTROL* pCtrl = new FORMCONTROL;
		if (pCtrl) {
			CRect rect;
			pWnd->GetWindowRect(&rect);
			ScreenToClient(&rect);
			pCtrl->nID = nID;
			pCtrl->rect = rect;
			pCtrl->tlType = tl_type;
			pCtrl->brType = br_type;
			pCtrl->hWnd = ::GetDlgItem(*this, nID);
			m_tpaControls.Add(pCtrl);
		}
	}
}

//------------------------------------------------------------------------------
// NAME:		CResizableFormView::AnchorControls()
//
// PURPOSE:		Uses the array containing the controls that are to be resized,
//				and calls the original AddAnchor function to establish their
//				resizing/positioning telemetry.
//
// PARAMETERS:	void
//
// RETURNS:		void
//------------------------------------------------------------------------------
void CResizableFormView::AnchorControls() {
	m_tpaControls.FreeExtra();
	int nCount = m_tpaControls.GetSize();

	for (int i = 0; i < nCount; i++) {
		FORMCONTROL* pCtrl = m_tpaControls.GetAt(i);
		AddAnchor(pCtrl->hWnd, pCtrl->tlType, pCtrl->brType);
	}
}

//------------------------------------------------------------------------------
// NAME:		CResizableFormView::GetFormControl()
//
// PURPOSE:		Retrieves a pointer to a FORMCONTROL item in the m_tpaControls
//				array that matches the control that's about to be
//				resized/repositioned.  This item is needed to restrict the
//				resizing of the controlling question.
//
// PARAMETERS:	HWND hWnd				The hWnd of the contro being resized.
//
// RETURNS:		FORMCONTROL* pResult	The associated control info item.
//------------------------------------------------------------------------------
FORMCONTROL* CResizableFormView::GetFormControl(HWND hWnd) {
	FORMCONTROL* pResult = NULL;

	int nCount = m_tpaControls.GetSize();
	for (int i = 0; i < nCount; i++) {
		FORMCONTROL* pCtrl = m_tpaControls.GetAt(i);
		if (pCtrl->hWnd == hWnd) {
			pResult = pCtrl;
			break;
		}
	}
	return pResult;
}