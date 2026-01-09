// EditDocumentationDlg.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "EditDocumentationDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditDocumentationDlg dialog

CEditDocumentationDlg::CEditDocumentationDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEditDocumentationDlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CEditDocumentationDlg)
	m_Comment = _T("");
	m_LotID = _T("");
	m_Operator = _T("");
	m_Project = _T("");
	//}}AFX_DATA_INIT
}

void CEditDocumentationDlg::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditDocumentationDlg)
	DDX_Text(pDX, IDC_COMMENT, m_Comment);
	DDX_Text(pDX, IDC_LOTID, m_LotID);
	DDX_Text(pDX, IDC_OPERATOR, m_Operator);
	DDX_Text(pDX, IDC_PROJECT, m_Project);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CEditDocumentationDlg, CDialog)
	//{{AFX_MSG_MAP(CEditDocumentationDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditDocumentationDlg message handlers