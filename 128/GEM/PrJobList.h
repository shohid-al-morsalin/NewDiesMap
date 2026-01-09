// PrJobList.h: interface for the CPrJobList class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
#include "..\..\IPC\Smol.h"	// Added by ClassView

class CPrJobList {
	CPtrList JobList;

public:
	SPcJobExec* GetByPJ(CString& PjID);
	SPcJobExec* GetPRJob(CString CarrierID);
	void Clear();
	void Add(SPcJobExec* Job);
	BOOL Remove(SPcJobExec& Pj);
	SPcJobExec* Get(short PortNo);
	SPcJobExec* Get(CString& CarrierID);

	CPrJobList();
	virtual ~CPrJobList();
};

extern CPrJobList PrJobs;
