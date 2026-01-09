#pragma once

#include "NCas.h"
#include "SYS/Var.h"

class CNCas;

class CCasMgr {
	CCriticalSection CS;
	short magic;

public:
	void SplitRecipeCarrierID(CString& filename, CString& recipe, CString& carrierID);
	void SetCassette(CNCas* pCas);
	//BOOL MakeCassetteName(CString& cassettfile, CString recipename, CString lotname, CString WID, BOOL bCurDir, BOOL bIncTime);
	//BOOL MakeCassetteName(CString& cassettfile, CString recipename, CString lotname, CString WID, CString cid, CString oprtr, BOOL bCurDir, BOOL bIncTime); // 20251013_Mohir
	BOOL MakeCassetteNameOld(CString& cassettfile, CString WID);
	enum STATE {
		NOTSELECTED, QUEUED, SELECTED, PROCESSING, COMPLETED, ABORTED
	};
	short port;
	short ScanNo;
	CNCas* pCas;
	HWND hWnd;

	STATE State;
	ScanParam::SCANMODE Mode;

	BOOL bDeleteCasPointer;
	BOOL AutoSave(CString& filename, short SlotID, CString WaferID, CString LotID, BOOL bCompleted, BOOL bIncTime, CString WaferType, CString WaferSize, CString Station, CTime startTime, CTime endTime); //20251030_Mohir
 	BOOL OpenCassette(CString filename);
	BOOL _OpenCassette(CString& filename);

	BOOL CloseCassette(CString filename);

	void Recalculate();

	BOOL SaveAsTextFile(CString filename, BOOL bDirectSave, BOOL bIncBaseline);
	void Log(CString msg, short lvl = 5);
	CNCas* GetCurCas();

	void LoadCassette(CArchive& ar);
	void SaveCassette(CArchive& ar);
	BOOL CreateNewCassette(CString filename);
	BOOL MakeCassetteName(CString& cassettfile, CString recipename, CString lotname, CString WID, BOOL bCurDir, BOOL bIncTime, CString OpName = "FSM"); // 20251013_Mohir
	BOOL SaveMeasurementLog(CString FileName, CString rcpName, short SlotID, CString LotID, CString opName, CString WaferID, CString WaferType, CString WaferSize, CString Station, CTime startTime, CTime endTime); //20251030_Mohir
	CCasMgr();
	virtual ~CCasMgr();
	void Serialize(CArchive& ar);

};
