// MPoint.h: interface for the CMPoint class.
class CData;
class CRdlPatt;

//
//////////////////////////////////////////////////////////////////////
#pragma once
#include "Coor.h"// Added by ClassView
#include "DataList.h"// Added by ClassView

class CMPoint {
protected:
	CDataList Dat;
	short progressNotUsedmy;
	int m_nNumSamplesdmy;

public:
	short magic;
	short MeSet;
	short PatSet;

	enum PROGSM {
		NOTMEASURE, MEASURING, COMPLETED, FAILED
	};
	PROGSM ProgressSM;

	short PaN;

	float baseline;

	CCoor Co;
	float m_fOffsetXdmy, m_fOffsetYdmy;
	float zN; // Temporary variable, not save to file [7/19/2013 Yuen]

	float m_fvalOffset;	// value offset [10/6/2020]

	enum ENGOF {
		NOTKNOWN, GOOD, UNDER, OVER, BAD
	};
	ENGOF GOFdmy;

public:

	void Conv(char** msg);
	long Binary(char** msg);
	long GetLength();

	CString GetValueOffset();

	BOOL GetSFPD(float& fSFPD);
	void SetData(CData* dat);
	void AddData(CData* dat);
	void AddHead(CData* dat);
	void ClearData();

	CData* GetData(short index, BOOL bCreate);	// bCreate if data is not available [5/14/2013 user]
	void GetCoor(POINT& pt);
	void GetCoor(float& x, float& y);
	CCoor* GetCoor();
	void SetCoorXYZ(short n, float x, float y, float z);
	void SetCoor(CCoor* p);
	CMPoint& operator = (CMPoint& co);
	void Serialize(CArchive& ar);

	CMPoint();
	virtual ~CMPoint();

public:
	void SetMarker(short n);
	void ClearMarker(short n);
	void Dump(FILE* fp);
	CString GetYCoorStr();
	CString GetXCoorStr();
	CString GetPatSetStr();
	CString GetMeSetStr();
	CString GetPropStr();
};
