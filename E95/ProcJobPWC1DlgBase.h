// ProcJobPWC1DlgBase.h: interface for the CProcJobPWC1DlgBase class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "ProcJobCommon.h"
#include "..\SRC\ResizableDialog.h"

class CProcJobPWC1DlgBase : public CResizableDialog,
	public CProcJobCommon {
public:
	char SlotMap[32];

public:
	CProcJobPWC1DlgBase();
	CProcJobPWC1DlgBase(UINT nIDTemplate, CWnd* pParentWnd = NULL);
	virtual ~CProcJobPWC1DlgBase();
};