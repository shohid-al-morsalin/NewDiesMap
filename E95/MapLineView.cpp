// MapLineView.cpp: implementation of the CMapLineView class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "MapLineView.h"
#include "ExportChart.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// IMPLEMENT_DYNCREATE(CMapLineView, CPlt)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMapLineView::CMapLineView()
{
	pCasMgr = NULL;
	pPData = new CPltData;
}

CMapLineView::~CMapLineView()
{
	if (pPData)
	{
		delete pPData;
	}
}

void CMapLineView::PostNcDestroy() 
{
	if (pPData) {
		delete pPData;
		pPData = NULL;
	}
	delete this;
}


BEGIN_MESSAGE_MAP(CMapLineView, CPlt)
	//{{AFX_MSG_MAP(CMapLineView)
	ON_COMMAND(ID_FILE_EXPORT, OnFileExport)
	ON_COMMAND(ID_DISPLAY_PROPERTIES, OnDisplayProperties)
	ON_COMMAND(ID_DISPLAY_EXPORT, OnDisplayExport)
	ON_COMMAND(ID_DISPLAY_EXPORTASTABLE, OnDisplayExportastable)
	ON_COMMAND(ID_DISPLAY_SAVEASBITMAP, OnDisplaySaveasbitmap)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMapLineView drawing

void CMapLineView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	CPlt::OnDraw(pDC);
}

/////////////////////////////////////////////////////////////////////////////
// CChartView printing

BOOL CMapLineView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CMapLineView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CMapLineView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


void CMapLineView::OnInitialUpdate() 
{
	// This function seems not call by instances greater then 1
	CPlt::OnInitialUpdate();
	
}


void CMapLineView::OnFileExport() 
{
	CExportChart dlg;
	
	dlg.DoModal();
	
	switch(dlg.extype) {
	case CExportChart::EXPORTNORMAL:
		ExportData();
		break;
	case CExportChart::EXPORTTABLE:
		ExportAsTable();
		break;
	case CExportChart::EXPORTIMAGE:
		ExportImage();
		break;
	default:
		break;
	}
}

void CMapLineView::OnDisplayProperties() 
{
}

void CMapLineView::OnDisplayExport() 
{
	ExportData();
}

void CMapLineView::OnDisplayExportastable() 
{
	ExportAsTable();	
}

void CMapLineView::OnDisplaySaveasbitmap() 
{
	ExportImage();	
}

CPltData * CMapLineView::GetDataPtr()
{
	return pPData;
}
