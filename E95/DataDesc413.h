#pragma once
// Total 16 x 9 + 3 columns [7/3/2012 user]
#define LASTXCOL	147

class CRecipe;

class CDataDesc413 : public CObject {
private:

	/**
	 * Version description: Should be used while serializing and deserilization
	 */
	short magic;
public:
	long Binary(char** msg);
	void Conv(char** msg);
	long GetLength();

	void Clear();
	// 	int GetDispIndex(int nMeasurementMethod);
	void SaveTM(CArchive& ar);
	void SaveM(CArchive& ar);
	void LoadTM(CArchive& ar);
	void LoadM(CArchive& ar);
	void InitTM();
	void InitM();

	/**
	 * The conversion WRT MKS system
	 */
	short GetVisCount();
	// 	short N;

public:
	void VerboseOut(FILE* fp);
	short Model;

	//    /**
	//      * To be used by the display unit to decide whether this point to be displayed or not
	//      */
	// 	// Note: The sequence should not be change or old recipe will show funny grid result [7/16/2009 Valued Customer]
	// 	enum VISCOLN
	// 	{
	// 		NOCOL,			// number
	// 		POSCOL,			// position
	// 		SITHKCOL,		// Si thickness or local warp
	// 		WARPCOL,		// Warpage or Top
	// 		TAPE1COL,		// Tape 1 or radius
	// 		TAPE2COL,		// Tape 2 or stress
	// 		TAPEBOTCOL,		// Bottom Tape
	// 		POLYCOL,		// Poly
	// 		PATTCOL,		// Pattern
	// 		TOTALCOL,		// Total
	// 		ROUGHCOL,		// Roughness Rave
	// 		TRENCHCOL,		// Trench
	// 		PEAK11COL,		// Peak11
	// 		PEAK12COL,		// Peak12
	// 		PEAK13COL,		// Peak13
	// 		PEAK14COL,		// Peak14
	// 		PEAK15COL,		// Peak15
	// 		PEAK21COL,		// Peak21
	// 		PEAK22COL,		// Peak22
	// 		PEAK23COL,		// Peak23
	// 		PEAK24COL,		// Peak24
	// 		PEAK25COL,		// Peak25
	// 		FILMPEAK1COL,	// Film peak 1
	// 		FILMPEAK2COL,	// Film peak 2
	// 		CURRENTCOL,		// Current
	// 		CHARGECOL,		// Charge
	// 		TIMECOL,		// Time
	// 		VOLTAGECOL,		// Voltage
	// 		TEMPERCOL,		// Temperature
	// 		LEAKAGECOL,		// Leakage
	// 		FBBENDCOL,		// Flat band bending
	// 		RECOMBCOL,		// Recombination
	// 		ROUGHMAXCOL,	// Rmax
	// 		LASTXCOL
	// 	};
	int Col[LASTXCOL];
	BOOL bVis[LASTXCOL];
	float unit[LASTXCOL];
	CString ID[LASTXCOL];

	short MapCol;  // Map column [3/13/2010 Valued Customer]

	CDataDesc413& operator=(CDataDesc413& co);

	void Serialize(CArchive& ar);

	CDataDesc413(/*short mdl*/);
	virtual ~CDataDesc413();

	//	BOOL ChangeVisualData(CRecipe *rp);
};
