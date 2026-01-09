// SVID_ITEM.h: interface for the SVID_ITEM class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "string"

#define MAXSVIDLEN	128
#define MAXUNITLEN	16

class RCMD_ITEM {
public:
	CString Name;

	RCMD_ITEM();
	void Set(CString name);
};

class SVID_ITEM {
	CString str;  // local use only [7/18/2010 Yuen]
	char cstr[128]; // local use only [7/27/2010 Yuen]

public:
	enum SVIDTyp {
		N_EXPORT, SV_TYPE, DV_TYPE, EC_TYPE
	};
	SVIDTyp m_bVarType;

public:
	enum SVVALTYPE {
		SVNOTDEFINE, SVSHORT, SVINT, SVUINT, SVLONG, SVFLOAT, SVDOUBLE, SVCHAR
	};
	SVVALTYPE ValType;
	void* val;
	char* cV;

	void SetV(char* v);
	void SetV(int* v);
	void SetV(UINT* v);
	void SetV(long* v);
	void SetV(short* v);
	void SetV(float* v);
	void SetV(double* v);
	void SetS(UINT No, char* Name, int Type, int Size, /*int Mode,*/ SVIDTyp VType = SV_TYPE, char* unit = "");
	void SetR(int min, int max, int Deft);
	void SetR(float min, float max, float Deft);

	char* GetV();
	void GetV(int& value);
	void GetV(UINT& value);
	void GetV(long& value);
	void GetV(short& value);
	void GetV(float& value);
	void GetV(double& value);
	void GetRMin(float& value);
	void GetRMax(float& value);
	char* GetRMinChar();
	char* GetRMaxChar();
	// 	char * iGetRMinChar();
	// 	char * iGetRMaxChar();
	char* GetUnitStr();

public:
	CString GetTypeStr();
	CString GetLengthStr();
	char* GetLengthChar();
	CString GetVATypeStr();
	CString GetSVIDStr();
	char* GetVChar(short pic = 6);
	std::string GetVStr(short pic = 6);
	std::string GetDVStr(short pic = 6);
	UINT m_svidno;
	char m_svname[MAXSVIDLEN + 1];
	int	m_type;
	int	m_size;
	// 	int	m_mode;
	char m_unit[MAXUNITLEN + 1];
	float m_min, m_max, m_deft;

	SVID_ITEM();
	virtual ~SVID_ITEM();
};
