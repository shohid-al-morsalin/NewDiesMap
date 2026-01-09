// SiMotor.h: interface for the CSiMotor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SIMOTOR_H__2B7CC3FD_69E1_4503_88D9_60A1BCFCCA61__INCLUDED_)
#define AFX_SIMOTOR_H__2B7CC3FD_69E1_4503_88D9_60A1BCFCCA61__INCLUDED_

#include "SiMotr.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CComm;

class CSiMotor : public CSiMotr  
{
	CComm * pCom;

public:
	BOOL CloseChamber();
	BOOL OpenChamber();
	short GetWaferPresent();
	CString GetDigitalInChamber();
	void SelectLaser(short wavelength);
	CString GetDigitalIn();

	BOOL TestClose();

	BOOL TestOpen();

	BOOL IsLightCurtainTriggered();

	BOOL DigOutM2(unsigned char Bit,BOOL bHigh);

	BOOL DigOutM1(unsigned char Bit,BOOL bHigh);

	BOOL DigInM2(short Bit );

	BOOL DigInM1(short Bit );

	BOOL IsMoving();
	BOOL Init(float Current,float Velocity);
	BOOL QuerryData(CString & Reply, long tout);
	unsigned long Position();
	BOOL Reset();
	BOOL SendCmd(CString str);
	BOOL FeedLength(long position,BOOL bStop = FALSE);
	BOOL Close();
	BOOL Open(short port);
	CSiMotor();
	virtual ~CSiMotor();
	BOOL CheckBit(short Bit);
	BOOL SetDigitalOut(unsigned char Byte,BOOL bHigh);
	BOOL CommOK();
};

#endif // !defined(AFX_SIMOTOR_H__2B7CC3FD_69E1_4503_88D9_60A1BCFCCA61__INCLUDED_)
