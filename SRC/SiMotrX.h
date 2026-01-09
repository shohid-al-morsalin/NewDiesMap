// SiMotr.h: interface for the CSiMotr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SIMOTR_H__BA7FDE5E_0A01_4370_8A7B_B8A4291F1D90__INCLUDED_)
#define AFX_SIMOTR_H__BA7FDE5E_0A01_4370_8A7B_B8A4291F1D90__INCLUDED_

#include <afxmt.h>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSiMotr  
{
public:
	BOOL bSimu;
	CSiMotr();
	virtual ~CSiMotr();

protected:	
    CCriticalSection CS;
public:
	virtual BOOL CloseChamber();
	virtual BOOL OpenChamber();
	virtual short GetWaferPresent();
	virtual CString GetDigitalInChamber();
	virtual void SelectLaser(short wavelength);
	virtual CString GetDigitalIn();
	virtual BOOL Open(short port);
	virtual BOOL CheckBit(short Bit);
	virtual BOOL SetDigitalOut(unsigned char Byte,BOOL bHigh);
	virtual BOOL DigInM1(short Bit );
	virtual BOOL DigInM2(short Bit );
	virtual BOOL DigOutM1(unsigned char Bit,BOOL bHigh);
	virtual BOOL DigOutM2(unsigned char Bit,BOOL bHigh);
	virtual BOOL IsLightCurtainTriggered();
	virtual BOOL TestOpen();
	virtual BOOL TestClose();
};

extern CSiMotr *pDIO;

#endif // !defined(AFX_SIMOTR_H__BA7FDE5E_0A01_4370_8A7B_B8A4291F1D90__INCLUDED_)
