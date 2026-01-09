#pragma once

struct CSMOL;

// Use by E95 only [11/14/2007 YUEN]
class CSecsSvr /*: public CSvrThread*//*, public COlSvrMsg*/
{
	//	DECLARE_DYNCREATE(CSecsSvr)

		// Attributes
public:
	CWnd* pActWin[4];
	// Operations
public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSecsSvr)
	//}}AFX_VIRTUAL

	// Implementation
public:
	CSecsSvr(); // protected constructor used by dynamic creation
	virtual ~CSecsSvr();

	// Generated message map functions
	//{{AFX_MSG(CSecsSvr)
	// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

//	DECLARE_MESSAGE_MAP()
public:
	void CleanUp();
	//	BOOL PostMsg(UINT msg, WPARAM wParam, LPARAM lParam);
	//	BOOL AGVOnOffChange(AGVStatus & As);
	//	BOOL AGVWrite(AGVStatus & As);
	//	BOOL AGVRead(AGVStatus & As);
	//	BOOL AGVStatusExec(CSMOL & SM);
	BOOL BCRHeadConditionChange(BCRStatus& Bs);
	BOOL BCRHeadStatusChange(BCRStatus& Bs);
	BOOL BCROnOffChange(BCRStatus& Bs);
	BOOL BCRAlarmStatusChange(BCRStatus& Bs);
	BOOL BCRWrite(BCRStatus& Bs);
	BOOL BCRRead(BCRStatus& Bs);
	BOOL BCROperationStatusChange(BCRStatus& Bs);
	BOOL BCRStatusExec(CSMOL& SM);
	//BOOL PrcsMgrService(CSMOL & SM);
	BOOL PrJobExec(CSMOL& SM);
	BOOL CarrierObjExec(CSMOL& SM);
	//BOOL VerifySlotmap(short Port);
	//BOOL OpenFoup(short Port);
	//BOOL ProceedWithCarrierID(CString &CarrierID, short port);
	//BOOL ReadCarrierID(CString &CarrierID, short port);
	//BOOL IsCarrierIDRAvailable(short port);
	//BOOL CarrierArrived(CSMOL &SM);
	BOOL LoadPortActionsExec(CSMOL& SM);
	BOOL StdActionsExec(CSMOL& SM);
	BOOL CarrierIDInfoExec(CSMOL& SM);
	BOOL WaferParamExec(CSMOL& SM);
	BOOL GEMEventsExec(CSMOL& SM);
	BOOL CarrierActionsExec(CSMOL& SM);
	BOOL LoadPortStatusWrite(CSMOL& SM);
	BOOL LoadPortStatusRead(CSMOL& SM);
	BOOL LoadPortStatusExec(CSMOL& SM);
	BOOL LineModeActionsExec(CSMOL& SM);
	BOOL ProcessCmd(CSMOL& SM);
};

//extern CSecsSvr * pSecsSvr;
/////////////////////////////////////////////////////////////////////////////
