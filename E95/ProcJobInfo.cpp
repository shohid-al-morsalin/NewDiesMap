#include "StdAfx.h"
#include "ProcJobInfo.h"
CProcJobInfo::CProcJobInfo(void) {
	Clear();
}

CProcJobInfo::~CProcJobInfo(void) {
	//	Clear();
}

void CProcJobInfo::Clear() {
	//	ScanNum = 0;
	//	CarrierID = _T("");
	//	RecipeName = _T("");
	Cas.Clear();
	CJobName = "";
	//	bRemoteOverride = FALSE;
	//	memset(SlotMap,0,sizeof(SlotMap));
}