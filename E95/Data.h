#pragma once

#include "MParam.h"

// 9 * 8 points [11/8/2011 Administrator]
// Changing this value will void CRecipe413 integrity [3/24/2012 Administrator]
// #define MAXDATAVALUE	72
// #define MaXDATAVALUE	144

#define SYSTEM_TM413	20

#define BADDATA			0
#define BADDATANEG		-9999990

/** Contains a set of result as a result of one measurement process on one point. */
//class CDataDesc;

class CData {
public:
	short model;
	float DVal[MAXMEPASET * MaXPA];	// Data value [11/8/2011 Administrator]

private:
	short magic;

public:
	void Set(short idx, float val);
	void Conv(char** msg);
	long Binary(char** msg);
	long GetLength();

	//     void FakeData();
	BOOL Format(CString& str, short index);

	float Get(short idx);
	void Clear() {
		for (int i = 0; i < MAXMEPASET * MaXPA; i++) {
			DVal[i] = 0;
		}
	}
	void Serialize(CArchive& ar);
	CData& operator = (CData& co);

	CData();
	virtual ~CData();
};
