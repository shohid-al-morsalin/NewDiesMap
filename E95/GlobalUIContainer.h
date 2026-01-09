#pragma once

#include "Const.h"
#include "ProcJobInfo.h"

class CTitlePanel;
class CInfoPanelMain;
class CInfoPanelSetup;
class CNavigationPanel;

class CInfoPanelHostMsg;
class CInfoPanelEqpMessage;
class CProcJob413ProcessingDlg;
class CProcJobDlg;
class CProcJobSMICDlg;
class CProcJobManDlg;
class CProcJobManual;
class CProcJobPWC1DlgBase;
class CProcJobPWC2Dlg;
class CProcJobQue413Dlg;
class CProcJobCompleteDlg;
class CProcJobCompleteManual;
class CProcJobCancelCarrierDlg;
class CProcJobUnloadCarrierDlg;
class CRecoveryDlg;
class CRecipeCreateDlg413;
class CProcJobManageDlg;

class CGlobalUIContainer {
public:
	BOOL CanStart(short ID);
	BOOL CheckIncompleteJob(short i);
	BOOL ClampCarrier(short PortNo);
	BOOL UnclampCarrier(short PortNo);
	void ClearAll(short i);
	BOOL Initialize();
	void EqpMessage(CString msg, short lvl);
	void StopUIElements();
	void StartUIElements();
	short IncompleteProcessDetection(short PortNo);
	BOOL PerformAction(int action, short unit, CString& CarrierID);
	void SetCurrentView(CString ViewName);
	void SaveINI();
	void LoadINI();
	int UserLevel();

	CTitlePanel* pTitlePanel;	// Self destruct, for reference only [11/1/2011 FSMT]
	CInfoPanelMain* pInfoPanelMain;	// Self destruct, for reference only [11/1/2011 FSMT]
	CInfoPanelSetup* pInfoPanelSetup;	// Self destruct, for reference only [11/1/2011 FSMT]
	CNavigationPanel* pNavigationPanel;	// Self destruct, for reference only [11/1/2011 FSMT]

	CProcJobInfo pProcJobInfo[MAX_LPSUPPORTED];  // Intermediate container for process job info
	CProcJob413ProcessingDlg* pProcJob413ProcessingDlg[MAX_LPSUPPORTED];
	CProcJobDlg* pProcJobGENDlg[MAX_LPSUPPORTED];
	CProcJobSMICDlg* pProcJobSMICDlg[MAX_LPSUPPORTED];
	CProcJobPWC1DlgBase* pProcJobPWCDlg[MAX_LPSUPPORTED];
	CProcJobPWC2Dlg* pProcJobPWC2Dlg[MAX_LPSUPPORTED];
	CProcJobQue413Dlg* pProcJobQueued413Dlg[MAX_LPSUPPORTED];
	CProcJobCompleteDlg* pProcJobCompleteDlg[MAX_LPSUPPORTED];
	CProcJobCancelCarrierDlg* pProcJobCancelCarrierDlg[MAX_LPSUPPORTED];
	CProcJobUnloadCarrierDlg* pProcJobUnloadCarrierDlg[MAX_LPSUPPORTED];
	CProcJobManageDlg* pProcJobManageDlg;
	CProcJobManual* pProcJobManual;
	CProcJobManDlg* pProcJobManDlg;
	CProcJobCompleteManual* pProcJobCompleteManual;

	CRecoveryDlg* pProcJobRecoveryDlg[MAX_LPSUPPORTED];
	CRecoveryDlg* pRecoveryDlg;
	CRecipeCreateDlg413* pRecipeCreateDlg413;

	CInfoPanelEqpMessage* pEqpMessage; 	// Self destruct, for reference only [11/1/2011 FSMT]
	CInfoPanelHostMsg* pHostMessage; 	// Self destruct, for reference only [11/1/2011 FSMT]

	CGlobalUIContainer();
	virtual ~CGlobalUIContainer();
};

extern CGlobalUIContainer* pUDev;
