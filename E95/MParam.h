#ifndef __MPARAM_H__
#define __MPARAM_H__

// Note: changing MaXPA or MAXMEPASET value will void CRecipe413 downward compatibility[3/24/2012 Administrator]
#define MaXPA				16
#define MAXMEPASET			9
#define MAXMATCHPATT		9
#define MAXTGTSET			4

#include "..\SRC\BMake.h"

struct MParam {
	short magic;

	// Echo Section [7/15/2010 C2C]
	short D;		// D = @ of decimal [6/2/2010 Valued Customer]
	short Fr, To;
	enum PRBNUM {
		NOPRB,				// NOPRB = no measurement [3/14/2010 Valued Customer]
		ECHO1PRB,			// Top echo probe [3/14/2010 Valued Customer]
		ECHO2PRB,			// Bottom echo probe [4/17/2013 Administrator]
		ECHOTT1PRB,			// ECHOTT1 = total echo set 1 [3/14/2010 Valued Customer]
		ECHO1AFPRB,			// Top echo probe with auto focus [3/14/2010 Valued Customer]
		ECHO2AFPRB,			// Bottom echo probe with auto focus[4/17/2013 Administrator]
		ECHOTTAFPRB,		// ECHOTT2 = total echo set 2 [3/14/2010 Valued Customer]
		ROUGHPRB,			// ROUGHPRB = roughness [3/14/2010 Valued Customer]  //03202024 ----> Mahedi Kamal {for adding rough probe}
		ODSTIPRB,			// ODSTIPRB = thin film probe for measuring OD and STI [3/14/2010 Valued Customer]
		MATHPRB,			// MATHPRB = virtual probe, calculation probe [3/14/2010 Valued Customer]
		NTRIMPROBE1,		// Narrow trim width probe using GetTrimWidth[4/15/2013 Administrator]
		STEPHEIGHTTHKPRB,	// Trim height using top probe [8/21/2011 FSMT]
		WTRIMPRB1,			// Wide trim width probe using GetTrimWidth [4/16/2013 Administrator]
		POFPRB,				// Pass or fail [9/9/2012 Administrator]
		TFILMSDIPRB,		// SDI Probe (not use) [2/26/2012 Administrator]
		INTENSI1PRB,		// Intensity top [4/17/2013 Administrator]
		INTENSI2PRB,		// Intensity bottom [4/17/2013 Administrator]
		STEPHEIGHTDISTPRB,	// Trim height using distance measurement top probe [8/21/2011 FSMT]
		PPOS1PRB,			// Peak position top [4/17/2013 Administrator]
		PPOS2PRB,			// Peak position bottom [4/17/2013 Administrator]
		SHAPEFAC1PRB,		// Peak shape top [4/17/2013 Administrator]
		SHAPEFAC2PRB,		// Peak shape bottom [4/17/2013 Administrator]
		BOWWARP1,			// Bow warp top [4/17/2013 Administrator]
		BOWWARP2,			// Bow warp bottom [4/17/2013 Administrator]
		STEPHEIGHTCOMPRB,	// Combine thickness and distance trim measurement [8/21/2011 FSMT]
		BUMPHEIGHTTPRB,		// Generic height different measurement using top probe [8/21/2011 FSMT]
		BUMPHEIGHTBPRB,		// Generic height different measurement using bottom probe [8/21/2011 FSMT]
		STEPHEIGHTTHKBPRB,	// trim height using bottom probe [8/21/2011 FSMT]
		STRESSPRB,			// Stress [2/23/2012 Administrator]
		BEVELPRB,			// Bevel measurement [5/29/2012 Yuen]
		TFILMSPMV,			// Single point for UMC B-PAD [11/20/2012 Yuen]
		ECMARK1, ECMARK2,	// Position markers for Echo Probe [11/3/2012 Yuen]
		TFMARK1, TFMARK2,	// Position markers for Thin Film Probe [11/3/2012 Yuen]
		TFILMPRB,			// Single point TF measurement [2/26/2012 Administrator]
		BESTVALUEPRB,		// Pick the best value from a list of measurement result [12/29/2012 Administrator]
		TFILMFASTPRB,		// One step pattern matching, single point [2/6/2013 Administrator]
		TRIMWHPROB,			// Combined trim width and height [4/13/2013 Administrator]
		NTRIMPROBE2,		// Narrow trim width probe using FindEdge 1 [4/15/2013 Administrator]
		WTRIMPRB2,			// Wide trim width probe using FindEdge 1 [4/16/2013 Administrator]
		NTRIMPROBE3,		// Narrow trim width probe using FindEdge 2 [4/15/2013 Administrator]
		WTRIMPRB3,			// Wide trim width probe using FindEdge 2 [4/16/2013 Administrator]
		RADIALMULTITPRB,	// Multiple measurement along radial direction, max point is 32 [9/18/2011 FSMT]
		RADIALMULTIBPRB,	// Multiple measurement along radial direction, max point is 32 [9/18/2011 FSMT]
		AFOCUSPRB,			// Auto focus [4/17/2013 Administrator]
		AEXPOSUREPRB,		// Auto exposure [4/17/2013 Administrator]
		SPCALIBPRB,			// Single probe calibration [4/17/2013 Administrator]
		DPCALIBPRB,			// Dual probe calibration  [4/17/2013 Administrator]
		STEPHEIGHTCOMDTT,	// Step height combining total thickness and distance [4/23/2013 Administrator]
		TRENCHTOPPRB,		// Trench depth using top probe
		TRENCHBOTPRB,		// Trench depth using bottom probe
		BEVELPRB2,			// Thin bevel measurement probe
		BEVEL_TF_PRB,		// Bevel width using thin film camera probe	[6/14/2022 ZHIMING]
		CONFOCAL_TOP,		// AKUSENSE Top Prob Measurement [20221108]	//11142022 / yukchiu
		CONFOCAL_BOT,		// AkUSENSE Bottom Probe Measurement
		CONFOCAL_TOTAL,		// AKUSENSE Confocal thickness measurement // Tushar // 20231227

		CONFOCAL_LTV,       // AKUSENSE Confocal Local Thickness Variation //24102024/MORSALIN
		CONFOCAL_STIR,       // AKUSENSE Site Total Indicator Reading //24102024/MORSALIN
		CONFOCAL_TIR,       // AKUSENSE Confocal Total Indicator Reading //24102024/MORSALIN
		CONFOCAL_LTV_STIR,   // AKUSENSE LTV and STIR //24102024/MORSALIN
		
		CONFOCAL_TTV,// AKUSENSE Confocal Total Indicator Reading //24102024/MORSALIN
		CONFOCAL_DUALWARP,	// AKUSENSE Dual probe Warp Measurement	/ 23102024 /Tushar
		CONFOCAL_WARP,		// AKUSENSE Warp Measurement	[20221108]	//11142022 / yukchiu
		CONFOCAL_ROUGHNESS, // AKUSENSE Roughness Measurement / 12102024 / Tushar
		RINGPRB,            // RINGPRB = Ring Width (Taiko) / Mahedi kamal -> 08132024
		CONFOCAL_LTV_MIN, // AKUSENSE LTV Min // 10222025 MORSALIN
		CONFOCAL_LTV_MAX, // AKUSENSE LTV Max // 10222025 MORSALIN
		CONFOCAL_LTV_AVG, // AKUSENSE LTV Avg // 10222025 MORSALIN
		DISPPRB = 99
	};
	PRBNUM Prb;				// Probe number [6/2/2010 Valued Customer]
	float RI, DF;			// RI = reflective index, DF = default value [6/2/2010 Valued Customer]
	float Lo, Hi;			// High low limits [3/11/2010 Valued Customer]
	float Off, OffX, OffY;  // Offsets [7/20/2011 XPMUser]
	CString Name;
	BOOL bHost;				// Report to Host [1/4/2011 FSMT]
	float fThkn;			// returned value for convenient only, not serialize [3/12/2010 Valued Customer]
	float fRough;			    // Mahedi Kamal - 03202024
	float LTV_MIN;
	float LTV_MAX;
	float LTV_AVG;
	
	// 08142024 - START
	float fRingWidth;
	float fRingSTD;
	float fRingBevelWidth;
	float fRingTTV;         
	float fRingStepHeight;   
	float fRingThickness;
	// 08142024 - END

	// Thin Film Section [7/15/2010 C2C]
	float FThckMindmy;		// Minimum search film thickness [7/15/2010 C2C]
	float FThckMaxdmy;		// Maximum search film thickness [7/15/2010 C2C]
	short IntegrationTimedmy;
	short NPeak;
	short Methoddmy;
	float FRIdmy;			// Thin film refractive index [7/15/2010 C2C]
	CString Formula; // [7/20/2011 XPMUser]

	void VerboseOutHeader(FILE* fp) {
		if (!fp) {
			return;
		}
		fprintf(fp, "Probe\tFrom\tTo\tRI\tLow\tHigh\tDecimal\tDefault\tOffset\t#Peak\tFormula\tHost\n");
	}
	void VerboseOut(FILE* fp, short index) {
		if (!fp) {
			return;
		}
		fprintf(fp, "%d\t%s\t%d\t%d\t%.6f\t%.6f\t%.6f\t%d\t%.3f\t%.3f\t%d\t%s\t%d\n", Prb, Name.GetBuffer(0), Fr, To, RI, Lo, Hi, D, DF, Off, NPeak, Formula.GetBuffer(0), bHost);
	}

	void Conv(char** msg) {
		CMake::Conv(D, msg);
		CMake::Conv(Fr, msg);
		CMake::Conv(To, msg);
		short p;
		CMake::Conv(p, msg);
		Prb = (PRBNUM)p;
		CMake::Conv(RI, msg);
		CMake::Conv(DF, msg);
		CMake::Conv(Lo, msg);
		CMake::Conv(Hi, msg);
		CMake::Conv(Off, msg);
		CMake::Conv(Name, msg);
		CMake::Conv(Formula, msg);
		CMake::Conv(fThkn, msg);
		CMake::Conv(FThckMindmy, msg);
		CMake::Conv(FThckMaxdmy, msg);
		CMake::Conv(IntegrationTimedmy, msg);
		CMake::Conv(NPeak, msg);
		CMake::Conv(FRIdmy, msg);
		CMake::Conv(Methoddmy, msg);
		CMake::Conv(bHost, msg);
	}

	long Binary(char** msg) {
		long sz = 0;

		sz += CMake::Binary(D, msg);
		sz += CMake::Binary(Fr, msg);
		sz += CMake::Binary(To, msg);
		short p = Prb;
		sz += CMake::Binary(p, msg);
		sz += CMake::Binary(RI, msg);
		sz += CMake::Binary(DF, msg);
		sz += CMake::Binary(Lo, msg);
		sz += CMake::Binary(Hi, msg);
		sz += CMake::Binary(Off, msg);
		sz += CMake::Binary(Name, msg);
		sz += CMake::Binary(Formula, msg);
		sz += CMake::Binary(fThkn, msg);
		sz += CMake::Binary(FThckMindmy, msg);
		sz += CMake::Binary(FThckMaxdmy, msg);
		sz += CMake::Binary(IntegrationTimedmy, msg);
		sz += CMake::Binary(NPeak, msg);
		sz += CMake::Binary(FRIdmy, msg);
		sz += CMake::Binary(Methoddmy, msg);
		sz += CMake::Binary(bHost, msg);
		return sz;
	}

	long GetLength() {
		long sz = 0;

		sz += CMake::GetLength(D);
		sz += CMake::GetLength(Fr);
		sz += CMake::GetLength(To);
		short p;
		sz += CMake::GetLength(p);
		sz += CMake::GetLength(RI);
		sz += CMake::GetLength(DF);
		sz += CMake::GetLength(Lo);
		sz += CMake::GetLength(Hi);
		sz += CMake::GetLength(Off);
		sz += CMake::GetLength(Name);
		sz += CMake::GetLength(Formula);
		sz += CMake::GetLength(fThkn);
		sz += CMake::GetLength(FThckMindmy);
		sz += CMake::GetLength(FThckMaxdmy);
		sz += CMake::GetLength(IntegrationTimedmy);
		sz += CMake::GetLength(NPeak);
		sz += CMake::GetLength(FRIdmy);
		sz += CMake::GetLength(Methoddmy);
		sz += CMake::GetLength(bHost);

		return sz;
	}

	MParam() {
		magic = 9;
		Clear();
	}
	BOOL IsAutoFocus() {
		switch (Prb) {
		case MParam::ECHO1AFPRB:
		case MParam::ECHO2AFPRB:
		case MParam::ECHOTTAFPRB:
			return TRUE;
			break;
		}
		return FALSE;
	}
	BOOL IsTTV() {
		switch (Prb) {
		case MParam::BOWWARP1:
		case MParam::BOWWARP2:
		case MParam::CONFOCAL_WARP:
		case MParam::INTENSI1PRB:
		case MParam::INTENSI2PRB:
		case MParam::PPOS1PRB:
		case MParam::PPOS2PRB:
		case MParam::SHAPEFAC1PRB:
		case MParam::SHAPEFAC2PRB:
		case MParam::STRESSPRB:
			return FALSE;
			break;
		}
		return TRUE;
	}
	BOOL IsProbeToSurface() {
		switch (Prb) {
		case ECHO1PRB:
		case ECHO2PRB:
		{
			if (((Fr == 1) && (To == 2)) || ((Fr == 2) && (To == 1))) {
				return TRUE;
			}
			break;
		}
		case BOWWARP1:
		case BOWWARP2:
		case CONFOCAL_WARP:
		{
			return TRUE;
			break;
		}
		//[23102024
		case CONFOCAL_DUALWARP:
		{
			return TRUE;
			break;
		}
		//]
		// // //[12102024
		case CONFOCAL_ROUGHNESS:
		{
			return TRUE;
			break;
		}
		//]
		// [11222022 ZHIMING
		case CONFOCAL_TOP:
		case CONFOCAL_BOT:
		{
			return TRUE;
			break;
		}
		// ]
		}
		return FALSE;
	}
	BOOL IsTopProbe() {
		switch (Prb) {
		case ECHO1PRB:
		case BOWWARP1:
		case CONFOCAL_TOP:
		case CONFOCAL_WARP:
		{
			return TRUE;
			break;
		}
		}
		return FALSE;
	}
	BOOL IsMathProbe() {
		switch (Prb) {
		case MATHPRB:
		case BESTVALUEPRB:
		{
			return TRUE;
			break;
		}
		}
		return FALSE;
	}
	BOOL IsBottomProbe() {
		switch (Prb) {
		case ECHO2PRB:
		case BOWWARP2:
		case CONFOCAL_BOT:
		{
			return TRUE;
			break;
		}
		}
		return FALSE;
	}
	BOOL IsStressMeasurement() {
		if (Prb == STRESSPRB) {
			return TRUE;
		}
		return FALSE;
	}
	//[ 03202024 ----> Mahedi Kamal {for showing rough probe}
	BOOL IsRoughnessMeasurement() {
		if (Prb == ROUGHPRB) {
			return TRUE;
		}
		return FALSE;
	}
	//]
	//[ 08132024 ----> Mahedi Kamal
	BOOL IsRingWidthMeasurement() {
		if (Prb == RINGPRB) {
			return TRUE;
		}
		return FALSE;
	}
	//]
	BOOL IsEchoMeasurement() {
		if (IsThicknessMeasurement()) {
			return TRUE;
		}
		if (IsTotalThicknessMeasurement()) {
			return TRUE;
		}
		if (IsEchoBowWarpMeasurement()) {
			return TRUE;
		}
		if (IsStressMeasurement()) {
			return TRUE;
		}
		if (IsMultiPointMeasurement()) {
			return TRUE;
		}
		return FALSE;
	}

	// [20231227 ZHIMING
	BOOL IsConfocalMeasurement() {
		switch (Prb) {
		case CONFOCAL_TOP:
		case CONFOCAL_BOT:
		case CONFOCAL_TOTAL:
		case CONFOCAL_WARP:
		//case CONFOCAL_STIR: //24102024/MORSALIN
		//case CONFOCAL_LTV: //24102024/MORSALIN
		case CONFOCAL_DUALWARP://23102024
		case CONFOCAL_ROUGHNESS://12102024
		case CONFOCAL_TIR:  //24102024/MORSALIN
		case CONFOCAL_TTV:  //24102024/MORSALIN
			return TRUE;
		}
		return FALSE;
	}

	// [ 24102024/MORSALIN
	BOOL IsConfocalLocalMeasurement()
	{
		switch (Prb)
		{
		case CONFOCAL_LTV:
		case CONFOCAL_STIR:
		case CONFOCAL_LTV_MIN:  // 10222025 MORSALIN
		case CONFOCAL_LTV_MAX: // 10222025 MORSALIN
		case CONFOCAL_LTV_AVG: // 10222025 MORSALIN
		//case CONFOCAL_TTV:
		case CONFOCAL_LTV_STIR:
			return TRUE;
		}
		return FALSE;
	}
	// ]

	// [11222022 ZHIMING
	BOOL IsConfocalWarpMeasurement() {
		if (Prb == CONFOCAL_WARP) return TRUE;
		return FALSE;
	}
	// ]

	// [23102024
	BOOL IsConfocalDualWarpMeasurement() {
		if (Prb == CONFOCAL_DUALWARP) return TRUE;
		return FALSE;
	}
	// ]

	// [12102024
	BOOL IsConfocalRoughnessMeasurement() {
		if (Prb == CONFOCAL_ROUGHNESS) return TRUE;
		return FALSE;
	}
	// ]

	// [Tushar //20231227
	BOOL IsConfocalTotalThicknessMeasurement() {
		if (Prb == CONFOCAL_TOTAL ) return TRUE; 
		return FALSE;
	}
	// ]

	//[ 12172024
	BOOL IsEchoThicknessMeasurement() {
		if (Prb == ECHO1PRB || Prb == ECHO2PRB || Prb == ECHOTT1PRB) {
			return TRUE;
		}
		return FALSE;
	}
	//]
	BOOL IsThicknessMeasurement() {
		switch (Prb) {
		case ECHO1PRB:
		case ECHO2PRB:
		case ECHO1AFPRB:
		case ECHO2AFPRB:
		{
			return TRUE;
			break;
		}
		}
		return FALSE;
	}
	BOOL IsEchoBowWarpMeasurement() {
		if ((Prb == BOWWARP1) || (Prb == BOWWARP2)) {
			return TRUE;
		}
		return FALSE;
	}
	BOOL IsTotalThicknessMeasurement() {
		if ((Prb == ECHOTT1PRB) || (Prb == ECHOTTAFPRB)) {
			return TRUE;
		}
		return FALSE;
	}
	BOOL IsThinFilmMeasurement() {
		if ((Prb == ODSTIPRB) || (Prb == TFILMPRB) || (Prb == TFILMSPMV)) {
			return TRUE;
		}
		return FALSE;
	}
	BOOL IsMarker() {
		if ((Prb == ECMARK1) || (Prb == ECMARK2) || (Prb == TFMARK1) || (Prb == TFMARK2)) {
			return TRUE;
		}
		return FALSE;
	}
	short IsEchoMarker() {
		if (Prb == ECMARK1) {
			return 1;
		}
		if (Prb == ECMARK2) {
			return 2;
		}
		return 0;
	}
	short IsThinFilmMarker() {
		if (Prb == TFMARK1) {
			return 1;
		}
		if (Prb == TFMARK2) {
			return 2;
		}
		return 0;
	}
	BOOL IsDisplayPrb() {
		if (Prb == DISPPRB) {
			return TRUE;
		}
		return FALSE;
	}
	BOOL IsEdgeMeasurement() {
		if (
			(Prb == NTRIMPROBE1)
			|| (Prb == WTRIMPRB1)
			|| (Prb == NTRIMPROBE2)
			|| (Prb == WTRIMPRB2)
			|| (Prb == NTRIMPROBE3)
			|| (Prb == WTRIMPRB3)
			|| (Prb == BEVELPRB)
			|| (Prb == TRIMWHPROB)
			|| (Prb == STEPHEIGHTDISTPRB)
			|| (Prb == STEPHEIGHTCOMPRB)
			) {
			return TRUE;
		}
		return FALSE;
	}
	BOOL IsHeightMeasurement() {
		if (
			(Prb == STEPHEIGHTTHKPRB) ||
			(Prb == STEPHEIGHTTHKBPRB) ||
			(Prb == STEPHEIGHTCOMDTT) ||
			(Prb == TRENCHTOPPRB) ||
			(Prb == TRENCHBOTPRB) ||
			(Prb == BUMPHEIGHTTPRB) ||
			(Prb == BUMPHEIGHTBPRB)
			) {
			return TRUE;
		}
		return FALSE;
	}
	BOOL IsBevelMeasurement() {
		if (Prb == BEVELPRB) {
			return TRUE;
		}
		if (Prb == BEVEL_TF_PRB) {
			return TRUE;
		}
		return FALSE;
	}
	BOOL IsBevelUsingTF() {
		if (Prb == BEVEL_TF_PRB) {
			return TRUE;
		}
		return FALSE;
	}
	BOOL IsMultiPointMeasurement() {
		if ((Prb == RADIALMULTITPRB) || (Prb == RADIALMULTIBPRB)) {
			return TRUE;
		}
		return FALSE;
	}
	BOOL IsUtilityMeasurement() {
		switch (Prb) {
		case AFOCUSPRB:
		case AEXPOSUREPRB:
		case SPCALIBPRB:
		case DPCALIBPRB:
			return TRUE;
			break;
		}
		return FALSE;
	}
	void Clear() {
		Name = "";
		Formula = "";
		Fr = To = 1;
		Prb = NOPRB;
		RI = 1;
		D = 2;
		DF = 0;
		Lo = Hi = 0;
		Off = OffX = OffY = 0;
		fThkn = 0;
		FThckMaxdmy = 0;
		FThckMindmy = 0;
		IntegrationTimedmy = 80;
		NPeak = 0;
		FRIdmy = 1;
		Methoddmy = 0;
		bHost = TRUE;
		OffX = OffY = 0;
	}
	void Serialize(CArchive& ar) {
		short a;
		if (ar.IsStoring()) {
			magic = 10;
			ar << magic;
			ar << Fr;
			ar << To;
			a = Prb;
			ar << a;
			ar << RI;
			ar << Hi;
			ar << Lo;
			ar << Name;
			ar << D;
			ar << DF;
			ar << Off;
			ar << OffX;
			ar << OffY;
			ar << Formula;
			ar << FThckMaxdmy;
			ar << FThckMindmy;
			ar << IntegrationTimedmy;
			ar << NPeak;
			ar << FRIdmy;
			ar << Methoddmy;
			ar << bHost;
		}
		else {
			ar >> magic;
			ar >> Fr;
			ar >> To;
			ar >> a;
			Prb = (PRBNUM)a;
			ar >> RI;
			ar >> Hi;
			ar >> Lo;
			ar >> Name;
			if (Prb == NOPRB) {
				Name = "";
			}
			if (magic > 1) {
				ar >> D;
			}
			if (magic > 2) {
				ar >> DF;
			}
			if (magic > 3) {
				ar >> Off;
				ar >> OffX;
				ar >> OffY;
				if (magic < 10) {
					OffX = 0;
					OffY = 0;
				}
			}
			else {
				Off = 0;
			}
			if (magic > 4) {
				ar >> Formula;
			}
			else {
				Formula = "";
			}
			if (magic > 5) {
				ar >> FThckMaxdmy;
				ar >> FThckMindmy;
				ar >> IntegrationTimedmy;
				ar >> NPeak;
				ar >> FRIdmy;
				if (magic < 9) {
					NPeak = 0;
				}
			}
			if (magic > 6) {
				ar >> Methoddmy;
			}
			if (magic > 7) {
				ar >> bHost;
			}
			else {
				bHost = TRUE;
			}
		}
	}
};

#endif
