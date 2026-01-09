#pragma once

class C413Confg {
	short magic;

public:
	BOOL Save();
	BOOL Load();

	BOOL bAdmin;

	// Modules [6/2/2011 XPMUser]
	BOOL bSecsMod;
	BOOL bStressMod;
	BOOL bEchoMod, bThinFilmMod, bPatMatchMod, bTrimMod;
	BOOL bEchoMicroscopeMod, bThinFilmMicroscopeMod;
	BOOL bPMotorMod, bEchoTopMotorMod, bThinFilmMotorMod;

	// [11302022 ZHIMING
	BOOL bConfocalMod;
	// [20240307 ZHIMING
	BOOL bBowWarp;
	// [04052024
	BOOL bRoughness;
	BOOL bTaiko; //Ring Width 08092024 Mahedi Kamal
	// [05032024
	BOOL bEchoBottomMotorMod;
	// [05092024
	BOOL bConfocalTopMotorMod;
	BOOL bRoughMotorMod;
	// ]

	C413Confg();
	virtual ~C413Confg();
	void Serialize(CArchive& ar);
};