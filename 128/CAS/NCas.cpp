// NCas.cpp: implementation of the CNCas class.
#include "stdafx.h"
#include "NCas.h"
#include "GlobalDeviceContainer.h"

#include "..\..\E95\MPoint.h"
#include "..\..\E95\Data.h"
#include "..\..\E95\Pattern.h"
#include "ExportPDF.h" //20251210_Mohir
#include "413App.h" //20251210_Mohir
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;

#define new DEBUG_NEW
#endif

CNCas::CNCas() {}

CNCas::~CNCas() {}

void CNCas::SetJobInfo(CJobInfo& job) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	JobInfo = job;
}
//////////////////////////////////////////////////////////////////////

void CNCas::SetSysInfo(CSysInfo& inf) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	SysInfo = inf;
}

CRecipe413* CNCas::GetRecipe413() {
	return &Recipe413;
}

void CNCas::SetRecipe413(CRecipe413& rcp) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	Recipe413 = rcp;
}

CString CNCas::GetCasID() {
	return JobInfo.cassettename;
}

BOOL CNCas::SetCasID(CString id) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	JobInfo.cassettename = id;
	return TRUE;
}

CSysInfo* CNCas::GetSysInfo() {
	return &SysInfo;
}
//////////////////////////////////////////////////////////////////////

CWaferInfoMgr* CNCas::GetWaferInfoMgr() {
	return &winfo;
}

void CNCas::Serialize(CArchive& ar) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	if (ar.IsStoring()) {
		magic = 5;
		ar << magic;
		JobInfo.Serialize(ar);
		SysInfo.Serialize(ar);
		winfo.Serialize(ar);
		Recipe413.Serialize(ar);
	}
	else {
		ar >> magic;
		JobInfo.Serialize(ar);
		SysInfo.Serialize(ar);
		winfo.Serialize(ar);
		if (magic > 4) {
			Recipe413.Serialize(ar);
		}
	}
}

CNCas& CNCas::operator=(CNCas& co) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	JobInfo = co.JobInfo;
	Recipe413 = co.Recipe413;
	SysInfo = co.SysInfo;
	winfo = co.winfo;
	return *this;
}

void CNCas::Clear() {
	JobInfo.Clear();
	Recipe413.Clear();
	SysInfo.Clear();
	winfo.Clear();
}

void CNCas::Log(CString msg, short lvl) {
	pGDev->LogFile.Log(msg, lvl, "CNCas");
	if (lvl <= 1) {
		pGDev->LogFile.LogLocal(msg);
	}
}
//[20251011_Mohir....
BOOL CNCas::GeneratePDFFromCSV() {
	try {
		// Generate PDF file path (same location, different extension)
		CString pdfFilePath = PdfFileName;
		int dotPos = pdfFilePath.ReverseFind('.');
		if (dotPos != -1) {
			pdfFilePath = pdfFilePath.Left(dotPos) + _T(".pdf");
		}
		else {
			pdfFilePath += _T(".pdf");
		}
		p413App->PdfExporter.m_csvFilePath = PdfFileName;
		if (!JobInfo.cassettename)
			JobInfo.cassettename = _T("N/A");
		p413App->PdfExporter.CassatteIdText = _T("Cassette Name: ") + JobInfo.cassettename;
		p413App->PdfExporter.ExportAsPDF(pdfFilePath);
		return TRUE;
	}
	catch (...) {
		AfxMessageBox(_T("Failed to export PDF"));
		return FALSE;
	}
}
//]

// filename shall contain full path info
BOOL CNCas::SaveAsText(CString filename) {
	CFile fp;
	BOOL ret = FALSE;
	if (fp.Open(filename, CFile::modeCreate | CFile::modeWrite)) {
		ret = ExportData(fp, FALSE);
		fp.Close();
		if (ret) {
			PdfFileName = filename;//20251011_Mohir....
		}
	}
	return ret;
}

// filename shall contain full path info
BOOL CNCas::SaveAsTextWithBaseline(CString filename) {
	CFile fp;
	BOOL ret = FALSE;
	if (fp.Open(filename, CFile::modeCreate | CFile::modeWrite)) {
		ret = ExportData(fp, TRUE);
		fp.Close();
	}
	return ret;
}

BOOL CNCas::ExportData(CFile& fp, BOOL bIncBaseline) {
	//return Recipe413.ExportData(fp, bIncBaseline); //20251014_Mohir comment
	return Recipe413.ExportDataWithHeader(fp, bIncBaseline, Recipe413.GetRecipeName(), JobInfo.batchname, Recipe413.WaferID, JobInfo.Station, JobInfo.WaferType, JobInfo.WaferSize, JobInfo.operatorname); // 20251014_Mohir

}

//[[20251013_Mohir
BOOL CNCas::SaveMeasurementLog(CString fileName, short SlotID, CString LotID, CString opName, CString WaferID, CString WaferType, CString WaferSize, CString Station, CTime startTime, CTime endTime) {
	return Recipe413.ExportMeasSumLog(fileName, SlotID, LotID, JobInfo.operatorname/*opName*/, WaferID, WaferType, WaferSize, Station, startTime, endTime);
	
}
//]]