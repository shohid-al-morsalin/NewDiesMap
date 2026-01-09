// 900SVar.h: interface for the C900SVar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_900SVAR_H__8C765641_3F52_43E0_A5D4_A7F4193E0AC4__INCLUDED_)
#define AFX_900SVAR_H__8C765641_3F52_43E0_A5D4_A7F4193E0AC4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class C900SVar  
{
public:
	float bp780, bp810;
	short OpMode,ServiceState;
	enum ENVRN { GASMODE, VACMODE, PURGEMODE, OFFMODE };
	enum GASSETTING { GASOFF, N2GAS, ARGAS, FORMINGGAS };
	
	BOOL bWaferPresent;
	BOOL bHeatOK;
	BOOL bVacE5;
	BOOL bRoughPump;
	BOOL bTurboPump;
	BOOL bOverTemp2;
	BOOL bChamberHot;
	BOOL bGasPresent;
	BOOL bWaterLeak;
	BOOL bWaterFlow;// Water interlock
	BOOL bOverTemp1;
	BOOL bVacE3;
	BOOL bGasFlow; // Gas interlock
	short GasSetting;	// N2/N2H2/Ar
	short Environ;		// Environment: Gas/Vac/Purge
	short Tempr;		// Ctrl Temperature.
	float Lamp[12];		// Lamp Current
	float VacIG;		// IG vacuum pressure
	float VacTC;		// TC vacuum pressure

	void Conv(char **msg);
	long Binary(char **msg);
	long GetLength();

	C900SVar();
	virtual ~C900SVar();
};

#endif // !defined(AFX_900SVAR_H__8C765641_3F52_43E0_A5D4_A7F4193E0AC4__INCLUDED_)
