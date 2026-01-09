#pragma once

#include <afxmt.h>

#include "SysInfo.h"		/* Added by ClassView */
#include "JobInfo.h"		/* Added by ClassView */
// #include "ScanDataMgr.h"	/* Added by ClassView */
#include "WaferInfoMgr.h"	/* Added by ClassView */
// #include "RGADataMgr.h"
// #include "PrcsInfoMgr.h"
// #include "CasLogMgr.h"
//
// #include "CasLog.h"
#include <atlimage.h> //20251210_Mohir
#include "..\..\E95\Recipe413.h"

class CNCas {
public:
	// 	CString GetReportPointStrX(int cnt, CRecipe413 *pRcp, CMPoint *p);
	BOOL ExportData(CFile& fp, BOOL bIncBaseline);
	void SetJobInfo(CJobInfo& job);
	void SetSysInfo(CSysInfo& inf);

	// 	CRecipe * GetRecipe128(void);
	// 	void SetRecipe128(CRecipe & rcp);

	CRecipe413* GetRecipe413(void);
	void SetRecipe413(CRecipe413& rcp);

	CString GetCasID(void);
	BOOL SetCasID(CString id);

	CSysInfo* GetSysInfo(void);
	// 	CCasLogMgr * GetCasLogMgr();
	// 	CRGADataMgr * GetRGADataMgr();
	// 	CScanDataMgr * GetScanDataMgr(void);
	CWaferInfoMgr* GetWaferInfoMgr(void);

	void Serialize(CArchive& ar);

	CNCas& operator = (CNCas& dat);

	// 	long GetLength();
	// 	BOOL Conv(char *msg);
	// 	long Binary(char *msg);

	void Clear();
	void Log(CString msg, short lvl = 5);
public:
	// 	void AddBeampath(CScanData *pline);
	BOOL SaveAsText(CString filename);
	BOOL SaveAsTextWithBaseline(CString filename);


public:
	BOOL GeneratePDFFromCSV();//20251210_Mohir
	CString PdfFileName; //20251011_Mohir
	CCriticalSection CS;
	short magic;
	CRecipe413 Recipe413;
	CJobInfo JobInfo;
	CSysInfo SysInfo;
	CWaferInfoMgr winfo;
	// 	CRGADataMgr rga;
	// 	CScanDataMgr scan;
	// 	CCasLogMgr CasLog;
	BOOL SaveMeasurementLog(CString fileName, short SlotID, CString LotID, CString opName, CString WaferID, CString WaferType, CString WaferSize, CString Station, CTime startTime, CTime endTime); // 20251013_Mohir
	CNCas(void);
	virtual ~CNCas(void);
};
