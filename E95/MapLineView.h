// MapLineView.h: interface for the CMapLineView class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAPLINEVIEW_H__2218F9B0_390F_4BD0_8EBE_6763355C56D7__INCLUDED_)
#define AFX_MAPLINEVIEW_H__2218F9B0_390F_4BD0_8EBE_6763355C56D7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\PLT\Plt.h"

class CMapLineView : public CPlt  
{
// protected:
// 	DECLARE_DYNCREATE(CMapLineView)
// Attributes
public:
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMapLineView)
public:
	virtual void OnInitialUpdate();
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL
	virtual void PostNcDestroy();
	
// Operations
public:
	
public:
	CPltData * GetDataPtr();
	CCasMgr * pCasMgr;
	CMapLineView();
	virtual ~CMapLineView();
	// Generated message map functions
protected:
	//{{AFX_MSG(CMapLineView)
	afx_msg void OnFileExport();
	afx_msg void OnDisplayProperties();
	afx_msg void OnDisplayExport();
	afx_msg void OnDisplayExportastable();
	afx_msg void OnDisplaySaveasbitmap();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

#endif // !defined(AFX_MAPLINEVIEW_H__2218F9B0_390F_4BD0_8EBE_6763355C56D7__INCLUDED_)
