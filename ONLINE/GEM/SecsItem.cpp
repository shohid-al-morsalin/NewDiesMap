// SecsItem.cpp: implementation of the SecsItem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math.h>
#include "SecsItem.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
int SecsItem::MsgLevel = 0;
string SecsItem::Sep = "      ";		// 6 Spaces

SecsItem::SecsItem() {
	eType = UNDEFINED;
	MsgLevel = 0;
}

SecsItem::~SecsItem() {}

int SecsItem::NumContainedItems() {
	return 0;
}

SecsItem* SecsItem::ContainedItems(int nIndex) {
	return NULL;
}

bool  SecsItem::GetBoolean(bool& Boolean) {
	bool bRet = false;
	if ((IsBoolean()) && (!IsZeroLen())) {
		string Data;
		GetData(Data);
		char Temp = Data.c_str()[0];
		Boolean = ((Temp == 't') || (Temp == 'T')) ? true : false;
		bRet = true;
	}
	return bRet;
}

bool  SecsItem::GetInt8(__int64& Int64) {
	bool bRet = false;
	if ((IsInt8()) && (!IsZeroLen())) {
		string Data;
		GetData(Data);
		Int64 = _atoi64(Data.c_str());
		bRet = true;
	}
	return bRet;
}

bool  SecsItem::GetUInt(UINT& UInt4) {
	bool bRet = false;
	if ((IsUInt()) && (!IsZeroLen())) {
		string Data;
		GetData(Data);
		UInt4 = atoi(Data.c_str());
		bRet = true;
	}
	return bRet;
}

bool  SecsItem::GetInt4(int& Int4) {
	bool bRet = false;
	if ((IsInt4()) && (!IsZeroLen())) {
		string Data;
		GetData(Data);
		Int4 = atoi(Data.c_str());
		bRet = true;
	}
	return bRet;
}

bool  SecsItem::GetInt2(short& Int2) {
	bool bRet = false;
	if ((IsInt2()) && (!IsZeroLen())) {
		string Data;
		GetData(Data);
		Int2 = atoi(Data.c_str());
		bRet = true;
	}
	return bRet;
}

bool  SecsItem::GetInt1(char& Int1) {
	bool bRet = false;
	if ((IsInt1()) && (!IsZeroLen())) {
		string Data;
		GetData(Data);
		Int1 = atoi(Data.c_str());
		bRet = true;
	}
	return bRet;
}

bool  SecsItem::GetUInt8(__int64& Int8) {
	bool bRet = false;
	if ((IsUInt8()) && (!IsZeroLen())) {
		string Data;
		GetData(Data);
		Int8 = _atoi64(Data.c_str());
		bRet = true;
	}
	return bRet;
}

bool  SecsItem::GetUInt4(UINT& UInt4) {
	bool bRet = false;
	if ((IsUInt4()) && (!IsZeroLen())) {
		string Data;
		GetData(Data);
		UInt4 = atoi(Data.c_str());
		bRet = true;
	}
	return bRet;
}

bool  SecsItem::GetUInt2(USHORT& UInt2) {
	bool bRet = false;
	if ((IsUInt2()) && (!IsZeroLen())) {
		string Data;
		GetData(Data);
		UInt2 = atoi(Data.c_str());
		bRet = true;
	}
	return bRet;
}

bool  SecsItem::GetUInt1(BYTE& UInt1) {
	bool bRet = false;
	if ((IsUInt1()) && (!IsZeroLen())) {
		string Data;
		GetData(Data);
		UInt1 = atoi(Data.c_str());
		bRet = true;
	}
	return bRet;
}

bool  SecsItem::GetFloat4(float& Float4) {
	bool bRet = false;
	if ((IsFloat4()) && (!IsZeroLen())) {
		string Data;
		GetData(Data);
		Float4 = atof(Data.c_str());
		bRet = true;
	}
	return bRet;
}

bool  SecsItem::GetFloat8(double& Float8) {
	bool bRet = false;
	if ((IsFloat8()) && (!IsZeroLen())) {
		string Data;
		GetData(Data);
		Float8 = atof(Data.c_str());
		bRet = true;
	}
	return bRet;
}

bool  SecsItem::GetASCII(string& String) {
	bool bRet = false;
	if (IsASCII()) {
		GetData(String);
		bRet = true;
	}
	return bRet;
}

bool  SecsItem::GetBinary(string& String) {
	bool bRet = false;
	if (IsBinary()) {
		GetData(String);
		bRet = true;
	}
	return bRet;
}

bool  SecsItem::GetBinary(BYTE* pData, UINT DataLen) {
	bool bRet = false;
	if (IsBinary() && GetLength() <= int(DataLen)) {
		RawCopy(pData);
		bRet = true;
	}
	return bRet;
}

BOOL  SecsItem::IsList() {
	return eType == ITEM_LIST;
}

BOOL  SecsItem::IsBinary() {
	return eType == ITEM_BINARY;
}

BOOL  SecsItem::IsBoolean() {
	return eType == ITEM_BOOLEAN;
}

BOOL  SecsItem::IsASCII() {
	return eType == ITEM_ASCII;
}
BOOL  SecsItem::IsJIS() {
	return eType == ITEM_JIS;
}

BOOL  SecsItem::Is2ByteChar() {
	return FALSE;
}

BOOL  SecsItem::IsInt8() {
	return eType == ITEM_INT8;
}

BOOL  SecsItem::IsArray() {
	return eType == ITEM_ARRAY;
}

BOOL SecsItem::IsAInt() {
	if (IsInt()) {
		return TRUE;
	}
	if (IsUInt()) {
		return TRUE;
	}
	return FALSE;
}

BOOL  SecsItem::IsInt() {
	return (eType == ITEM_INT1) ||
		(eType == ITEM_INT2) ||
		(eType == ITEM_INT4) ||
		(eType == ITEM_INT8);
}

BOOL  SecsItem::IsUInt() {
	return (eType == ITEM_UINT1) ||
		(eType == ITEM_UINT2) ||
		(eType == ITEM_UINT4) ||
		(eType == ITEM_UINT8);
}

BOOL  SecsItem::IsFloat() {
	return (eType == ITEM_FLOAT4) ||
		(eType == ITEM_FLOAT8);
}

BOOL  SecsItem::IsInt4() {
	return eType == ITEM_INT4;
}

BOOL  SecsItem::IsInt2() {
	return eType == ITEM_INT2;
}

BOOL  SecsItem::IsInt1() {
	return eType == ITEM_INT1;
}

BOOL  SecsItem::IsUInt8() {
	return eType == ITEM_UINT8;
}

BOOL  SecsItem::IsUInt4() {
	return eType == ITEM_UINT4;
}

BOOL  SecsItem::IsUInt2() {
	return eType == ITEM_UINT2;
}

BOOL  SecsItem::IsUInt1() {
	return eType == ITEM_UINT1;
}

BOOL  SecsItem::IsFloat4() {
	return eType == ITEM_FLOAT4;
}

BOOL  SecsItem::IsFloat8() {
	return eType == ITEM_FLOAT8;
}

void SecsItem::MakeDataLength(int DataLen, BYTE* pByte, int Len) {
	switch (DataLen) {
	case 1:
		pByte[0] = (BYTE)Len;
		break;
	case 2:
		pByte[0] = (BYTE)(Len >> 8);
		pByte[1] = (BYTE)(Len);
		break;
	case 3:
		pByte[0] = (BYTE)(Len >> 16);
		pByte[1] = (BYTE)(Len >> 8);
		pByte[2] = (BYTE)(Len);
		break;
	}
}

BYTE SecsItem::GetBytesReqd(int Len) {
	BYTE nNumBytes = 0;
	if (Len < pow(2.0f, 8)) {
		nNumBytes = 1;
	}
	else if (Len < pow(2.0f, 16)) {
		nNumBytes = 2;
	}
	else if (Len < pow(2.0f, 24)) {
		nNumBytes = 3;
	}
	else {
		throw "Error in Getting Required Bytes !";
	}
	return nNumBytes;
}

BYTE SecsItem::MakeDataByte(DataType Data, BYTE nDataBytes) {
	ASSERT((nDataBytes < 4) && (nDataBytes >= 0));
	BYTE DataByte = (BYTE)(Data << 2);
	DataByte |= nDataBytes;
	return DataByte;
}

void SecsItem::ByteSwap(void* pVoid, int nNumBytes) {
	BYTE* pByte = (BYTE*)pVoid;
	int mid = nNumBytes / 2;
	BYTE Temp;
	for (int nIndex = 0; nIndex < mid; nIndex++) {
		Temp = pByte[nNumBytes - 1 - nIndex];
		pByte[nNumBytes - 1 - nIndex] = pByte[nIndex];
		pByte[nIndex] = Temp;
	}
}

int SecsItem::GetSECSLength() {
	return 0;
}

// List implementation

BOOL  SecsItemList::AddItem(SecsItem* pItem) {
	if ((pItem != NULL) && (pItem != this)) {
		this->Item.push_back(pItem);
		return TRUE;
	}
	return FALSE;
}

void  SecsItemList::GetData(string& Data) {
	//Data = "0"; // No meaning for List
	char Dummy[25] = { 0 };
	int nSize = this->Item.size();
	int nSpace;
	for (nSpace = 0; nSpace < MsgLevel; nSpace++) {
		Data += Sep;
	}
	Data += "< L [";
	_itoa(nSize, Dummy, 10);
	Data += (Dummy);
	Data += "]\r\n";
	MsgLevel++;
	for (int nIndex = 0; nIndex < nSize; nIndex++) {
		Item[nIndex]->PrintSML(Data);
	}
	MsgLevel--;
	for (nSpace = 0; nSpace < MsgLevel; nSpace++) {
		Data += Sep;
	}
	Data += (">\r\n");
}

void  SecsItemList::PrintBinary(BYTE* pByte) {
	UINT nSize = this->Item.size();
	BYTE nNumBytes = GetBytesReqd(nSize);
	pByte[0] = MakeDataByte(eType, nNumBytes);
	MakeDataLength(nNumBytes, &(pByte[1]), nSize);
	UINT ByteWritten = 1 + nNumBytes;
	UINT DataLen = 0;
	for (UINT nIndex = 0; nIndex < nSize; nIndex++) {
		DataLen = Item[nIndex]->GetSECSLength();
		pByte += ByteWritten;
		Item[nIndex]->PrintBinary(pByte);
		ByteWritten = DataLen;
	}
}

void  SecsItemList::PrintASCII() {
	int nSize = this->Item.size();
	for (int nIndex = 0; nIndex < nSize; nIndex++) {
		Item[nIndex]->PrintASCII();
	}
}

void  SecsItemList::PrintSML() {
	int nSize = this->Item.size();
	MsgLevel++;
	for (int nIndex = 0; nIndex < nSize; nIndex++) {
		Item[nIndex]->PrintSML();
	}
	MsgLevel--;
}

void  SecsItemList::PrintSML(string& Data) {
	char Dummy[25] = { 0 };
	int nSize = this->Item.size();
	int nSpace;
	for (nSpace = 0; nSpace < MsgLevel; nSpace++) {
		Data += Sep;
	}
	Data += "< L [";
	_itoa(nSize, Dummy, 10);
	Data += (Dummy);
	Data += "]\r\n";
	MsgLevel++;
	for (int nIndex = 0; nIndex < nSize; nIndex++) {
		Item[nIndex]->PrintSML(Data);
	}
	MsgLevel--;
	for (nSpace = 0; nSpace < MsgLevel; nSpace++) {
		Data += Sep;
	}
	Data += (">\r\n");
}

int	  SecsItemList::GetLength() {
	return Item.size();
}

SecsItemList::SecsItemList() {
	eType = ITEM_LIST;
}

int SecsItemList::NumContainedItems() {
	return Item.size();
}

int SecsItemList::GetSECSLength() {
	int nSize = this->Item.size();
	UINT TotalLen = 0;
	for (int nIndex = 0; nIndex < nSize; nIndex++) {
		TotalLen += Item[nIndex]->GetSECSLength();
	}
	return TotalLen + GetBytesReqd(nSize) + 1;
}

SecsItemList::~SecsItemList() {
	int nSize = this->Item.size();
	for (int nIndex = 0; nIndex < nSize; nIndex++) {
		delete(Item[nIndex]);
	}
	Item.clear();
}

BOOL  SecsItemBinary::AddItem(SecsItem* pItem) {
	return FALSE;
}

void  SecsItemBinary::PrintBinary(BYTE* pByte) {
	BYTE nNumBytes = GetBytesReqd(Len);
	pByte[0] = MakeDataByte(eType, nNumBytes);;
	MakeDataLength(nNumBytes, &(pByte[1]), Len);
	memcpy(&(pByte[nNumBytes + 1]), Item, Len);
}

void  SecsItemBinary::RawCopy(BYTE* pByte) {
	memcpy(pByte, Item, Len);
}

void  SecsItemBinary::PrintASCII() {
	string Data = "\n{ Binary data ";
	char Buf[12];
	for (int nIndex = 0; nIndex < Len; nIndex++) {
		sprintf(Buf, " 0x%02X", Item[nIndex]);
		Data += Buf;
	}
	Data += " }";
}

void  SecsItemBinary::GetData(string& Data) {
	char Buf[12];
	Data = "";
	for (int nIndex = 0; nIndex < Len; nIndex++) {
		sprintf(Buf, " %03d", Item[nIndex]);
		Data += Buf;
	}
}

void  SecsItemBinary::PrintSML() {
	string Data;
	for (int nSpace = 0; nSpace < MsgLevel; nSpace++) {
		Data += "  ";
	}
	Data += "< B ";
	char Buf[12];
	for (int nIndex = 0; nIndex < Len; nIndex++) {
		sprintf(Buf, " 0x%02x", Item[nIndex]);
		Data += Buf;
	}
	Data += " >\r\n";
}

void  SecsItemBinary::PrintSML(string& Data) {
	for (int nSpace = 0; nSpace < MsgLevel; nSpace++) {
		Data += Sep;
	}
	Data += "< B ";
	char Buf[12];
	for (int nIndex = 0; nIndex < Len; nIndex++) {
		sprintf(Buf, " 0x%02x", Item[nIndex]);
		Data += Buf;
	}
	Data += " >\r\n";
}

int	  SecsItemBinary::GetLength() {
	return Len;
}

SecsItemBinary::SecsItemBinary(BYTE* pByte, int nLen) {
	if (nLen > 0X1000000) {
		throw "Illegal length of binary data";
	}
	this->Len = nLen;
	this->Item = new BYTE[nLen];
	memcpy(Item, pByte, nLen);
	eType = ITEM_BINARY;
}

int SecsItemBinary::GetSECSLength() {
	return 1 + GetBytesReqd(Len) + Len;
}

SecsItemBinary::~SecsItemBinary() {
	delete[]Item;
	Len = 0;
	Item = NULL;
}

BOOL  SecsItemBoolean::AddItem(SecsItem* pItem) {
	return FALSE;
}

void  SecsItemBoolean::PrintBinary(BYTE* pByte) {
	BYTE nNumBytes = GetBytesReqd(1);
	pByte[0] = MakeDataByte(eType, nNumBytes);;
	MakeDataLength(nNumBytes, &(pByte[1]), 1);
	pByte[nNumBytes + 1] = (Item == TRUE) ? 0x80 : 0;
}

void  SecsItemBoolean::RawCopy(BYTE* pByte) {
	pByte[0] = (Item == TRUE) ? 0x80 : 0;
}

void  SecsItemBoolean::GetData(string& Data) {
	Data = ((Item == TRUE) ? ("true") : ("false"));
}

void  SecsItemBoolean::PrintASCII() {
	string Data = "\n{ Boolean ";
	Data += (Item == TRUE) ? "true\r\n" : "false\r\n";
	Data += " }";
}

void  SecsItemBoolean::PrintSML() {
	string Data;
	for (int nSpace = 0; nSpace < MsgLevel; nSpace++) {
		Data += "  ";
	}
	Data += "< Boolean ";
	Data += (Item == TRUE) ? "true " : "false ";
	Data += " >\r\n";
}

void  SecsItemBoolean::PrintSML(string& Data) {
	for (int nSpace = 0; nSpace < MsgLevel; nSpace++) {
		Data += Sep;
	}
	Data += "< Boolean ";
	Data += (Item == TRUE) ? "true " : "false ";
	Data += " >\r\n";
}

int	SecsItemBoolean::GetLength() {
	return 1;
}

SecsItemBoolean::SecsItemBoolean(BOOL pByte) {
	eType = ITEM_BOOLEAN;
	Item = pByte;
}

int SecsItemBoolean::GetSECSLength() {
	return 1 + GetBytesReqd(1) + 1;
}

SecsItemBoolean::~SecsItemBoolean() {}

BOOL  SecsItemASCII::AddItem(SecsItem* pItem) {
	return FALSE;
}

void  SecsItemASCII::PrintBinary(BYTE* pByte) {
	int Len = Item.size();
	BYTE nNumBytes = GetBytesReqd(Len);
	pByte[0] = MakeDataByte(eType, nNumBytes);;
	MakeDataLength(nNumBytes, &(pByte[1]), Len);
	memcpy(&(pByte[nNumBytes + 1]), Item.c_str(), Len);
}

void  SecsItemASCII::RawCopy(BYTE* pByte) {
	memcpy(pByte, Item.c_str(), Item.size());
}

void  SecsItemASCII::PrintASCII() {
	string Data = "\n { ASCII ";
	Data += Item;
	Data += " }";
}

void  SecsItemASCII::PrintSML() {
	string Data;
	for (int nSpace = 0; nSpace < MsgLevel; nSpace++) {
		Data += "  ";
	}
	Data += "< A \"";
	Data += Item;
	Data += "\" >\r\n";
}

void  SecsItemASCII::PrintSML(string& Data) {
	for (int nSpace = 0; nSpace < MsgLevel; nSpace++) {
		Data += Sep;
	}
	Data += "< A \"";
	Data += Item;
	Data += "\" >\r\n";
}

int	  SecsItemASCII::GetLength() {
	return Item.size();
}

SecsItemASCII::SecsItemASCII(const string& Item) {
	eType = ITEM_ASCII;
	ASSERT(Item.size() < pow(2.0f, 23));
	this->Item = Item;
}

SecsItemASCII::SecsItemASCII(const float& Item) {
	eType = ITEM_ASCII;
	CString str;
	str.Format("%.6f", Item);
	ASSERT(str.GetLength() < pow(2.0f, 23));
	this->Item = str;
}

SecsItemASCII::SecsItemASCII(const int& Item) {
	eType = ITEM_ASCII;
	CString str;
	str.Format("%d", Item);
	ASSERT(str.GetLength() < pow(2.0f, 23));
	this->Item = str;
}

int SecsItemASCII::GetSECSLength() {
	int nSize = Item.size();
	return 1 + GetBytesReqd(nSize) + nSize;
}

void  SecsItemASCII::GetData(string& Data) {
	Data = Item;
}

SecsItemASCII::~SecsItemASCII() {}

BOOL  SecsItemInt8::AddItem(SecsItem* pItem) {
	return FALSE;
}

void  SecsItemInt8::PrintBinary(BYTE* pByte) {
	int Len = 8;
	__int64 Dummy = Item;
	BYTE nNumBytes = GetBytesReqd(Len);
	pByte[0] = MakeDataByte(eType, nNumBytes);;
	MakeDataLength(nNumBytes, &(pByte[1]), Len);
	ByteSwap(&Dummy, Len);
	memcpy(&(pByte[nNumBytes + 1]), &Dummy, Len);
}

void  SecsItemInt8::RawCopy(BYTE* pByte) {
	int Len = 8;
	__int64 Dummy = Item;
	ByteSwap(&Dummy, Len);
	memcpy(pByte, &Dummy, Len);
}

void  SecsItemInt8::PrintASCII() {
	string Data = "\n { Int8 ";
	char Buf[52];
	sprintf(Buf, "%lld", Item);
	Data += Buf;
	Data += " }";
}

void  SecsItemInt8::PrintSML() {
	string Data;
	for (int nSpace = 0; nSpace < MsgLevel; nSpace++) {
		Data += "  ";
	}
	Data += "< I8 ";
	char Buf[52];
	sprintf(Buf, "%lld", Item);
	Data += Buf;
	Data += " >\r\n";
}

void  SecsItemInt8::PrintSML(string& Data) {
	for (int nSpace = 0; nSpace < MsgLevel; nSpace++) {
		Data += Sep;
	}
	Data += "< I8 ";
	char Buf[52];
	sprintf(Buf, "%lld", Item);
	Data += Buf;
	Data += " >\r\n";
}

int	  SecsItemInt8::GetLength() {
	return 8;
}

SecsItemInt8::SecsItemInt8(__int64 Item) {
	eType = ITEM_INT8;
	this->Item = Item;
}

int SecsItemInt8::GetSECSLength() {
	return 10;
}

void  SecsItemInt8::GetData(string& Data) {
	char Buf[52];
	_i64toa(Item, Buf, 10);
	Data = Buf;
}

SecsItemInt8::~SecsItemInt8() {}

BOOL  SecsItemInt1::AddItem(SecsItem* pItem) {
	return FALSE;
}

void  SecsItemInt1::PrintBinary(BYTE* pByte) {
	BYTE Len = 1;
	BYTE nNumBytes = /*GetBytesReqd(Len)*/1;
	pByte[0] = MakeDataByte(eType, nNumBytes);;
	pByte[1] = Len;
	pByte[2] = Item;
}

void  SecsItemInt1::RawCopy(BYTE* pByte) {
	pByte[0] = Item;
}

void  SecsItemInt1::PrintASCII() {
	string Data = "\n{ Int 1 Byte";
	char Buf[5];
	sprintf(Buf, " %d", Item);
	Data += " }";
}

void  SecsItemInt1::PrintSML() {
	string Data;
	for (int nSpace = 0; nSpace < MsgLevel; nSpace++) {
		Data += "  ";
	}
	Data += "< I1 ";
	char Buf[5];
	sprintf(Buf, "%d", Item);
	Data += Buf;
	Data += " >\r\n";
}

void  SecsItemInt1::PrintSML(string& Data) {
	for (int nSpace = 0; nSpace < MsgLevel; nSpace++) {
		Data += Sep;
	}
	Data += "< I1 ";
	char Buf[5];
	sprintf(Buf, "%d", Item);
	Data += Buf;
	Data += " >\r\n";
}

void  SecsItemInt1::GetData(string& Data) {
	char Buf[20];
	_itoa(Item, Buf, 10);
	Data = Buf;
}

int	  SecsItemInt1::GetLength() {
	return 1;
}

SecsItemInt1::SecsItemInt1(char Item) {
	eType = ITEM_INT1;
	this->Item = Item;
}

int SecsItemInt1::GetSECSLength() {
	return 3;
}

SecsItemInt1::~SecsItemInt1() {}

BOOL  SecsItemInt2::AddItem(SecsItem* pItem) {
	return FALSE;
}

void  SecsItemInt2::PrintBinary(BYTE* pByte) {
	int Len = 2;
	short Dummy = Item;
	BYTE nNumBytes = GetBytesReqd(Len);
	pByte[0] = MakeDataByte(eType, nNumBytes);;
	MakeDataLength(nNumBytes, &(pByte[1]), Len);
	ByteSwap(&Dummy, Len);
	memcpy(&(pByte[nNumBytes + 1]), &Dummy, Len);
}

void  SecsItemInt2::RawCopy(BYTE* pByte) {
	int Len = 2;
	short Dummy = Item;
	ByteSwap(&Dummy, Len);
	memcpy(pByte, &Dummy, Len);
}

void  SecsItemInt2::PrintASCII() {
	string Data = "\n{ Int 2 Byte";
	char Buf[5];
	sprintf(Buf, " %d", Item);
	Data += " }";
}

void  SecsItemInt2::PrintSML() {
	string Data;
	for (int nSpace = 0; nSpace < MsgLevel; nSpace++) {
		Data += "  ";
	}
	Data += "< I2 ";
	char Buf[25];
	sprintf(Buf, " %d", Item);
	Data += Buf;
	Data += " >\r\n";
}

void  SecsItemInt2::PrintSML(string& Data) {
	for (int nSpace = 0; nSpace < MsgLevel; nSpace++) {
		Data += Sep;
	}
	Data += "< I2 ";
	char Buf[25];
	sprintf(Buf, " %d", Item);
	Data += Buf;
	Data += " >\r\n";
}

void  SecsItemInt2::GetData(string& Data) {
	char Buf[20];
	_itoa(Item, Buf, 10);
	Data = Buf;
}

int	  SecsItemInt2::GetLength() {
	return 2;
}

int SecsItemInt2::GetSECSLength() {
	return 4;
}

SecsItemInt2::SecsItemInt2(short Item) {
	eType = ITEM_INT2;
	this->Item = Item;
}

SecsItemInt2::~SecsItemInt2() {}

BOOL  SecsItemInt4::AddItem(SecsItem* pItem) {
	return FALSE;
}

void  SecsItemInt4::PrintBinary(BYTE* pByte) {
	int Len = sizeof(Item);
	int Dummy = Item;
	BYTE nNumBytes = GetBytesReqd(Len);
	pByte[0] = MakeDataByte(eType, nNumBytes);;
	MakeDataLength(nNumBytes, &(pByte[1]), Len);
	ByteSwap(&Dummy, Len);
	memcpy(&(pByte[nNumBytes + 1]), &Dummy, Len);
}

void  SecsItemInt4::RawCopy(BYTE* pByte) {
	int Len = sizeof(Item);
	int Dummy = Item;
	ByteSwap(&Dummy, Len);
	memcpy(pByte, &Dummy, Len);
}

void  SecsItemInt4::PrintASCII() {
	string Data = "\n{ Int 4 Byte";
	char Buf[5];
	sprintf(Buf, " %d", Item);
	Data += " }";
}

void  SecsItemInt4::PrintSML() {
	string Data;
	for (int nSpace = 0; nSpace < MsgLevel; nSpace++) {
		Data += Sep;
	}
	Data += "< I4 ";
	char Buf[25];
	sprintf(Buf, "%d", Item);
	Data += Buf;
	Data += " >\r\n";
}

void  SecsItemInt4::PrintSML(string& Data) {
	for (int nSpace = 0; nSpace < MsgLevel; nSpace++) {
		Data += Sep;
	}
	Data += "< I4 ";
	char Buf[25];
	sprintf(Buf, "%d", Item);
	Data += Buf;
	Data += " >\r\n";
}

int	  SecsItemInt4::GetLength() {
	return 4;
}

SecsItemInt4::SecsItemInt4(int Item) {
	eType = ITEM_INT4;
	this->Item = Item;
}

void  SecsItemInt4::GetData(string& Data) {
	char Buf[20];
	_itoa(Item, Buf, 10);
	Data = Buf;
}

int SecsItemInt4::GetSECSLength() {
	return 6;
}

SecsItemInt4::~SecsItemInt4() {}

BOOL  SecsItemUInt8::AddItem(SecsItem* pItem) {
	return FALSE;
}

void  SecsItemUInt8::PrintBinary(BYTE* pByte) {
	int Len = sizeof(Item);
	UINT64 Dummy = Item;
	BYTE nNumBytes = GetBytesReqd(Len);
	pByte[0] = MakeDataByte(eType, nNumBytes);;
	MakeDataLength(nNumBytes, &(pByte[1]), Len);
	ByteSwap(&Dummy, Len);
	memcpy(&(pByte[nNumBytes + 1]), &Dummy, Len);
}

void  SecsItemUInt8::RawCopy(BYTE* pByte) {
	int Len = sizeof(Item);
	UINT64 Dummy = Item;
	ByteSwap(&Dummy, Len);
	memcpy(pByte, &Dummy, Len);
}

void  SecsItemUInt8::PrintASCII() {
	string Data = "\n{ Unsigned Int 8 Byte";
	char Buf[5];
	sprintf(Buf, " %lld", Item);
	Data += " }";
}

void  SecsItemUInt8::GetData(string& Data) {
	char Buf[52];
	_ui64toa(Item, Buf, 10);
	Data = Buf;
}

void  SecsItemUInt8::PrintSML() {
	string Data;
	for (int nSpace = 0; nSpace < MsgLevel; nSpace++) {
		Data += Sep;
	}
	Data += "< U8 ";
	char Buf[52];
	sprintf(Buf, " %lld", Item);
	Data += Buf;
	Data += " >\r\n";
}

void  SecsItemUInt8::PrintSML(string& Data) {
	for (int nSpace = 0; nSpace < MsgLevel; nSpace++) {
		Data += Sep;
	}
	Data += "< U8 ";
	char Buf[52];
	sprintf(Buf, " %lld", Item);
	Data += Buf;
	Data += " >\r\n";
}

int	SecsItemUInt8::GetLength() {
	return 8;
}

SecsItemUInt8::SecsItemUInt8(UINT64 Item) {
	eType = ITEM_UINT8;
	this->Item = Item;
}

int SecsItemUInt8::GetSECSLength() {
	return 10;
}

SecsItemUInt8::~SecsItemUInt8() {}

BOOL  SecsItemUInt1::AddItem(SecsItem* pItem) {
	return FALSE;
}

void  SecsItemUInt1::PrintBinary(BYTE* pByte) {
	pByte[0] = MakeDataByte(eType, 1);;
	pByte[1] = 1;
	pByte[2] = Item;
}

void  SecsItemUInt1::RawCopy(BYTE* pByte) {
	pByte[0] = Item;
}

void  SecsItemUInt1::PrintASCII() {
	string Data = "\n{ Unsigned Int 1 Byte";
	char Buf[5];
	sprintf(Buf, " %d", Item);
	Data += " }";
}

void  SecsItemUInt1::GetData(string& Data) {
	char Buf[52];
	_itoa(Item, Buf, 10);
	Data = Buf;
}

void  SecsItemUInt1::PrintSML() {
	string Data;
	for (int nSpace = 0; nSpace < MsgLevel; nSpace++) {
		Data += Sep;
	}
	Data += "< U1 ";
	char Buf[6];
	sprintf(Buf, "%u", Item);
	Data += Buf;
	Data += " >\r\n";
}

void  SecsItemUInt1::PrintSML(string& Data) {
	for (int nSpace = 0; nSpace < MsgLevel; nSpace++) {
		Data += Sep;
	}
	Data += "< U1 ";
	char Buf[6];
	sprintf(Buf, "%u", Item);
	Data += Buf;
	Data += " >\r\n";
}

int	  SecsItemUInt1::GetLength() {
	return 1;
}

int SecsItemUInt1::GetSECSLength() {
	return 3;
}

SecsItemUInt1::SecsItemUInt1(UCHAR Item) {
	eType = ITEM_UINT1;
	this->Item = Item;
}

SecsItemUInt1::~SecsItemUInt1() {}

BOOL  SecsItemUInt2::AddItem(SecsItem* pItem) {
	return FALSE;
}

void  SecsItemUInt2::PrintBinary(BYTE* pByte) {
	int Len = sizeof(Item);
	USHORT Dummy = Item;
	BYTE nNumBytes = GetBytesReqd(Len);
	pByte[0] = MakeDataByte(eType, nNumBytes);;
	MakeDataLength(nNumBytes, &(pByte[1]), Len);
	ByteSwap(&Dummy, Len);
	memcpy(&(pByte[nNumBytes + 1]), &Dummy, Len);
}

void  SecsItemUInt2::RawCopy(BYTE* pByte) {
	int Len = sizeof(Item);
	USHORT Dummy = Item;
	ByteSwap(&Dummy, Len);
	memcpy(pByte, &Dummy, Len);
}

void  SecsItemUInt2::PrintASCII() {
	string Data = "\n{ Unsigned Int 2 Byte";
	char Buf[5];
	sprintf(Buf, " %u", Item);
	Data += " }";
}

void  SecsItemUInt2::PrintSML() {
	string Data;
	for (int nSpace = 0; nSpace < MsgLevel; nSpace++) {
		Data += "  ";
	}
	Data += "< U2 ";
	char Buf[24];
	sprintf(Buf, "%u", Item);
	Data += Buf;
	Data += " >\r\n";
}

void  SecsItemUInt2::PrintSML(string& Data) {
	for (int nSpace = 0; nSpace < MsgLevel; nSpace++) {
		Data += Sep;
	}
	Data += "< U2 ";
	char Buf[24];
	sprintf(Buf, "%u", Item);
	Data += Buf;
	Data += " >\r\n";
}

int	  SecsItemUInt2::GetLength() {
	return 2;
}

void  SecsItemUInt2::GetData(string& Data) {
	char Buf[52];
	_itoa(Item, Buf, 10);
	Data = Buf;
}

int SecsItemUInt2::GetSECSLength() {
	return 4;
}

SecsItemUInt2::SecsItemUInt2(USHORT Item) {
	eType = ITEM_UINT2;
	this->Item = Item;
}

SecsItemUInt2::~SecsItemUInt2() {}

BOOL  SecsItemUInt4::AddItem(SecsItem* pItem) {
	return FALSE;
}

void  SecsItemUInt4::PrintBinary(BYTE* pByte) {
	int Len = sizeof(Item);
	UINT Dummy = Item;
	BYTE nNumBytes = GetBytesReqd(Len);
	pByte[0] = MakeDataByte(eType, nNumBytes);;
	MakeDataLength(nNumBytes, &(pByte[1]), Len);
	ByteSwap(&Dummy, Len);
	memcpy(&(pByte[nNumBytes + 1]), &Dummy, Len);
}

void  SecsItemUInt4::RawCopy(BYTE* pByte) {
	int Len = sizeof(Item);
	UINT Dummy = Item;
	ByteSwap(&Dummy, Len);
	memcpy(pByte, &Dummy, Len);
}

void  SecsItemUInt4::PrintASCII() {
	string Data = "\n{ Unsigned Int 4 Byte";
	char Buf[5];
	sprintf(Buf, " %u", Item);
	Data += " }";
}
void  SecsItemUInt4::PrintSML() {
	string Data;
	for (int nSpace = 0; nSpace < MsgLevel; nSpace++) {
		Data += "  ";
	}
	Data += "< U4 ";
	char Buf[24];
	sprintf(Buf, " %u", Item);
	Data += Buf;
	Data += " >\r\n";
}
void  SecsItemUInt4::PrintSML(string& Data) {
	for (int nSpace = 0; nSpace < MsgLevel; nSpace++) {
		Data += Sep;
	}
	Data += "< U4 ";
	char Buf[24];
	sprintf(Buf, " %u", Item);
	Data += Buf;
	Data += " >\r\n";
}

void  SecsItemUInt4::GetData(string& Data) {
	char Buf[52];
	_itoa(Item, Buf, 10);
	Data = Buf;
}

int	  SecsItemUInt4::GetLength() {
	return 4;
}

int SecsItemUInt4::GetSECSLength() {
	return GetLength() + 2;
}

SecsItemUInt4::SecsItemUInt4(UINT Item) {
	eType = ITEM_UINT4;
	this->Item = Item;
}

SecsItemUInt4::~SecsItemUInt4() {}

BOOL  SecsItemFloat4::AddItem(SecsItem* pItem) {
	return FALSE;
}

void  SecsItemFloat4::PrintBinary(BYTE* pByte) {
	int Len = sizeof(Item);
	float Dummy = Item;
	BYTE nNumBytes = GetBytesReqd(Len);
	pByte[0] = MakeDataByte(eType, nNumBytes);;
	MakeDataLength(nNumBytes, &(pByte[1]), Len);
	ByteSwap(&Dummy, Len);
	memcpy(&(pByte[nNumBytes + 1]), &Dummy, Len);
}

void  SecsItemFloat4::RawCopy(BYTE* pByte) {
	int Len = sizeof(Item);
	float Dummy = Item;
	ByteSwap(&Dummy, Len);
	memcpy(pByte, &Dummy, Len);
}

void  SecsItemFloat4::PrintASCII() {
	string Data = "\n{ 4 byte float";
	char Buf[5];
	sprintf(Buf, " %f", Item);
	Data += " }";
}

void  SecsItemFloat4::GetData(string& Data) {
	char Buf[112];
	// YFL:  [4/7/2005]
	//_gcvt(Item,100,Buf);
	sprintf(Buf, "%.3f", Item);

	Data = Buf;
}

void  SecsItemFloat4::PrintSML() {
	string Data;
	for (int nSpace = 0; nSpace < MsgLevel; nSpace++) {
		Data += "  ";
	}
	Data += "< F4 ";
	char Buf[48];
	sprintf(Buf, "%f", Item);
	Data += Buf;
	Data += " >\r\n";
}

void  SecsItemFloat4::PrintSML(string& Data) {
	for (int nSpace = 0; nSpace < MsgLevel; nSpace++) {
		Data += Sep;
	}
	Data += "< F4 ";
	char Buf[48];
	sprintf(Buf, "%f", Item);
	Data += Buf;
	Data += " >\r\n";
}

int	  SecsItemFloat4::GetLength() {
	return 4;
}

int SecsItemFloat4::GetSECSLength() {
	return GetLength() + 2;
}

SecsItemFloat4::SecsItemFloat4(float Item) {
	eType = ITEM_FLOAT4;
	this->Item = Item;
}

SecsItemFloat4::~SecsItemFloat4() {}

BOOL  SecsItemFloat8::AddItem(SecsItem* pItem) {
	return FALSE;
}

void  SecsItemFloat8::PrintBinary(BYTE* pByte) {
	int Len = sizeof(Item);
	double Dummy = Item;
	BYTE nNumBytes = GetBytesReqd(Len);
	pByte[0] = MakeDataByte(eType, nNumBytes);;
	MakeDataLength(nNumBytes, &(pByte[1]), Len);
	ByteSwap(&Dummy, Len);
	memcpy(&(pByte[nNumBytes + 1]), &Dummy, Len);
}

void  SecsItemFloat8::RawCopy(BYTE* pByte) {
	int Len = sizeof(Item);
	double Dummy = Item;
	ByteSwap(&Dummy, Len);
	memcpy(pByte, &Dummy, Len);
}

void  SecsItemFloat8::PrintASCII() {
	string Data = "\n{ 8 byte float";
	char Buf[5];
	sprintf(Buf, " %f", Item);
	Data += " }";
}

void  SecsItemFloat8::PrintSML() {
	string Data;
	for (int nSpace = 0; nSpace < MsgLevel; nSpace++) {
		Data += "  ";
	}
	Data += "< F8 ";
	char Buf[96];
	sprintf(Buf, "%f", Item);
	Data += Buf;
	Data += " >\r\n";
}

void  SecsItemFloat8::PrintSML(string& Data) {
	for (int nSpace = 0; nSpace < MsgLevel; nSpace++) {
		Data += Sep;
	}
	Data += "< F8 ";
	char Buf[96];
	sprintf(Buf, "%f", Item);
	Data += Buf;
	Data += " >\r\n";
}

int	  SecsItemFloat8::GetLength() {
	return 8;
}

void  SecsItemFloat8::GetData(string& Data) {
	char Buf[256];
	_gcvt(Item, 225, Buf);
	Data = Buf;
}

int SecsItemFloat8::GetSECSLength() {
	return GetLength() + 2;
}

SecsItemFloat8::SecsItemFloat8(double Item) {
	eType = ITEM_FLOAT8;
	this->Item = Item;
}

SecsItemFloat8::~SecsItemFloat8() {}

SecsItem* SecsItemList::ContainedItems(int nIndex) {
	if (nIndex < int(Item.size())) {
		return Item[nIndex];
	}
	else {
		return NULL;
	}
}

SecsItem* SecsItemASCII::CloneSelf() {
	return new SecsItemASCII(Item);
}

SecsItem* SecsItemBinary::CloneSelf() {
	return new SecsItemBinary(Item, Len);
}

SecsItem* SecsItemBoolean::CloneSelf() {
	return new SecsItemBoolean(Item);
}

SecsItem* SecsItemFloat4::CloneSelf() {
	return new SecsItemFloat4(Item);
}
SecsItem* SecsItemFloat8::CloneSelf() {
	return new SecsItemFloat8(Item);
}

SecsItem* SecsItemInt1::CloneSelf() {
	return new SecsItemInt1(Item);
}
SecsItem* SecsItemInt2::CloneSelf() {
	return new SecsItemInt2(Item);
}
SecsItem* SecsItemInt4::CloneSelf() {
	return new SecsItemInt4(Item);
}
SecsItem* SecsItemInt8::CloneSelf() {
	return new SecsItemInt8(Item);
}

SecsItem* SecsItemList::CloneSelf() {
	SecsItemList* pClone = new SecsItemList;
	for (int nIndex = 0; nIndex < size(); nIndex++) {
		pClone->AddItem(Item[nIndex]->CloneSelf());
	}
	return pClone;
}

SecsItem* SecsItemUInt1::CloneSelf() {
	return new SecsItemUInt1(Item);
}

SecsItem* SecsItemUInt2::CloneSelf() {
	return new SecsItemUInt2(Item);
}
SecsItem* SecsItemUInt4::CloneSelf() {
	return new SecsItemUInt4(Item);
}
SecsItem* SecsItemUInt8::CloneSelf() {
	return new SecsItemUInt8(Item);
}

SecsItem* SecsItemUndefined::CloneSelf() {
	return new SecsItemUndefined;
}

SecsItemArray::SecsItemArray(vector< SecsItemUInt1* >& SecsItems) {
	ASSERT(SecsItems.size() > 1);
	eType = ITEM_ARRAY;
	for (UINT nIndex = 0; nIndex < SecsItems.size(); nIndex++) {
		Item.push_back(SecsItems[nIndex]);
	}
}

SecsItemArray::SecsItemArray(vector< SecsItemUInt2* >& SecsItems) {
	ASSERT(SecsItems.size() > 1);
	eType = ITEM_ARRAY;
	for (UINT nIndex = 0; nIndex < SecsItems.size(); nIndex++) {
		Item.push_back(SecsItems[nIndex]);
	}
}

SecsItemArray::SecsItemArray(vector< SecsItemUInt4* >& SecsItems) {
	ASSERT(SecsItems.size() > 1);
	eType = ITEM_ARRAY;
	for (UINT nIndex = 0; nIndex < SecsItems.size(); nIndex++) {
		Item.push_back(SecsItems[nIndex]);
	}
}
SecsItemArray::SecsItemArray(vector< SecsItemUInt8* >& SecsItems) {
	ASSERT(SecsItems.size() > 1);
	eType = ITEM_ARRAY;
	for (UINT nIndex = 0; nIndex < SecsItems.size(); nIndex++) {
		Item.push_back(SecsItems[nIndex]);
	}
}
SecsItemArray::SecsItemArray(vector< SecsItemInt1* >& SecsItems) {
	ASSERT(SecsItems.size() > 1);
	eType = ITEM_ARRAY;
	for (UINT nIndex = 0; nIndex < SecsItems.size(); nIndex++) {
		Item.push_back(SecsItems[nIndex]);
	}
}
SecsItemArray::SecsItemArray(vector< SecsItemInt2* >& SecsItems) {
	ASSERT(SecsItems.size() > 1);
	eType = ITEM_ARRAY;
	for (UINT nIndex = 0; nIndex < SecsItems.size(); nIndex++) {
		Item.push_back(SecsItems[nIndex]);
	}
}

SecsItemArray::SecsItemArray(vector< SecsItemInt4* >& SecsItems) {
	ASSERT(SecsItems.size() > 1);
	eType = ITEM_ARRAY;
	for (UINT nIndex = 0; nIndex < SecsItems.size(); nIndex++) {
		Item.push_back(SecsItems[nIndex]);
	}
}

SecsItemArray::SecsItemArray(vector< SecsItemInt8* >& SecsItems) {
	ASSERT(SecsItems.size() > 1);
	eType = ITEM_ARRAY;
	for (UINT nIndex = 0; nIndex < SecsItems.size(); nIndex++) {
		Item.push_back(SecsItems[nIndex]);
	}
}

SecsItemArray::SecsItemArray(vector< SecsItemBoolean* >& SecsItems) {
	ASSERT(SecsItems.size() > 1);
	eType = ITEM_ARRAY;
	for (UINT nIndex = 0; nIndex < SecsItems.size(); nIndex++) {
		Item.push_back(SecsItems[nIndex]);
	}
}

SecsItemArray::SecsItemArray(vector< SecsItemFloat4* >& SecsItems) {
	ASSERT(SecsItems.size() > 1);
	eType = ITEM_ARRAY;
	for (UINT nIndex = 0; nIndex < SecsItems.size(); nIndex++) {
		Item.push_back(SecsItems[nIndex]);
	}
}

SecsItemArray::SecsItemArray(vector< SecsItemFloat8* >& SecsItems) {
	ASSERT(SecsItems.size() > 1);
	eType = ITEM_ARRAY;
	for (UINT nIndex = 0; nIndex < SecsItems.size(); nIndex++) {
		Item.push_back(SecsItems[nIndex]);
	}
}

SecsItemArray::SecsItemArray() {
	eType = ITEM_ARRAY;
}

SecsItem* SecsItemArray::CloneSelf() {
	SecsItemList* pClone = new SecsItemArray;
	for (int nIndex = 0; nIndex < size(); nIndex++) {
		pClone->AddItem(Item[nIndex]->CloneSelf());
	}
	return pClone;
}

void  SecsItemArray::PrintASCII() {
	string Data = "\n{ Array of : ";
	string Temp;
	Item[0]->TypeString(Temp);
	Data += Temp;
	for (UINT nIndex = 0; nIndex < Item.size(); nIndex++) {
		Item[nIndex]->GetData(Temp);
		Data += " ";
		Data += Temp;
	}
	Data += " \r\n";
	Data += " }";
}

int SecsItemArray::GetSECSLength() {
	UINT nSize = (Item.size()) * Item[0]->GetLength();
	return nSize + GetBytesReqd(nSize) + 1;
}

int SecsItemArray::GetLength() {
	return (Item.size()) * Item[0]->GetLength();
}

void  SecsItemArray::PrintBinary(BYTE* pByte) {
	ASSERT(Item.size() > 1);
	UINT nSize = (Item.size()) * Item[0]->GetLength();
	BYTE nNumBytes = GetBytesReqd(nSize);
	pByte[0] = MakeDataByte(Item[0]->GetType(), nNumBytes);
	MakeDataLength(nNumBytes, &(pByte[1]), nSize);
	UINT ByteWritten = 1 + nNumBytes;
	UINT DataLen = 0;
	pByte += ByteWritten;
	for (UINT nIndex = 0; nIndex < Item.size(); nIndex++) {
		DataLen = Item[nIndex]->GetLength();
		Item[nIndex]->RawCopy(pByte);
		pByte += DataLen;
	}
}

void  SecsItemArray::PrintSML() {
	int nSize = this->Item.size();
	for (int nIndex = 0; nIndex < nSize; nIndex++) {
		Item[nIndex]->PrintSML();
	}
}

void  SecsItemArray::PrintSML(string& Data) {
	string Temp;
	for (int nSpace = 0; nSpace < MsgLevel; nSpace++) {
		Data += Sep;
	}
	Item[0]->PrintSML(Temp);
	int nTemp = Temp.rfind('>');
	if (nTemp != string::npos) {
		Temp = Temp.substr(0, nTemp);
	}
	Data += Temp;
	Data += " ";
	for (int nIndex = 1; nIndex < size(); nIndex++) {
		Item[nIndex]->GetData(Temp);
		Data += Temp;
		Data += "  ";
	}
	Data += (">\r\n");
}

void  SecsItemArray::GetData(string& Data) {
	PrintSML(Data);
}

SecsItemArray::~SecsItemArray() {}

BOOL SecsItem::TypeString(string& TypStr) {
	switch (eType) {
	case UNDEFINED:
		TypStr = "Undefined";
		break;
	case ITEM_LIST:
		TypStr = "List";
		break;
	case ITEM_ARRAY:
		TypStr = "Array";
		break;
	case ITEM_BINARY:
		TypStr = "Binary";
		break;
	case ITEM_BOOLEAN:
		TypStr = "Boolean";
		break;
	case ITEM_ASCII:
		TypStr = "ASCII";
		break;
	case ITEM_JIS:
		TypStr = "JIS";
		break;
	case ITEM_UNICODE:
		TypStr = "Unicode";
		break;
	case ITEM_INT8:
		TypStr = "Integer 8 Byte";
		break;
	case ITEM_INT1:
		TypStr = "Integer 4 Byte";
		break;
	case ITEM_INT2:
		TypStr = "Integer 2 Byte";
		break;
	case ITEM_INT4:
		TypStr = "Integer 1 Byte";
		break;
	case ITEM_FLOAT8:
		TypStr = "Float 8 Byte";
		break;
	case ITEM_FLOAT4:
		TypStr = "Float 4 Byte";
		break;
	case ITEM_UINT8:
		TypStr = "Unsigned Integer 8 Byte";
		break;
	case ITEM_UINT1:
		TypStr = "Unsigned Integer 1 Byte";
		break;
	case ITEM_UINT2:
		TypStr = "Unsigned Integer 2 Byte";
		break;
	case ITEM_UINT4:
		TypStr = "Unsigned Integer 4 Byte";
		break;
	default:
		TypStr = "<Unknown Type >";
		return FALSE;
	}
	return TRUE;
}

BOOL SecsItem::IsZeroLen() {
	return FALSE;
}

SecsItem* SecsItemZeroLen::CloneSelf() {
	return new SecsItemZeroLen(eType);
}
BOOL  SecsItemZeroLen::IsZeroLen() {
	return TRUE;
}
BOOL  SecsItemZeroLen::AddItem(SecsItem* pItem) {
	return FALSE;
}

void  SecsItemZeroLen::PrintBinary(BYTE* pByte) {
	BYTE nNumBytes = GetBytesReqd(0);
	pByte[0] = MakeDataByte(eType, nNumBytes);;
	MakeDataLength(nNumBytes, &(pByte[1]), 0);
}

void  SecsItemZeroLen::PrintASCII() {
	string Data = "\n{ Zero Length data ";
	Data += TypeStr();
	Data += " }";
}

void  SecsItemZeroLen::PrintSML() {}

void  SecsItemZeroLen::PrintSML(string& Data) {
	for (int nSpace = 0; nSpace < MsgLevel; nSpace++) {
		Data += Sep;
	}
	Data += "< ";
	Data += TypeStrSML();
	Data += " >\r\n";
}

int	  SecsItemZeroLen::GetLength() {
	return 0;
}

int	  SecsItemZeroLen::GetSECSLength() {
	return 1 + GetBytesReqd(0);
}

SecsItemZeroLen::SecsItemZeroLen(DataType Typ) {
	eType = Typ;
}

void  SecsItemZeroLen::GetData(string& Data) {
	Data = "";
}

SecsItemZeroLen::~SecsItemZeroLen() {}

void  SecsItemZeroLen::RawCopy(BYTE*) {}

string SecsItem::TypeStrSML() {
	string TypeStr = "";
	switch (eType) {
	case UNDEFINED:TypeStr = ""; break;
	case ITEM_LIST: TypeStr = "L"; break;
	case ITEM_ARRAY: TypeStr = ""; break;
	case ITEM_BINARY: TypeStr = "B"; break;
	case ITEM_BOOLEAN: TypeStr = "BOOLEAN"; break;
	case ITEM_ASCII: TypeStr = "A"; break;
	case ITEM_JIS: TypeStr = "J"; break;
	case ITEM_UNICODE: TypeStr = "U"; break;
	case ITEM_INT8: TypeStr = "I8"; break;
	case ITEM_INT1: TypeStr = "I1"; break;
	case ITEM_INT2: TypeStr = "I2"; break;
	case ITEM_INT4: TypeStr = "I4"; break;
	case ITEM_FLOAT8: TypeStr = "F8"; break;
	case ITEM_FLOAT4: TypeStr = "F4"; break;
	case ITEM_UINT8: TypeStr = "U8"; break;
	case ITEM_UINT1: TypeStr = "U1"; break;
	case ITEM_UINT2: TypeStr = "U2"; break;
	case ITEM_UINT4: TypeStr = "U4"; break;
	default:
		ASSERT(0);
		TypeStr = "UNKNOWN";
		break;
	}
	return TypeStr;
}

string SecsItem::TypeStr(short type) {
	string TypeStr = "";
	switch (type) {
	case UNDEFINED:TypeStr = "UNDEFINED"; break;
	case ITEM_LIST: TypeStr = "LIST"; break;
	case ITEM_ARRAY: TypeStr = "ARRAY"; break;
	case ITEM_BINARY: TypeStr = "BINARY"; break;
	case ITEM_BOOLEAN: TypeStr = "BOOLEAN"; break;
	case ITEM_ASCII: TypeStr = "ASCII"; break;
	case ITEM_JIS: TypeStr = "JIS"; break;
	case ITEM_UNICODE: TypeStr = "UNICODE"; break;
	case ITEM_INT8: TypeStr = "INT8"; break;
	case ITEM_INT1: TypeStr = "INT1"; break;
	case ITEM_INT2: TypeStr = "INT2"; break;
	case ITEM_INT4: TypeStr = "INT4"; break;
	case ITEM_FLOAT8: TypeStr = "FLOAT8"; break;
	case ITEM_FLOAT4: TypeStr = "FLOAT4"; break;
	case ITEM_UINT8: TypeStr = "UINT8"; break;
	case ITEM_UINT1: TypeStr = "UINT1"; break;
	case ITEM_UINT2: TypeStr = "UINT2"; break;
	case ITEM_UINT4: TypeStr = "UINT4"; break;
	default:
		ASSERT(0);
		TypeStr = "UNKNOWN";
		break;
	}
	return TypeStr;
}

string SecsItem::TypeStr() {
	return TypeStr(eType);
}

BOOL SecsItem::IsCollection() {
	return (IsArray() || IsList());
}

BOOL SecsItem::GetAInt(UINT& UVal) {
	bool bRet = false;
	if ((IsAInt()) && (!IsZeroLen())) {
		string Data;
		GetData(Data);
		UVal = atoi(Data.c_str());
		bRet = true;
	}
	return bRet;
}