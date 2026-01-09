#pragma once
class CRouPar
{
private:
	void WriteINI(char* section, char* key, int val, CString& inifile);
	void WriteINI(char* section, char* key, double val, CString& inifile);
	double ReadINI(char* section, char* name, double defvalue, CString& inifile);
public:
	CRouPar();
	CRouPar(CRouPar& p);
	virtual ~CRouPar();

	float sfc = 1.0f;
	BOOL bCalibType = FALSE;
	double fRdOffset = 0, fRsOffset = 0, SigmaCalibr1 = 100, SigmaCalibr2 = 1000;
	double kRefl1 = 1.0f, kRefl2 = 1.0f; // calibration factor 1 & 2 [1/2/2020 yuenl]
	double fReflMin = 0.001, fScatMin = 0.01;
	double Vf1 = 0, Vf2 = 0, Phase = 0;
	int Lockin = 0, Nmeas = 3, ProbeXPos = 0;
	int ProbeYPos = 0, nRefreshTime = 30;
	double RaFactor = 2.1600; // added by mahedi // factor to toggle between ra and rms unit

	void LoadParam();
	void SaveParam();

};

