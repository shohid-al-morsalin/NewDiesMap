#pragma once
// ProbeTest.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProbeTest dialog

class CProbeTest : public CDialog {
	// Construction
public:
	CProbeTest(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CProbeTest)
	enum { IDD = IDD_PROBETEST };
	CButton	m_cRecord;
	CButton	m_cRecordPos;
	//}}AFX_DATA

	enum PTCMD {
		PTNONE, PTRECORD, PTN1, PTN2, PTDUMPDATA, PTRECORDPOS, PTEXPORTWITHBASELINE, PROBEDISTANCE, PROBEOPTIMIZE
	};
	int Cmd;
	BOOL bDump, bDumpPeaks;

	void Log(CString msg, short lvl = 5);

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CProbeTest)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CProbeTest)
	afx_msg void OnRecord();
	afx_msg void OnRecordPos();
	afx_msg void OnSavespec();
	afx_msg void OnStartmotor();
	afx_msg void OnStopmotor();
	afx_msg void OnReloadmotor();
	virtual BOOL OnInitDialog();
	afx_msg void OnExport();
	afx_msg void OnProbedist();
	afx_msg void OnOptimizeprobes();
	afx_msg void OnConvert2text();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
