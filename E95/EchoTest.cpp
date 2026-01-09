// EchoTest.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "EchoTest.h"

#include "413App.h"
#include "..\413\DMCMotorBase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEchoTest dialog

CEchoTest::CEchoTest(CWnd* pParent /*=NULL*/)
	: CDialog(CEchoTest::IDD, pParent) {
	//{{AFX_DATA_INIT(CEchoTest)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	P = 1.0f; dP = 1.0f;
	I = 0.04f; dI = 0.04f;
	D = 0.0f; dD = 5.0f;
	cnt = 0;
}

void CEchoTest::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEchoTest)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CEchoTest, CDialog)
	//{{AFX_MSG_MAP(CEchoTest)
	ON_BN_CLICKED(IDC_START, OnStart)
	ON_BN_CLICKED(IDC_NEXT, OnNext)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEchoTest message handlers

void CEchoTest::OnStart() {
	CString prg;
	prg =
		"TL 9.9\
TM 250\
MT -1\
KP5\
KD5\
KI0.2\
EC 0\
K210\
DS 250\
DB 500\
ZP1\
ZN-1\
#AGAIN\
SH\
DP 0\
JG 600000\
BG X\
#N1\
IF (_TE < 40000)\
JP #N1\
ENDIF\
ST\
AMX\
DP0\
JG -600000\
BG X\
#N2\
IF (_TE > -80000)\
JP #N2\
ENDIF\
ST\
AMX\
DP 0\
JG 600000\
BG X\
#N5\
IF (_TE < 40000)\
JP #N5\
ENDIF\
ST\
AMX\
DP0\
JG -600000\
BG X\
#N6\
IF (_TE > -80000)\
JP #N6\
ENDIF\
ST\
AMX\
DP 0\
JG 600000\
BG X\
#N3\
IF (_TE < 40000)\
JP #N3\
ENDIF\
ST\
AMX\
DP0\
JG -600000\
BG X\
#N4\
IF (_TE > -80000)\
JP #N4\
ENDIF\
ST\
AMX\
CB1\
CB2\
SP 320000\
AC 29999104\
DC 29999104\
DP 0\
PA 42000\
BGX\
AMX\
DP0\
#B\
WT 0\
PA 30000\
BG X\
WT 30\
CB1\
AMX\
CB1\
WT 0\
PA -30000\
BG X\
WT 30\
SB1\
AMX\
CB1\
IF (_TE <-8050)\
TE\
JP #AGAIN\
ENDIF\
JP #B\
EN\
";
	p413App->pDMC->Stop();

	p413App->pDMC->LoadProgram(prg);

	p413App->pDMC->Run();
}

void CEchoTest::OnNext() {}

BOOL CEchoTest::OnInitDialog() {
	CDialog::OnInitDialog();

	P = 1; dP = 1;
	I = 0.04f; dI = 0.04f;
	D = 0.0f; dD = 5.0f;
	cnt = 0;

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}