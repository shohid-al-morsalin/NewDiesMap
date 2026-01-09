// PrJobList.cpp: implementation of the CPrJobList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PrJobList.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CPrJobList PrJobs;

CPrJobList::CPrJobList() {}

CPrJobList::~CPrJobList() {
	Clear();
}

void CPrJobList::Clear() {
	POSITION pos;
	pos = JobList.GetHeadPosition();
	while (pos) {
		SPcJobExec* j = (SPcJobExec*)JobList.GetNext(pos);
		if (j) delete j;
	}
	JobList.RemoveAll();
}

void CPrJobList::Add(SPcJobExec* Job) {
	JobList.AddTail(Job);
}

BOOL CPrJobList::Remove(SPcJobExec& Pj) {
	POSITION pos, opos;
	pos = JobList.GetHeadPosition();
	while (pos) {
		opos = pos;
		SPcJobExec* j = (SPcJobExec*)JobList.GetNext(pos);
		if (j) {
			if (strcmp(j->PrJID, Pj.PrJID) == 0) {
				//if (strcmp(j->CJobID,Pj.CJobID)==0) {
				JobList.RemoveAt(opos);
				delete j;
				return TRUE;
				//}
			}
		}
	}
	return FALSE;
}

SPcJobExec* CPrJobList::Get(CString& CarrierID) {
	POSITION pos, opos;
	pos = JobList.GetHeadPosition();
	while (pos) {
		opos = pos;
		SPcJobExec* j = (SPcJobExec*)JobList.GetNext(pos);
		if (j) {
			if (strcmp(j->CarrierID, CarrierID) == 0) {
				JobList.RemoveAt(opos);
				return j;
			}
		}
	}
	return NULL;
}

SPcJobExec* CPrJobList::Get(short PortNo) {
	POSITION pos, opos;
	pos = JobList.GetHeadPosition();
	while (pos) {
		opos = pos;
		SPcJobExec* j = (SPcJobExec*)JobList.GetNext(pos);
		if (j) {
			if (j->PortNo == PortNo) {
				JobList.RemoveAt(opos);
				return j;
			}
		}
	}
	return NULL;
}

SPcJobExec* CPrJobList::GetPRJob(CString CarrierID) {
	POSITION pos, opos;
	pos = JobList.GetHeadPosition();
	while (pos) {
		opos = pos;
		SPcJobExec* j = (SPcJobExec*)JobList.GetNext(pos);
		if (j) {
			if (CarrierID == j->CarrierID) {
				return j;
			}
		}
	}
	return NULL;
}

SPcJobExec* CPrJobList::GetByPJ(CString& PjID) {
	POSITION pos, opos;
	pos = JobList.GetHeadPosition();
	while (pos) {
		opos = pos;
		SPcJobExec* j = (SPcJobExec*)JobList.GetNext(pos);
		if (j) {
			if (strcmp(j->PrJID, PjID) == 0) {
				JobList.RemoveAt(opos);
				return j;
			}
		}
	}
	return NULL;
}