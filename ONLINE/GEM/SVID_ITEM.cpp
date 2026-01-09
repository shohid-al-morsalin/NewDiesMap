// SVID_ITEM.cpp: implementation of the SVID_ITEM class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "strSOp.h"
#include "SVID_ITEM.h"
#include "..\INC\Define.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVID_ITEM::SVID_ITEM() {
	m_svidno = 0;
	memset(m_svname, 0, 128);
	m_type = 0;
	m_size = 0;
	// 	m_mode = 0;
	m_min = m_max = m_deft = 0;
	memset(m_unit, 0, 16);
	m_bVarType = SV_TYPE;
	ValType = SVNOTDEFINE;
	cV = NULL;
	val = NULL;
}

SVID_ITEM::~SVID_ITEM() {
	if (cV) {
		delete[] cV;
	}
}

void SVID_ITEM::SetS(UINT No, char* Name, int Type, int Size, /*int Mode,*/ SVIDTyp VType, char* unit) {
	m_svidno = No;
	strscpy(m_svname, MAXSVIDLEN, Name);
	m_type = Type;
	m_size = Size;
	// 	m_mode = Mode;
	strscpy(m_unit, MAXUNITLEN, unit);
	m_bVarType = VType;
}

void SVID_ITEM::SetV(int* v) {
	val = (void*)v;
	ValType = SVINT;
}

void SVID_ITEM::SetV(UINT* v) {
	val = (void*)v;
	ValType = SVUINT;
}

void SVID_ITEM::SetV(long* v) {
	val = (void*)v;
	ValType = SVLONG;
}

void SVID_ITEM::SetV(short* v) {
	val = (void*)v;
	ValType = SVSHORT;
}

void SVID_ITEM::SetV(float* v) {
	val = (void*)v;
	ValType = SVFLOAT;
}

void SVID_ITEM::SetV(double* v) {
	val = (void*)v;
	ValType = SVDOUBLE;
}

void SVID_ITEM::SetV(char* v) {
	if (cV) {
		delete[] cV;
		cV = NULL;
	}
	short len = short(strlen(v));
	if (len > 0) {
		cV = new char[len + 1];
	}
	if (cV) {
		strscpy(cV, len + 1, v);
		ValType = SVCHAR;
	}
	else {
		//memset(cV,0,len+1);
		ValType = SVNOTDEFINE;
	}
}

char* SVID_ITEM::GetV() {
	if (ValType == SVCHAR) {
		return cV;
	}
	ASSERT(0);
	return NULL;
}

void SVID_ITEM::GetV(int& value) {
	if (ValType == SVINT) {
		value = *(int*)val;
		return;
	}
	ASSERT(0);
}

void SVID_ITEM::GetV(UINT& value) {
	if (ValType == SVUINT) {
		value = *(int*)val;
		return;
	}
	ASSERT(0);
}

void SVID_ITEM::GetV(long& value) {
	if (ValType == SVLONG) {
		value = *(long*)val;
		return;
	}
	ASSERT(0);
}

void SVID_ITEM::GetV(short& value) {
	if (ValType == SVSHORT) {
		value = *(short*)val;
		return;
	}
	ASSERT(0);
}

void SVID_ITEM::GetV(float& value) {
	if (ValType == SVFLOAT) {
		value = *(float*)val;
		return;
	}
	ASSERT(0);
}

void SVID_ITEM::GetV(double& value) {
	if (ValType == SVDOUBLE) {
		value = *(double*)val;
		return;
	}
	ASSERT(0);
}

std::string SVID_ITEM::GetVStr(short pic) {
	CString str, str1;
	static std::string VStr = "";

	switch (ValType) {
	case SVCHAR:
		str.Format("%s", cV);
		VStr = str;
		break;
	case SVINT:
		str.Format("%d", *(int*)val);
		VStr = str;
		break;
	case SVUINT:
		str.Format("%d", *(UINT*)val);
		VStr = str;
		break;
	case SVSHORT:
		str.Format("%d", *(short*)val);
		VStr = str;
		break;
	case SVLONG:
		str.Format("%ld", *(long*)val);
		VStr = str;
		break;
	case SVFLOAT:
		str.Format("%%.%df", pic);
		str1.Format(str, *(float*)val);
		VStr = str1;
		break;
	case SVDOUBLE:
		str.Format("%%.%df", pic);
		str1.Format(str, *(double*)val);
		VStr = str1;
		break;
	default:
		ASSERT(0);
		break;
	}
	return VStr;
}

char* SVID_ITEM::GetVChar(short pic) {
	CString str;
	static char buf[128];

	switch (ValType) {
	case SVCHAR:
		sprintf(buf, "%s", cV);
		break;
	case SVINT:
		sprintf(buf, "%d", *(int*)val);
		break;
	case SVUINT:
		sprintf(buf, "%d", *(UINT*)val);
		break;
	case SVSHORT:
		sprintf(buf, "%d", *(short*)val);
		break;
	case SVLONG:
		sprintf(buf, "%ld", *(long*)val);
		break;
	case SVFLOAT:
		str.Format("%%.%df", pic);
		sprintf(buf, str, *(float*)val);
		break;
	case SVDOUBLE:
		str.Format("%%.%df", pic);
		sprintf(buf, str, *(double*)val);
		break;
	default:
		ASSERT(0);
		break;
	}
	return buf;
}

std::string SVID_ITEM::GetDVStr(short pic) {
	CString str;
	static char buf[128];

	switch (ValType) {
		// 	case SVCHAR:
		// 		sprintf(buf, "%s", cV);
		// 		break;
	case SVINT:
		sprintf(buf, "%d", (int)m_deft);
		break;
	case SVUINT:
		sprintf(buf, "%d", (UINT)m_deft);
		break;
	case SVSHORT:
		sprintf(buf, "%d", (short)m_deft);
		break;
	case SVLONG:
		sprintf(buf, "%ld", (long)m_deft);
		break;
	case SVFLOAT:
		str.Format("%%.%df", pic);
		sprintf(buf, str, (float)m_deft);
		break;
	case SVDOUBLE:
		str.Format("%%.%df", pic);
		sprintf(buf, str, (double)m_deft);
		break;
	default:
		ASSERT(0);
		break;
	}
	return buf;
}

CString SVID_ITEM::GetSVIDStr() {
	str.Format("%d", m_svidno);
	return str;
}

//////////////////////////////////////////////////////////////////////
// #define   svLIST         0x01
// #define   svASCII        0x41
// #define   svBINARY		0x21
// #define	 svBOOLEAN		0x25
// #define   svUNSIGN1		0xA5		// 1BYTE UNSIGNED
// #define   svUNSIGN2		0xA9		// 2BYTE UNSIGNED
// #define   svUNSIGN4      0xB1        // 4BYTE UNSIGNED
// #define   svUNSIGN8      0xA1        // 8BYTE UNSIGNED
// #define   svINT1			0x65		// 1BYTE SIGNED
// #define   svINT2			0x69		// 2BYTE SIGNED
// #define   svINT4			0x71        // 4BYTE SIGNED
// #define   svINT8			0x61        // 8BYTE SIGNED
// #define   svFLOAT4       0x91        // 4Byte FLOAT
// #define   svFLOAT8       0x81        // 8Byte FLOAT
//////////////////////////////////////////////////////////////////////
CString SVID_ITEM::GetVATypeStr() {
	switch (m_type) {
	case svLIST:
		str.Format("0x%2X (LIST)", svLIST);
		break;
	case svASCII:
		str.Format("0x%2X (ASCII)", svASCII);
		break;
	case svBINARY:
		str.Format("0x%2X (BINARY)", svBINARY);
		break;
	case svBOOLEAN:
		str.Format("0x%2X (BOOL)", svBOOLEAN);
		break;
	case svUNSIGN1:
		str.Format("0x%2X (UNSIGN1)", svUNSIGN1);
		break;
	case svUNSIGN2:
		str.Format("0x%2X (UNSIGN2)", svUNSIGN2);
		break;
	case svUNSIGN4:
		str.Format("0x%2X (UNSIGN4)", svUNSIGN4);
		break;
	case svUNSIGN8:
		str.Format("0x%2X (UNSIGN8)", svUNSIGN8);
		break;
	case svINT1:
		str.Format("0x%2X (INT1)", svINT1);
		break;
	case svINT2:
		str.Format("0x%2X (INT2)", svINT2);
		break;
	case svINT4:
		str.Format("0x%2X (INT4)", svINT4);
		break;
	case svINT8:
		str.Format("0x%2X (INT8)", svINT8);
		break;
	case svFLOAT4:
		str.Format("0x%2X (FLOAT4)", svFLOAT4);
		break;
	case svFLOAT8:
		str.Format("0x%2X (FLOAT8)", svFLOAT8);
		break;
	}
	return str;
}

CString SVID_ITEM::GetLengthStr() {
	str.Format("%d", m_size);
	return str;
}

CString SVID_ITEM::GetTypeStr() {
	switch (m_bVarType) {
	case N_EXPORT:
		str = "EXPORT";
		break;
	case SV_TYPE:
		str = "SV";
		break;
	case DV_TYPE:
		str = "DV";
		break;
	case EC_TYPE:
		str = "EC";
		break;
	}
	return str;
}

void SVID_ITEM::GetRMin(float& value) {
	value = m_min;
}

void SVID_ITEM::GetRMax(float& value) {
	value = m_max;
}

char* SVID_ITEM::GetRMinChar() {
	CString str;
	switch (ValType) {
	case SVCHAR:
	case SVINT:
	case SVUINT:
	case SVSHORT:
		sprintf(cstr, "%d", (int)m_min);
		break;
	case SVLONG:
		sprintf(cstr, "%d", (int)m_min);
		break;
	case SVFLOAT:
		str.Format("%%.%df", 6);
		sprintf(cstr, str, (float)m_min);
		break;
	case SVDOUBLE:
		str.Format("%%.%df", 6);
		sprintf(cstr, str, (double)m_min);
		break;
	default:
		ASSERT(0);
		break;
	}
	// 	sprintf(cstr,"%f",m_min);
	return cstr;
}

char* SVID_ITEM::GetRMaxChar() {
	CString str;
	switch (ValType) {
	case SVCHAR:
	case SVINT:
	case SVUINT:
	case SVSHORT:
		sprintf(cstr, "%d", (int)m_max);
		break;
	case SVLONG:
		sprintf(cstr, "%ld", (long)m_max);
		break;
	case SVFLOAT:
		str.Format("%%.%df", 6);
		sprintf(cstr, str, (float)m_max);
		break;
	case SVDOUBLE:
		str.Format("%%.%df", 6);
		sprintf(cstr, str, (double)m_max);
		break;
	default:
		ASSERT(0);
		break;
	}
	//	sprintf(cstr,"%f",m_max);
	return cstr;
}

// char * SVID_ITEM::iGetRMinChar()
// {
// 	sprintf(cstr,"%d",(int)m_min);
// 	return cstr;
// }
//
// char * SVID_ITEM::iGetRMaxChar()
// {
// 	sprintf(cstr,"%d",(int)m_max);
// 	return cstr;
// }

char* SVID_ITEM::GetUnitStr() {
	return m_unit;
}

void SVID_ITEM::SetR(float min, float max, float Deft) {
	m_min = min;
	m_max = max;
	m_deft = Deft;
	ValType = SVFLOAT;
}

void SVID_ITEM::SetR(int min, int max, int Deft) {
	m_min = min;
	m_max = max;
	m_deft = Deft;
	ValType = SVUINT;
}

char* SVID_ITEM::GetLengthChar() {
	sprintf(cstr, "%d", m_size);
	return cstr;
}

RCMD_ITEM::RCMD_ITEM() {
	Name = "";
}

void RCMD_ITEM::Set(CString name) {
	Name = name;
}