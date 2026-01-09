#pragma once

#include "SYS/Var.h"

class CDirSys {
	CString rDir;
public:
	void ExtractFileName(CString& name);
	//BOOL MakeCurTxtPath(CString& curPath); //20251013_Mohir
    //BOOL IsFileEntryExist(CString& filename); //20251013_Mohir comment
	//BOOL GetC2CFileName(CString& filename, BOOL bCurDir); //20251013_Mohir comment
	BOOL GetTxTFileName(CString& filename);
	CString ExtractFilePath(CString& fullname);
	//BOOL MakeCurC2CPath(CString& curPath);
	CString GetRootDir();
	BOOL LocateFile(CString& filename);
	void ChgExt(CString& filename, CString ext);
	void SetRootDir(CString str);
	BOOL GotoDirec(CString fullpath);
	BOOL GetC2CFileName(CString& filename, BOOL bCurDir, CString rcpName = "DEFRCP", CString OpName = "FSM"); // 20251013_Mohir
	BOOL MakeCurC2CPath(CString& curPath, CString rcpName = "DEFRCP", CString OpName = "FSM"); // 20251013_Mohir
	BOOL IsFileEntryExist(CString& filename, CString rcpName = "DEFRCP", CString OpName = "FSM"); // 20251013_Mohir
	BOOL MakeCurTxtPath(CString& curPath, CString rcpName = "DEFRCP"); // 20251310_Mohir

	CDirSys();
	virtual ~CDirSys();
};
