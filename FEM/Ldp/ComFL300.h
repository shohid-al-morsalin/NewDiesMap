#pragma once

#include "combase.h"

class CComFL300 : public CComBase {
	BOOL bCarrierPresent;
	BOOL bClampResponse;
	BOOL bUnclampResponse;

public:
	BOOL UnclampEx();
	BOOL ClampEx();
	void ProcessButtonPushed();
	void ProcessEndClose();
	void ProcessEndOpen();
	void ProcessCarrierRemoved();
	void ProcessCarrierArrived();
	CString GetLPLight(CString state);
	void RefreshLED();
	BOOL IsOperatable();
	void ButtonPushSensed();
	void Log(CString msg, short lvl = 5);
	BOOL EnableButtonReport();

	CString GetOnOff(char ch);
	BOOL EnableHOAvbl();
	BOOL PostMsg(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

	BOOL LEDManual(BOOL on);
	BOOL LEDReserved(BOOL on);
	BOOL LEDAlarm(BOOL on);
	BOOL LEDRTL(BOOL on);
	BOOL LEDRTU(BOOL on);
	BOOL LEDLOD(BOOL on);
	BOOL LEDDOP(BOOL on);

	BOOL ClearAllLED();

	BOOL NoVac();
	BOOL NeedVac();
	BOOL Calib();
	BOOL Auto();
	BOOL Reset(BOOL bCalib = FALSE);
	BOOL Close();
	BOOL Open();
	BOOL Map();

	BOOL SetDefault();
	BOOL _SetDefault();

	BOOL Status();
	BOOL CloseNoRelease();
	BOOL Unclamp();
	BOOL Clamp();

	void Decode(CString& msg);

	BOOL AccessModeChange(CLPObj::ACCESSMODE mode);
	BOOL ReadMap();
	BOOL ExtractSlotmap(char* map);
	BOOL SendMsg(CSMHdr& SM);

	virtual BOOL Connect(short port);

	virtual BOOL Disconnect();

	CComFL300();

	virtual ~CComFL300();
};