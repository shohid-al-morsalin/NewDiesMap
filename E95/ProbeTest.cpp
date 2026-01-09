// ProbeTest.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "ProbeTest.h"
#include "413App.h"
#include "GlobalDeviceContainer.h"
#include "..\413\DMCMotorBase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProbeTest dialog

CProbeTest::CProbeTest(CWnd* pParent /*=NULL*/)
	: CDialog(CProbeTest::IDD, pParent) {
	//{{AFX_DATA_INIT(CProbeTest)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	Cmd = PTNONE;
	bDump = FALSE;
	bDumpPeaks = FALSE;
}

void CProbeTest::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProbeTest)
	DDX_Control(pDX, IDC_RECORD, m_cRecord);
	DDX_Control(pDX, IDC_RECORDPOS, m_cRecordPos);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CProbeTest, CDialog)
	//{{AFX_MSG_MAP(CProbeTest)
	ON_BN_CLICKED(IDC_RECORD, OnRecord)
	ON_BN_CLICKED(IDC_RECORDPOS, OnRecordPos)
	ON_BN_CLICKED(IDC_SAVESPEC, OnSavespec)
	ON_BN_CLICKED(IDC_STARTMOTOR, OnStartmotor)
	ON_BN_CLICKED(IDC_STOPMOTOR, OnStopmotor)
	ON_BN_CLICKED(IDC_RELOADMOTOR, OnReloadmotor)
	ON_BN_CLICKED(IDC_EXPORT, OnExport)
	ON_BN_CLICKED(IDC_PROBEDIST, OnProbedist)
	ON_BN_CLICKED(IDC_OPTIMIZEPROBES, OnOptimizeprobes)
	ON_BN_CLICKED(IDC_CONVERT2TEXT, OnConvert2text)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProbeTest message handlers

void CProbeTest::Log(CString msg, short lvl) {
	pGDev->LogFile.Log(msg, lvl, "CSetupProp3Dlg");
	if (lvl <= 1) {
		pGDev->LogFile.LogLocal(msg);
	}
}

void CProbeTest::OnRecord() {
	Cmd = PTRECORD;
	OnOK();
}

void CProbeTest::OnRecordPos() {
	Cmd = PTRECORDPOS;
	OnOK();
}

void CProbeTest::OnSavespec() {
	Cmd = PTDUMPDATA;
	OnOK();
}

BOOL CProbeTest::OnInitDialog() {
	CDialog::OnInitDialog();

	if (bDump) {
		m_cRecord.SetWindowText("Stop Rec.");
	}
	else {
		m_cRecord.SetWindowText("Record");
	}

	if (bDumpPeaks) {
		m_cRecordPos.SetWindowText("Stop Rec. Pos");
	}
	else {
		m_cRecordPos.SetWindowText("Record Pos");
	}
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CProbeTest::OnStartmotor() {
	p413App->pDMC->Run();
	Log("Start Echo motor");
}

void CProbeTest::OnStopmotor() {
	p413App->pDMC->Stop();
	Log("Stop Echo motor");
}

void CProbeTest::OnReloadmotor() {
	p413App->pDMC->MotorFile = "";
	p413App->pDMC->Reload(p413App->Global.RcpSetup.MotorFile);
	Log("Restart Echo motor");
}

void CProbeTest::OnExport() {
	Cmd = PTEXPORTWITHBASELINE;
	OnOK();
}

void CProbeTest::OnProbedist() {
	Cmd = PROBEDISTANCE;
	OnOK();
}

void CProbeTest::OnOptimizeprobes() {
	Cmd = PROBEOPTIMIZE;
	OnOK();
}

void CProbeTest::OnConvert2text() {
	CString name;
	name.Format("C:\\TEMP\\%s.RCP.CSV", p413App->Global.RcpSetup.RcpeName);
	p413App->Global.RcpSetup.Verbose(name);
	Cmd = PTNONE;
	OnOK();
}