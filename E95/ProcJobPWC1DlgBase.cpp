// ProcJobPWC1DlgBase.cpp: implementation of the CProcJobPWC1DlgBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "e95.h"
#include "ProcJobPWC1DlgBase.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProcJobPWC1DlgBase::CProcJobPWC1DlgBase() {
	memset(SlotMap, 0, sizeof(SlotMap));
}

CProcJobPWC1DlgBase::CProcJobPWC1DlgBase(UINT nIDTemplate, CWnd* pParentWnd)
	: CResizableDialog(nIDTemplate, pParentWnd) {
	memset(SlotMap, 0, sizeof(SlotMap));
}

CProcJobPWC1DlgBase::~CProcJobPWC1DlgBase() {}