#pragma once

#include "RouPar.h"

class CRoughProbeBase
{
private:
	virtual BOOL InitComPort(CRouPar& HWPar) {
		return TRUE;
	}
public:
	CRouPar HWPar;
	double VRs = 0, VRd = 0, Iva = 0, Ivb = 0; // maintain for UI purposes only [1/10/2020 yuenl]

	CRoughProbeBase() {}
	virtual ~CRoughProbeBase() {
		Deinitialize();
	}

	virtual BOOL Deinitialize() {
		return TRUE;
	}
	virtual BOOL Initialize(int nPort, CListBox* pMessage) {
		pMessage->InsertString(pMessage->GetCount(), "Rough system (simu) started OK");
		return TRUE;
	}
	virtual void LoadParam() {}
	virtual void SaveParam() {}
	virtual BOOL CalibrateRH(short NAver, CString& msg) {
		return TRUE;
	}
	double Read(short NAver);
	double Read();
	virtual short ReadVolts(short NAver);
	double CalculateRH(double Rs, double Rd);
	double Lamda4PI2();
};

