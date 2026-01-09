#pragma once
#include "..\IPC\SMOL.h"

// #define BADDATA			9999999

class CVar {
public:
	int WinID;
	//BOOL bOnline;
/* Indicates whether SECS is active */
// 	BOOL bSECS;

	/* perform background subtraction */

	BOOL bBkgSubtract;

	/* maximum, slot number in a cassette */

	short MaxSlot;

	/* maximum plotable lines in a chart */

	short MaxPlotLine;

	/* Ugly code to take care of hangup during Scanning */
	BOOL bScanning;
	BOOL bSortByRecipe;

	short Storagtype; // 0-same disk, othermultiple disk [12/17/2007 YUEN]

public:

	short SPCMethod; //  [11/3/2009 Yuen]
	short Laser1, Laser2;  //  [11/3/2009 Yuen]

	BOOL bFlatten;  //  [11/3/2009 Yuen]
	BOOL bAirCyl;
	BOOL bFFO[5]; // FOUP FOR ONE, TRUE on active

	long HostWDTime;

	BOOL bHostWD;
	BOOL bClampOnUnload;
	BOOL bEnabled;

	short nCIDR;
	// 	short LoadUnloadScheme;
	short UIMode;

	BOOL bEnableBaseline;
	BOOL bByPassCIDR;
	BOOL bTransferInProgress[4];
	BOOL bAutoClose;
	BOOL bAMHS; // AMHS handoff active
	BOOL bBkgSubstract;

	short Repeat;
	short nLoadPort;
	short PrintPitch;

	CString WorkDir;
	CString ShowWaferFilter;
	CString FilmThicknessUnit;
	CString StressUnit;

	BOOL bSkipFirstScan;
	BOOL bHideMapLine;
	BOOL bShowAllWafer;
	BOOL bReCalc;
	BOOL bCalcHeightUsingWaferSize;

	float FilmThicknessFactor;
	float StressFactor;

	BOOL SetTransferInProgress(short port, BOOL state);

	void SaveParam(void);

	void LoadParam(void);

	// CTE support
	double sc1, sc2, gd1, gd2, cte, bem; // CTE calculation
	BOOL ismegapascal;

	CVar(void);

	virtual ~CVar(void);
};

extern CVar Var;
