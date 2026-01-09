// S2Msg.h: interface for the CS2Msg class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>
using namespace std;

#include "..\INC\SecsHead.h"
#include "..\GEM\SecsItem.h"

class CLenHdr {
public:
	BOOL OK;
	BYTE Type;
	BYTE NumLenBytes;
	UINT DataLen;

public:
	CLenHdr() {
		OK = FALSE;
		Type = 0;
		NumLenBytes = 0;
		DataLen = 0;
	};
};

class CS2Msg {
	UINT DataLen;
	UINT nTotLen;

protected:
	vector<SecsItem*> Items;

private:
	CLenHdr TypeOf(BYTE* byte);
	UINT AtoUint(unsigned char* pData);
	void ByteSwap(void* pVoid, int nNumBytes);
	SecsItem* MakeInt(BYTE* pByte, UINT& nLen);
	SecsItem* MakeUInt(BYTE* pByte, UINT& nLen);
	SecsItem* MakeList(BYTE* pByte, UINT& nLen);
	SecsItem* MakeBinary(BYTE* pByte, UINT& nLen);
	SecsItem* MakeBoolean(BYTE* pByte, UINT& nLen);	// Yuen: Need to extend to array of boolean [2002/10/29  15:05]
	SecsItem* MakeASCII(BYTE* pByte, UINT& nLen);
	SecsItem* MakeJIS(BYTE* pByte, UINT& nLen);
	SecsItem* MakeUnicode(BYTE* pByte, UINT& nLen);
	SecsItem* MakeFloat8(BYTE* pByte, UINT& nLen);
	SecsItem* MakeFloat4(BYTE* pByte, UINT& nLen);

public:
	void AddItem(SecsItem* pItem);
	UINT MakeBinaryMessage(BYTE* pByte);
	BOOL Decode(GEM_HEADER* pHeader);
	virtual BOOL Validate() { return true; }

public:
	SecsItem* GetItem(int idx);
	CS2Msg();
	virtual ~CS2Msg();
};