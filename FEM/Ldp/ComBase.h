#pragma once

#include "QueOutMgr.h"
#include "LPObj.h"

#define MAXSLOTMAPLEN	32

class CComm;
struct CSMOL;
struct CSMHdr;

class CComBase {
public:
	CComm* pCom;
	BOOL bPresent;
	BOOL bOpen;
	BOOL bReady;
	BOOL bInPlace;
	BOOL bAlarmed;
	HWND hWnd, hWndBak;
	HWND hWndLPBar;

	BOOL bStarted;
	char SlotMap[32];
	char Slot13Map[15]; // masum13slots
	short MaxSlot;
	BOOL bIgnorDoubleSlotted;

	enum ACCMODE {
		AMMANUAL, AMAUTO
	};
	ACCMODE AccMode;
	virtual BOOL Abort();
	BOOL SendTDK(CString msg, int Rpy);
	char* GetResp();
	//	void Log(CString msg, short lvl);
	BOOL Reply(CSMOL& SM, BOOL ret);
	BOOL Reply(CSMHdr& SM, BOOL ret);

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CComThread)

		//}}AFX_VIRTUAL

	void WaitTillStart();
	virtual BOOL GetRespLFFoup(char* msg, short max, int tout);

	virtual BOOL SendMsg(CSMHdr& SM) = 0;
	virtual void Decode(CString& msg) = 0;
	virtual BOOL SetDefault() = 0;
	virtual BOOL Connect(short port);
	virtual BOOL Disconnect();

	BOOL SendHCA(CString msg, int tout, BOOL bLog = TRUE);
	BOOL SendAck(CString msg, int tout, BOOL bLog = TRUE);
	BOOL Read(CString msg, char* text, short max, int tout);
	BOOL ReadTDK(const CString& msg, char* text, short max);
	void CheckComIn();

	virtual BOOL NoVac();
	virtual BOOL NeedVac();
	virtual BOOL Auto();
	virtual BOOL Reset(BOOL bCalib = FALSE);
	virtual BOOL Calib();
	virtual BOOL Open();
	virtual BOOL Close();
	virtual BOOL CloseNoRelease();
	virtual BOOL Map();
	virtual BOOL Unclamp();
	virtual BOOL Clamp();
	virtual BOOL Status();
	virtual BOOL AccessModeChange(CLPObj::ACCESSMODE mode);
	//	virtual BOOL LEDLoadUnload(PrcsMgrServices::LPBSTATE LpState);
	virtual BOOL ClearAllLED();
	virtual BOOL LEDReserved(BOOL on);
	virtual BOOL LEDManual(BOOL on);
	virtual BOOL LEDAlarm(BOOL on);
	virtual BOOL EnableHOAvbl();
	virtual BOOL EnableButtonReport();

	CComBase();
	virtual ~CComBase();

private:
	CQueOutMgr QueOut;

protected:
	CCriticalSection CS;

public:
	void Log(CString msg, short lvl = 5);
	short unit;
	short RTLLed;
	short RTULed;
	short MANLed;
	short RSVLed;
	short DOPLed;
	short ALRLed;
	short AUTLed;
	short ACCLed;
	short LODLed;
	short PBTPort;
	short P68Manual, P68Auto;
};