// WaferInfo.h: interface for the CWaferInfo class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
class CRgaData;

class CWaferInfo/* : public CObject*/
{
public:
	void SetWaferID(CString ID);
	short magic;
	CString WaferID;	// wafer id
	short wafernum;		// wafer number	, aka slotnum
	float fthick;		// film thickness , Amstrong
	float wthick;		// wafer thickness , mm
	short ScanNo;		// Scan Number, used in online only [9/10/2007 LYF]
	CString RecipeName;	// Recipe name
	CString LotID;		// Lot ID
	BOOL bSelected;		// TRUE if selected for processing
	//[20251022_Mohir
	CString WaferType;
	CString WaferSize;
	CString Station;
	//]]
	void Conv(char** msg);
	long Binary(char** msg);
	long GetLength();
	void Set(CString SubstID, short wafernum, float fthick, float wthick, short ScanNo);

	CWaferInfo();
	virtual ~CWaferInfo();
	void Serialize(CArchive& ar);
	CWaferInfo& operator =(CWaferInfo& dat);
};
