// JobInfo.h: interface for the CJobInfo class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

class CNCas;

class CJobInfo {
public:
	time_t time;
	short magic;
	short LPUnit;
	CString comment;
	CString CarrierID;
	CString batchname;	// use by non-online mode as LotID
	CString projectname;
	CString operatorname;
	CString cassettename;
	CString LotID;		// LotID is use by online mode as part of filename,
	//[[20251014_Mohir
	CString WaferSize; 
	CString Station;  
	CString WaferType; 
	//]]

public:
	void Conv(char** msg);
	long Binary(char** msg);
	long GetLength();
	void Clear();
	void Serialize(CArchive& ar);

	CJobInfo();
	virtual ~CJobInfo();
	CJobInfo& operator =(CJobInfo& dat);
};
