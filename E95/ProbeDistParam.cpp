// ProbeDistParam.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "ProbeDistParam.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProbeDistParam dialog

CProbeDistParam::CProbeDistParam(CWnd* pParent /*=NULL*/)
	: CDialog(CProbeDistParam::IDD, pParent) {
	//{{AFX_DATA_INIT(CProbeDistParam)
	m_Distance = 0.0f;
	m_NSteps = 0;
	//}}AFX_DATA_INIT
}

void CProbeDistParam::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProbeDistParam)
	DDX_Text(pDX, IDC_DISTANCE, m_Distance);
	DDX_Text(pDX, IDC_NSTEPS, m_NSteps);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CProbeDistParam, CDialog)
	//{{AFX_MSG_MAP(CProbeDistParam)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProbeDistParam message handlers