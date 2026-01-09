// Tool.h: interface for the CTool class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
#include "..\128\Cas\CasMgr.h"	// Added by ClassView
//#include "..\128\Cas\DataDesc.h"	// Added by ClassView
#include "..\128\GEM\PortParam.h"
//#include "..\IPC\SMOL.h"

class CRGADataMgr;
class CScanDataMgr;
class CWaferInfoMgr;

#define MAXOBJ	1
// This class contains most of the global variables used
class CTool {
	CCasMgr* pCasMgr;

	CCasMgr CasMgr1;
	CCasMgr CasMgr2;
	CCasMgr CasMgr3;
	CCasMgr CasMgr4;

public:

	//	CDataDesc desc;
	CString IDRCarrierID1;
	CString IDRCarrierID2;
	CString IDRCarrierID3;
	CString IDRCarrierID4;

	CarrierObjStatus CoBind;
	CarrierObjStatus CoPWCID;	// Proceed with carrier ID Yuen [7/6/2002/20:09]
	CarrierObjStatus CoPWCSM;	// Proceed with carrier SM Yuen [7/6/2002/20:09]
	CarrierObjStatus CoCancelBind;
	CarrierObjStatus CoCancelCarrier;
	CarrierAction CaStart;
	CarrierAction CaClose;
	CarrierAction CaAbort;	// Info send from ONLINE and will be returned to ONLINE after completion of action
	CPortParam MCArrive;

public:
	CCasMgr::STATE GetCasState(short unit);
	CNCas* GetCurCas();
	CCasMgr* GetCasMgr(short unit);
	CCasMgr* GetCurCasMgr();
	CScanDataMgr* GetCurScanDataMgr();
	CWaferInfoMgr* GetCurWaferInfoMgr();

	BOOL SetCasStateQueued(short unit);
	BOOL SetCasStateSelected(short unit);
	BOOL SetCasStateProcessing(short unit);
	BOOL SetCasStateCompleted(short unit);
	BOOL SetCasStateAborted(short unit);

	BOOL IsCasInSelectedState(short unit);
	BOOL IsAnyCasInProcessState(short unit);
	BOOL IsCassetteSelected(CString& id);

	void LoadParam();
	void SaveParam();

	CTool();
	virtual ~CTool();
};

extern CTool Tool;
