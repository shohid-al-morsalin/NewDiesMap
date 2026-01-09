// SecsMessage.cpp: implementation of the SecsMessage class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "SecsMessage.h"
#include "..\INC\E5-ErrCodes.h"

#include "GlobalDeviceContainer.h"

using namespace std;
#pragma warning(disable:4355)

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;

#define new DEBUG_NEW
#endif
/*
 *	Static helper functions intended to be used only in this file
 */

BOOL bIgnorRecipeParameter; //  [9/6/2007 LYF]
static 	BOOL ExtractRecipeParams(SecsItem& Param, PrjobRepNew& PrJob);

/*
 *	End static helpers
 */
 //////////////////////////////////////////////////////////////////////
 // Construction/Destruction
 //////////////////////////////////////////////////////////////////////
SecsMessage::SecsMessage() : MsgRef(*this) {
	bDirty = TRUE;
}

SecsMessage::~SecsMessage() {
	for (UINT nIndex = 0; nIndex < Items.size(); nIndex++) {
		delete Items[nIndex];
	}
	Items.clear();
	bDirty = TRUE;
}

/*
	The type of the item is checked the type is returned and also the
	number of length bytes are decoded.
	Valid values
		return values 0....n Error Value -1
		The parameter NumLenBytes valid values Min 0 Max 3 No invalid values !

*/

BYTE SecsMessage::TypeOf(BYTE bCheck, int& NumLenBytes) {
	NumLenBytes = bCheck & 3;
	return BYTE(bCheck >> 2);
}

/*
		The length of  the item is determined from the number of bytes

*/

int SecsMessage::GetItemLen(BYTE* pByte, int nNumberOfBytes) {
	// Check number of bytes
	ASSERT((nNumberOfBytes < 4) && (nNumberOfBytes >= 0));
	int Len = 0;
	int mid = nNumberOfBytes / 2;
	BYTE Temp;
	for (int nIndex = 0; nIndex < mid; nIndex++) {
		Temp = pByte[nNumberOfBytes - 1 - nIndex];
		pByte[nNumberOfBytes - 1 - nIndex] = pByte[nIndex];
		pByte[nIndex] = Temp;
	}
	memcpy(&Len, pByte, nNumberOfBytes);
	return Len;
}

BOOL SecsMessage::DecodeMessage(BYTE* pByte, int MessageLen) {
	Items.clear();
	int nNumLenBytes = 0;
	int DataLen = 0;
	BYTE Type;
	bDirty = TRUE;
	for (int nIndex = 0; nIndex < MessageLen; ) {
		Type = TypeOf(pByte[nIndex], nNumLenBytes);
		DataLen = GetItemLen(&(pByte[nIndex + 1]), nNumLenBytes);
		switch (Type) {
		case SecsItem::ITEM_LIST:
			Items.push_back(MakeList(&(pByte[nIndex + nNumLenBytes + 1]), DataLen));
			nIndex += DataLen; // Different treatment for List ???
			break;
		case SecsItem::ITEM_BINARY:
			Items.push_back(MakeBinary(&(pByte[nIndex + nNumLenBytes + 1]), DataLen));
			nIndex += (nNumLenBytes + DataLen + 1);
			break;
		case SecsItem::ITEM_BOOLEAN:
			Items.push_back(MakeBoolean(&(pByte[nIndex + nNumLenBytes + 1]), DataLen));
			nIndex += (nNumLenBytes + DataLen + 1);
			break;
		case SecsItem::ITEM_FLOAT4:
			Items.push_back(MakeFloat4(&(pByte[nIndex + nNumLenBytes + 1]), DataLen));
			nIndex += (nNumLenBytes + DataLen + 1);
			break;
		case SecsItem::ITEM_FLOAT8:
			Items.push_back(MakeFloat8(&(pByte[nIndex + nNumLenBytes + 1]), DataLen));
			nIndex += (nNumLenBytes + DataLen + 1);
			break;
		case SecsItem::ITEM_ASCII:
			Items.push_back(MakeASCII(&(pByte[nIndex + nNumLenBytes + 1]), DataLen));
			nIndex += (nNumLenBytes + DataLen + 1);
			break;
		case SecsItem::ITEM_JIS:
			nIndex += (nNumLenBytes + DataLen + 1);
			break;
		case SecsItem::ITEM_UNICODE:
			nIndex += (nNumLenBytes + DataLen + 1);
			break;
		case SecsItem::ITEM_INT8:
		case SecsItem::ITEM_INT4:
		case SecsItem::ITEM_INT2:
		case SecsItem::ITEM_INT1:
			Items.push_back(MakeInt(SecsItem::DataType(Type), &(pByte[nIndex + nNumLenBytes + 1]), DataLen));
			nIndex += (nNumLenBytes + DataLen + 1);
			break;
		case SecsItem::ITEM_UINT8:
		case SecsItem::ITEM_UINT4:
		case SecsItem::ITEM_UINT2:
		case SecsItem::ITEM_UINT1:
			Items.push_back(MakeUInt(SecsItem::DataType(Type), &(pByte[nIndex + nNumLenBytes + 1]), DataLen));
			nIndex += (nNumLenBytes + DataLen + 1);
			break;
		case SecsItem::UNDEFINED:
			Log("Undefined format", 2);
			//ASSERT(0);
			return FALSE;
			break;
		default:
		{
			CString Temp;
			Temp.Format("Unknown SECS Format code{%d} At %d", Type, nIndex);
			Log(Temp, 2);
			//ASSERT(0);
			nIndex = MessageLen + 1;
			return FALSE;
		}
		break;
		}
	}
	bDirty = FALSE;
	return TRUE;
}

SecsItem* SecsMessage::MakeBinary(BYTE* pByte, int nLen) {
	SecsItem* pItem = NULL;
	switch (nLen) {
	case 0:
		pItem = new SecsItemZeroLen(SecsItem::ITEM_BINARY);
		break;
	default:
		pItem = new SecsItemBinary(pByte, nLen);
		break;
	}
	return pItem;
}

SecsItem* SecsMessage::MakeInt(SecsItem::DataType Type, BYTE* pByte, int nLen) {
	SecsItem* pItem = NULL;
	switch (Type) {
	case SecsItem::ITEM_INT8:
		pItem = MakeInt8(pByte, nLen);
		break;
	case SecsItem::ITEM_INT4:
		pItem = MakeInt4(pByte, nLen);
		break;
	case SecsItem::ITEM_INT2:
		pItem = MakeInt2(pByte, nLen);
		break;
	case SecsItem::ITEM_INT1:
		pItem = MakeInt1(pByte, nLen);
		break;
	default:
		ASSERT(0);
		break;
	}
	return pItem;
}

SecsItem* SecsMessage::MakeInt8(BYTE* pByte, int nLen) {
	__int64 Int8;
	SecsItem* pItem = NULL;
	const int I8Len = 8;
	switch (nLen) {
	case 0:
		pItem = new SecsItemZeroLen(SecsItem::ITEM_INT8);
		break;
	case I8Len:
		memcpy(&Int8, pByte, I8Len);
		ByteSwap(&Int8, I8Len);
		pItem = new SecsItemInt8(Int8);
		break;
	default:
	{
		ASSERT((nLen > I8Len) && (0 == nLen % I8Len));
		if ((nLen > I8Len) && (0 == nLen % I8Len)) {
			UINT nItems = nLen / I8Len;
			std::vector<SecsItemInt8*> TempVec;
			for (UINT nIndex = 0; nIndex < nItems; nIndex++) {
				memcpy(&Int8, pByte + (nIndex * I8Len), I8Len);
				ByteSwap(&Int8, I8Len);
				TempVec.push_back(new SecsItemInt8(Int8));
			}
			pItem = new SecsItemArray(TempVec);
		}
	}
	break;
	}
	return pItem;
}

SecsItem* SecsMessage::MakeInt4(BYTE* pByte, int nLen) {
	int Int4;
	SecsItem* pItem = NULL;
	const int I4Len = 4;
	switch (nLen) {
	case 0:
		pItem = new SecsItemZeroLen(SecsItem::ITEM_INT4);
		break;
	case I4Len:
		memcpy(&Int4, pByte, I4Len);
		ByteSwap(&Int4, I4Len);
		pItem = new SecsItemInt4(Int4);
		break;
	default:
	{
		ASSERT((nLen > I4Len) && (0 == nLen % I4Len));
		if ((nLen > I4Len) && (0 == nLen % I4Len)) {
			UINT nItems = nLen / I4Len;
			// Make SecsItemArray Here
			std::vector<SecsItemInt4*> TempVec;
			for (UINT nIndex = 0; nIndex < nItems; nIndex++) {
				memcpy(&Int4, pByte + (nIndex * I4Len), I4Len);
				ByteSwap(&Int4, I4Len);
				TempVec.push_back(new SecsItemInt4(Int4));
			}
			pItem = new SecsItemArray(TempVec);
		}
	}
	break;
	}
	return pItem;
}

SecsItem* SecsMessage::MakeInt2(BYTE* pByte, int nLen) {
	short Int2;
	SecsItem* pItem = NULL;
	const int I2Len = 2;
	typedef SecsItemInt2 I2Type;
	typedef std::vector<I2Type*> I2Vec;
	switch (nLen) {
	case 0:
		pItem = new SecsItemZeroLen(SecsItem::ITEM_INT2);
		break;
	case I2Len:
		memcpy(&Int2, pByte, I2Len);
		ByteSwap(&Int2, I2Len);
		pItem = new I2Type(Int2);
		break;
	default:
	{
		ASSERT((nLen > I2Len) && (0 == nLen % I2Len));
		if ((nLen > I2Len) && (0 == nLen % I2Len)) {
			UINT nItems = nLen / I2Len;
			I2Vec TempVec;
			for (UINT nIndex = 0; nIndex < nItems; nIndex++) {
				memcpy(&Int2, pByte + (nIndex * I2Len), I2Len);
				ByteSwap(&Int2, I2Len);
				TempVec.push_back(new I2Type(Int2));
			}
			pItem = new SecsItemArray(TempVec);
		}
	}
	break;
	}
	return pItem;
}

SecsItem* SecsMessage::MakeInt1(BYTE* pByte, int nLen) {
	char Int1;
	SecsItem* pItem = NULL;
	const int I1Len = 1;
	switch (nLen) {
	case 0:
		pItem = new SecsItemZeroLen(SecsItem::ITEM_INT1);
		break;
	case I1Len:
		Int1 = (char)pByte[0];
		pItem = new SecsItemInt1(Int1);
		break;
	default:
	{
		ASSERT((nLen > I1Len));
		if ((nLen > I1Len)) {
			UINT nItems = nLen;
			std::vector<SecsItemInt1*> TempVec;
			for (UINT nIndex = 0; nIndex < nItems; nIndex++) {
				Int1 = (char)pByte[nIndex];
				TempVec.push_back(new SecsItemInt1(Int1));
			}
			pItem = new SecsItemArray(TempVec);
		}
	}
	break;
	}
	return pItem;
}

SecsItem* SecsMessage::MakeUInt8(BYTE* pByte, int nLen) {
	__int64 UInt8;
	SecsItem* pItem = NULL;
	const int U8Len = 8;
	switch (nLen) {
	case 0:
		pItem = new SecsItemZeroLen(SecsItem::ITEM_UINT8);
		break;
	case U8Len:
		memcpy(&UInt8, pByte, U8Len);
		ByteSwap(&UInt8, U8Len);
		pItem = new SecsItemUInt8(UInt8);
		break;
	default:
	{
		ASSERT((nLen > U8Len) && (0 == nLen % U8Len));
		if ((nLen > U8Len) && (0 == nLen % U8Len)) {
			UINT nItems = nLen / U8Len;
			std::vector<SecsItemUInt8*> TempVec;
			for (UINT nIndex = 0; nIndex < nItems; nIndex++) {
				memcpy(&UInt8, pByte + (nIndex * U8Len), U8Len);
				ByteSwap(&UInt8, U8Len);
				TempVec.push_back(new SecsItemUInt8(UInt8));
			}
			pItem = new SecsItemArray(TempVec);
		}
	}
	break;
	}
	return pItem;
}

SecsItem* SecsMessage::MakeUInt4(BYTE* pByte, int nLen) {
	UINT UInt4;
	SecsItem* pItem = NULL;
	const int U4Len = 4;
	switch (nLen) {
	case 0:
		pItem = new SecsItemZeroLen(SecsItem::ITEM_UINT4);
		break;
	case U4Len:
		memcpy(&UInt4, pByte, U4Len);
		ByteSwap(&UInt4, U4Len);
		pItem = new SecsItemUInt4(UInt4);
		break;
	default:
	{
		ASSERT((nLen > U4Len) && (0 == nLen % U4Len));
		if ((nLen > U4Len) && (0 == nLen % U4Len)) {
			UINT nItems = nLen / U4Len;
			std::vector<SecsItemUInt4*> TempVec;
			for (UINT nIndex = 0; nIndex < nItems; nIndex++) {
				memcpy(&UInt4, pByte + (nIndex * U4Len), U4Len);
				ByteSwap(&UInt4, U4Len);
				TempVec.push_back(new SecsItemUInt4(UInt4));
			}
			pItem = new SecsItemArray(TempVec);
		}
	}
	break;
	}
	return pItem;
}

SecsItem* SecsMessage::MakeUInt2(BYTE* pByte, int nLen) {
	USHORT UInt2;
	const int U2Len = 2;
	SecsItem* pItem = NULL;
	switch (nLen) {
	case 0:
		pItem = new SecsItemZeroLen(SecsItem::ITEM_UINT2);
		break;
	case U2Len:
		memcpy(&UInt2, pByte, U2Len);
		ByteSwap(&UInt2, U2Len);
		pItem = new SecsItemUInt2(UInt2);
		break;
	default:
	{
		ASSERT((nLen > U2Len) && (0 == nLen % U2Len));
		if ((nLen > U2Len) && (0 == nLen % U2Len)) {
			UINT nItems = nLen / U2Len;
			std::vector<SecsItemUInt2*> TempVec;
			for (UINT nIndex = 0; nIndex < nItems; nIndex++) {
				memcpy(&UInt2, pByte + (nIndex * U2Len), U2Len);
				ByteSwap(&UInt2, U2Len);
				TempVec.push_back(new SecsItemUInt2(UInt2));
			}
			pItem = new SecsItemArray(TempVec);
		}
	}
	break;
	}
	return pItem;
}

SecsItem* SecsMessage::MakeUInt1(BYTE* pByte, int nLen) {
	BYTE UInt1;
	SecsItem* pItem = NULL;
	const int U1Len = 1;
	switch (nLen) {
	case 0:
		pItem = new SecsItemZeroLen(SecsItem::ITEM_UINT1);
		break;
	case U1Len:
		UInt1 = pByte[0];
		pItem = new SecsItemUInt1(UInt1);
		break;
	default:
	{
		ASSERT((nLen > U1Len));
		if ((nLen > U1Len)) {
			std::vector<SecsItemUInt1*> TempVec;
			for (int nIndex = 0; nIndex < nLen; nIndex++) {
				UInt1 = pByte[nIndex];
				TempVec.push_back(new SecsItemUInt1(UInt1));
			}
			pItem = new SecsItemArray(TempVec);
		}
	}
	break;
	}
	return pItem;
}

SecsItem* SecsMessage::MakeUInt(SecsItem::DataType Type, BYTE* pByte, int nLen) {
	SecsItem* pItem = NULL;
	switch (Type) {
	case SecsItem::ITEM_UINT8:
		pItem = MakeUInt8(pByte, nLen);
		break;
	case SecsItem::ITEM_UINT4:
		pItem = MakeUInt4(pByte, nLen);
		break;
	case SecsItem::ITEM_UINT2:
		pItem = MakeUInt2(pByte, nLen);
		break;
	case SecsItem::ITEM_UINT1:
		pItem = MakeUInt1(pByte, nLen);
		break;
	default:
		ASSERT(0);
		break;
	}
	return pItem;
}

SecsItem* SecsMessage::MakeASCII(BYTE* pByte, int nLen) {
	char* String = new char[nLen + 1];
	memcpy(String, pByte, nLen);
	String[nLen] = 0;
	SecsItem* pItem = new SecsItemASCII(string(String));
	delete[] String;
	return pItem;
}

SecsItem* SecsMessage::MakeJIS(BYTE* pByte, int nLen) {
	return new SecsItemBinary(pByte, nLen);
}

SecsItem* SecsMessage::MakeUnicode(BYTE* pByte, int nLen) {
	return new SecsItemBinary(pByte, nLen);
}

SecsItem* SecsMessage::MakeFloat8(BYTE* pByte, int nLen) {
	SecsItem* pItem = NULL;
	double Float8;
	const int F8Len = 8;
	switch (nLen) {
	case 0:
		pItem = new SecsItemZeroLen(SecsItem::ITEM_FLOAT8);
		break;
	case F8Len:
		memcpy(&Float8, pByte, F8Len);
		ByteSwap(&Float8, F8Len);
		pItem = new SecsItemFloat8(Float8);
		break;
	default:
	{
		ASSERT((nLen > F8Len) && (0 == nLen % F8Len));
		if ((nLen > F8Len) && (0 == nLen % F8Len)) {
			UINT nItems = nLen / F8Len;
			std::vector<SecsItemFloat8*> F8Vec;
			for (UINT nIndex = 0; nIndex < nItems; nIndex++) {
				memcpy(&Float8, pByte + (nIndex * F8Len), F8Len);
				ByteSwap(&Float8, F8Len);
				F8Vec.push_back(new SecsItemFloat8(Float8));
			}
			pItem = new SecsItemArray(F8Vec);
		}
	}
	break;
	}
	return pItem;
}

SecsItem* SecsMessage::MakeFloat4(BYTE* pByte, int nLen) {
	SecsItem* pItem = NULL;
	float Float4;
	const int F4Len = 4;
	switch (nLen) {
	case 0:
		pItem = new SecsItemZeroLen(SecsItem::ITEM_FLOAT4);
		break;
	case F4Len:
		memcpy(&Float4, pByte, F4Len);
		ByteSwap(&Float4, F4Len);
		pItem = new SecsItemFloat4(Float4);
		break;
	default:
	{
		ASSERT((nLen > F4Len) && (0 == nLen % F4Len));
		if ((nLen > F4Len) && (0 == nLen % F4Len)) {
			UINT nItems = nLen / F4Len;
			std::vector<SecsItemFloat4*> F4Vec;
			for (UINT nIndex = 0; nIndex < nItems; nIndex++) {
				memcpy(&Float4, pByte + (nIndex * F4Len), F4Len);
				ByteSwap(&Float4, F4Len);
				F4Vec.push_back(new SecsItemFloat4(Float4));
			}
			pItem = new SecsItemArray(F4Vec);
		}
	}
	break;
	}
	return pItem;
}

SecsItem* SecsMessage::MakeBoolean(BYTE* pByte, int nLen) {
	SecsItem* pItem = NULL;
	switch (nLen) {
	case 0:
		pItem = new SecsItemZeroLen(SecsItem::ITEM_BOOLEAN);
		break;
	case 1:
		pItem = new SecsItemBoolean(pByte[0] == 0 ? FALSE : TRUE);
		break;
	default:
	{
		std::vector<SecsItemBoolean*> BoolVec;
		for (int nIndex = 0; nIndex < nLen; nIndex++) {
			BoolVec.push_back(new SecsItemBoolean(pByte[nIndex] == 0 ? FALSE : TRUE));
		}
		pItem = new SecsItemArray(BoolVec);
	}
	break;
	}
	return pItem;
}

SecsItem* SecsMessage::MakeList(BYTE* pByte, int& nLen) {
	SecsItemList* pList = new SecsItemList;
	int nNumLenBytes = 0;
	int DataLen = 0;
	BYTE Type;
	int BytesRead = 0;
	int DataIndex = 0;
	for (int nIndex = 0; nIndex < nLen; nIndex++) {
		Type = TypeOf(pByte[BytesRead], nNumLenBytes);
		DataLen = GetItemLen(&(pByte[BytesRead + 1]), nNumLenBytes);
		DataIndex = BytesRead + nNumLenBytes + 1;
		switch (Type) {
		case SecsItem::UNDEFINED:
			ASSERT(0); // Some mistake n the program
			break;
		case SecsItem::ITEM_LIST:
			pList->AddItem(MakeList(&(pByte[DataIndex]), DataLen));
			BytesRead += DataLen;
			break;
		case SecsItem::ITEM_BINARY:
			pList->AddItem(MakeBinary(&(pByte[DataIndex]), DataLen));
			BytesRead += nNumLenBytes + DataLen + 1;
			break;
		case SecsItem::ITEM_BOOLEAN:
			pList->AddItem(MakeBoolean(&(pByte[DataIndex]), DataLen));
			BytesRead += nNumLenBytes + DataLen + 1;
			break;
		case SecsItem::ITEM_ASCII:
			pList->AddItem(MakeASCII(&(pByte[DataIndex]), DataLen));
			BytesRead += nNumLenBytes + DataLen + 1;
			break;
		case SecsItem::ITEM_FLOAT4:
			pList->AddItem(MakeFloat4(&(pByte[DataIndex]), DataLen));
			BytesRead += (nNumLenBytes + DataLen + 1);
			break;
		case SecsItem::ITEM_FLOAT8:
			pList->AddItem(MakeFloat8(&(pByte[DataIndex]), DataLen));
			BytesRead += (nNumLenBytes + DataLen + 1);
			break;
		case SecsItem::ITEM_JIS:
			break;
		case SecsItem::ITEM_UNICODE:
			//
			break;
		case SecsItem::ITEM_INT8:
		case SecsItem::ITEM_INT4:
		case SecsItem::ITEM_INT2:
		case SecsItem::ITEM_INT1:
			pList->AddItem(MakeInt(SecsItem::DataType(Type), &(pByte[DataIndex]), DataLen));
			BytesRead += nNumLenBytes + DataLen + 1;
			break;
		case SecsItem::ITEM_UINT8:
		case SecsItem::ITEM_UINT4:
		case SecsItem::ITEM_UINT2:
		case SecsItem::ITEM_UINT1:
			pList->AddItem(MakeUInt(SecsItem::DataType(Type), &(pByte[DataIndex]), DataLen));
			BytesRead += nNumLenBytes + DataLen + 1;
			break;
		}
	}
	nLen = BytesRead + 2;
	return pList;
}

void SecsMessage::ByteSwap(void* pVoid, int nNumBytes) {
	BYTE* pByte = (BYTE*)pVoid;
	int mid = nNumBytes / 2;
	BYTE Temp;
	for (int nIndex = 0; nIndex < mid; nIndex++) {
		Temp = pByte[nNumBytes - 1 - nIndex];
		pByte[nNumBytes - 1 - nIndex] = pByte[nIndex];
		pByte[nIndex] = Temp;
	}
}

void SecsMessage::PrintSML() {
	// Add header part later ??????
	for (UINT nIndex = 0; nIndex < Items.size(); nIndex++) {
		Items[nIndex]->PrintSML();
	}
}

void SecsMessage::PrintSML(string& Data) {
	// Add header part later ??????
	for (UINT nIndex = 0; nIndex < Items.size(); nIndex++) {
		Items[nIndex]->PrintSML(Data);
	}
}

int SecsMessage::MakeBinaryMessage(BYTE* pByte) {
	int nSize = 0;
	int nTotLen = 0;
	for (UINT nIndex = 0; nIndex < Items.size(); nIndex++) {
		Items[nIndex]->PrintBinary(pByte);
		nSize = Items[nIndex]->GetSECSLength();
		pByte += nSize;
		nTotLen += nSize;
	}
	return nTotLen;
}

UINT SecsMessage::BinaryMessageLength() {
	int nTotLen = 0;
	for (UINT nIndex = 0; nIndex < Items.size(); nIndex++) {
		nTotLen += Items[nIndex]->GetSECSLength();
	}
	return nTotLen;
}

void SecsMessage::AddItem(SecsItem* pItem) {
	Items.push_back(pItem);
}

void SecsMessage::ClearItems() {
	for (UINT nIndex = 0; nIndex < Items.size(); nIndex++) {
		delete Items[nIndex];
	}
	Items.clear();
}

BOOL MessageS1F3::Validate() {
	string MsgName = "(S1F3)";
	if ((MsgRef.size() == 1) &&
		(MsgRef[0].IsAInt()) &&
		(MsgRef[0].IsZeroLen())) {
		MsgName += "Zero Length Integer found";
		LogInfo(MsgName);
		return TRUE;
	}
	if (!(MsgRef[0].IsCollection())) {
		MsgName += " Format not zero length integer or a list or an array";
		LogErr(MsgName);
		return FALSE;
	}
	UINT SVID;
	char Buf[52];
	MsgName += "SVIDs :";
	SVIDs.clear();
	for (int iItems = 0; iItems < int(MsgRef[0].size()); iItems++) {
		if (!MsgRef[0][iItems].GetAInt(SVID)) {
			string Temp;
			MsgRef[0][iItems].TypeString(Temp);
			sprintf(Buf, "Incorrect format (%s) At %d", Temp.c_str(), iItems + 1);
			MsgName += Buf;
			LogErr(MsgName);
			return FALSE;
		}
		sprintf(Buf, " %d ,", SVID);
		MsgName += Buf;
		SVIDs.push_back(SVID);
	}
	LogInfo(MsgName);
	return TRUE;
}

void MessageS1F3::GetSvidList(std::vector < UINT >& Svids) {
	Svids = SVIDs;
}

BOOL MessageS2F13::Validate() {
	string MsgName = "(S2F13)";
	if ((1 != MsgRef.size())) {
		MsgName += " More than one items found";
		LogErr(MsgName);
		return FALSE;
	}
	if ((MsgRef[0].IsAInt()) &&
		(MsgRef[0].IsZeroLen())) {
		MsgName += "Zero Length integer found";
		LogInfo(MsgName);
		return TRUE;
	}
	if (!MsgRef[0].IsCollection()) {
		MsgName += "Item is not a zero length integer or a list or an array ";
		LogErr(MsgName);
		return FALSE;
	}
	UINT ECID;
	char Buf[52];
	MsgName += " ECIDs : ";
	ECIDs.clear();
	for (int iECID = 0; iECID<int(MsgRef[0].size()); iECID++) {
		if (!MsgRef[0][iECID].GetAInt(ECID)) {
			string Temp;
			MsgRef[0][iECID].TypeString(Temp);
			MsgName += " Invalid item in list " + Temp;
			LogErr(MsgName);
			return FALSE;
		}
		ECIDs.push_back(ECID);
		sprintf(Buf, " %d ,", ECID);
		MsgName += Buf;
	}
	LogInfo(MsgName);
	return TRUE;
}

void MessageS2F13::GetEcidList(std::vector < UINT >& Ecids) {
	Ecids = ECIDs;
}

// Note @@Vijay Parameters are not supported at thie time so there
// ... is no validation for it. Should add when support for the parameter is to be added
BOOL MessageS3F25::Validate() {
	string StrTemp;
	std::vector < ParamData > Parameters;
	ParamData Dummy;
	if ((MsgRef.size() != 1) ||
		(!MsgRef[0].IsList()) ||
		(MsgRef[0].size() != 3)) {
		return FALSE;
	}
	if ((!MsgRef[0][0].GetASCII(PortAction)) ||
		//(!MsgRef[0][1].GetUInt1(nPort))||
		(!MsgRef[0][2].IsList())) {
		return FALSE;
	}

	if (MsgRef[0][1].IsAInt()) {
		if (!MsgRef[0][1].GetUInt1(nPort)) return FALSE;
	}
	else if (MsgRef[0][1].IsBinary()) {
		BYTE stPort[3];
		if (MsgRef[0][1].GetBinary(stPort, 2)) {
			nPort = stPort[0];
		}
		else {
			return FALSE;
		}
	}
	else {
		return FALSE;
	}

	ParamName.clear();
	ParamValue.clear();
	for (int iParam = 0; iParam < int(MsgRef[0][2].size()); iParam++) {
		if ((!MsgRef[0][2][iParam].IsList()) ||
			(MsgRef[0][2][iParam].size() != 2) ||
			(!MsgRef[0][2][iParam][0].GetASCII(StrTemp))) {
			return FALSE;
		}
		ParamName.push_back(StrTemp);
		ParamValue.push_back(&MsgRef[0][2][iParam][1]);
	}
	return TRUE;
}

// Message must have decoded before this
// Redone on 24 June 02
BOOL MessageS3F21::Validate() {
	UCHAR Port;
	if ((MsgRef.size() != 1) ||
		(!MsgRef[0].IsList()) ||
		(MsgRef[0].size() != 3)) {
		return FALSE;
	}
	if ((!MsgRef[0][0].GetASCII(PGName)) ||
		(!MsgRef[0][1].GetUInt1(PortAccess))) {
		return FALSE;
	}
	if ((!MsgRef[0][2].IsList()) ||
		(!MsgRef[0][2].size())) {
		return FALSE;
	}
	Ports.clear();
	for (int iPort = 0; iPort < int(MsgRef[0][2].size()); iPort++) {
		if (!MsgRef[0][2][iPort].GetUInt1(Port)) {
			return FALSE;
		}
		Ports.push_back(Port);
	}
	return TRUE;
}

void MessageS3F21::GetPortAccess(string& PAccess) {
	switch (PortAccess) {
	case 0:
		PAccess = "Manual";
		break;
	case 1:
		PAccess = "Auto";
		break;
	default:
		PAccess = "Unknown";
		break;
	}
}

BOOL MessageS3F23::Validate() {
	string Temp;
	if (!GetDataAt(0, 0, SecsItem::ITEM_UINT2, this->PGrpAction)) {
		return false;
	}
	BYTE Tmp = atoi(this->PGrpAction.c_str());
	switch (Tmp) {
	case 1:
		this->PGrpAction = "OUTOFSERVICE";
		break;
	case 2:
		this->PGrpAction = "INSERVICE";
		break;
	case 3:
		this->PGrpAction = "TRANSFERBLOCKED";
		break;
	case 4:
		this->PGrpAction = "READYTOLOAD";
		break;
	case 5:
		this->PGrpAction = "READYTOUNLOAD";
		break;
	case 6:
		this->PGrpAction = "AUTO";
		break;
	case 7:
		this->PGrpAction = "MANUAL";
		break;
	case 8:
		this->PGrpAction = "RESERVATIONATPORT";
		break;
	case 9:
		this->PGrpAction = "CANCELRESERVATIONATPORT";
		break;
	case 10:
		this->PGrpAction = "DEFINE";
		break;
	default:
		ASSERT(0);
		break;
	}
	if (!GetDataAt(0, 1, SecsItem::ITEM_ASCII, this->PGrpName)) {
		return FALSE;
	}
	// Parameters are ignored at this time
	// This message is not fully defined in SEMI standards
	return TRUE;
}

BOOL MessageS2F15::Validate() {
	string Data;
	EcidData Ecv;
	if ((MsgRef.size() != 1) ||
		(!MsgRef[0].IsList()) ||
		(MsgRef[0].size() == 0)) {
		return FALSE;
	}
	EcidVals.clear();
	for (int iECPair = 0; iECPair < int(MsgRef[0].size()); iECPair++) {
		if ((!MsgRef[0][iECPair].IsList()) ||
			(MsgRef[0][iECPair].size() != 2) ||
			(!MsgRef[0][iECPair][0].GetAInt(Ecv.ECID))) {
			return FALSE;
		}
		MsgRef[0][iECPair][1].GetData(Ecv.EcVal);
		EcidVals.push_back(Ecv);
	}
	return TRUE;
}

void MessageS2F15::GetNewECVal(std::vector < EcidData >& NewVals) {
	NewVals = EcidVals;
}

BOOL MessageS2F17::Validate() {
	if (0 != MsgRef.size()) {
		return FALSE;
	}
	return TRUE;
}

BOOL MessageS2F21::Validate() {
	if (1 != MsgRef.size()) {
		return FALSE;
	}
	MsgRef[0].GetUInt1(RCmd);
	return TRUE;
}

BYTE MessageS2F21::GetRemoteCmd() {
	return RCmd;
}

BOOL MessageS2F25::Validate() {
	string Temp;
	if ((MsgRef.size() != 1) ||
		(!MsgRef[0].GetBinary(Temp))) {
		return FALSE;
	}
	return TRUE;
}

BOOL MessageS2F33::Validate() {
	string Data;
	RptData Dummy;
	UINT VID;
	if ((MsgRef.size() != 1) ||
		(!MsgRef[0].IsList()) ||
		(MsgRef[0].size() != 2) ||
		(!MsgRef[0][0].GetAInt(DataId)) ||
		(!MsgRef[0][1].IsList())) {
		return FALSE;
	}
	SecsItem& Ref = MsgRef[0][1];
	RptList.clear();
	for (int iReport = 0; iReport < int(Ref.size()); iReport++) {
		if ((!Ref[iReport].IsList()) ||
			(Ref[iReport].size() != 2) ||
			(!Ref[iReport][0].GetAInt(Dummy.RptId)) ||
			(!Ref[iReport][1].IsList())) {
			return FALSE;
		}
		Dummy.Vids.clear();
		for (int iVID = 0; iVID < int(Ref[iReport][1].size()); iVID++) {
			if (!Ref[iReport][1][iVID].GetAInt(VID)) {
				return FALSE;
			}
			Dummy.Vids.push_back(VID);
		}
		RptList.push_back(Dummy);
	}
	return TRUE;
}

void MessageS2F33::GetRptDefs(std::vector < RptData >& RptDefs) {
	RptDefs = RptList;
}

BOOL MessageS2F35::Validate() {
	CString msg;
	string Data;
	LinkData Dummy;
	UINT RptID;
	if ((MsgRef.size() != 1) ||
		(MsgRef[0].size() != 2) ||
		(!MsgRef[0].IsList())) {
		return FALSE;
	}
	if ((!MsgRef[0][0].GetAInt(DataId)) ||
		(!MsgRef[0][1].IsList()) ||
		(!MsgRef[0][1].size())) {
		return FALSE;
	}
	SecsItem& Ref = MsgRef[0][1];
	LinkList.clear();
	for (int iCEID = 0; iCEID < int(Ref.size()); iCEID++) {
		if ((!Ref[iCEID].IsList()) ||
			(Ref[iCEID].size() != 2) ||
			(!Ref[iCEID][0].GetAInt(Dummy.CeId)) ||
			(!Ref[iCEID][1].IsList())) {
			msg.Format("Fail to retrieve CEID at %d", iCEID);
			Log(msg, 2, "S2F35");
			return FALSE;
		}
		Dummy.RptIds.clear();
		for (int iReport = 0; iReport < int(Ref[iCEID][1].size()); iReport++) {
			if (!Ref[iCEID][1][iReport].GetAInt(RptID)) {
				msg.Format("Fail to retrieve report at %d for CEID at %d", iReport, iCEID);
				Log(msg, 2, "S2F35");
				return FALSE;
			}
			Dummy.RptIds.push_back(RptID);
		}
		LinkList.push_back(Dummy);
	}
	return TRUE;
}

void MessageS2F35::GetLinkDefs(std::vector < LinkData >& LinkDefs) {
	LinkDefs = LinkList;
}

BOOL MessageS10F3::Validate() {
	if ((MsgRef.size() != 1) ||
		(!MsgRef[0].IsList()) ||
		(MsgRef[0].size() != 2)) {
		return FALSE;
	}
	if ((!MsgRef[0][0].GetBinary(&TID, 1))) {
		return FALSE;
	}
	if ((MsgRef[0][1].IsBinary()) ||
		(MsgRef[0][1].IsASCII()) ||
		(MsgRef[0][1].IsAInt())) {
		MsgRef[0][1].GetData(Message);
		return TRUE;
	}
	return FALSE;
}

BYTE MessageS10F3::GetTerminalMsg(string& msg) {
	msg = Message;
	return TID;
}

BOOL MessageS10F5::Validate() {
	string Data;
	if ((MsgRef.size() != 1) ||
		(!MsgRef[0].IsList()) ||
		(MsgRef[0].size() != 2)) {
		return FALSE;
	}
	if ((!MsgRef[0][0].GetBinary(&TID, 1)) ||
		(!MsgRef[0][1].IsList()) ||
		(!MsgRef[0][1].size())) {
		return FALSE;
	}
	for (int iLine = 0; iLine < int(MsgRef[0][1].size()); iLine++) {
		if (!((MsgRef[0][1][iLine].IsBinary()) ||
			(MsgRef[0][1][iLine].IsASCII()) ||
			(MsgRef[0][1][iLine].IsAInt()))) {
			return FALSE;
		}
		MsgRef[0][1][iLine].GetData(Data);
		Message += Data;
		Message += "\r\n";
	}
	return TRUE;
}

BYTE MessageS10F5::GetTerminalMsg(string& msg) {
	msg = Message;
	return TID;
}

BOOL MessageS2F29::Validate() {
	string Data;
	Ecids.clear();
	UINT ECID;
	if ((MsgRef.size() != 1) ||
		(!MsgRef[0].IsList())) {
		return FALSE;
	}
	Ecids.clear();
	for (int iECID = 0; iECID < int(MsgRef[0].size()); iECID++) {
		if (!MsgRef[0][iECID].GetAInt(ECID)) {
			return FALSE;
		}
		Ecids.push_back(ECID);
	}
	return TRUE;
}

void MessageS2F29::GetEcids(std::vector < UINT >& EcidList) {
	EcidList = Ecids;
}

BOOL MessageS3F17::Validate() {
	if ((MsgRef.size() != 1) ||
		(!MsgRef[0].IsList()) ||
		(MsgRef[0].size() != 5)) {
		return FALSE;
	}
	UINT DataID;
	if ((!MsgRef[0][0].GetAInt(DataID)) ||
		(!MsgRef[0][1].GetASCII(Action)) ||
		(!MsgRef[0][2].GetASCII(CarrierID))) {
		return FALSE;
	}
	if (MsgRef[0][3].IsZeroLen()) {
		nPort = -1;
	}
	else if (MsgRef[0][3].IsUInt1()) {
		if (!MsgRef[0][3].GetAInt(nPort)) {
			return FALSE;
		}
	}
	else if (MsgRef[0][3].IsBinary()) {
		BYTE stPort[3];
		MsgRef[0][3].GetBinary(stPort, 2);
		nPort = stPort[0];
	}
	else {
		return FALSE;
	}

	if (!MsgRef[0][4].IsList()) {
		return FALSE;
	}
	string AttrID;
	AttrValues.clear();
	AttrIDs.clear();
	for (int iAttr = 0; iAttr < int(MsgRef[0][4].size()); iAttr++) {
		if ((!MsgRef[0][4][iAttr].IsList()) ||
			(MsgRef[0][4][iAttr].size() != 2) ||
			(!MsgRef[0][4][iAttr][0].GetASCII(AttrID))) {
			return FALSE;
		}
		AttrValues.push_back(&MsgRef[0][4][iAttr][1]);
		AttrIDs.push_back(AttrID);
	}
	return TRUE;
}

BOOL MessageS16F17::Validate() {
	string Str;
	if ((MsgRef.size() != 1) ||
		(!MsgRef[0].IsList())) {
		return FALSE;
	}
	PrJobList.clear();
	for (int iPJIDs = 0; iPJIDs < int(MsgRef[0].size()); iPJIDs++) {
		if (!MsgRef[0][iPJIDs].GetASCII(Str)) {
			return FALSE;
		}
		PrJobList.push_back(Str);
	}
	return TRUE;
}

BOOL MessageS16F19::Validate() {
	if (0 == MsgRef.size()) {
		return TRUE;
	}
	return FALSE;
}

BOOL MessageS16F21::Validate() {
	if (0 == MsgRef.size()) {
		return TRUE;
	}
	return FALSE;
}

BOOL MessageS16F3::Validate() {
	SecsItem* pList = NULL;
	SecsItem* pParams = NULL;
	string Temp;
	JobDetails Dummy;
	/*	if ((MsgRef.size()!=1)||
			(!MsgRef[0].IsList())||
			(MsgRef[0].size() != 5)||
			(!MsgRef[0][2].IsList())||
			(!MsgRef[0][3].IsList()))
		{
			return FALSE;
		}
		if ((!MsgRef[0][0].GetAInt(DataID)) ||
			(!(MsgRef[0][1].IsBinary()||MsgRef[0][1].IsASCII())))
		{
			return FALSE;
		}
		string MtlFmt;
		MsgRef[0][1].GetData(MtlFmt);
		MF = atoi(MtlFmt.c_str());*/

	if ((Items.size()) && (Items[0]->IsList()) && (Items[0]->NumContainedItems() == 2)) {
		pList = Items[0]->ContainedItems(1);
		if (pList->IsList() && (5 == pList->NumContainedItems())) {
			// Get the Portid NOW
			if (pList->ContainedItems(0)->IsUInt()) {
				pList->ContainedItems(0)->GetData(Temp);
				this->PortId = (BYTE)atoi(Temp.c_str());
			}
			else {
				return FALSE;
			}
			if (pList->ContainedItems(1)->IsList()) {
				pParams = pList->ContainedItems(1);
				// Traverse all the parameters
				for (int iParams = 0; iParams < pParams->NumContainedItems(); iParams++) {
					if (3 == pParams->ContainedItems(iParams)->NumContainedItems()) {
						// Slot No
						if (pParams->ContainedItems(iParams)->ContainedItems(0)->IsASCII()) {
							pParams->ContainedItems(iParams)->ContainedItems(0)->GetData(Dummy.SlotNum);
						}
						else {
							return FALSE;
						}
						// WaferThickness
						if (pParams->ContainedItems(iParams)->ContainedItems(1)->IsASCII()) {
							pParams->ContainedItems(iParams)->ContainedItems(1)->GetData(Dummy.WaferThickness);
						}
						else {
							return FALSE;
						}
						// Film Thickness
						if (pParams->ContainedItems(iParams)->ContainedItems(2)->IsASCII()) {
							pParams->ContainedItems(iParams)->ContainedItems(2)->GetData(Dummy.filmThickness);
						}
						else {
							return FALSE;
						}
						this->PrJobDetails.push_back(Dummy);
					}
					else {
						return FALSE;
					}
				}
			}
			else {
				return FALSE;
			}
			if (pList->ContainedItems(2)->IsBinary()) {
				pList->ContainedItems(2)->GetData(Temp);
				this->AutoStart = ((atoi(Temp.c_str()) > 0) ? TRUE : FALSE);
			}
			else {
				return FALSE;
			}
			if (pList->ContainedItems(3)->IsUInt()) {
				pList->ContainedItems(3)->GetData(Temp);
				this->ScanLines = atoi(Temp.c_str());
			}
			else {
				return FALSE;
			}
			if (pList->ContainedItems(4)->IsUInt()) {
				pList->ContainedItems(4)->GetData(Temp);
				this->ScanNo = atoi(Temp.c_str());
			}
			else {
				return FALSE;
			}
		}
		else {
			return FALSE;
		}
	}
	else {
		return FALSE;
	}
	return TRUE;
}

BOOL MessageS16F5::Validate() {
	if ((MsgRef.size() != 1) ||
		(!MsgRef[0].IsList()) ||
		(MsgRef[0].size() != 4) ||
		(!MsgRef[0][3].IsList())) {
		return FALSE;
	}
	if ((!MsgRef[0][0].GetAInt(DataID)) ||
		(!MsgRef[0][1].GetASCII(PrJobId)) ||
		(!MsgRef[0][2].GetASCII(Command))) {
		return FALSE;
	}
	//string CMParam;
	// Command parameters are not specified But code for validation exists @@Vijay [11/29/2002]
	SecsItem& CPRef = MsgRef[0][3];
	for (int iCP = 0; iCP < int(CPRef.size()); iCP++) {
		if ((!CPRef[iCP].IsList()) ||
			(CPRef[iCP].size() != 2) ||
			(!(CPRef[iCP][0].IsASCII() || CPRef[iCP][0].IsAInt()))) {
			return FALSE;
		}
		//CPRef[iCP][0].GetData(CMParam);
		//CmdMap.insert(CmdParamMap::value_type(CMParam,&CPRef[iCP][1]));
	}
	return TRUE;
}

BOOL MessageS16F27::Validate() {
	if ((MsgRef.size() != 1) ||
		(!MsgRef[0].IsList()) ||
		(MsgRef[0].size() != 3) ||
		(!MsgRef[0][2].IsList())) {
		return FALSE;
	}
	if ((!MsgRef[0][0].GetASCII(CtrlJOB)) ||
		(!MsgRef[0][1].GetUInt1(Command))) {
		return FALSE;
	}
	if (MsgRef[0][2].size()) {
		if ((MsgRef[0][2].size() != 2) ||
			(!(MsgRef[0][2][0].IsASCII() || MsgRef[0][2][0].IsAInt())) ||
			(MsgRef[0][2][1].IsFloat() || MsgRef[0][2][1].IsCollection() || MsgRef[0][2][1].IsZeroLen())) {
			return FALSE;
		}
		MsgRef[0][2][0].GetData(ParamName);
		MsgRef[0][2][1].GetData(ParamVal);
	}
	return TRUE;
}

BOOL MessageS6F15::Validate() {
	if ((MsgRef.size() != 1) ||
		(!MsgRef[0].GetAInt(Ceid))) {
		return FALSE;
	}
	return TRUE;
}

BOOL MessageMBEnq::Validate() {
	UINT DATAID;
	if ((MsgRef.size() != 1) ||
		(!MsgRef[0].IsList()) ||
		(MsgRef[0].size() != 2) ||
		(!MsgRef[0][0].GetAInt(DATAID)) ||
		(!MsgRef[0][1].GetAInt(DataLen))) {
		return FALSE;
	}
	return TRUE;
}

BOOL MessageS16F15::Validate() {
	PrjobRepNew Temp;
	Temp.ScanNo = 0;
	Temp.NumScanLines = 0;
	if ((MsgRef.size() != 1) ||
		(MsgRef[0].size() != 2) ||
		(!MsgRef[0][1].IsList()) ||
		(!MsgRef[0][1].size())) {
		Log("Incorrect format", 2, "S16F15");
		return FALSE;
	}
	if (!MsgRef[0][0].GetAInt(DataID)) {
		return FALSE;
	}
	SecsItem& PrList = MsgRef[0][1];
	PrJobs.clear();
	for (int iPr = 0; iPr < int(PrList.size()); iPr++) {
		if (!ReadPrJob(PrList[iPr], Temp)) {
			return FALSE;
		}
		PrJobs.push_back(Temp);
		Temp.WaferParams.clear();
		Temp.Slots.clear();
		Temp.PauseEvts.clear();
		Temp.MatlIds.clear();
		Temp.ScanNo = 0;
		Temp.NumScanLines = 0;
		Temp.RecipeID.erase();
	}
	return TRUE;
}

BOOL MessageS16F15::ReadPrJob(SecsItem& PList, PrjobRepNew& PrJob) {
	string Dummy;
	CString src = "S16F15";

	if (!PList.IsList()) {
		LogFormatError(src);
		return FALSE;
	}

	if (PList.size() != 6) {
		Log("List size not 6", 2, src);
		return FALSE;
	}

	if (!PList[2].IsList()) {
		LogFormatError(src);
		return FALSE;
	}

	if (PList[2].size() != 1) {
		Log("CarrierID spec is not 1", 2, src);
		return FALSE;
	}

	if (!PList[3].IsList()) {
		LogFormatError(src);
		return FALSE;
	}

	if (PList[3].size() != 3) {
		Log("Wrong PRRECIPEMETHOD size", 2, src);
		return FALSE;
	}

	if (!PList[5].IsList()) {
		Log("PRPAUSEEVENT format error", 0, src);
		return FALSE;
	}
	if (!PList[0].GetASCII(PrJob.PrJobID)) {
		Log("Fail to retrieve PRJOBID", 2, src);
		return FALSE;
	}

	// Move into deeper level [9/6/2007 LYF]
	//if (!PrJob.PrJobID.size()) {
	//	Log("Zero length PRJOBID",2,src);
	//	return FALSE;
	//}

	string MtlFmt;
	if (!(PList[1].GetASCII(MtlFmt) || PList[1].GetBinary(MtlFmt))) {
		Log("Illegal MF code format", 2, src);
		return FALSE;
	}

	PrJob.MF = (BYTE)atoi(MtlFmt.c_str());
	// Moved into deeper level [9/6/2007 LYF]
	//if ((PrJob.MF < 13) || (PrJob.MF > 14)) {
	//	Log("MF value not 13 or 14",2,src);
	//	return FALSE;
	//}

	BYTE Slot;
	if ((!PList[2][0][0].GetASCII(PrJob.CarrierID))) {
		Log("Fail to retrieve CARRIERID", 2, src);
		return FALSE;
	}

	if (!PrJob.CarrierID.size()) {
		Log("CarrierID size is NULL", 2, src);
		return FALSE;
	}

	PrJob.Slots.clear();
	if (PList[2][0].size() == 2) {
		SecsItem& SlotRef = PList[2][0][1];
		if (SlotRef.IsCollection()) {
			if (!SlotRef.size()) {
				Log("SLOTID size is NULL", 2, src);
				return FALSE;
			}
			for (int iSlot = 0; iSlot < int(SlotRef.size()); iSlot++) {
				if (!SlotRef[iSlot].GetUInt1(Slot)) {
					Log("Fail to retrieve SLOTID", 2, src);
					return FALSE;
				}
				PrJob.Slots.push_back(Slot);
			}
		}
	}
	else {
		Log("No SLOTID specified", 2, src);
	}

	if (!PList[3][0].GetUInt1(PrJob.RcpMethd)) {
		Log("Fail to retrieve PRRECIPEMETHOD", 2, src);
		return FALSE;
	}

	// Moved into deeper level  [9/6/2007 LYF]
	//if (PrJob.RcpMethd != 2) {
	//	Log("Recipe Method is not 2",2,src);
	//	return FALSE;
	//}

	if (!PList[3][1].GetASCII(PrJob.RecipeID)) {
		Log("Fail to retrieve RCPSPEC", 2, src);
		return FALSE;
	}
	if (!PrJob.RecipeID.size()) {
		Log("RCPSPEC size is NULL", 2, src);
		return FALSE;
	}

	if (!PList[3][2].size()) { // added bIgnorRecipeParameter [9/6/2007 LYF]
		Log("Recipe parameter not specified", 2, src);
		if (!bIgnorRecipeParameter) {
			return FALSE;
		}
		Log("Recipe parameter ignored", 2, src);
	}
	if (!ExtractRecipeParams(PList[3][2], PrJob)) {
		Log("Recipe parameter not available", 2, src);
		if (!bIgnorRecipeParameter) {
			return FALSE;
		}
		Log("Recipe parameter ignored", 2, src);
	}
	bool bAuto;
	if (!PList[4].GetBoolean(bAuto)) {
		return FALSE;
	}
	PrJob.bAutoStart = (bAuto) ? 1 : 0;
	UINT PEvent;
	PrJob.PauseEvts.clear();
	for (int iPEvt = 0; iPEvt < int(PList[5].size()); iPEvt++) {
		if (!PList[5][iPEvt].GetAInt(PEvent)) {
			return FALSE;
		}
		PrJob.PauseEvts.push_back(PEvent);
	}
	return TRUE;
}

BOOL MessageS16F11::Validate() {
	CString src = "S16F11";

	string Dummy;
	PrJob.ScanNo = 0;
	PrJob.NumScanLines = 0;
	if ((MsgRef.size() != 1) ||
		(!MsgRef[0].IsList()) ||
		(MsgRef[0].size() != 7)) {
		return FALSE;
	}
	if ((!MsgRef[0][0].GetAInt(DataID)) ||
		(!MsgRef[0][1].GetASCII(PrJob.PrJobID)) ||
		(!(MsgRef[0][2].GetBinary(Dummy) || MsgRef[0][2].GetASCII(Dummy))) ||
		(!MsgRef[0][3].IsList()) ||
		(MsgRef[0][3].size() != 1) ||
		(!MsgRef[0][6].IsList())) {
		return FALSE;
	}
	PrJob.MF = atoi(Dummy.c_str()); //  [9/6/2007 LYF]
	// Moved to deeper level [9/6/2007 LYF]
	//if (13 != (PrJob.MF = atoi(Dummy.c_str()))) //  [9/6/2007 LYF]
	//{
	//	return FALSE;
	//}
	SecsItem& MtlRef = MsgRef[0][3];
	if ((!MtlRef[0].IsList()) ||
		(MtlRef[0].size() != 2) ||
		(!MtlRef[0][0].GetASCII(PrJob.CarrierID)) ||
		(!MtlRef[0][1].IsList())/*||*/
		/*(!MtlRef[0][1].size())*/)	// Slotmap allowed to be zero.
	{
		return FALSE;
	}
	if (MtlRef[0][1].size()) {
		SecsItem& SlotRef = MtlRef[0][1];
		UCHAR SlotNo;
		PrJob.Slots.clear();
		for (int nIndex = 0; nIndex < SlotRef.size(); nIndex++) {
			if (!SlotRef[nIndex].GetUInt1(SlotNo)) {
				return FALSE;
			}
			PrJob.Slots.push_back(SlotNo);
		}
	}
	if ((!MsgRef[0][4].IsList()) ||
		(MsgRef[0][4].size() != 3) ||
		(!MsgRef[0][4][0].GetUInt1(PrJob.RcpMethd)) ||
		(!MsgRef[0][4][1].GetASCII(PrJob.RecipeID))) {
		return FALSE;
	}
	if (TRUE != ExtractRecipeParams(MsgRef[0][4][2], PrJob)) {
		Log("Mendatory recipe parameter not specified", 2, src);
		if (!bIgnorRecipeParameter) {
			return FALSE;
		}
		Log("Recipe parameter ignored", 2, src);
	}
	bool bAutoStart;
	if (!MsgRef[0][5].GetBoolean(bAutoStart)) {
		return FALSE;
	}
	PrJob.bAutoStart = (bAutoStart) ? 1 : 0;
	int nNumPEvt = MsgRef[0][6].size();
	UINT PEvt;
	PrJob.PauseEvts.clear();
	for (int nIndex = 0; nIndex < nNumPEvt; nIndex++) {
		if (!MsgRef[0][6][nIndex].GetAInt(PEvt)) {
			return FALSE;
		}
		PrJob.PauseEvts.push_back(PEvt);
	}
	return TRUE;
}

BOOL MessageS16F13::Validate() {
	PrjobRepNew Temp;
	BOOL bAutoStart;
	if ((MsgRef.size() != 1) ||
		(!MsgRef[0].IsList()) ||
		(MsgRef[0].size() != 5) ||
		(!MsgRef[0][1].IsList()) ||
		(!MsgRef[0][1].size()) ||
		(!MsgRef[0][2].IsList()) ||
		(MsgRef[0][2].size() != 3) ||
		(!MsgRef[0][4].IsList())) {
		return FALSE;
	}
	if (!MsgRef[0][0].GetAInt(DataID)) {
		return FALSE;
	}
	PrJobs.clear();
	int nIndex;
	for (nIndex = 0; nIndex < int(MsgRef[0][1].size()); nIndex++) {
		if (!ReadPrJob(MsgRef[0][1][nIndex], Temp)) {
			return FALSE;
		}
		PrJobs.push_back(Temp);
	}
	PrjobRepNew PrJob;
	if ((!MsgRef[0][2][0].GetUInt1(PrJob.RcpMethd)) ||
		(!MsgRef[0][2][1].GetASCII(PrJob.RecipeID))) {
		return FALSE;
	}
	if (!ExtractRecipeParams(MsgRef[0][2][2], PrJob)) {
		return FALSE;
	}
	bool bAuto = false;
	if (!MsgRef[0][3].GetBoolean(bAuto)) {
		return FALSE;
	}
	bAutoStart = bAuto ? 1 : 0;
	UINT PREVentID;
	PrJob.PauseEvts.clear();
	for (int iPause = 0; iPause < int(MsgRef[0][4].size()); iPause++) {
		if (!MsgRef[0][4][iPause].GetAInt(PREVentID)) {
			return FALSE;
		}
		PrJob.PauseEvts.push_back(PREVentID);
	}
	for (nIndex = 0; nIndex < int(PrJobs.size()); nIndex++) {
		PrJobs[nIndex].bAutoStart = bAutoStart;
		PrJobs[nIndex].PauseEvts = PrJob.PauseEvts;
		PrJobs[nIndex].ScanNo = PrJob.ScanNo;
		PrJobs[nIndex].NumScanLines = PrJob.NumScanLines;
		PrJobs[nIndex].RecipeID = PrJob.RecipeID;
		PrJobs[nIndex].RcpMethd = PrJob.RcpMethd;
		for (int nIndex2 = 0; nIndex2 < int(PrJob.WaferParams.size()); nIndex2++) {
			PrJobs[nIndex].WaferParams.push_back(PrJob.WaferParams[nIndex2]);
		}
	}
	return TRUE;
}

BOOL MessageS16F13::ReadPrJob(SecsItem& PList, PrjobRepNew& PrJob) {
	string Dummy;
	UCHAR SlotID;
	if ((!PList.IsList()) ||
		(PList.size() != 3) ||
		(!PList[2].IsList()) ||
		(PList[2].size() != 1)) {
		return FALSE;
	}
	if ((!PList[0].GetASCII(PrJob.PrJobID)) ||
		(!(PList[1].GetBinary(Dummy) || PList[1].GetASCII(Dummy)))) {
		return FALSE;
	}
	PrJob.MF = (BYTE)atoi(Dummy.c_str());
	if (13 == PrJob.MF) {
		if ((!PList[2][0][0].GetASCII(PrJob.CarrierID)) ||
			(!PList[2][0][1].IsCollection())) {
			return FALSE;
		}
		SecsItem& SlotRef = PList[2][0][1];
		if (!SlotRef.IsCollection()) {
			return FALSE;
		}
		PrJob.Slots.clear();
		for (int iSlot = 0; iSlot < int(SlotRef.size()); iSlot++) {
			if (!SlotRef[iSlot].GetUInt1(SlotID)) {
				return FALSE;
			}
			PrJob.Slots.push_back(SlotID);
		}
	}
	else {
		return FALSE;
	}
	return TRUE;
}

BOOL MessageS14F1::Validate() {
	string Dummy;
	AttrData Data;
	if ((MsgRef.size() != 1) ||
		(!MsgRef[0].IsList()) ||
		(MsgRef[0].size() != 5) ||
		(!MsgRef[0][2].IsList()) ||
		(!MsgRef[0][3].IsList()) ||
		(!MsgRef[0][4].IsList())) {
		return FALSE;
	}
	if ((!MsgRef[0][0].GetASCII(ObjSpec)) ||
		(!MsgRef[0][1].GetASCII(ObjType))) {
		return FALSE;
	}
	ObjIds.clear();
	for (int iObjId = 0; iObjId < int(MsgRef[0][2].size()); iObjId++) {
		if ((!MsgRef[0][2][iObjId].GetASCII(Dummy))) {
			return FALSE;
		}
		ObjIds.push_back(Dummy);
	}

	AttrData Temp;
	for (int iAttFilt = 0; iAttFilt < int(MsgRef[0][3].size()); iAttFilt++) {
		if ((!MsgRef[0][3][iAttFilt].IsList()) ||
			(MsgRef[0][3][iAttFilt].size() != 3) ||
			(!MsgRef[0][3][iAttFilt][0].GetASCII(Temp.AttrId)) ||
			(!MsgRef[0][3][iAttFilt][2].GetUInt1(Temp.AttrrElen))) {
			return FALSE;
		}
		// @@Vijay To be uncommented later [11/17/2002]Temp.AttData = pDat.CloneSelf();
		AtrDat.push_back(Temp);
	}
	AttrID.clear();
	for (int iAttrID = 0; iAttrID < int(MsgRef[0][4].size()); iAttrID++) {
		if ((!MsgRef[0][4][iAttrID].GetASCII(Dummy))) {
			return FALSE;
		}
		AttrID.push_back(Dummy);
	}
	return TRUE;
}

BOOL MessageS14F3::Validate() {
	if ((MsgRef.size() != 1) ||
		(!MsgRef[0].IsList()) ||
		(MsgRef[0].size() != 4) ||
		(!MsgRef[0][2].IsList()) ||
		(!MsgRef[0][2].size()) ||
		(!MsgRef[0][3].IsList()) ||
		(!MsgRef[0][3].size())) {
		return FALSE;
	}

	if ((!MsgRef[0][0].GetASCII(ObjSpec)) ||
		(!MsgRef[0][1].GetASCII(ObjType))) {
		return FALSE;
	}
	string ObjID;
	ObjIds.clear();
	for (int iObIDs = 0; iObIDs < int(MsgRef[0][2].size()); iObIDs++) {
		if ((!MsgRef[0][2][iObIDs].GetASCII(ObjID))) {
			return FALSE;
		}
		ObjIds.push_back(ObjID);
	}
	string AttID;
	AttrID.clear();
	AttrValue.clear();
	for (int iAttrDat = 0; iAttrDat < int(MsgRef[0][3].size()); iAttrDat++) {
		if ((!MsgRef[0][3][iAttrDat].IsList()) ||
			(MsgRef[0][3][iAttrDat].size() != 2) ||
			(!MsgRef[0][3][iAttrDat][0].GetASCII(AttID))) {
			return FALSE;
		}
		AttrID.push_back(AttID);
		AttrValue.push_back(&MsgRef[0][3][iAttrDat][1]);
	}
	return TRUE;
}

BOOL MessageS14F5::Validate() {
	if ((MsgRef.size() != 1) ||
		(!MsgRef[0].GetASCII(ObjSpec))) {
		return FALSE;
	}
	return TRUE;
}

BOOL MessageS14F7::Validate() {
	if ((MsgRef.size() != 1) ||
		(!MsgRef[0].IsList()) ||
		(MsgRef[0].size() != 2)) {
		return FALSE;
	}
	if ((!MsgRef[0][0].GetASCII(ObjSpec)) ||
		(!MsgRef[0][1].IsList()) ||
		(!MsgRef[0][1].size())) {
		return FALSE;
	}
	SecsItem& TypeRef = MsgRef[0][1];
	string Type;
	ObjType.clear();
	for (int iObTyp = 0; iObTyp <int(TypeRef.size()); iObTyp++) {
		if ((!TypeRef[iObTyp].GetASCII(Type))) {
			return FALSE;
		}
		ObjType.push_back(Type);
	}
	return TRUE;
}

BOOL MessageS14F9::Validate() {
	CString src = "S14F9";
	if ((MsgRef.size() != 1) ||
		(!MsgRef[0].IsList()) ||
		(MsgRef[0].size() != 3) ||
		(!MsgRef[0][2].IsList())) {
		Log("Format error", 2, src);
		return FALSE;
	}
	if (!MsgRef[0][0].GetASCII(ObjSpec)) {
		Log("Fail to retrieve ObjSpec", 2, src);
		return FALSE;
	}
	if (!MsgRef[0][1].GetASCII(ObjType)) {
		Log("Fail to retrieve ObjType", 2, src);
		return FALSE;
	}

	if (!ObjType.size()) {
		Log("Illegal ObjType", 2, src);
		return FALSE;
	}

	//	Attributes.clear();

	if (!MsgRef[0][2].size()) {
		Log("No attribute specified", 2, src);
		return FALSE;
	}

	SecsItem& Ref = MsgRef[0][2];
	string AttrName;
	Attributes.clear();
	for (int iAttr = 0; iAttr < int(Ref.size()); iAttr++) {
		if ((!Ref[iAttr].IsList()) ||
			(Ref[iAttr].size() != 2) ||
			(!Ref[iAttr][0].GetASCII(AttrName))) {
			return FALSE;
		}
		Attributes.insert(AttrMap::value_type(AttrName, Ref[iAttr][1].CloneSelf()));
	}
	return TRUE;
}

MessageS14F9::~MessageS14F9() {
	AttrMap::iterator it = Attributes.begin();
	for (; it != Attributes.end(); it++) {
		delete it->second;
	}
	Attributes.clear();
}
BOOL MessageS14F11::Validate() {
	if ((MsgRef.size() != 1) ||
		(!MsgRef[0].IsList()) ||
		(MsgRef[0].size() != 2) ||
		(!MsgRef[0][1].IsList())) {
		return FALSE;
	}
	if ((!MsgRef[0][0].GetASCII(ObjID))) {
		return FALSE;
	}
	SecsItem& Ref = MsgRef[0][1];
	string AttrName;
	Attributes.clear();
	for (int iAttr = 0; iAttr < int(Ref.size()); iAttr++) {
		if ((!Ref[iAttr].IsList()) ||
			(Ref[iAttr].size() != 2) ||
			(!Ref[iAttr][0].GetASCII(AttrName))) {
			return FALSE;
		}
		Attributes.insert(AttrMap::value_type(AttrName, Ref[iAttr][1].CloneSelf()));
	}
	return TRUE;
}
MessageS14F11::~MessageS14F11() {
	AttrMap::iterator it = Attributes.begin();
	for (; it != Attributes.end(); it++) {
		delete it->second;
	}
	Attributes.clear();
}

// Temporarty arrangement ! Later put into configuration file & read in
BOOL ErrorMessages::GetErrTxt(int ErrNo, string& ErrTexr) {
	switch (ErrNo) {
	case 1:
		ErrTexr = "Parameters improperly specified";
		break;
	case 2:
		ErrTexr = "Insufficient Parameters specified";
		break;
	case 3:
		ErrTexr = "Unsupported option specified";
		break;
	case 4:
		ErrTexr = "Busy Queue Full";
		break;
	case 5:
		ErrTexr = "Unavailable for process jobs";
		break;
	case 6:
		ErrTexr = "Command invalid in current state";
		break;
	case 7:
		ErrTexr = "No material altered";
		break;
	case 8:
		ErrTexr = "Material partially processed";
		break;
	case 9:
		ErrTexr = "All material processed";
		break;
	case 10:
		ErrTexr = "Recipe specificatio error";
		break;
	case 11:
		ErrTexr = "Failed during material processing";
		break;
	case 12:
		ErrTexr = "Failed not during material processing";
		break;
	case 13:
		ErrTexr = "Failed due to non arrival of material ";
		break;
	case 14:
		ErrTexr = "Failed during material processing";
		break;
	case 15:
		ErrTexr = "Process job aborted";
		break;
	case 16:
		ErrTexr = "Process job stopped";
		break;
	case 17:
		ErrTexr = "Process job cancelled";
		break;
	case 18:
		ErrTexr = "Other error";
		break;
	case 19:
		ErrTexr = "Duplicate Prjob ID";
		break;
	case 20:
		ErrTexr = "Prjob space not available";
		break;
	case 21:
		ErrTexr = "Invalid port number specified";
		break;
	default:
		break;
	}
	return TRUE;
}

BOOL ErrorMessages::GetErrTxtE5(int ErrNo, string& ErrText) {
	switch (ErrNo) {
	case NO_E5_ERROR:
		ErrText = "No error";
		break;
	case UNKNOWN_OBJECT_IN_OBJECT_SPECIFIER:
		ErrText = "Unknown object in Object Specifier";
		break;
	case UNKNOWN_TARGET_OBJECT_TYPE:
		ErrText = "Unknown target object type";
		break;
	case UNKNOWN_OBJECT_INSTANCE:
		ErrText = "Unknown object instance";
		break;
	case UNKNOWN_ATTRIBUTE_NAME:
		ErrText = "Unknown attribute name";
		break;
	case READ_ONLY_ATTRIBUTE_ACCESS_DENIED:
		ErrText = "Read­only attribute ?access denied";
		break;
	case UNKNOWN_OBJECT_TYPE:
		ErrText = "Unknown object type";
		break;
	case INVALID_ATTRIBUTE_VALUE:
		ErrText = "Invalid attribute value";
		break;
	case SYNTAX_ERROR:
		ErrText = "Syntax error";
		break;
	case VERIFICATION_ERROR:
		ErrText = "Verification error";
		break;
	case VALIDATION_ERROR:
		ErrText = "Validation error";
		break;
	case OBJECT_IDENTIFIER_IN_USE:
		ErrText = "Object identifier in use";
		break;
	case PARAMETERS_IMPROPERLY_SPECIFIED:
		ErrText = "Parameters improperly specified";
		break;
	case INSUFFICIENT_PARAMETERS_SPECIFIED:
		ErrText = "Insufficient parameters specified";
		break;
	case UNSUPPORTED_OPTION_REQUESTED:
		ErrText = "Unsupported option requested";
		break;
	case BUSY:
		ErrText = "Busy";
		break;
	case NOT_AVAILABLE_FOR_PROCESSING:
		ErrText = "Not available for processing";
		break;
	case COMMAND_NOT_VALID_FOR_CURRENT_STATE:
		ErrText = "Command not valid for current state";
		break;
	case NO_MATERIAL_ALTERED:
		ErrText = "No material altered";
		break;
	case MATERIAL_PARTIALLY_PROCESSED:
		ErrText = "Material partially processed";
		break;
	case ALL_MATERIAL_PROCESSED:
		ErrText = "All material processed";
		break;
	case RECIPE_SPECIFICATION_RELATED_ERROR:
		ErrText = "Recipe specification related error";
		break;
	case FAILED_DURING_PROCESSING:
		ErrText = "Failed during processing";
		break;
	case FAILED_WHILE_NOT_PROCESSING:
		ErrText = "Failed while not processing";
		break;
	case FAILED_DUE_TO_LACK_OF_MATERIAL:
		ErrText = "Failed due to lack of material";
		break;
	case JOB_ABORTED:
		ErrText = "Job aborted";
		break;
	case JOB_STOPPED:
		ErrText = "Job stopped";
		break;
	case JOB_CANCELLED:
		ErrText = "Job cancelled";
		break;
	case JOB_INQUEUE:
		ErrText = "Job in queue";
		break;
	case CANNOT_CHANGE_SELECTED_RECIPE:
		ErrText = "Cannot change selected recipe";
		break;
	case UNKNOWN_EVENT:
		ErrText = "Unknown event";
		break;
	case DUPLICATE_REPORT_ID:
		ErrText = "Duplicate report ID";
		break;
	case UNKNOWN_DATA_REPORT:
		ErrText = "Unknown data report";
		break;
	case DATA_REPORT_NOT_LINKED:
		ErrText = "Data report not linked";
		break;
	case UNKNOWN_TRACE_REPORT:
		ErrText = "Unknown trace report";
		break;
	case DUPLICATE_TRACE_ID:
		ErrText = "Duplicate trace ID";
		break;
	case TOO_MANY_DATA_REPORTS:
		ErrText = "Too many data reports";
		break;
	case SAMPLE_PERIOD_OUT_OF_RANGE:
		ErrText = "Sample period out of range";
		break;
	case GROUP_SIZE_TO_LARGE:
		ErrText = "Group size to large";
		break;
	case RECOVERY_ACTION_CURRENTLY_INVALID:
		ErrText = "Recovery action currently invalid";
		break;
	case BUSY_WITH_ANOTHER_RECOVERY_CURRENTLY_UNABLE_TO_PERFORM_THE_RECOVERY:
		ErrText = "Busy with another recovery currently unable to perform the recovery";
		break;
	case NO_ACTIVE_RECOVERY_ACTION:
		ErrText = "No active recovery action";
		break;
	case EXCEPTION_RECOVERY_FAILED:
		ErrText = "Exception recovery failed";
		break;
	case EXCEPTION_RECOVERY_ABORTED:
		ErrText = "Exception recovery aborted";
		break;
	case INVALID_TABLE_ELEMENT:
		ErrText = "Invalid table element";
		break;
	case UNKNOWN_TABLE_ELEMENT:
		ErrText = "Unknown table element";
		break;
	case CANNOT_DELETE_PREDEFINED:
		ErrText = "Cannot delete predefined";
		break;
	case INVALID_TOKEN:
		ErrText = "Invalid token";
		break;
	case INVALID_PARAMETER:
		ErrText = "Invalid parameter";
		break;
	case UNKNOWN_E5_ERROR:
		ErrText = "Unknown E5 error";
		break;
	case SPECIFIED_PRJOB_NOT_IN_QUEUE:
		ErrText = "Specified PRJob not in queue";
		break;
	case OBJECT_CREATION_ERROR:
		ErrText = "Object creation error";
		break;
	case INVALID_CARRIER_OBJECT_SPECIFIED:
		ErrText = "Invalid carrier object specified";
		break;
	case EQUIPMENT_OUT_OF_SERVICE:
		ErrText = "Equipment out of service";
		break;
	case NO_PRJOB_SPECIFIED:
		ErrText = "No PR Job specified";
		break;
	case RECIPE_NOT_FOUND:
		ErrText = "Recipe Not Found";
		break;
	case NULL_IDENTIFIER_SPECIFIED:
		ErrText = "Zero length Object identifier specified";
		break;
	case RECIPE_METHOD_NOT_2:
		ErrText = "Recipe method error";
		break;
	case ILLEGAL_MF_CODE:
		ErrText = "Illegal MF code";
		break;
	case ZERO_LENGTH_PRJOBID:
		ErrText = "Zero length PrJob ID";
		break;
	default:
		ErrText = "Unknown error";
		break;
	}
	return TRUE;
}

BOOL ErrorMessages::GetErrorTextS16F27(int Service, int ErrNo, string& ErrText) {
	switch (ErrNo) {
	case 1:
		ErrText = "Unknown Object Instance";
		break;
	case 2:
		ErrText = "Parameters improperly specified";
		break;
	case 3:
		ErrText = "Insufficient parameters specified";
		break;
	case 4:
		ErrText = "Command not valid for current state";
		break;
	case 5:
		ErrText = "Command not valid for current state";
		break;
	case 6:
		ErrText = "Command not valid for current state";
		break;
	case 7:
		ErrText = "Job Cancelled";
		break;
	case 8:
		ErrText = "Command not valid for current state";
		break;
	case 9:
		ErrText = "Command not valid for current state";
		break;
	case 10:
		ErrText = "Busy";
		break;
	case 11:
		ErrText = "Job stopped";
		break;
	case 12:
		ErrText = "Job aborted";
		break;
	case 13:
		ErrText = "Command not valid for current state";
		break;
	default:
		ErrText = "Unknown error";
		break;
	}
	return TRUE;
}

BOOL ErrorMessages::GetErrorTextS3F18(int ErrNo, string& ErrText) {
	switch (ErrNo) {
	case INVALID_CMD:
		ErrText = "Invalid command";
		break;
	case CANNOT_PERFORM_NOW:
		ErrText = "Cannot perform now";
		break;
	case INVALID_DATA_OR_ARG:
		ErrText = "Invalid data or argument";
		break;
	case ACK_NOTIFY_LATER:
		ErrText = "Acknowledge, request will be performed with completion signaled later by an event";
		break;
	case REJECT_INVALID_STATE:
		ErrText = "Rejected. Invalid state.";
		break;
	case RESERVED_CMD_NOT_SUPPORTED:
		ErrText = "This command is not supported";
		break;
	case RESERVE_PRJOB_DEFINED:
		ErrText = "Rejected material in use";
		break;
	default:
		ErrText = "Unknown error";
		break;
	}
	return TRUE;
}

// @@Vijay TBD Move all error codes to a ASCII text file having different sections
//    .... depending upon the origin of the error
BOOL ErrorMessages::GetErrTxtS16F15(int ErrNo, string& ErrTexr) {
	switch (ErrNo) {
	case PARAMETERS_IMPROPERLY_SPECIFIED:
		ErrTexr = "Parameters improperly specified";
		break;
	case INSUFFICIENT_PARAMETERS_SPECIFIED:
		ErrTexr = "Insufficient parameters specified";
		break;
	case UNSUPPORTED_OPTION_REQUESTED:
		ErrTexr = "Unsupported option requested";
		break;
	case BUSY_NOT_QUEUE_OR_QUEUE_FULL:
		ErrTexr = "Busy (no queueing or queue full)";
		break;
	case OBJECT_IDENTIFIER_IN_USE:
		ErrTexr = "Object identifier in use";
		break;
	case RECIPE_NOT_FOUND:
		ErrTexr = "Recipe Not Found";
		break;
	case NO_PRJOB_IN_QUEUE:
		ErrTexr = "No PRJob in queue";
		break;
	default:
		ErrTexr = "Unknown error";
		break;
	}
	return TRUE;
}

BOOL MessageS3F27::Validate() {
	UCHAR PTN;
	if ((MsgRef.size() != 1) ||
		(!MsgRef[0].IsList()) ||
		(MsgRef[0].size() != 2)) {
		return FALSE;
	}
	if ((!MsgRef[0][0].GetUInt1(PortAction)) ||
		(!MsgRef[0][1].IsList())) {
		return FALSE;
	}
	BYTE stPort[3];
	Ports.clear();
	for (int iPort = 0; iPort < int(MsgRef[0][1].size()); iPort++) {
		if (MsgRef[0][1][iPort].GetUInt1(PTN)) {
			Ports.push_back(PTN);
		}
		else if (MsgRef[0][1][iPort].GetBinary(stPort, 2)) {
			PTN = stPort[0];
			Ports.push_back(PTN);
		}
		else {
			return FALSE;
		}
	}
	return TRUE;
}

BOOL MessageS5F3::Validate() {
	if ((MsgRef.size() != 1) ||
		(!MsgRef[0].IsList()) ||
		(MsgRef[0].size() != 2)) {
		return FALSE;
	}
	string Temp;
	if (!MsgRef[0][0].GetBinary(Temp)) {
		return FALSE;
	}
	UCHAR ALED = atoi(Temp.c_str());
	bEnable = (ALED & 0X80) ? true : false;
	if ((MsgRef[0][1].IsAInt())) {
		if (MsgRef[0][1].IsZeroLen()) {
			bAll = true;
		}
		else if (!MsgRef[0][1].GetAInt(AlarmID)) {
			return FALSE;
		}
	}
	return TRUE;
}

BOOL MessageS5F5::Validate() {
	if (!MsgRef.size()) {
		return FALSE;
	}
	UINT AlarmID;
	if (MsgRef[0].IsAInt()) {
		if (!MsgRef[0].IsZeroLen()) {
			MsgRef[0].GetAInt(AlarmID);
			ALIDs.push_back(AlarmID);
		}
		return TRUE;
	}
	if (MsgRef[0].IsArray()) {
		ALIDs.clear();
		for (int iAlarm = 0; iAlarm < int(MsgRef[0].size()); iAlarm++) {
			if (!MsgRef[0][iAlarm].GetAInt(AlarmID)) {
				return FALSE;
			}
			ALIDs.push_back(AlarmID);
		}
		return TRUE;
	}
	return FALSE;
}

BOOL MessageS7F1::Validate() {
	if ((MsgRef.size() != 1) ||
		(!MsgRef[0].IsList()) ||
		(MsgRef[0].size() != 2)) {
		return FALSE;
	}
	if ((!MsgRef[0][0].GetASCII(PPID)) ||
		(!MsgRef[0][1].GetAInt(PPLen))) {
		return FALSE;
	}
	return TRUE;
}

BOOL MessageS7F3::Validate() {
	if ((MsgRef.size() != 1) ||
		(!MsgRef[0].IsList()) ||
		(MsgRef[0].size() != 2)) {
		return FALSE;
	}
	if ((!MsgRef[0][0].GetASCII(PPID)) ||
		(!MsgRef[0][1].IsBinary())) {
		return FALSE;
	}
	PPLen = MsgRef[0][1].GetLength();
	if (PPBODY) {
		delete[] PPBODY;
		PPBODY = NULL;
	}
	PPBODY = new BYTE[PPLen];
	if (!MsgRef[0][1].GetBinary(PPBODY, PPLen)) {
		return FALSE;
	}
	return TRUE;
}

BOOL MessageS7F5::Validate() {
	if ((MsgRef.size() != 1) ||
		(!MsgRef[0].GetASCII(PPID))) {
		return FALSE;
	}
	return TRUE;
}

BOOL MessageS7F2::Validate() {
	PPGNT = 0;
	if ((MsgRef.size() != 1) ||
		(!MsgRef[0].GetBinary(&PPGNT, 1))) {
		return FALSE;
	}
	return TRUE;
}

//@@ Vijay#3June$02 New message added
BOOL MessageS7F17::Validate() {
	string sTemp;
	if ((MsgRef.size() != 1) ||
		(!MsgRef[0].IsList())) {
		return FALSE;
	}
	PPIDs.clear();
	for (int iPPID = 0; iPPID < int(MsgRef[0].size()); iPPID++) {
		if (!MsgRef[0][iPPID].GetASCII(sTemp)) {
			return FALSE;
		}
		PPIDs.push_back(sTemp);
	}
	return TRUE;
}

//@@ Vijay#3June$02 New message added
BOOL MessageS7F19::Validate() {
	if (MsgRef.size() == 0) {
		return TRUE;
	}
	return FALSE;
}

//@@ Vijay#1July$02 New message added
BOOL MessageS6F19::Validate() {
	if ((MsgRef.size() != 1) ||
		(!MsgRef[0].GetAInt(RptId))) {
		return FALSE;
	}
	return TRUE;
}

BOOL MessageS18F1::Validate() {
	if ((MsgRef.size() != 1) ||
		(!MsgRef[0].IsList()) ||
		(MsgRef[0].size() != 2)) {
		return FALSE;
	}
	if ((!MsgRef[0][0].GetASCII(TargetID)) ||
		(!MsgRef[0][1].IsList())) {
		return FALSE;
	}
	int nAttrib = MsgRef[0][1].size();
	string AttrID;
	AttrIDs.clear();
	for (int nIndex = 0; nIndex < nAttrib; nIndex++) {
		if (!MsgRef[0][1][nIndex].GetASCII(AttrID)) {
			return FALSE;
		}
		AttrIDs.push_back(AttrID);
	}
	return TRUE;
}

BOOL MessageS18F3::Validate() {
	int nNum;
	if ((MsgRef.size() != 1) ||
		(!MsgRef[0].IsList()) ||
		(MsgRef[0].size() != 2)) {
		return FALSE;
	}
	if (!MsgRef[0][0].GetASCII(TargetID)) {
		return FALSE;
	}
	if (0 == (nNum = MsgRef[0][1].size())) {
		return FALSE;
	}
	string Data;
	AttrIDs.clear();
	AttrVals.clear();
	for (int nIndex = 0; nIndex < nNum; nIndex++) {
		if (2 != MsgRef[0][1][nIndex].size()) {
			return FALSE;
		}
		MsgRef[0][1][nIndex][0].GetASCII(Data);
		AttrIDs.push_back(Data);
		MsgRef[0][1][nIndex][1].GetData(Data);
		AttrVals.push_back(Data);
	}
	return true;
}

BOOL MessageS18F5::Validate() {
	if ((1 != size()) || (!MsgRef[0].IsList()) || (3 != MsgRef[0].size())) {
		return false;
	}
	if ((!MsgRef[0][0].GetASCII(this->TargetID)) || (!MsgRef[0][1].GetASCII(this->DatSeg)) ||
		(!MsgRef[0][2].GetUInt4(this->DataLength))) {
		return false;
	}
	return true;
}

BOOL MessageS18F7::Validate() {
	if ((1 != size()) || (!MsgRef[0].IsList()) || (4 != MsgRef[0].size())) {
		return false;
	}
	if ((!MsgRef[0][0].GetASCII(this->TargetID)) || (!MsgRef[0][1].GetASCII(this->DatSeg)) ||
		(!MsgRef[0][2].GetUInt4(this->DataLength)) || (!MsgRef[0][3].GetASCII(this->Data))) {
		return false;
	}
	return true;
}

BOOL MessageS18F9::Validate() {
	if ((1 != size()) || (!MsgRef[0].GetASCII(this->TargetID))) {
		return false;
	}
	return true;
}

BOOL MessageS18F11::Validate() {
	if ((1 != size()) || (!MsgRef[0].IsList()) || (2 != MsgRef[0].size())) {
		return false;
	}
	if ((!MsgRef[0][0].GetASCII(this->TargetID)) || (!MsgRef[0][1].GetASCII(this->MID))) {
		return false;
	}
	return true;
}

BOOL MessageS18F13::Validate() {
	if ((1 != size()) || (!MsgRef[0].IsList()) || (3 != MsgRef[0].size())) {
		return false;
	}
	if ((!MsgRef[0][0].GetASCII(this->TargetID)) || (!MsgRef[0][1].GetASCII(this->SSCMD))) {
		return false;
	}
	if (!MsgRef[0][2].IsList()) {
		return false;
	}
	int nCps = MsgRef[0][2].size();
	string CmdPar;
	CpList.clear();
	for (int nIndex = 0; nIndex < nCps; nIndex++) {
		if (!MsgRef[0][2][nIndex].GetASCII(CmdPar)) {
			return false;
		}
		this->CpList.push_back(CmdPar);
	}
	return true;
}

BOOL MessageS2F37::Validate() {
	if ((MsgRef.size() != 1) ||
		(!MsgRef[0].IsList()) ||
		(MsgRef[0].size() != 2) ||
		(!MsgRef[0][1].IsList())) {
		return FALSE;
	}
	if (!MsgRef[0][0].GetBoolean(CEED)) {
		return FALSE;
	}
	UINT CEID;
	CEIDs.clear();
	for (int nIndex = 0; nIndex<int(MsgRef[0][1].size()); nIndex++) {
		if (!MsgRef[0][1][nIndex].GetAInt(CEID)) {
			return FALSE;
		}
		CEIDs.push_back(CEID);
	}
	return TRUE;
}

BOOL MessageS2F43::Validate() {
	BYTE StreamID = 0, FunctID = 0;
	SpDat Temp;
	if ((MsgRef.size() != 1) ||
		(!MsgRef[0].IsList())) {
		return FALSE;
	}
	UINT sz = MsgRef[0].size();
	StFuncDat.clear();
	for (UINT nIndex = 0; nIndex < sz; nIndex++) {
		if ((!MsgRef[0][nIndex].IsList()) ||
			(MsgRef[0][nIndex].size() != 2) ||
			(!MsgRef[0][nIndex][0].GetUInt1(Temp.StreamID)) ||
			(!MsgRef[0][nIndex][1].IsList())) {
			return FALSE;
		}
		Temp.FcnID.clear();
		for (int iFuncId = 0; iFuncId <int(MsgRef[0][nIndex][1].size()); iFuncId++) {
			if (!MsgRef[0][nIndex][1][iFuncId].GetUInt1(FunctID)) {
				return FALSE;
			}
			Temp.FcnID.push_back(FunctID);
		}
		StFuncDat.push_back(Temp);
	}
	return TRUE;
}

BOOL MessageS6F23::Validate() {
	if ((MsgRef.size() != 1) ||
		(!MsgRef[0].GetUInt1(RSDC))) {
		return FALSE;
	}
	return TRUE;
}

BOOL MessageS10F1::Validate() {
	if ((MsgRef.size() != 1) ||
		(!MsgRef[0].IsList()) ||
		(MsgRef[0].size() != 2)) {
		return FALSE;
	}
	string sTID;
	if ((!MsgRef[0][0].GetBinary(sTID)) || (!MsgRef[0][1].GetASCII(Text))) {
		return FALSE;
	}
	TID = atoi(sTID.c_str());
	return TRUE;
}

BOOL MessageS10F10::Validate() {
	if (MsgRef.size() != 1) {
		return FALSE;
	}
	if ((MsgRef[0].IsBinary()) ||
		(MsgRef[0].IsASCII()) ||
		(MsgRef[0].IsAInt())) {
		MsgRef[0].GetData(Text);
		return TRUE;
	}
	return FALSE;
}

BOOL MessageS16F25::Validate() {
	if ((MsgRef.size() != 1) ||
		(MsgRef[0].size() != 2) ||
		(MsgRef[0][0].size() == 0)) {
		return FALSE;
	}
	string PrID;
	PrJobs.clear();
	for (int nIndex = 0; nIndex<int(MsgRef[0][0].size()); nIndex++) {
		if (!MsgRef[0][0][nIndex].GetASCII(PrID)) {
			return FALSE;
		}
		PrJobs.push_back(PrID);
	}
	if (!MsgRef[0][1].GetBoolean(StartMethod)) {
		return FALSE;
	}
	return TRUE;
}

// @@Vijay#14June$02 New message added
bool SecsMessage::GetDataAt(int Itm, int SubItm, SecsItem::DataType Type, string& Data) {
	// Check for Parameters first
	if (Itm < 0 || SubItm < 0) {
		return false;
	}
	// Check top Tree first
	if (int(Items.size()) < (Itm + 1)) {
		return false;
	}
	// Check SubItem
	SecsItem* pItm = Items[Itm];
	if (SubItm >= pItm->NumContainedItems()) {
		return false;
	}
	return TypedExtractData(pItm->ContainedItems(SubItm), Type, Data);
}

bool SecsMessage::GetDataAt(int Itm, int SubItm, int SubSubItm, SecsItem::DataType Type, string& Data) {
	// Check for Parameters first
	if (Itm < 0 || SubItm < 0 || SubSubItm < 0) {
		return false;
	}
	// Check top Tree first
	if (Itm >= int(Items.size())) {
		return false;
	}
	// Check SubItem
	SecsItem* pItm = Items[Itm];
	if (SubItm >= pItm->NumContainedItems()) {
		return false;
	}
	SecsItem* pSubItm = pItm->ContainedItems(SubItm);
	if ((!pSubItm->IsList()) || (SubSubItm >= pSubItm->NumContainedItems())) {
		return false;
	}
	return TypedExtractData(pSubItm->ContainedItems(SubSubItm), Type, Data);
}

// @@Vijay#14June$02 New message added
bool SecsMessage::GetDataAt(int Itm, SecsItem::DataType Type, string& Data) {
	if (Itm < 0) {
		return false;
	}
	// Check top Tree first
	if (int(Items.size()) < (Itm + 1)) {
		return false;
	}
	return TypedExtractData(Items[Itm], Type, Data);
}

bool SecsMessage::GetDataAt(const std::vector < int >& LocList, SecsItem::DataType Type, string& Data) {
	SecsItem* pItem = GetItemAt(LocList);
	if (NULL == pItem) {
		return false;
	}
	return TypedExtractData(pItem, Type, Data);
}

// @@Vijay#14June$02 New message added
bool SecsMessage::TypedExtractData(SecsItem* pItm, SecsItem::DataType Type, string& Data) {
	if (!pItm) {
		return false;
	}
	//	ASSERT(!Data.size());
	switch (Type) {
	case SecsItem::ITEM_BINARY:
		if (!pItm->IsBinary()) {
			return false;
		}
		break;
	case SecsItem::ITEM_BOOLEAN:
		if (!pItm->IsBoolean()) {
			return false;
		}
		break;
	case SecsItem::ITEM_ASCII:
		if (!pItm->IsASCII()) {
			return false;
		}
		break;
	case SecsItem::ITEM_JIS:
		if (!pItm->IsJIS()) {
			return false;
		}
		break;
	case SecsItem::ITEM_INT8:
	case SecsItem::ITEM_INT4:
	case SecsItem::ITEM_INT2:
	case SecsItem::ITEM_INT1:
		if (!pItm->IsInt()) {
			return false;
		}
		break;
	case SecsItem::ITEM_UINT8:
	case SecsItem::ITEM_UINT4:
	case SecsItem::ITEM_UINT2:
	case SecsItem::ITEM_UINT1:
		if (!pItm->IsUInt()) {
			return false;
		}
		break;
	default:
		return false;
		break;
	}
	pItm->GetData(Data);
	return true;
}

bool SecsMessage::ExtractDataFromList(SecsItem* pList, int Ind, SecsItem::DataType Type, string& Data) {
	if ((NULL == pList) || (!pList->IsList()) || (Ind >= pList->NumContainedItems())) {
		return false;
	}
	return TypedExtractData(pList->ContainedItems(Ind), Type, Data);
}

bool SecsMessage::ExtractSimilarList(SecsItem* pList, SecsItem::DataType Type, std::vector < string >& Data) {
	if ((NULL == pList) || (!pList->IsList())) {
		return false;
	}
	Data.clear(); // Safety
	string TempStr;
	for (int nIndex = 0; nIndex < pList->NumContainedItems(); nIndex++) {
		if (!TypedExtractData(pList->ContainedItems(nIndex), Type, TempStr)) {
			Data.clear(); // Safety
			return false;
		}
		Data.push_back(TempStr);
	}
	return true;
}

bool SecsMessage::ExtractVarList(SecsItem* pList, std::vector < SecsItem::DataType > Type, std::vector < string >& Data) {
	if ((NULL == pList) || (!pList->IsList())) {
		return false;
	}
	Data.clear(); // Safety
	string TempStr;
	for (int nIndex = 0; nIndex < pList->NumContainedItems(); nIndex++) {
		if (!TypedExtractData(pList->ContainedItems(nIndex), Type[nIndex], TempStr)) {
			Data.clear(); // Safety
			return false;
		}
		Data.push_back(TempStr);
	}
	return true;
}

// Note the Only illegal value for number can be -1 because List can be Zero Length
bool SecsMessage::GetNumItemsAt(const std::vector < int >& Location, int& Num) {
	Num = -1;
	SecsItem* pItem = GetItemAt(Location);
	if (NULL == pItem) {
		return false;
	}
	return GetListSize(pItem, Num);
}

bool SecsMessage::GetListSize(SecsItem* pItem, int& Num) {
	if (NULL == pItem) {
		return false;
	}
	if (!pItem->IsList()) {
		return false;
	}
	Num = pItem->NumContainedItems();
	return true;
}

SecsItem* SecsMessage::GetItemAt(const std::vector < int >& LocList) {
	if ((LocList.empty()) || (LocList[0] >= int(Items.size()))) {
		return NULL;
	}
	if (1 == LocList.size()) {
		return Items[LocList[0]];
	}
	SecsItem* pItem = Items[LocList[0]];
	if (!pItem->IsList()) {
		return NULL;
	}
	for (int nIndex = 1; nIndex < int(LocList.size() - 1); nIndex++) {
		SecsItem* pTemp = pItem->ContainedItems(LocList[nIndex]);
		if ((NULL == pTemp) || (!pTemp->IsList())) {
			return NULL;
		}
		pItem = pTemp;
	}
	return pItem->ContainedItems(LocList[LocList.size() - 1]);
}

bool SecsMessage::CheckListSize(SecsItem* pItem, int Min, int Max) {
	if ((NULL == pItem) || (!pItem->IsList())) {
		return false;
	}
	int nSize = pItem->NumContainedItems();
	return ((nSize >= Min) && (nSize <= Max));
}

bool SecsMessage::CheckListSize(const std::vector < int >& LocList, int Min, int Max) {
	SecsItem* pItem = GetItemAt(LocList);
	return CheckListSize(pItem, Min, Max);
}

bool SecsMessage::CheckTextSize(SecsItem* pItem, int Min, int Max) {
	if ((NULL == pItem) || (!pItem->IsASCII())) {
		return false;
	}
	int nSize = pItem->GetLength();
	return ((nSize >= Min) && (nSize <= Max));
}

bool SecsMessage::CheckListStruct(SecsItem* pList, const std::vector < SecsItem::DataType >& Type) {
	if ((NULL == pList) || (!pList->IsList()) || (Type.size() != pList->NumContainedItems())) {
		return false;
	}
	int nItems = pList->NumContainedItems();
	for (int nIndex = 0; nIndex < nItems; nIndex++) {
		SecsItem* pItm = pList->ContainedItems(nIndex);
		if (!pItm->IsTypeEq(Type[nIndex])) {
			return false;
		}
	}
	return true;
}

// Dual responsibility
bool SecsMessage::CheckListStruct(const std::vector < int >& LocList, const std::vector < SecsItem::DataType >& Type) {
	SecsItem* pItem = GetItemAt(LocList);
	return CheckListStruct(pItem, Type);
}

bool SecsMessage::ExtractSimilarList(const std::vector < int >& LocList, const SecsItem::DataType Type, std::vector < string >& Data) {
	SecsItem* pItem = GetItemAt(LocList);
	return ExtractSimilarList(pItem, Type, Data);
}

bool SecsMessage::CheckItemStruct(const std::vector < SecsItem::DataType >& Type) {
	if (Items.size() != Type.size()) {
		return false;
	}
	for (int nIndex = 0; nIndex < int(Type.size()); nIndex++) {
		if (!Items[nIndex]->IsTypeEq(Type[nIndex])) {
			return false;
		}
	}
	return true;
}

bool SecsMessage::GetListSizeAt(const std::vector < int >& LocList, int& Num) {
	Num = -1;
	if (LocList.empty()) {
		Num = Items.size();
		return true;
	}
	SecsItem* pItem = GetItemAt(LocList);
	if ((NULL != pItem) && (pItem->IsList())) {
		Num = pItem->NumContainedItems();
		return true;
	}
	return false;
}

void SecsMessage::Log(CString msg, short lvl, CString src) {
	if (src.GetLength()) {
		pGDev->LogFile.Log(msg, lvl, src);
	}
	else {
		pGDev->LogFile.Log(msg, lvl, "SecsMessage");
	}
	if (lvl <= 1) {
		pGDev->LogFile.LogLocal(msg);
	}
}

void SecsMessage::LogInfo(string& Msg) {
	Log(Msg.c_str(), 10);
}

void SecsMessage::LogWarn(string& Msg) {
	Log(Msg.c_str(), 2);
}

void SecsMessage::LogErr(string& Msg) {
	Log(Msg.c_str(), 2);
}

void SecsMessage::LogFatalErr(string& Msg) {
	Log(Msg.c_str(), 0);
}

BOOL MessageS1F11::Validate() {
	string MsgName = "(S1F11)", Temp;
	if (MsgRef[0].IsList()) {
		UINT SVID;
		MsgName += " SVIDs : ";
		char Buf[52];
		SVIDs.clear();
		for (UINT iSVID = 0; iSVID < UINT(MsgRef[0].size()); iSVID++) {
			SecsItem& IRef = MsgRef[0][iSVID];
			if (!IRef.GetAInt(SVID)) {
				IRef.TypeString(Temp);
				sprintf(Buf, "Incorrect format (%s) At %d", Temp.c_str(), iSVID + 1);
				MsgName += Buf;
				LogErr(MsgName);
				return FALSE;
			}
			sprintf(Buf, " %d ,", SVID);
			MsgName += Buf;
			SVIDs.push_back(SVID);
		}
		LogInfo(MsgName);
		return TRUE;
	}
	MsgRef[0].TypeString(Temp);
	MsgName += "Incorrect type found " + Temp;
	LogErr(MsgName);
	return FALSE;
}

BOOL MessageS1F13::Validate() {
	string MsgName = "(S1F13)";
	if ((MsgRef.size() == 1) &&
		(MsgRef[0].IsList()) &&
		(MsgRef[0].size() == 0)) {
		return TRUE;
	}
	MsgName += "Incorrect format  ";
	LogErr(MsgName);
	return FALSE;
}

BOOL MessageS1F15::Validate() {
	string MsgName = "(S1F15)";
	if (0 == MsgRef.size()) {
		return TRUE;
	}
	MsgName += "Incorrect format  ";
	LogErr(MsgName);
	return FALSE;
}

BOOL MessageS1F17::Validate() {
	string MsgName = "(S1F17)";
	if (0 == MsgRef.size()) {
		return TRUE;
	}
	MsgName += "Incorrect format  ";
	LogErr(MsgName);
	return FALSE;
}

BOOL MessageS1F11::GetSvidList(std::vector<UINT>& SvList) {
	SvList = SVIDs;
	return TRUE;
}

BOOL SecsMessage::CheckMessage(BYTE* LenBytes, BYTE* MessageBytes) {
	ByteSwap(LenBytes, 4);
	UINT nLen;
	memcpy(&nLen, LenBytes, 4);
	nLen -= 10;
	string Message;
	if (!DecodeMessage(MessageBytes, nLen)) {
		Message = "Message Decode Error";
		LogErr(Message);
		return FALSE;
	}
	if (!Validate()) {
		Message = "Validation Error";
		LogErr(Message);
		return FALSE;
	}
	return TRUE;
}

BOOL MessageS5F7::Validate() {
	if (0 == MsgRef.size()) {
		return TRUE;
	}
	return FALSE;
}
/*
 *	Static helper implementation
 */
static BOOL ExtractRecipeParams(SecsItem& Param, PrjobRepNew& PrJob) {
	ParamNew Temp[26];
	int Index;
	for (Index = 0; Index < 26; Index++) {
		Temp[Index].SlotNo = 0;
		Temp[Index].ScanNo = 0;
		Temp[Index].FThick = 0.f;
		Temp[Index].WThick = 0.f;
	}
	if (!Param.IsList()) {
		return FALSE;
	}
	int nNumPar = Param.size();
	string ParamName, ParamVal;
	BOOL bParamOk = FALSE;
	int SlotNo = 0;
	short ScanNo = 0; // 0 - No scan number override, availability of film thickness will decide whether it is a first or second scan [9/10/2007 LYF]
	for (Index = 0; Index < nNumPar; Index++) {
		if ((Param[Index].size() != 2) ||
			(!Param[Index][0].GetASCII(ParamName)) ||
			(!Param[Index][1].GetASCII(ParamVal))) {
			return FALSE;
		}
		if (0 == ParamName.find("FILMTHICKNESS")) {
			SlotNo = atoi(&(ParamName.c_str()[13]));
			if ((SlotNo > 0) && (SlotNo < 27)) {
				float fthick = atof(ParamVal.c_str());
				Temp[SlotNo - 1].FThick = fthick;
				Temp[SlotNo - 1].SlotNo = SlotNo;
				if (!fthick) {
					Temp[SlotNo - 1].ScanNo = 1;
				}
				else {
					Temp[SlotNo - 1].ScanNo = 2;
				}
				bParamOk = TRUE;
			}
		}
		else if (0 == ParamName.find("WAFERTHICKNESS")) {
			SlotNo = atoi(&(ParamName.c_str()[14]));
			if ((SlotNo > 0) && (SlotNo < 27)) {
				Temp[SlotNo - 1].WThick = atof(ParamVal.c_str());
				Temp[SlotNo - 1].SlotNo = SlotNo;
			}
		}
		else if (0 == ParamName.find("SCANNUMBER")) {
			ScanNo = atoi(ParamVal.c_str()); // scan number override detected [9/10/2007 LYF]
		}
		// ignor other recipe parameters [9/10/2007 LYF]
		//else
		//{
		//	return FALSE;
		//}
	}
	if (SlotNo) {
		// Scan number override [9/10/2007 LYF]
		for (Index = 0; Index < 26; Index++) {
			if (Temp[Index].SlotNo) {
				Temp[Index].ScanNo = ScanNo;
			}
		}
	}
	PrJob.WaferParams.clear(); // Safety measure
	for (Index = 0; Index < 26; Index++) {
		if (Temp[Index].SlotNo) {
			PrJob.WaferParams.push_back(Temp[Index]);
		}
	}
	return (bParamOk) ? TRUE : FALSE;
}

void SecsMessage::LogFormatError(const CString& src) {
	Log("Format Error", 2, src);
}