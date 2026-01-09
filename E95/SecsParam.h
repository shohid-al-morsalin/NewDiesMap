// SecsParam.h: interface for the CSecsParam class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
#include "SecsParamItem.h"

class CSecsParam {
public:
	void UpdateHSMSParam();
	void SaveParam();
	void Load();
	enum spPARAM {
		spNONE, spT3TIMER, spT5TIMER, spT6TIMER, spT7TIMER, spT8TIMER,
		spSYSTEMBYTE, spSYSTEMBYTEINC,
		spDEVICEID, spENTITY, spPASSIVEPORT, spACTIVEIP, spACTIVEPORT,
		spDEFAULTONLINESTATE, spDEFAULTOFFLINESTATE, spLASTID
	};

	CSecsParamItem Param[spLASTID];

	CSecsParam();
	virtual ~CSecsParam();
};