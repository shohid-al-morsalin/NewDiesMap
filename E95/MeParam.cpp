// MeParam.cpp: implementation of the CMeParam class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MeParam.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMeParam::CMeParam() {
	magic = 8;
	for (int i = 0; i < MaXPA; i++) {
		MPa[i].Clear();
	}
	PSG[0].Clear();
	PSG[1].Clear();
	Clear();
}

CMeParam::~CMeParam() {}

void CMeParam::Serialize(CArchive& ar) {
	int i;
	if (ar.IsStoring()) {
		magic = 9;
		ar << magic;
		ar << NAve;
		for (i = 0; i < MaXPA; i++) {
			MPa[i].Serialize(ar);
		}
		for (i = 0; i < 2; i++) {
			PSG[i].Serialize(ar);
		}
		ar << Ta.ThkMin[0];
		ar << Ta.ThkMax[0];
		ar << Ta.IntegTime;
		ar << Ta.Boxcar;
		ar << Ta.RI[0];
		ar << Ta.Method;
		for (i = 0; i < MAXPEAKCOUNT; i++) {
			PS1[i].Serialize(ar);
			PS2[i].Serialize(ar);
		}
		ar << Ta.xlmin;
		ar << Ta.xlmax;
		ar << Ta.ThkMin[1];
		ar << Ta.ThkMax[1];
		for (i = 0; i < 2; i++) {
			ar << NPeak[i];
			ar << bEqual[i];
		}
		ar << Ta.RI[1];
		//int i;
		for (i = 2; i < 3/*MAXTPAVLU*/; i++) {
			ar << Ta.ThkMin[i];
			ar << Ta.ThkMax[i];
			ar << Ta.RI[i];
		}
		for (i = 3; i < MAXTHKVLU; i++) {
			ar << Ta.ThkMin[i];
			ar << Ta.ThkMax[i];
			ar << Ta.RI[i];
		}
	}
	else {
		ar >> magic;
		ar >> NAve;
		if (magic < 7) {
			for (i = 0; i < 8; i++) {
				MPa[i].Serialize(ar);
			}
		}
		else {
			for (i = 0; i < MaXPA; i++) {
				MPa[i].Serialize(ar);
			}
		}
		for (i = 0; i < 2; i++) {
			PSG[i].Serialize(ar);
		}
		if (magic > 1) {
			ar >> Ta.ThkMin[0];
			ar >> Ta.ThkMax[0];
			ar >> Ta.IntegTime;
			ar >> Ta.Boxcar;
			ar >> Ta.RI[0];
			ar >> Ta.Method;
		}
		if (magic > 2) {
			for (i = 0; i < MAXPEAKCOUNT; i++) {
				PS1[i].Serialize(ar);
				PS2[i].Serialize(ar);
			}
		}
		if (magic > 3) {
			ar >> Ta.xlmin;
			ar >> Ta.xlmax;
		}
		if (magic > 4) {
			ar >> Ta.ThkMin[1];
			ar >> Ta.ThkMax[1];
		}
		if (magic > 5) {
			for (i = 0; i < 2; i++) {
				ar >> NPeak[i];
				ar >> bEqual[i];
			}
		}
		if (magic > 7) {
			ar >> Ta.RI[1];
			for (i = 2; i < 3/*MAXTPAVLU*/; i++) {
				ar >> Ta.ThkMin[i];
				ar >> Ta.ThkMax[i];
				ar >> Ta.RI[i];
			}
		}
		if (magic > 8) {
			for (i = 3; i < MAXTHKVLU; i++) {
				ar >> Ta.ThkMin[i];
				ar >> Ta.ThkMax[i];
				ar >> Ta.RI[i];
			}
		}
	}
}

CMeParam& CMeParam::operator =(CMeParam& co) {
	int i;
	magic = co.magic;
	NAve = co.NAve;
	for (i = 0; i < MAXTHKVLU; i++) {
		Ta.ThkMin[i] = co.Ta.ThkMin[i];
		Ta.ThkMax[i] = co.Ta.ThkMax[i];
		Ta.RI[i] = co.Ta.RI[i];
	}
	Ta.IntegTime = co.Ta.IntegTime;
	Ta.Boxcar = co.Ta.Boxcar;
	Ta.Method = co.Ta.Method;
	Ta.xlmin = co.Ta.xlmin;
	Ta.xlmax = co.Ta.xlmax;

	for (i = 0; i < 2; i++) {
		NPeak[i] = co.NPeak[i];
		bEqual[i] = co.bEqual[i];
		PSG[i] = co.PSG[i];
	}
	for (i = 0; i < MAXPEAKCOUNT; i++) {
		PS1[i] = co.PS1[i];
		PS2[i] = co.PS2[i];
	}
	for (i = 0; i < MaXPA; i++) {
		MPa[i] = co.MPa[i];
	}
	return *this;
}

void CMeParam::Conv(char** msg) {
	int i;
	CMake::Conv(NAve, msg);
	for (i = 0; i < MaXPA; i++) {
		MPa[i].Conv(msg);
	}
	PSG[0].Conv(msg);
	PSG[1].Conv(msg);
	for (i = 0; i < MAXTHKVLU; i++) {
		CMake::Conv(Ta.ThkMin[i], msg);
		CMake::Conv(Ta.ThkMax[i], msg);
		CMake::Conv(Ta.RI[i], msg);
	}
	CMake::Conv(Ta.IntegTime, msg);
	CMake::Conv(Ta.Boxcar, msg);
	CMake::Conv(Ta.Method, msg);
	for (i = 0; i < MAXPEAKCOUNT; i++) {
		PS1[i].Conv(msg);
		PS2[i].Conv(msg);
	}
	CMake::Conv(Ta.xlmin, msg);
	CMake::Conv(Ta.xlmax, msg);
	for (i = 0; i < 2; i++) {
		CMake::Conv(NPeak[i], msg);
		CMake::Conv(bEqual[i], msg);
	}
}

long CMeParam::Binary(char** msg) {
	int i;
	long sz = 0;

	sz += CMake::Binary(NAve, msg);
	for (i = 0; i < MaXPA; i++) {
		sz += MPa[i].Binary(msg);
	}
	sz += PSG[0].Binary(msg);
	sz += PSG[1].Binary(msg);
	for (i = 0; i < MAXTHKVLU; i++) {
		sz += CMake::Binary(Ta.ThkMin[i], msg);
		sz += CMake::Binary(Ta.ThkMax[i], msg);
		sz += CMake::Binary(Ta.RI[i], msg);
	}
	sz += CMake::Binary(Ta.IntegTime, msg);
	sz += CMake::Binary(Ta.Boxcar, msg);
	sz += CMake::Binary(Ta.Method, msg);
	for (i = 0; i < MAXPEAKCOUNT; i++) {
		sz += PS1[i].Binary(msg);
		sz += PS2[i].Binary(msg);
	}
	sz += CMake::Binary(Ta.xlmin, msg);
	sz += CMake::Binary(Ta.xlmax, msg);
	for (i = 0; i < 2; i++) {
		sz += CMake::Binary(NPeak[i], msg);
		sz += CMake::Binary(bEqual[i], msg);
	}
	return sz;
}

long CMeParam::GetLength() {
	int i;
	long sz = 0;

	sz += CMake::GetLength(NAve);
	for (i = 0; i < MaXPA; i++) {
		sz += MPa[i].GetLength();
	}
	sz += PSG[0].GetLength();
	sz += PSG[1].GetLength();
	for (i = 0; i < MAXTHKVLU; i++) {
		sz += CMake::GetLength(Ta.ThkMin[i]);
		sz += CMake::GetLength(Ta.ThkMax[i]);
		sz += CMake::GetLength(Ta.RI[i]);
	}
	sz += CMake::GetLength(Ta.IntegTime);
	sz += CMake::GetLength(Ta.Boxcar);
	sz += CMake::GetLength(Ta.Method);
	for (i = 0; i < MAXPEAKCOUNT; i++) {
		sz += PS1[i].GetLength();
		sz += PS2[i].GetLength();
	}
	sz += CMake::GetLength(Ta.xlmin);
	sz += CMake::GetLength(Ta.xlmax);
	for (i = 0; i < 2; i++) {
		sz += CMake::GetLength(NPeak[i]);
		sz += CMake::GetLength(bEqual[i]);
	}
	return sz;
}

short CMeParam::GetDataCol(short idx, short& start) {
	// 	short cnt = 0;
	//
	for (int i = 0; i < MaXPA; i++) {
		if (MPa[i].Prb > 0) {
			if (/*cnt*/start == idx) {
				return i;
			}
			//cnt++;
			start++;
		}
	}
	return -1;
}

void CMeParam::VerboseOut(FILE* fp) {
	if (!fp) {
		return;
	}

	int i;

	fprintf(fp, "Measurement Parameters\n");
	fprintf(fp, "Version: %d\n", magic);
	fprintf(fp, "# averaging: %d\n", NAve);
	fprintf(fp, "Top probe # peaks: %d, compare: %s\n", NPeak[0], bEqual[0] ? "Equal" : "Greater");
	fprintf(fp, "Bottom probe # peaks: %d, compare: %s\n", NPeak[1], bEqual[1] ? "Equal" : "Greater");
	//	fprintf(fp, "Wavelength: min=%.3f max=%.3f\n", lmin, lmax);
	for (i = 0; i < MAXTHKVLU; i++) {
		fprintf(fp, "Search thickness (%d): min=%.3f max=%.3f  RI=%.4f\n", i + 1, Ta.ThkMin[i], Ta.ThkMax[i], Ta.RI[i]);
	}
	fprintf(fp, "Integration time: %d\n", Ta.IntegTime);
	fprintf(fp, "Boxcar: %d\n", Ta.Boxcar);
	fprintf(fp, "Method: %d\n", Ta.Method);

	fprintf(fp, "Peak specification\n");
	fprintf(fp, "Global\n");
	fprintf(fp, "Top Probe\n");
	PSG[0].VerboseHeaderOut(fp);
	PSG[0].VerboseOut(fp, 0);
	fprintf(fp, "Bottom Probe\n");
	PSG[1].VerboseHeaderOut(fp);
	PSG[1].VerboseOut(fp, 0);

	fprintf(fp, "Top probe per peak\n");
	PS1[0].VerboseHeaderOut(fp);
	for (i = 0; i < MAXPEAKCOUNT; i++) {
		PS1[i].VerboseOut(fp, i + 1);
	}
	fprintf(fp, "Bottom probe per peak\n");
	PS2[0].VerboseHeaderOut(fp);
	for (i = 0; i < MAXPEAKCOUNT; i++) {
		PS2[i].VerboseOut(fp, i + 1);
	}

	fprintf(fp, "Measurement setting\n");
	MPa[0].VerboseOutHeader(fp);
	for (i = 0; i < MaXPA; i++) {
		MPa[i].VerboseOut(fp, i + 1);
	}
}

void CMeParam::Clear() {
	int i;
	NAve = 8;
	for (i = 0; i < 2; i++) {
		NPeak[i] = 0;
		bEqual[i] = TRUE;
	}
	for (i = 0; i < MaXPA; i++) {
		MPa[i].Clear();
	}
	PSG[0].Clear();
	PSG[1].Clear();
	for (i = 0; i < MAXPEAKCOUNT; i++) {
		PS1[i].Clear();
		PS2[i].Clear();
	}
	Ta.Clear();
}

BOOL CMeParam::IsBowWarp() {
	for (int i = 0; i < MaXPA; i++) {
		if (MPa[i].IsEchoBowWarpMeasurement() || MPa[i].IsConfocalWarpMeasurement()) {	//11252022 ZHIMING
			return TRUE;
		}
	}
	return FALSE;
}