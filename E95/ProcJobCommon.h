#pragma once

#include "CarrierObj.h"

class CRecipe413;
class CInfoPanelMain;

class CProcJobCommon {
public:
	enum CANSTARTSTATE {
		NOTREADY, OKTOSTART, ABORT
	};

	short ID;
	short type;
	CString m_CarrierID;
	CInfoPanelMain* m_Parent;

	short CJMONTimeoutCounter;
	short AreYouThereRetryCount;
	BOOL bOutOfServiceAlarmSet;

	CANSTARTSTATE CanStart();
	void ProceedToProcessing();
	virtual void Message(CString msg, short lvl = 5);

public:
	void SetOHTLoadComplete();
	void SetIDVerificationOK();
	CCarrierObj::IDSTATUS VerifyCarrierID();
	CCarrierObj::IDSTATUS VerifyID();
	enum RTNCODE1 {
		ofERR, ofRTN1, ofRTN2, ofRTN3, ofRTN4
	};
	RTNCODE1 CarrierIDVerification();
	void SetCarrierIDReadOK(char* carrierID);
	void SetSMVerificationOK();
	void SetLatchFail();
	void SetLatchCmpl();
	void SetMaterialRemoved();
	void SetMaterialRecieved();
	BOOL NoOtherProcJobRuning();
	void ShowContentMap();
	enum RTNCODE {
		ofOK, ofFAIL, ofOBJERR
	};
	RTNCODE ReadCarrierID(HWND hWnd, int ReplyOK, int ReplyFail);
	RTNCODE LatchFOUP(HWND hWnd, int ReplyOK, int ReplyFAIL);
	RTNCODE OpenFOUP(HWND hWnd, int ReplyOK, int ReplyFAIL);
	void ProceedToCancelCarrier();
	enum RTNCJMON {
		pjcERR, pjcCANSTART, pjcCJMON
	};
	RTNCJMON DoCJMonitor();
	//BOOL LoadRecipe(CRecipe413 *pRcp, short Option=0);
	void SetContentMap();
	virtual void Log(CString msg, short lvl);
	BOOL RegisterWithSecsSvr(CWnd* pWnd, short ID);

	RTNCODE UnlatchFOUP(HWND hWnd); //20130821 SCOTT ADD

	CProcJobCommon();
	virtual ~CProcJobCommon();

protected:
};
