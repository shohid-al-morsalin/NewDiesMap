// S2Msg.cpp: implementation of the CS2Msg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "S2Msg.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CS2Msg::CS2Msg() {
	DataLen = 0;
}

CS2Msg::~CS2Msg() {
	for (UINT nIndex = 0; nIndex < Items.size(); nIndex++) {
		delete Items[nIndex];
	}
	Items.clear();
}

CLenHdr CS2Msg::TypeOf(BYTE* byte) {
	CLenHdr Hdr;
	Hdr.Type = BYTE(*byte >> 2);
	Hdr.NumLenBytes = BYTE(*byte & 3);
	byte++;
#ifdef _DEBUG
	if ((Hdr.NumLenBytes < 0) || (Hdr.NumLenBytes > 3)) {
		ASSERT(0);
		CLenHdr tmp;
		return tmp;
	}
#endif
	switch (Hdr.NumLenBytes) {
	case 1:
		Hdr.DataLen = *byte;
		break;
	case 2:
		Hdr.DataLen = (int(*byte) << 8) + *(byte + 1);
		break;
	case 3:
		Hdr.DataLen = (int(*byte) << 16) + (int(*(byte + 1) << 8)) + *(byte + 2);
		break;
	}
	Hdr.OK = TRUE;
	return Hdr;
}

UINT CS2Msg::AtoUint(unsigned char* pData) {
	unsigned int retc = (UINT)pData[0] << 24 | (UINT)pData[1] << 16 | (UINT)pData[2] << 8 | (UINT)pData[3];
	return retc;
}

SecsItem* CS2Msg::MakeBinary(BYTE* pByte, UINT& nLen) {
	return new SecsItemBinary(pByte, nLen);
}

SecsItem* CS2Msg::MakeBoolean(BYTE* pByte, UINT& nLen) {
	BYTE* String = new BYTE[nLen + 1];
	memcpy(String, pByte, nLen);
	String[nLen] = 0;
	SecsItem* pItem = new SecsItemBoolean(String[0]);
	delete[] String;
	return pItem;
}

SecsItem* CS2Msg::MakeASCII(BYTE* pByte, UINT& nLen) {
	char* String = new char[nLen + 1];
	memcpy(String, pByte, nLen);
	String[nLen] = 0;
	SecsItem* pItem = new SecsItemASCII(string(String));
	delete[] String;
	return pItem;
}

void CS2Msg::ByteSwap(void* pVoid, int nNumBytes) {
	BYTE* pByte = (BYTE*)pVoid;
	int mid = nNumBytes / 2;
	BYTE Temp;
	for (int nIndex = 0; nIndex < mid; nIndex++) {
		Temp = pByte[nNumBytes - 1 - nIndex];
		pByte[nNumBytes - 1 - nIndex] = pByte[nIndex];
		pByte[nIndex] = Temp;
	}
}

SecsItem* CS2Msg::MakeInt(BYTE* pByte, UINT& nLen) {
	BYTE Dummy[32];
	if (nLen >= sizeof(Dummy)) {
		ASSERT(0);
		return NULL;
	}
	memcpy(Dummy, pByte, nLen);
	ByteSwap(Dummy, nLen);
	switch (nLen) {
	case 1:
		return new SecsItemInt1((char)pByte[0]);
		break;
	case 2:
	{
		short Int2;
		memcpy(&Int2, Dummy, nLen);
		return new SecsItemInt2(Int2);
	}
	break;
	case 4:
	{
		int Int4;
		memcpy(&Int4, Dummy, nLen);
		return new SecsItemInt4(Int4);
	}
	break;
	case 8:
	{
		__int64 Int8;
		memcpy(&Int8, Dummy, nLen);
		return new SecsItemInt8(Int8);
	}
	break;
	default:
		ASSERT(0);
		break;
		;
	}
	return NULL;
}

SecsItem* CS2Msg::MakeUInt(BYTE* pByte, UINT& nLen) {
	BYTE Dummy[32];
	if (nLen >= sizeof(Dummy)) {
		ASSERT(0);
		return NULL;
	}
	memcpy(Dummy, pByte, nLen);
	ByteSwap(Dummy, nLen);
	switch (nLen) {
	case 1:
		return new SecsItemUInt1(pByte[0]);
		break;
	case 2:
	{
		USHORT UInt2;
		memcpy(&UInt2, Dummy, nLen);
		return new SecsItemUInt2(UInt2);
	}
	break;
	case 4:
	{
		UINT UInt4;
		memcpy(&UInt4, Dummy, nLen);
		return new SecsItemUInt4(UInt4);
	}
	break;
	case 8:
	{
		UINT64 UInt8;
		memcpy(&UInt8, Dummy, nLen);
		return new SecsItemUInt8(UInt8);
	}
	break;
	default:
		ASSERT(0);
		break;
	}
	return NULL;
}

SecsItem* CS2Msg::MakeJIS(BYTE* pByte, UINT& nLen) {
	return new SecsItemBinary(pByte, nLen);
}

SecsItem* CS2Msg::MakeUnicode(BYTE* pByte, UINT& nLen) {
	return new SecsItemBinary(pByte, nLen);
}

SecsItem* CS2Msg::MakeFloat8(BYTE* pByte, UINT& nLen) {
	double Dummy;
	memcpy(&Dummy, pByte, nLen);
	ByteSwap(&Dummy, nLen);
	return new SecsItemFloat8(Dummy);
}

SecsItem* CS2Msg::MakeFloat4(BYTE* pByte, UINT& nLen) {
	float Dummy;
	memcpy(&Dummy, pByte, nLen);
	ByteSwap(&Dummy, nLen);
	return new SecsItemFloat8(Dummy);
}

SecsItem* CS2Msg::MakeList(BYTE* pByte, UINT& nLen) {
	CLenHdr Hdr;
	SecsItemList* pList = new SecsItemList;
	int nIndex = 0;
	for (int i = 0; i < int(nLen); i++) {
		Hdr = TypeOf(&pByte[nIndex]);
		int DataIndex = nIndex + Hdr.NumLenBytes + 1;
		switch (Hdr.Type) {
		case SecsItem::ITEM_LIST:
			pList->AddItem(MakeList(&(pByte[DataIndex]), Hdr.DataLen));
			nIndex += Hdr.DataLen;
			break;
		case SecsItem::ITEM_BINARY:
			pList->AddItem(MakeBinary(&(pByte[DataIndex]), Hdr.DataLen));
			nIndex += Hdr.NumLenBytes + Hdr.DataLen + 1;
			break;
		case SecsItem::ITEM_BOOLEAN:
			pList->AddItem(MakeBoolean(&(pByte[DataIndex]), Hdr.DataLen));
			nIndex += Hdr.NumLenBytes + Hdr.DataLen + 1;
			break;
		case SecsItem::ITEM_ASCII:
			pList->AddItem(MakeASCII(&(pByte[DataIndex]), Hdr.DataLen));
			nIndex += Hdr.NumLenBytes + Hdr.DataLen + 1;
			break;
		case SecsItem::ITEM_INT8:
		case SecsItem::ITEM_INT4:
		case SecsItem::ITEM_INT2:
		case SecsItem::ITEM_INT1:
			pList->AddItem(MakeInt(&(pByte[DataIndex]), Hdr.DataLen));
			nIndex += Hdr.NumLenBytes + Hdr.DataLen + 1;
			break;
		case SecsItem::ITEM_UINT8:
		case SecsItem::ITEM_UINT4:
		case SecsItem::ITEM_UINT2:
		case SecsItem::ITEM_UINT1:
			pList->AddItem(MakeUInt(&(pByte[DataIndex]), Hdr.DataLen));
			nIndex += Hdr.NumLenBytes + Hdr.DataLen + 1;
			break;
		case SecsItem::ITEM_JIS:
			pList->AddItem(MakeJIS(&(pByte[DataIndex]), Hdr.DataLen));
			nIndex += (Hdr.NumLenBytes + Hdr.DataLen + 1);
			break;
		case SecsItem::ITEM_UNICODE:
			pList->AddItem(MakeUnicode(&(pByte[DataIndex]), Hdr.DataLen));
			nIndex += (Hdr.NumLenBytes + Hdr.DataLen + 1);
			break;
		case SecsItem::ITEM_FLOAT4:
			pList->AddItem(MakeFloat4(&(pByte[DataIndex]), Hdr.DataLen));
			nIndex += (Hdr.NumLenBytes + Hdr.DataLen + 1);
			break;
		case SecsItem::ITEM_FLOAT8:
			pList->AddItem(MakeFloat8(&(pByte[DataIndex]), Hdr.DataLen));
			nIndex += (Hdr.NumLenBytes + Hdr.DataLen + 1);
			break;
		default:
			ASSERT(0);
			break;
		}
	}
	nLen = nIndex + 2;
	return pList;
}

BOOL CS2Msg::Decode(GEM_HEADER* pHeader) {
	Items.clear();

	if (!pHeader->m_Stream && !pHeader->m_Function) {
		// No message to decode [10/29/2002]
		// Probably PRIMARY message from equipment [10/29/2002]
		return TRUE;
	}

	BYTE* pByte = pHeader->m_Message;
	DataLen = AtoUint(pHeader->m_DataLen) - 10;

	CLenHdr Hdr;
	for (int nIndex = 0; nIndex < int(DataLen); ) {
		Hdr = TypeOf(&pByte[nIndex]);
		int DataIndex = nIndex + Hdr.NumLenBytes + 1;
		switch (Hdr.Type) {
		case SecsItem::ITEM_LIST:
			Items.push_back(MakeList(&(pByte[DataIndex]), Hdr.DataLen));
			nIndex += Hdr.DataLen;
			break;
		case SecsItem::ITEM_BINARY:
			Items.push_back(MakeBinary(&(pByte[DataIndex]), Hdr.DataLen));
			nIndex += (Hdr.NumLenBytes + Hdr.DataLen + 1);
			break;
		case SecsItem::ITEM_BOOLEAN:
			Items.push_back(MakeBoolean(&(pByte[DataIndex]), Hdr.DataLen));
			nIndex += (Hdr.NumLenBytes + Hdr.DataLen + 1);
			break;
		case SecsItem::ITEM_ASCII:
			Items.push_back(MakeASCII(&(pByte[DataIndex]), Hdr.DataLen));
			nIndex += (Hdr.NumLenBytes + Hdr.DataLen + 1);
			break;
		case SecsItem::ITEM_INT8:
		case SecsItem::ITEM_INT4:
		case SecsItem::ITEM_INT2:
		case SecsItem::ITEM_INT1:
			Items.push_back(MakeInt(&(pByte[DataIndex]), Hdr.DataLen));
			nIndex += (Hdr.NumLenBytes + Hdr.DataLen + 1);
			break;
		case SecsItem::ITEM_UINT8:
		case SecsItem::ITEM_UINT4:
		case SecsItem::ITEM_UINT2:
		case SecsItem::ITEM_UINT1:
			Items.push_back(MakeUInt(&(pByte[DataIndex]), Hdr.DataLen));
			nIndex += (Hdr.NumLenBytes + Hdr.DataLen + 1);
			break;
		case SecsItem::ITEM_JIS:
			Items.push_back(MakeJIS(&(pByte[DataIndex]), Hdr.DataLen));
			nIndex += (Hdr.NumLenBytes + Hdr.DataLen + 1);
			break;
		case SecsItem::ITEM_UNICODE:
			Items.push_back(MakeUnicode(&(pByte[DataIndex]), Hdr.DataLen));
			nIndex += (Hdr.NumLenBytes + Hdr.DataLen + 1);
			break;
		case SecsItem::ITEM_FLOAT4:
			Items.push_back(MakeFloat4(&(pByte[DataIndex]), Hdr.DataLen));
			nIndex += (Hdr.NumLenBytes + Hdr.DataLen + 1);
			break;
		case SecsItem::ITEM_FLOAT8:
			Items.push_back(MakeFloat8(&(pByte[DataIndex]), Hdr.DataLen));
			nIndex += (Hdr.NumLenBytes + Hdr.DataLen + 1);
			break;
		default:
			ASSERT(0);
			return FALSE;
			break;
		}
	}
	return TRUE;
}

UINT CS2Msg::MakeBinaryMessage(BYTE* pByte) {
	int nSize = 0;
	nTotLen = 0;
	for (UINT nIndex = 0; nIndex < Items.size(); nIndex++) {
		Items[nIndex]->PrintBinary(pByte);
		nSize = Items[nIndex]->GetSECSLength();
		pByte += nSize;
		nTotLen += nSize;
	}
	return nTotLen;
}

void CS2Msg::AddItem(SecsItem* pItem) {
	Items.push_back(pItem);
}

SecsItem* CS2Msg::GetItem(int idx) {
	return Items[idx];
}