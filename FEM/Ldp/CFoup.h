#pragma once

class CComThread;
class CFoupThread;
class CIDRUnit;
class CQueOutMgr;
struct CSMHdr;
struct CSMOL;
struct RPLY;

/** Base class for all kinds of foups */
class CFoup {
public:
	CComThread* pComTh;
	CString Caller;

	/** Simulation use only */
	BOOL bInPlace;

	/** Simulation use only */
	BOOL bReady;

	/** Simulation use only */
	BOOL bAlarmed;
	BOOL bNoClose;
	CString ErrorStr;
	BOOL bDisabled;
	short Unit;
	HWND m_hWnd, m_hWndBak;
	HWND m_hWndLPBar;

	/** Indicate whether the foup is open or close */
	BOOL bOpen;
public:
	BOOL Add(CSMHdr& SM);
	void SetInP(BOOL bInP);
	BOOL Reply(CSMHdr& SM, BOOL ret);
	void Log(CString msg, short lvl = 5);
	BOOL StartServer(CComThread* pComTh);
	//	BOOL UpdateLightTower();
public:
	virtual BOOL StartComThread(short unit);
	virtual BOOL Connect(RPLY& rpy, short unit, short port);
	virtual BOOL Disconnect();
	virtual BOOL Reset(RPLY& rpy);
	//	virtual BOOL LEDLoadUnload(CSMHdr & SM);
	virtual BOOL LedReserved(CSMHdr& SM);
	virtual BOOL LedManual(CSMHdr& SM);
	virtual BOOL LedClearAll();
	virtual CString GetSM();
	virtual CQueOutMgr* GetQueOutMgr();
	virtual BOOL SetE84AccessMode(CSMHdr& SM);
	virtual BOOL NoVac(RPLY& rpy);
	virtual BOOL NeedVac(RPLY& rpy);
	virtual BOOL Calib(CSMHdr& SM);
	virtual BOOL Map(CSMHdr& SM);
	virtual BOOL Auto(RPLY& rpy);
	virtual BOOL Open(CSMHdr& SM);
	virtual BOOL Close(CSMHdr& SM);
	virtual BOOL Initialize();
	virtual BOOL Unclamp(CSMHdr& SM);
	virtual BOOL Clamp(CSMHdr& SM);
	virtual BOOL EnableHOAvbl(CSMHdr& SM);
	virtual BOOL EnableButtonReport(CSMHdr& SM);
	virtual void PushButton2(CSMHdr& SM);
	virtual BOOL SetDefault(CSMHdr& SM);
	virtual BOOL CloseNoRelease(CSMHdr& SM);
	virtual BOOL GetStatus(BOOL& bRdy, BOOL& bInP, BOOL& bAlr);
	virtual BOOL Status(CSMHdr& SM);
	virtual BOOL Stop(CSMHdr& SM);
	virtual BOOL Register(CSMHdr& SM);
	virtual BOOL RegisterEx(CSMHdr& SM);
	virtual BOOL UnRegisterEx(CSMHdr& SM);
	virtual BOOL RegisterLPBar(CSMHdr& SM);

private:
	/** @link dependency */
	/*# CLTWMgr lnkCLTWMgr; */

public:
	virtual BOOL Abort(RPLY& rpy);
	//	virtual BOOL LedAlarmed(CSMHdr &SM);
	virtual ~CFoup();
	CFoup();
};
