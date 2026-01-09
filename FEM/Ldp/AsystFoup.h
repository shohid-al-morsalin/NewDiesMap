#pragma once

#include "CFoup.h"

struct CSMHdr;

class CAsystFoup : public CFoup {
public:
	BOOL Abort(RPLY& rpy);
	BOOL NeedVac(RPLY& rpy);
	BOOL NoVac(RPLY& rpy);
	BOOL Auto(RPLY& rpy);
	BOOL Reset(RPLY& rpy);
	BOOL Disconnect();
	BOOL Connect(RPLY& rpy, short unit, short port);
	//	BOOL LedAlarmed(CSMHdr &SM);

	//	CSMClt ipc;

	//	BOOL LEDLoadUnload(CSMHdr & SM);

	BOOL LedReserved(CSMHdr& SM);

	BOOL LedManual(CSMHdr& SM);

	BOOL LedClearAll();

	BOOL EnableHOAvbl(CSMHdr& SM);

	BOOL EnableButtonReport(CSMHdr& SM);

	CString GetSM();

	BOOL Clamp(CSMHdr& SM);
	BOOL Unclamp(CSMHdr& SM);

	BOOL SetDefault(CSMHdr& SM);
	BOOL CloseNoRelease(CSMHdr& SM);

	BOOL GetStatus(BOOL& bRdy, BOOL& bInP, BOOL& bAlr);
	BOOL Status(CSMHdr& SM);
	BOOL Stop(CSMHdr& SM);
	BOOL Register(CSMHdr& SM);
	BOOL RegisterLPBar(CSMHdr& SM);
	BOOL SetE84AccessMode(CSMHdr& SM);

	BOOL Calib(CSMHdr& SM);
	BOOL Map(CSMHdr& SM);

	BOOL Open(CSMHdr& SM);
	BOOL Close(CSMHdr& SM);

	//BOOL ReadMap(CSMHdr &SM);
	//void PushButton2(CSMHdr &SM);

	CAsystFoup();
	virtual ~CAsystFoup();
};