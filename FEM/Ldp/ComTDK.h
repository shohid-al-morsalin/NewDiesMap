#pragma once

#include "ComBase.h"

class CComTDK : public CComBase {
private:

	BOOL HasError();
	void ShowError();

public:

	BOOL Abort();
	BOOL SetDefault();
	void Decode(CString& msg);
	BOOL Auto();
	BOOL Reset(BOOL bCalib = FALSE);
	BOOL Open();
	BOOL Close();
	BOOL Map();
	BOOL NoVac();
	BOOL NeedVac();
	BOOL CloseNoRelease();
	BOOL Calib();
	BOOL Status();
	BOOL Unclamp();
	BOOL Clamp();
	BOOL AccessModeChange(CLPObj::ACCESSMODE mode);
	//	BOOL LEDLoadUnload(PrcsMgrServices::LPBSTATE LpState);
	BOOL LEDManual(BOOL on);
	BOOL LEDReserved(BOOL on);
	BOOL LEDAlarm(BOOL on);
	BOOL ClearAllLED();
	BOOL EnableHOAvbl();
	BOOL EnableButtonReport();
	BOOL PostMsg(HWND hWNd, UINT Msg, WPARAM wParam, LPARAM lParam);
	BOOL ReadMap(BOOL bFirst);
	BOOL ExtractSlotmap(char* map);
	void ReportNAK(char* map);
	CString GetOnOff(char ch);
	BOOL SendMsg(CSMHdr& SM);

	virtual BOOL Connect(short port);
	virtual BOOL Disconnect();

	CComTDK();
	virtual ~CComTDK();
};
