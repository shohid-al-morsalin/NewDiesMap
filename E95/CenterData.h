#pragma once

struct SCenterData {
	BOOL bOffsetInUse = FALSE;
	float X0 = 0, Y0 = 0;
	float X90 = 0, Y90 = 0;
	float X180 = 0, Y180 = 0;
	float X270 = 0, Y270 = 0;

	float XOff = 0, YOff = 0;

	void ClearOffset();
	void FindOffset();
	void Serialize(CArchive& ar);
	void SetOffset(float X, float Y);
	void Load(CString filename);
	void Save(CString filename);
};

extern SCenterData WC;
