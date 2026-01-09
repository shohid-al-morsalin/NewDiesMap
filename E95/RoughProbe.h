#pragma once
#include "RouPar.h"
#include "GlobalDeviceContainer.h"
#include "RoughProbeBase.h"

//[ 03202024 ---> Mahedi Kamal 
class CSerialICC; // to use for serial communication

class CRoughProbe : public CRoughProbeBase
{
private:
	CSerialICC* m_pRoughCom = NULL;

public:
	BOOL Deinitialize();
	BOOL Initialize(int nPort, CListBox* pMessage);
	void LoadParam();
	void SaveParam();
	BOOL CalibrateRH(short NAver, CString& msg);
	short ReadVolts(short NAver);
	CRoughProbe() {}
	~CRoughProbe() {
		Deinitialize();
	}
};

//]