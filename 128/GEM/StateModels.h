// StateModels.h: interface for the CStateModels class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
#include "..\..\IPC\SMOL.h"

struct EqpStatus;

class CStateModels {
public:
	C900Obj C900;
	C128Obj C128;
	C413Obj C413;
	CRbtObj RbtObj;
	CScaleObj ScaleObj;

public:
	CSubstLocObj Algn;
	CSubstLocObj Scale;
	CSubstLocObj RbtArm;
	CSubstLocObj Stage1;	// first measurement stage, can be anyone, but need to be consistant [8/5/2010 XPMUser]
	CSubstLocObj Stage2;	// second measurement stagem can be anyone, but need to be consistant [8/5/2010 XPMUser]

	EqpStatus EqpState;

	CString GetScaleServStr();
	CString GetRbtServStr();
	CString Get128ServStr();
	CString Get900ServStr();
	CString Get413ServStr();
	void SetScaleServ(CScaleObj::SERVICESTATUS status);
	void SetRbtServ(CRbtObj::SERVICESTATUS status);
	// 	void Set128Serv(C128Obj::SERVICESTATUS status);
	// 	void Set900Serv(C900Obj::SERVICESTATUS status);
	void Set413Serv(C413Obj::SERVICESTATUS status);
	CScaleObj* GetScaleObj();
	CRbtObj* GetRbtObj();
	C128Obj* Get128Obj();
	C900Obj* Get900Obj();
	C413Obj* Get413Obj();
	EqpStatus* GetEqpStatus();
	CSubstLocObj* Get413SubstLocObj();
	CSubstLocObj* Get900SubstLocObj();
	CSubstLocObj* Get128SubstLocObj();
	CSubstLocObj* GetScaleSubstLocObj();
	CSubstLocObj* GetAlgnSubstLocObj();
	CSubstLocObj* GetRbtArmSubstLocObj();

	CStateModels();
	virtual ~CStateModels();
};
