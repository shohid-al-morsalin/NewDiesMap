#include "stdafx.h"

#include "..\serial\SerialICC.h"
#include "413App.h"

#include "RoughProbe.h"

//[ 03202024 ---> Mahedi Kamal {roughness measurement process}
BOOL CRoughProbe::Deinitialize() {

	m_pRoughCom->Close();
	if (m_pRoughCom) {
		delete m_pRoughCom;
		m_pRoughCom = NULL;
	}

	return TRUE;
}

void CRoughProbe::LoadParam() {
	HWPar.LoadParam();
}

void CRoughProbe::SaveParam() {
	HWPar.SaveParam();
}

//[ 03202024 ---> Mahedi Kamal {roughness measurement process}
/*load rough paramand connect with the port mentioned in INI file*/
BOOL CRoughProbe::Initialize(int nPort, CListBox* pMessage) {
	LoadParam(); SaveParam();
	if (m_pRoughCom) { delete m_pRoughCom; m_pRoughCom = NULL; }

	m_pRoughCom = new CSerialICC;
	if (m_pRoughCom) {
		if (m_pRoughCom->Open(nPort, 57600, 'N', 8, 1)) {
			pMessage->InsertString(pMessage->GetCount(), "Rough system started OK");
			return TRUE;
		}
	}
	pMessage->InsertString(pMessage->GetCount(), "Rough system started failed");
	return FALSE;
}

//[ 03202024 ---> Mahedi Kamal {roughness measurement process}
BOOL CRoughProbe::CalibrateRH(short NAver, CString& msg) {
	msg = "";
	for (int i = 0; i < 10; i++) {
		ReadVolts(NAver);
	}
	Sleep(1000);
	ReadVolts(NAver);
	double a = Lamda4PI2();
	VRd = sqrt(Iva * Iva + Ivb * Ivb);
	VRd -= HWPar.fRdOffset; if (VRd < 0) VRd = 0;
	VRs -= HWPar.fRsOffset; if (VRs < 0) VRs = 0;
	double Rsd = sqrt(VRd / (VRd + VRs));
	HWPar.kRefl1 = 0;
	if ((Rsd != 0) && (a != 0)) {
		HWPar.kRefl1 = HWPar.SigmaCalibr1 / (a * Rsd/* * HWPar.sfc*/);
		HWPar.SaveParam();
		msg.Format(" kRefl1 %.5f, \n NAver %d, \n Calibrate standard %.5f", HWPar.kRefl1, NAver, HWPar.SigmaCalibr1);
		return TRUE;
	}
	return FALSE;
}

//[ 03202024 ---> Mahedi Kamal {roughness measurement process}
/*read value from wafer usinng rough probe by serial communication*/
short CRoughProbe::ReadVolts(short NAver) {
	if (!m_pRoughCom) return 0;
	int count = 0;
	float v = 5.0f / 32768.0f; // conversion factor (digitizer to volt) [7/11/2020 yuenl]
	char Buff[9601];
	VRs = 0; Iva = 0; Ivb = 0;
	for (int i = 0; i < 5 * NAver; i++) {
		//if (pCom->ReadLine(Buff, 255)) {
		if (m_pRoughCom->ReadData(Buff, 9600)) {
			// search for @ sign
			short n = 0; // limit number data point per read [7/11/2020 yuenl]
			int nV1, nV2, nV3, nTemp1, nTemp2;
			for (int i = 0; i < 86; i++) {
				// search for first occurrence of @ [7/11/2020 yuenl]
				if (Buff[i] == '@') {
					n++; count++;
					sscanf(&Buff[i + 1], "%d ,%d ,%d ,%d ,%d ,",
						&nV1, &nV2, &nV3, &nTemp1, &nTemp2);
					VRs += v * nV1; Iva += v * nV2; Ivb += v * nV3;
					if (n > 7) break;
					if (count > NAver) break;
				}
			}
			VRs /= float(count); Iva /= float(count); Ivb /= float(count);
			return count;
		}
	}
	return 0;
}
//]

