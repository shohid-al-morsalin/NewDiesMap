// SysInfo.h: interface for the CSysInfo class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

class CSysInfo {
public:
	short magic;
	//  [6/30/2008 Yuen]
	//float beampath810;
	//float beampath750;
	float BP810;	// beampath for 810 laser, obsolete [6/30/2008 Yuen]
	float BP750;	// beampath for 750 laser, obsolete [6/30/2008 Yuen]
	float stressconst;
	enum REFSCHEME { rfZERO, rfFIRST, rfPREVIOUSSCAN, rfPREVIOUSWAFER, rfSPECIFIEDSCAN, rfSPECIFIEDWAFER };
	short RefScheme;
	enum SHOWLINES { lnREF = 1, lnCUR = 2, lnDIF = 4, lnLSQ = 8, lnPRO = 16 };
	short ShowLines;
public:

public:
	BOOL bFlipX;
	float NotchAngle;
	void Conv(char** msg);
	long Binary(char** msg);
	long GetLength();
	void Clear();
	void SaveParam();
	void LoadParam();

	CSysInfo();
	virtual ~CSysInfo();
	void Serialize(CArchive& ar);
	CSysInfo& operator =(CSysInfo& dat);
};

extern CSysInfo SysInf;
