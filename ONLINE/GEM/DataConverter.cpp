#include "stdafx.h"
#include "DataConverter.h"
#include "SecsItem.h"
#include "ONLINE/INC/Define.h"
#include <limits.h>
#include <string>
using namespace std;

DataConverter::~DataConverter() {
	Clear();
}

DataConverter::DataConverter() {
	pFrom = NULL;
}

void DataConverter::Val(unsigned char Data) {
	Clear();
	pFrom = new SecsItemUInt1(Data);
}

void DataConverter::Val(unsigned short Data) {
	Clear();
	pFrom = new SecsItemUInt2(Data);
}

void DataConverter::Val(unsigned int Data) {
	Clear();
	pFrom = new SecsItemUInt4(Data);
}

void DataConverter::Val(unsigned __int64 Data) {
	Clear();
	pFrom = new SecsItemUInt8(Data);
}

void DataConverter::Val(char Data) {
	Clear();
	pFrom = new SecsItemInt1(Data);
}

void DataConverter::Val(short Data) {
	Clear();
	pFrom = new SecsItemInt2(Data);
}

void DataConverter::Val(int Data) {
	Clear();
	pFrom = new SecsItemInt4(Data);
}

void DataConverter::Val(__int64 Data) {
	Clear();
	pFrom = new SecsItemInt8(Data);
}

void DataConverter::Val(unsigned char* Data, UINT Len) {
	Clear();
	pFrom = new SecsItemBinary(Data, Len);
}

void DataConverter::Val(bool Data) {
	Clear();
	pFrom = new SecsItemBoolean(Data);
}

void DataConverter::Val(std::string& Data) {
	Clear();
	pFrom = new SecsItemASCII(Data);
}

void DataConverter::Val(const char* Data) {
	Val(string(Data));
}

void DataConverter::Val(float Data) {
	Clear();
	pFrom = new SecsItemFloat4(Data);
}

void DataConverter::Val(double Data) {
	Clear();
	pFrom = new SecsItemFloat8(Data);
}

void DataConverter::Val(SecsItemList* Data) {
	Clear();
	pFrom = Data;
}

void DataConverter::Clear() {
	if (pFrom) {
		delete pFrom;
		pFrom = NULL;
	}
}

DataConverter::operator unsigned char() {
	unsigned char Conv = 0;
	if (!pFrom) {
		//ASSERT(0);
	}
	else if (pFrom->IsCollection()) {
		Conv = pFrom->size();
	}
	else {
		if (!pFrom->GetUInt1(Conv)) {
			string Data;
			UINT Tmp;
			pFrom->GetData(Data);
			Tmp = atoi(Data.c_str());
#ifdef _DEBUG
			if ((Tmp > UCHAR_MAX)) {
				//TRACE("unsigned char truncated (%d) \n",Tmp);
			}
#endif
			Conv = (unsigned char)Tmp;
		}
	}
	return Conv;
}

DataConverter::operator unsigned short() {
	unsigned short Conv = 0;
	if (!pFrom) {
		//ASSERT(0);
	}
	else if (pFrom->IsCollection()) {
		Conv = pFrom->size();
	}
	else {
		if (!pFrom->GetUInt2(Conv)) {
			string Data;
			UINT Tmp;
			pFrom->GetData(Data);
			Tmp = atoi(Data.c_str());
#ifdef _DEBUG
			if ((Tmp > USHRT_MAX)) {
				//TRACE("unsigned short truncated (%d) \n",Tmp);
			}
#endif
			Conv = (unsigned short)Tmp;
		}
	}
	return Conv;
}

DataConverter::operator unsigned int() {
	unsigned int Conv = 0;
	if (!pFrom) {
		//ASSERT(0);
	}
	else if (pFrom->IsCollection()) {
		Conv = pFrom->size();
	}
	else {
		if (!pFrom->GetUInt4(Conv)) {
			string Data;
			pFrom->GetData(Data);
			unsigned __int64 Tmp = _atoi64(Data.c_str());
#ifdef _DEBUG
			if ((Tmp > UINT_MAX)) {
				//TRACE("unsigned int truncated (%d) \n",Tmp);
			}
#endif
			Conv = (unsigned int)Tmp;
		}
	}
	return Conv;
}

DataConverter::operator unsigned __int64() {
	unsigned __int64 Conv = 0;
	if (!pFrom) {
		//ASSERT(0);
	}
	else if (pFrom->IsCollection()) {
		Conv = pFrom->size();
	}
	else {
		if (!pFrom->GetUInt8((__int64&)Conv)) {
			string Data;
			pFrom->GetData(Data);
			Conv = _atoi64(Data.c_str());
#ifdef _DEBUG
			// No (~standard~) way to check overflow
#endif
		}
	}
	return Conv;
}

DataConverter::operator char() {
	char Conv = 0;
	if (!pFrom) {
		//ASSERT(0);
	}
	else if (pFrom->IsCollection()) {
		Conv = pFrom->size();
	}
	else {
		if (!pFrom->GetInt1(Conv)) {
			string Data;
			pFrom->GetData(Data);
			int Tmp = atoi(Data.c_str());
#ifdef _DEBUG
			if ((Tmp > SCHAR_MAX) || (Tmp < SCHAR_MIN)) {
				//TRACE("char truncated (%d) \n",Tmp);
			}
#endif
			Conv = (char)Tmp;
		}
	}
	return Conv;
}

DataConverter::operator short() {
	short Conv = 0;
	if (!pFrom) {
		//ASSERT(0);
	}
	else if (pFrom->IsCollection()) {
		Conv = pFrom->size();
	}
	else {
		if (!pFrom->GetInt2(Conv)) {
			string Data;
			pFrom->GetData(Data);
			int Tmp = atoi(Data.c_str());
#ifdef _DEBUG
			if ((Tmp > SHRT_MAX) || (Tmp < SHRT_MIN)) {
				//TRACE("short truncated (%d) \n",Tmp);
			}
#endif
			Conv = (short)Tmp;
		}
	}
	return Conv;
}

DataConverter::operator int() {
	int Conv = 0;
	if (!pFrom) {
		//ASSERT(0);
	}
	else if (pFrom->IsCollection()) {
		Conv = pFrom->size();
	}
	else {
		if (!pFrom->GetInt4(Conv)) {
			string Data;
			pFrom->GetData(Data);
			__int64 Tmp = _atoi64(Data.c_str());
#ifdef _DEBUG
			if ((Tmp > INT_MAX) || (Tmp < INT_MIN)) {
				//TRACE("int truncated (%d) \n",Tmp);
			}
#endif
			Conv = (int)Tmp;
		}
	}
	return Conv;
}

DataConverter::operator __int64() {
	__int64 Conv = 0;
	if (!pFrom) {
		//ASSERT(0);
	}
	else if (pFrom->IsCollection()) {
		Conv = pFrom->size();
	}
	else {
		if (!pFrom->GetInt8(Conv)) {
			string Data;
			pFrom->GetData(Data);
			Conv = _atoi64(Data.c_str());
#ifdef _DEBUG
			// No (~standard~) way to check overflow
#endif
		}
	}
	return Conv;
}

DataConverter::operator float() {
	float Conv = 0;
	if (!pFrom) {
		//ASSERT(0);
	}
	else if (pFrom->IsCollection()) {
		Conv = pFrom->size();
	}
	else {
		if (!pFrom->GetFloat4(Conv)) {
			string Data;
			pFrom->GetData(Data);
			Conv = atof(Data.c_str());
		}
	}
	return Conv;
}

DataConverter::operator double() {
	double Conv = 0;
	if (!pFrom) {
		//ASSERT(0);
	}
	else if (pFrom->IsCollection()) {
		Conv = pFrom->size();
	}
	else {
		if (!pFrom->GetFloat8(Conv)) {
			string Data;
			pFrom->GetData(Data);
			Conv = atof(Data.c_str());
		}
	}
	return Conv;
}

DataConverter::operator string() {
	string Conv = "";
	if (!pFrom) {
		//ASSERT(0);
	}
	else {
		pFrom->GetData(Conv);
	}
	return Conv;
}

DataConverter::operator SecsItemList* () {
	SecsItemList* Conv = NULL;
	if (!pFrom) {
		Conv = new SecsItemList;
	}
	else if (pFrom->IsList()) {
		Conv = (SecsItemList*)pFrom->CloneSelf();
	}
	else {
		Conv = new SecsItemList;
		Conv->AddItem(pFrom->CloneSelf());
	}
	return Conv;
}

// @@Vijay To be done Meaningful implementation
// Support only one byte conversion
DataConverter::operator unsigned char* () {
	ASSERT(0);
	return new BYTE[1];
}

DataConverter::operator bool() {
	bool Conv = false;
	if (!pFrom) {
		//ASSERT(0);
	}
	else if (pFrom->IsCollection()) {
		UINT Sz = pFrom->size();
		Conv = (Sz) ? true : false;
	}
	else {
		if (!pFrom->GetBoolean(Conv)) {
			string Data;
			pFrom->GetData(Data);
			Conv = (atoi(Data.c_str())) ? true : false;
		}
	}
	return Conv;
}

SecsItem* DataConverter::GetType(UINT ItmType) {
	SecsItem* pItem = NULL;
	switch (ItmType) {
	case svASCII:
		pItem = new SecsItemASCII(string(*this));
		break;
	case svBINARY:
		pItem = new SecsItemBinary((unsigned char*)(*this), 1);
		break;
	case svUNSIGN1:
		pItem = new SecsItemUInt1(unsigned char(*this));
		break;
	case svUNSIGN2:
		pItem = new SecsItemUInt2(unsigned short(*this));
		break;
	case svUNSIGN4:
		pItem = new SecsItemUInt4(unsigned int(*this));
		break;
	case svUNSIGN8:
		pItem = new SecsItemUInt8(unsigned __int64(*this));
		break;
	case svINT1:
		pItem = new SecsItemInt1(char(*this));
		break;
	case svINT2:
		pItem = new SecsItemInt2(short(*this));
		break;
	case svINT4:
		pItem = new SecsItemInt4(int(*this));
		break;
	case svINT8:
		pItem = new SecsItemInt8(__int64(*this));
		break;
	case svLIST:
		pItem = (SecsItemList*)(*this);
		break;
	case svBOOLEAN:
		pItem = new SecsItemBoolean(bool(*this));
		break;
	case svFLOAT4:
		pItem = new SecsItemFloat4(float(*this));
		break;
	case svFLOAT8:
		pItem = new SecsItemFloat8(double(*this));
		break;
	default:
		ASSERT(0);
		break;
	}
	return pItem;
}