#pragma once

struct CSMOL;
struct CSMHdr;
class CSMSvr;

/////////////////////////////////////////////////////////////////////////////
// CSvrThread thread
class CSvrThread : public CWinThread {
	//DECLARE_DYNCREATE(CSvrThread)
protected:
	CSvrThread(); // protected constructor used by dynamic creation

	// Attributes
public:
	BOOL loop;

	short type; // 1 = SMOL,  2 = SMHdr

	CSMSvr* ipc;

	CString SvrName;

	CWnd* pWnd;

	CWinThread* pWinT;

	unsigned long nThreadID;

	// Operations
public:
	short UIMode;

	long mileage;

	BOOL bStarted;

	virtual void CleanUp() = 0;

	long GetMileage();

	BOOL Reply(CSMOL& SM, BOOL ret);

	BOOL Reply(CSMHdr& SM, BOOL ret);

	void Yld();

	//	void ClearLog();
	int RunSMOL();

	int RunSMHdr();

	//	void Log(CString msg,short lvl);
	virtual BOOL ProcessCmd(CSMOL& SM);

	virtual BOOL ProcessCmd(CSMHdr& SM);

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSvrThread)
public:
	virtual BOOL InitInstance();

	virtual int ExitInstance();

	virtual int Run();

	//}}AFX_VIRTUAL
	// Implementation
protected:
	virtual ~CSvrThread();
	// Generated message map functions
	//{{AFX_MSG(CSvrThread)
	// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
