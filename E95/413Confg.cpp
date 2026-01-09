#include "stdafx.h"

#include "SRC/DOSUtil.h"

#include "413Confg.h"

C413Confg::C413Confg() {
	bSecsMod = FALSE;
	bStressMod = FALSE;
	bEchoMod = FALSE;
	bThinFilmMod = FALSE;
	bPatMatchMod = FALSE;
	bTrimMod = FALSE;
	bEchoMicroscopeMod = FALSE;
	bThinFilmMicroscopeMod = FALSE;
	bPMotorMod = FALSE;
	bEchoTopMotorMod = FALSE;
	bThinFilmMotorMod = FALSE;
	// [11302022 ZHIMING
	bConfocalMod = FALSE;
	// [20240307
	bBowWarp = FALSE;
	// [04052024
	bRoughness = FALSE;
	// [05032024
	bEchoBottomMotorMod = FALSE;
	// [05092024
	bConfocalTopMotorMod = FALSE;
	bRoughMotorMod = FALSE;
	// ]
	bAdmin = FALSE;
	bTaiko = FALSE; // 08092024 Mahedi Kamal

	char str[256];
	CString inifile;

	DosUtil.GetLocalCfgFile(inifile);
	CString sec = "Measurement";
	GetPrivateProfileString(sec, "Admin", "0", str, 255, inifile);
	bAdmin = atoi(str);

	Load();

	if (bPatMatchMod) {
		if (bThinFilmMod) {
			bThinFilmMicroscopeMod = TRUE;
		}
		if (bEchoMod) {
			bEchoMicroscopeMod = TRUE;
		}
	}
	if (bTrimMod) {
		bEchoMicroscopeMod = TRUE;
	}
}

C413Confg::~C413Confg() {}

void C413Confg::Serialize(CArchive& ar) {
	if (ar.IsStoring()) {
		magic = 8; // 7;//6;//5;//4; // 3; 11302022 ZHIMING	//20240307 ZHIMING//04052024//05032024/08092024
		ar << magic;
		ar << bSecsMod;
		ar << bEchoMod;
		ar << bThinFilmMod;
		ar << bPatMatchMod;
		ar << bTrimMod;
		ar << bEchoMicroscopeMod;
		ar << bThinFilmMicroscopeMod;
		ar << bPMotorMod;
		ar << bEchoTopMotorMod;
		ar << bThinFilmMotorMod;
		ar << bStressMod;
		// [11302022 ZHIMING
		ar << bConfocalMod;
		// [20240307
		ar << bBowWarp;
		// [04052024
		ar << bRoughness;
		// [05032024
		ar << bEchoBottomMotorMod;
		// [05092024
		ar << bConfocalTopMotorMod;
		ar << bRoughMotorMod;
		// ]
		ar << bTaiko; // 08092024 Mahedi Kamal
	}
	else {
		ar >> magic;
		ar >> bSecsMod;
		ar >> bEchoMod;
		ar >> bThinFilmMod;
		ar >> bPatMatchMod;
		ar >> bTrimMod;
		ar >> bEchoMicroscopeMod;
		ar >> bThinFilmMicroscopeMod;
		ar >> bPMotorMod;
		if (magic > 1) {
			ar >> bEchoTopMotorMod;
			ar >> bThinFilmMotorMod;
		}
		if (magic > 2) {
			ar >> bStressMod;
		}
		else {
			bStressMod = FALSE;
		}
		// [11302022 ZHIMING
		if (magic > 3) {
			ar >> bConfocalMod;
		}
		else {
			bConfocalMod = FALSE;
		}
		// [20240307
		if (magic > 4) {
			ar >> bBowWarp;
		}
		else {
			bBowWarp = FALSE;
		}
		// [04052024
		if (magic > 5) {
			ar >> bRoughness;
		}
		else {
			bRoughness = FALSE;
		}
		// [05032024
		if (magic > 6) {
			ar >> bEchoBottomMotorMod;
			ar >> bConfocalTopMotorMod;
			ar >> bRoughMotorMod;
		}
		else {
			bEchoBottomMotorMod = FALSE;
			bConfocalTopMotorMod = FALSE;
			bRoughMotorMod = FALSE;
		}
		// ]
		// [ 08092024 Mahedi Kamal
		if (magic > 7) {
			ar >> bTaiko;
		}
		else {
			bTaiko = FALSE;
		}
	}
}

BOOL C413Confg::Load() {
	CString mdir, filename;

	DosUtil.GetProgramDir(mdir);
	filename = mdir + "FSM413.BIN";

	CFile theFile;
	if (theFile.Open(filename, CFile::modeRead)) {
		CArchive archive(&theFile, CArchive::load);
		Serialize(archive);
		archive.Close();
		theFile.Close();
		return TRUE;
	}
	return FALSE;
}

BOOL C413Confg::Save() {
	CString mdir, filename;

	DosUtil.GetProgramDir(mdir);
	filename = mdir + "FSM413.BIN";

	CFile theFile;
	if (theFile.Open(filename, CFile::modeWrite | CFile::modeCreate)) {
		CArchive archive(&theFile, CArchive::store);
		Serialize(archive);
		archive.Close();
		theFile.Close();
		return TRUE;
	}
	return FALSE;
}