// RecipeNSMgr.cpp: implementation of the CRecipe413NSMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Recipe413NSMgr.h"
#include "Recipe413.h"

#include "..\SRC\DOSUtil.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRecipe413NSMgr RcpMgr;

CRecipe413NSMgr::CRecipe413NSMgr() {
	CreateRecipeDirectory();
}

CRecipe413NSMgr::~CRecipe413NSMgr() {}

void CRecipe413NSMgr::CreateRecipeDirectory() {
	CString dir;
	DosUtil.GetProgramDir(dir);
	dir += "Recipe\\";
	int nTemp = ::GetFileAttributes(dir);
	if (!(FILE_ATTRIBUTE_DIRECTORY & nTemp) || (nTemp == -1)) {
		if (!::CreateDirectory(dir, NULL)) {
			return;
		}
	}
}

BOOL CRecipe413NSMgr::ListRecipes(vector <string>& RList) {
	WIN32_FIND_DATA FindFileData;

	CString dir;
	DosUtil.GetProgramDir(dir);
	dir += "Recipe\\*.RCP";

	HANDLE hFind;
	hFind = ::FindFirstFile(dir, &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE) {
		return FALSE;
	}
	string name;
	name = FindFileData.cFileName;
	RList.push_back(name.substr(0, name.size()/* - 4*/));
	while (FindNextFile(hFind, &FindFileData)) {
		name = FindFileData.cFileName;
		RList.push_back(name.substr(0, name.size()/* - 4*/));
	}
	::FindClose(hFind);
	return TRUE;
}

// Input: Rcp must be properly filled [10/31/2002]
BOOL CRecipe413NSMgr::Save(CRecipe413* Rcp) {
	if (!Rcp) {
		ASSERT(0);
		return FALSE;
	}
	CString dir;
	DosUtil.GetProgramDir(dir);
	dir += "Recipe\\";
	dir += Rcp->GetRecipeName();
	dir += ".RCP";
	CFile theFile;
	if (theFile.Open(dir, CFile::modeWrite | CFile::modeCreate)) {
		CArchive archive(&theFile, CArchive::store);
		Rcp->Serialize(archive);
		archive.Close();
		theFile.Close();
		return TRUE;
	}
	return FALSE;
}

// Input: Rcp.ObjID - Recipe name [10/31/2002] [10/31/2002]
BOOL CRecipe413NSMgr::Load(CRecipe413* Rcp) {
	if (!Rcp) {
		ASSERT(0);
		return FALSE;
	}
	CString dir;
	DosUtil.GetProgramDir(dir);
	dir += "Recipe\\";
	dir += Rcp->GetRecipeName();
	dir += ".RCP";
	CFile theFile;
	if (theFile.Open(dir, CFile::modeRead)) {
		CArchive archive(&theFile, CArchive::load);
		Rcp->Serialize(archive);
		archive.Close();
		theFile.Close();
		return TRUE;
	}
	return FALSE;
}

BOOL CRecipe413NSMgr::CanDeleteRecipes(vector <string>& RList) {
	if (RList.empty()) {
		return TRUE;
	}
	CString dir;
	DosUtil.GetProgramDir(dir);
	dir += "Recipe\\";
	string FileName;
	int nTemp;
	for (int nIndex = 0; nIndex < int(RList.size()); nIndex++) {
		FileName = dir + RList[nIndex].c_str();
		FileName += ".RCP";
		nTemp = ::GetFileAttributes(FileName.c_str());
		// Does this PPID really exists ?
		if (nTemp == -1) {
			return FALSE;
		}
		if (FILE_ATTRIBUTE_DIRECTORY & nTemp) {
			return FALSE;
		}
		if (!(FILE_ATTRIBUTE_ARCHIVE & nTemp)) {
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CRecipe413NSMgr::DeleteRecipes(vector <string>& RList) {
	if (RList.empty()) {
		return TRUE;
	}
	if (CanDeleteRecipes(RList)) {
		for (int i = 0; i<int(RList.size()); i++) {
			if (!DeleteRecipe(RList[i])) {
				return FALSE;
			}
		}
	}
	return TRUE;
}

BOOL CRecipe413NSMgr::DeleteRecipe(string& name) {
	CString dir;
	DosUtil.GetProgramDir(dir);
	dir += "Recipe\\";
	dir += name.c_str();
	dir += ".RCP";
	if (DeleteFile(dir)) {
		return TRUE;
	}
	return FALSE;
}

BOOL CRecipe413NSMgr::DeleteAllRecipes() {
	vector <string> RList;

	BOOL bOK = TRUE;
	ListRecipes(RList);
	for (int i = 0; i<int(RList.size()); i++) {
		if (!DeleteRecipe(RList[i])) bOK = FALSE;
	}
	return bOK;
}

BOOL CRecipe413NSMgr::IsRecipeAvailable(string& rcp) {
	CString dir;
	DosUtil.GetProgramDir(dir);
	dir += "Recipe\\";
	dir += rcp.c_str();
	//	dir += ".RCP";  // Already included [11/30/2010 C2C]

	HANDLE hFind;
	WIN32_FIND_DATA FindFileData;
	hFind = ::FindFirstFile(dir, &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE) {
		return FALSE;
	}
	return TRUE;
}