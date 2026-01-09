#include "stdafx.h"

#include "CasMgr.h"
#include "SRC/DOSUtil.h"
#include "GlobalDeviceContainer.h"
#include "ExportPDF.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
//class CNCas;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CCasMgr::CCasMgr() {
	pCas = NULL;
	ScanNo = 0;
	Mode = ScanParam::RTMODE;
	hWnd = NULL;
	port = 0;
	bDeleteCasPointer = TRUE;
	State = NOTSELECTED;
	magic = 1;
}

CCasMgr::~CCasMgr() {
	if (pCas && bDeleteCasPointer) {
		delete pCas;
		pCas = NULL;
	}
}

void CCasMgr::LoadCassette(CArchive& ar) {
	Log("Load Cassette ...", 5);
	CSingleLock Lock(&CS);
	Lock.Lock();
	if (bDeleteCasPointer) {
		if (pCas) {
			delete pCas;
			pCas = NULL;
		}
		pCas = new CNCas;
	}
	if (pCas) {
		SetCursor(LoadCursor(0, IDC_WAIT));
		short cnt;
		ar >> magic;
		ar >> cnt;
		pCas->Serialize(ar);
		SetCursor(LoadCursor(0, IDC_ARROW));
	}
}

void CCasMgr::SaveCassette(CArchive& ar) {
	Log("Saving Cassette ...", 5);
	CSingleLock Lock(&CS);
	Lock.Lock();
	if (pCas) {
		SetCursor(LoadCursor(0, IDC_WAIT));
		short cnt = 1;
		magic = 1;
		ar << magic;
		ar << cnt;
		pCas->Serialize(ar);
		SetCursor(LoadCursor(0, IDC_ARROW));
		Log("Cassette saved", 5);
		return;
	}
	Log("Cassette not saved", 5);
}

void CCasMgr::Serialize(CArchive& ar) {
	if (ar.IsStoring()) {
		SaveCassette(ar);
	}
	else {
		LoadCassette(ar);
	}
}

BOOL CCasMgr::CreateNewCassette(CString filename) {
	if (!bDeleteCasPointer) {
		ASSERT(0); // Cannot create new because pCas is a reference
	}
	if (pCas) {
		delete pCas;
		pCas = NULL;
	}
	pCas = new CNCas;
	if (pCas) {
		bDeleteCasPointer = TRUE;
		pCas->JobInfo.time = CTime::GetCurrentTime().GetTime();
		pCas->SysInfo = SysInf;
		pCas->SetCasID(filename);
		return TRUE;
	}
	return FALSE;
}
//[[20251013_Mohir
BOOL CCasMgr::AutoSave(CString& filename, short SlotID, CString WaferID, CString LotID, BOOL bCompleted, BOOL bIncTime, CString WaferType, CString WaferSize, CString Station, CTime startTime, CTime endTime) {
	CString str, str2;
	if (bCompleted) {
		if (WaferID.GetLength() > 0) {
			str.Format("%s-%d", WaferID, SlotID);
		}
		else {
			str.Format("S%d", SlotID);
		}
	}
	else {
		if (WaferID.GetLength() > 0) {
			str.Format("%s-%d-Aborted", WaferID, SlotID);
		}
		else {
			str.Format("S%d-Aborted", SlotID);
		}
	}
	if (LotID.GetLength() > 0) {
		str2 = LotID;
	}
	else {
		str2 = pCas->JobInfo.CarrierID;
	}
	CString OpName = "FSM"; 
	if (strlen(pGDev->username) != 0) OpName = pGDev->username;  
	if (!MakeCassetteName(filename, pCas->Recipe413.RcpeName, str, str2, FALSE, bIncTime, OpName)) {
		ASSERT(0);
		return FALSE;
	}

	if (!CloseCassette(filename)) {
		return FALSE;
	}
	CString msg;
	msg.Format("Data file saved: %s", filename);
	Log(msg, 2);
	SaveMeasurementLog(filename, pCas->Recipe413.RcpeName, SlotID, LotID, OpName, WaferID, WaferType, WaferSize, Station, startTime, endTime); //20251030_Mohir
	// file name should not contain path info
	SaveAsTextFile(filename, FALSE, FALSE);
	msg.Format("Text file saved: %s", filename);
	Log(msg, 2);

 
	return TRUE;
} 
//]]
//[[20521013_Mohir
BOOL CCasMgr::SaveMeasurementLog(CString FileName, CString rcpName, short SlotID, CString LotID, CString opName, CString WaferID, CString WaferType, CString WaferSize, CString Station, CTime startTime, CTime endTime) 
{


	CNCas* cas = GetCurCas();
	if (!cas) {
		return FALSE;
	}

	CString TxtFile;
	TxtFile = FileName.Left(FileName.Find(rcpName));
	TxtFile += (rcpName + CTime::GetCurrentTime().Format("\\%Y"));
	return cas->SaveMeasurementLog(TxtFile, SlotID, LotID, opName, WaferID, WaferType, WaferSize, Station, startTime, endTime);
 


}

//]]


// Load cassette
// Will search through all specified drive to locate file. [12/15/2007 YUEN]
// filename should be fully qualified contains path info and recipe~~carrierID [12/18/2007 Yuen]
BOOL CCasMgr::OpenCassette(CString filename) {
	if (_OpenCassette(filename)) {
		return TRUE;
	}
	return FALSE;
}

BOOL CCasMgr::_OpenCassette(CString& filename) {
	CFile theFile;
	if (theFile.Open(filename, CFile::modeRead)) {
		CArchive archive(&theFile, CArchive::load);
		Serialize(archive);
		archive.Close();
		theFile.Close();
		return TRUE;
	}
	Log("Cannot open file", 2);
	return FALSE;
}

// Save cassette
// OK [12/18/2007 Yuen]
BOOL CCasMgr::CloseCassette(CString filename) {
	CString path = DosUtil.DirSys.ExtractFilePath(filename);
	DosUtil.DirSys.GotoDirec(path);
	CFile theFile;
	if (theFile.Open(filename, CFile::modeCreate | CFile::modeWrite)) {
		CArchive archive(&theFile, CArchive::store);
		Serialize(archive);
		archive.Close();
		theFile.Close();
		return TRUE;
	}
	return FALSE;
}

void CCasMgr::Recalculate() {
	CNCas* pCas = GetCurCas();
	if (!pCas) {
		ASSERT(0);
		return;
	}
}

// Should not be called from E95 [12/15/2007 YUEN]
// filename should not contains path information
BOOL CCasMgr::SaveAsTextFile(CString filename, /*CVar::DISKSELK mode,*/ BOOL bDirectSave, BOOL bIncBaseline) {
	CNCas* cas = GetCurCas();
	if (!cas) {
		return FALSE;
	}

	CString TxtFile;
	if (bDirectSave) {
		TxtFile = filename.Left(filename.Find('.'));
		TxtFile += ".CSV";
		if (!bIncBaseline) {
			return cas->SaveAsText(TxtFile);
		}
		else {
			return cas->SaveAsTextWithBaseline(TxtFile);
		}
	}

	TxtFile = filename.Left(filename.ReverseFind('.'));
	TxtFile += ".CSV";
	// Create the directory i00000000f not exit
	CString path = DosUtil.DirSys.ExtractFilePath(TxtFile/*filename*/);

	filename = path.Left(path.Find("\\C2C"));
	path = filename + "\\TXT";
	DosUtil.DirSys.GotoDirec(path);
	DosUtil.DirSys.ExtractFileName(TxtFile);
	path = path + "\\" + TxtFile;

	if (!bIncBaseline) {
		return cas->SaveAsText(path);
	}
	else {
		return cas->SaveAsTextWithBaseline(path);
	}
	return FALSE;  // prevent compiler from complaining [10/4/2012 Yuen]
}

void CCasMgr::Log(CString msg, short lvl) {
	pGDev->LogFile.Log(msg, lvl, "CasMgr");
	if (lvl <= 1) {
		pGDev->LogFile.LogLocal(msg);
	}
}

CNCas* CCasMgr::GetCurCas() {
	return pCas;
}

//[[777_Mohir comment this for 20251013_Mohir
// Mode: 0 =  local, 128 for 128 module, 900 for 900 module [12/15/2007 YUEN]
// Naming format: [RcpName]~[LotID]~[[WaferID]/[CarrierID]].C2C
//BOOL CCasMgr::MakeCassetteName(CString& cassettefile, CString recipename, CString lotname, CString WID, BOOL bCurDir, BOOL bIncTime) {
//	if (bIncTime) {
//		CTime t = CTime::GetCurrentTime();
//		CString tt = t.Format("-%d%H%M%S");
//		lotname += tt;
//	}
//
//	CString ID;
//	if (Var.bSortByRecipe) {
//		ID = recipename;
//		ID += "~";
//		ID += lotname;	// LYF LotID will be use in ONLINE only [11/2/2005]
//		ID += "~";
//		ID += WID;
//	}
//	else {
//		ID = WID;
//		ID += "~";
//		ID += lotname;	// LYF LotID will be use in ONLINE only [11/2/2005]
//		ID += "~";
//		ID += recipename;
//	}
//
//	cassettefile = ID + ".C2C";
//	if (DosUtil.DirSys.GetC2CFileName(cassettefile, bCurDir)) {
//		CString path = DosUtil.DirSys.ExtractFilePath(cassettefile);
//		DosUtil.DirSys.GotoDirec(path);
//		return TRUE;
//	}
//	else {
//		CString path = DosUtil.DirSys.ExtractFilePath(cassettefile);
//		DosUtil.DirSys.GotoDirec(path);
//		return TRUE;
//	}
//	return FALSE;
//}
//[[20251013_Mohir
BOOL CCasMgr::MakeCassetteName(CString& cassettefile, CString recipename, CString lotname, CString WID, BOOL bCurDir, BOOL bIncTime, CString OpName) {
	if (bIncTime) {
		//[[20250408/Sihab
		lotname += "-" + pCas->JobInfo.batchname;
		//]]
		CTime t = CTime::GetCurrentTime();
		CString tt = t.Format("-%d%H%M%S");
		lotname += tt;
	}

	CString ID;
	if (Var.bSortByRecipe) {
		ID = recipename;
		ID += "~";
		ID += lotname;	// LYF LotID will be use in ONLINE only [11/2/2005]
		ID += "~";
		ID += WID;
	}
	else {
		ID = WID;
		ID += "~";
		ID += lotname;	// LYF LotID will be use in ONLINE only [11/2/2005]
		ID += "~";
		ID += recipename;
	}

	cassettefile = ID + ".C2C";
	if (DosUtil.DirSys.GetC2CFileName(cassettefile, bCurDir, recipename, OpName)) {
		CString path = DosUtil.DirSys.ExtractFilePath(cassettefile);
		DosUtil.DirSys.GotoDirec(path);
		return TRUE;
	}
	else {
		CString path = DosUtil.DirSys.ExtractFilePath(cassettefile);
		DosUtil.DirSys.GotoDirec(path);
		return TRUE;
	}
	return FALSE;
}

//]]



// 121823 MASUM - As per kevin filename request Data file didn't dipaly input infromation include Carrier ID and Opeartor

//[[20251013_Mohir comment this

//BOOL CCasMgr::MakeCassetteName(CString& cassettfile, CString recipename, CString lotname, CString WID, CString cid, CString oprtr, BOOL bCurDir, BOOL bIncTime)
//{
//	if (bIncTime) {
//		CTime t = CTime::GetCurrentTime();
//		CString tt = t.Format("-%d%H%M%S");
//		lotname += tt;
//	}
//
//	CString ID;
//	if (Var.bSortByRecipe) {
//		ID = recipename;
//		ID += "~";
//		ID += lotname;	// LYF LotID will be use in ONLINE only [11/2/2005]
//		ID += "~";
//		ID += WID;
//	}
//	else {
//		ID = WID;
//		ID += "~";
//		ID += lotname;	// LYF LotID will be use in ONLINE only [11/2/2005]
//		ID += "~";
//		ID += cid;
//		ID += "~";
//		ID += oprtr;
//		ID += "~";
//		ID += recipename;
//	}
//
//	cassettfile = ID + ".C2C";
//	if (DosUtil.DirSys.GetC2CFileName(cassettfile, bCurDir)) {
//		CString path = DosUtil.DirSys.ExtractFilePath(cassettfile);
//		DosUtil.DirSys.GotoDirec(path);
//		return TRUE;
//	}
//	else {
//		CString path = DosUtil.DirSys.ExtractFilePath(cassettfile);
//		DosUtil.DirSys.GotoDirec(path);
//		return TRUE;
//	}
//	return FALSE;
//}
//]]

BOOL CCasMgr::MakeCassetteNameOld(CString& cassettefile, CString WID) {
	cassettefile = WID + ".C2C";
	if (!DosUtil.DirSys.GetC2CFileName(cassettefile, FALSE)) {
		Log("ABORT: Fail to create cassette file directory", 1);
		ASSERT(0);
		return FALSE;
	}
	return TRUE;
}

void CCasMgr::SetCassette(CNCas* Cas) {
	if (pCas) {
		delete pCas;
	}
	pCas = Cas;
	bDeleteCasPointer = FALSE;
}

void CCasMgr::SplitRecipeCarrierID(CString& filename, CString& recipe, CString& carrierID) {
	int idx = filename.Find("~", 0);
	if (idx > 1) {
		recipe = filename.Left(idx);
	}
	idx = filename.ReverseFind('~');
	if (idx > 1) {
		carrierID = filename.Mid(idx + 1);
	}
	idx = carrierID.Find('.');
	if (idx > 1) {
	}
	carrierID = carrierID.Left(idx);
}