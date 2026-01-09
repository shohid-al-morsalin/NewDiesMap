// 128SVar.h: interface for the C128SVar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_128SVAR_H__0452C386_B5BE_4628_AB1B_1C73F5F9FF99__INCLUDED_)
#define AFX_128SVAR_H__0452C386_B5BE_4628_AB1B_1C73F5F9FF99__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class C128SVar  
{
public:
	float bp780, bp810;
	short OpMode, ServiceState;
	BOOL bScannerOnline;
	BOOL bScaleOnline;
	void Conv(char **msg);
	long Binary(char **msg);
	long GetLength();
	short WaferPresent; // indicate whether wafer is present at station
	C128SVar();
	virtual ~C128SVar();
};

#endif // !defined(AFX_128SVAR_H__0452C386_B5BE_4628_AB1B_1C73F5F9FF99__INCLUDED_)
