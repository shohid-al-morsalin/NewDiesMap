// SecsParam.cpp: implementation of the CSecsParam class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SecsParam.h"
#include "..\ONLINE\TCPIP\HsmsParaMeter.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSecsParam::CSecsParam() {
	Load();
	for (int i = 1; i < spLASTID; i++) {
		Param[i].LoadParam();  // Load must be called first [7/27/2010 Yuen]
	}
	SaveParam();
	UpdateHSMSParam();
}

CSecsParam::~CSecsParam() {}

void CSecsParam::Load() {
	Param[spT3TIMER].Set("T3TIMER", "T3 Timer", 45, 1, 120);
	Param[spT5TIMER].Set("T5TIMER", "T5 Timer", 10, 1, 240);
	Param[spT6TIMER].Set("T6TIMER", "T6 Timer", 5, 1, 240);
	Param[spT7TIMER].Set("T7TIMER", "T7 Timer", 10, 1, 240);
	Param[spT8TIMER].Set("T8TIMER", "T8 Timer", 5, 1, 120);

	Param[spSYSTEMBYTE].Set("SYSTEMBYTE", "System Byte", 0, 0, 0xFFFF);
	Param[spSYSTEMBYTEINC].Set("SYSTEMBYTEINC", "System Byte Increment, 1=Yes, 0=No", 1, 0, 1);
	Param[spDEVICEID].Set("DEVICEID", "Device ID", 1, 1, 0xFFFF);
	Param[spENTITY].Set("ENTITY", "Entity, 0=PASSIVE, 1=ACTIVE", 0, 0, 1);
	Param[spPASSIVEPORT].Set("PASSIVEPORT", "Passive Port Number", 5000, 3000, 7000);
	Param[spACTIVEIP].Set("ACTIVEIP", "Active IP Address", "192.168.1,1", 0, 0);
	Param[spACTIVEPORT].Set("ACTIVEPORT", "Active Port Number", 5000, 3000, 7000);
	Param[spDEFAULTONLINESTATE].Set("DEFTONLINE", "Default On-line State", 3, 0, 4);
	Param[spDEFAULTOFFLINESTATE].Set("DEFTOFFLINE", "Default Off-line State", 1, 0, 4);
}

void CSecsParam::SaveParam() {
	for (int i = 1; i < spLASTID; i++) {
		Param[i].SaveParam();
	}
}

void CSecsParam::UpdateHSMSParam() {
	// 	m_T3Timer = Param[spT3TIMER].iValue;
	// 	m_T5Timer = Param[spT5TIMER].iValue;
	// 	m_T6Timer = Param[spT6TIMER].iValue;
	//	m_T7Timer = Param[spT7TIMER].iValue;
	//	m_T8Timer = Param[spT8TIMER].iValue;

	m_DeviceId = Param[spDEVICEID].iValue;
	m_SystemByte = Param[spSYSTEMBYTE].iValue;
	m_SysByteInc = Param[spSYSTEMBYTEINC].iValue;

	m_SocketPortNo = Param[spPASSIVEPORT].iValue;
	m_ConnectionMode = Param[spENTITY].iValue;
	strcpy(m_cActIP, Param[spACTIVEIP].cValue);
	m_ActPort = Param[spACTIVEPORT].iValue;

	m_DefOnlineState = Param[spDEFAULTONLINESTATE].iValue;
	m_DefOfflineState = Param[spDEFAULTOFFLINESTATE].iValue;
}