// SecsItem.h: interface for the SecsItem class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
#include <vector>
using namespace std;

#pragma warning(disable : 4786)

typedef unsigned __int64 UINT64;

class SecsItem {
	SecsItem& operator =(SecsItem&);
	SecsItem(SecsItem&);
public:
	static string TypeStr(short type);
	CString GetTypeStr();
	BOOL GetAInt(UINT& UVal);
	BOOL IsCollection();
	virtual BOOL IsZeroLen();
	enum DataType {
		UNDEFINED = -1,
		ITEM_LIST = 000,
		ITEM_ARRAY = 001,
		ITEM_BINARY = 010,
		ITEM_BOOLEAN = 011,
		ITEM_ASCII = 020,
		ITEM_JIS = 021,
		ITEM_UNICODE = 022,
		ITEM_INT8 = 030,
		ITEM_INT1 = 031,
		ITEM_INT2 = 032,
		ITEM_INT4 = 034,
		ITEM_FLOAT8 = 040,
		ITEM_FLOAT4 = 044,
		ITEM_UINT8 = 050,
		ITEM_UINT1 = 051,
		ITEM_UINT2 = 052,
		ITEM_UINT4 = 054
	};
	bool IsTypeEq(SecsItem::DataType Type) { return (eType == Type); }
	DataType GetType() { return eType; };
	virtual BOOL  AddItem(SecsItem* pItem) { return FALSE; };
	virtual int   GetSECSLength(void);
	virtual BOOL  IsList();
	virtual BOOL  IsBinary();
	virtual BOOL  IsBoolean();
	virtual BOOL  IsASCII();
	virtual BOOL  IsJIS();
	virtual BOOL  Is2ByteChar();
	BOOL  IsAInt();
	virtual BOOL  IsInt();
	virtual BOOL  IsInt8();
	virtual BOOL  IsInt4();
	virtual BOOL  IsInt2();
	virtual BOOL  IsInt1();
	virtual BOOL  IsUInt();
	virtual BOOL  IsUInt8();
	virtual BOOL  IsUInt4();
	virtual BOOL  IsUInt2();
	virtual BOOL  IsUInt1();
	virtual BOOL  IsFloat();
	virtual BOOL  IsFloat4();
	virtual BOOL  IsFloat8();
	virtual BOOL  IsArray();
	virtual bool  GetUInt(UINT&);

	virtual bool  GetBoolean(bool&);
	virtual bool  GetInt8(__int64&);
	virtual bool  GetInt4(int&);
	virtual bool  GetInt2(short&);
	virtual bool  GetInt1(char&);
	virtual bool  GetUInt8(__int64&);
	virtual bool  GetUInt4(UINT&);
	virtual bool  GetUInt2(USHORT&);
	virtual bool  GetUInt1(BYTE&);
	virtual bool  GetFloat4(float&);
	virtual bool  GetFloat8(double&);
	virtual bool  GetASCII(string&);
	virtual bool  GetBinary(string&);
	virtual bool  GetBinary(BYTE*, UINT Len);
	virtual int	  GetLength() { return -1; };
	virtual SecsItem* ContainedItems(int Index);
	virtual int NumContainedItems();
	virtual void  PrintBinary(BYTE* pByte) = 0;
	virtual void  RawCopy(BYTE* pByte) { return; };
	virtual void  PrintASCII() = 0;
	virtual void  PrintSML() = 0;
	virtual void  PrintSML(string&) = 0;
	virtual void  GetData(string&) = 0;
	virtual SecsItem* CloneSelf() = 0;
	SecsItem();

	virtual ~SecsItem();
	// Note: This default implementation can have either of the 2 choices
	//      ..  1) One that produces guaranteed crash or (Better for early detection of errors)
	//      ..  2) Does not crash but can be very confusing to user if he tries [] on
	//            .. anything other than list. Will be a bit of work to figure out.
	//            .. Also this allows a funny syntax like Item[1][12222][2][9][8][8][1]....[infinity]
	//            .. without a crash !
	virtual SecsItem& operator[](int nIndex) {
		/*
		ASSERT(0);
		return *((SecsItem*)NULL);
		*/
		return *this;
	}
	virtual int size() {
		return 0;
	}
	BOOL TypeString(string&);
	string TypeStr();
protected:
	string TypeStrSML();
	void ByteSwap(void* pVoid, int nNuymBytes);
	BYTE GetBytesReqd(int DataLen);
	BYTE MakeDataByte(DataType Data, BYTE DataLen);
	DataType eType;
	static int MsgLevel;
	static std::string Sep;
	void MakeDataLength(int DataLen, BYTE* pByte, int Len);
};

class SecsItemList :public SecsItem {
	// Data here
protected:
	vector<SecsItem*> Item;
public:
	SecsItem* CloneSelf();
	//	virtual BOOL  IsList();
	virtual BOOL  AddItem(SecsItem* pItem);
	virtual void  PrintBinary(BYTE* pByte);
	virtual void  PrintASCII();
	virtual void  PrintSML();
	virtual void  PrintSML(string&);
	virtual int	  GetLength(); // Number of items in this case
	virtual int	  GetSECSLength();
	virtual void  GetData(string&);
	virtual SecsItem* ContainedItems(int Index);
	virtual int NumContainedItems();
	// Only this item has default constructor
	virtual SecsItem& operator[](int nIndex) {
		if (nIndex < int(Item.size())) {
			return *(Item[nIndex]);
		}
		else {
			return SecsItem::operator[](nIndex);
		}
	};
	virtual int size() {
		return NumContainedItems();
	};
	SecsItemList();

	virtual ~SecsItemList();
};

class SecsItemUndefined :public SecsItem {
	// Data here
	virtual BOOL  AddItem(SecsItem* pItem) { return FALSE; }
	virtual void  PrintBinary(BYTE* pByte) { ASSERT(0); };
	virtual void  PrintASCII() { ASSERT(0); }
	virtual void  PrintSML() { ASSERT(0); }
	virtual void  PrintSML(string&) { ASSERT(0); }
	virtual int	  GetSECSLength() { ASSERT(0); return 0; }
	virtual void  GetData(string&) {}
public:
	SecsItem* CloneSelf();
	// Note : See note for Operator [] for SecsItem
	virtual SecsItem& operator[](int nIndex) {
		return *(this);
	};
	virtual int size() {
		return 0;
	};
	SecsItemUndefined() {}

	virtual ~SecsItemUndefined() {}
};

class SecsItemBinary :public SecsItem {
	// Data here
	BYTE* Item;
	int Len;
	virtual void  RawCopy(BYTE* pByte);
public:
	SecsItem* CloneSelf();
	//	virtual BOOL  IsBinary();
	virtual BOOL  AddItem(SecsItem* pItem);
	virtual void  PrintBinary(BYTE* pByte);
	virtual void  PrintASCII();
	virtual void  PrintSML();
	virtual void  PrintSML(string&);
	virtual int	  GetLength();
	virtual int	  GetSECSLength();

	explicit SecsItemBinary(BYTE* pByte, int nLen);
	virtual void  GetData(string&);
	virtual ~SecsItemBinary();
};

class SecsItemZeroLen :public SecsItem {
	virtual void  RawCopy(BYTE* pByte);
public:
	BOOL IsZeroLen();
	SecsItem* CloneSelf();
	virtual BOOL  AddItem(SecsItem* pItem);
	virtual void  PrintBinary(BYTE* pByte);
	virtual void  PrintASCII();
	virtual void  PrintSML();
	virtual void  PrintSML(string&);
	virtual int	  GetLength();
	virtual int	  GetSECSLength();

	explicit SecsItemZeroLen(DataType Typ);
	virtual void  GetData(string&);
	virtual ~SecsItemZeroLen();
};

// Yuen: Array of boolean is invalid [2002/10/29  15:04]
class SecsItemBoolean :public SecsItem {
	// Data here
	BOOL  Item;
	virtual void  RawCopy(BYTE* pByte);
public:
	SecsItem* CloneSelf();
	//	virtual BOOL  IsBoolean();
	virtual BOOL  AddItem(SecsItem* pItem);
	virtual void  PrintBinary(BYTE* pByte);
	virtual void  PrintASCII();
	virtual void  PrintSML();
	virtual void  PrintSML(string& Data);
	virtual int	  GetLength();
	virtual int	  GetSECSLength();
	explicit SecsItemBoolean(BOOL pByte);
	virtual void  GetData(string&);
	virtual ~SecsItemBoolean();
};

class SecsItemASCII :public SecsItem {
	// Data here
	string Item;
	virtual void  RawCopy(BYTE* pByte);
public:
	SecsItem* CloneSelf();
	//	virtual BOOL  IsASCII();
	virtual BOOL  AddItem(SecsItem* pItem);
	virtual void  PrintBinary(BYTE* pByte);
	virtual void  PrintASCII();
	virtual void  PrintSML();
	virtual void  PrintSML(string&);
	virtual int	  GetLength();
	virtual int	  GetSECSLength();
	explicit SecsItemASCII(const string& Rhs);
	SecsItemASCII(const int& ival);
	SecsItemASCII(const float& fval);
	virtual void  GetData(string&);
	virtual ~SecsItemASCII();
};

class SecsItemInt8 :public SecsItem {
	// Data here
	__int64 Item;
	virtual void  RawCopy(BYTE* pByte);
public:
	SecsItem* CloneSelf();
	//	virtual BOOL  IsInt8();
	virtual BOOL  AddItem(SecsItem* pItem);
	virtual void  PrintBinary(BYTE* pByte);
	virtual void  PrintASCII();
	virtual void  PrintSML();
	virtual void  PrintSML(string&);
	virtual int	  GetLength();
	virtual int	  GetSECSLength();
	explicit SecsItemInt8(__int64 Rhs);
	virtual void  GetData(string&);
	virtual ~SecsItemInt8();
};

class SecsItemInt1 :public SecsItem {
	// Data here
	char Item;
	virtual void  RawCopy(BYTE* pByte);
public:
	virtual SecsItem* CloneSelf();
	//	virtual BOOL  IsInt1();
	virtual BOOL  AddItem(SecsItem* pItem);
	virtual void  PrintBinary(BYTE* pByte);
	virtual void  PrintASCII();
	virtual void  PrintSML();
	virtual void  PrintSML(string&);
	virtual int	  GetLength();
	virtual int	  GetSECSLength();
	explicit SecsItemInt1(char Rhs);
	virtual void  GetData(string&);
	virtual ~SecsItemInt1();
};

class SecsItemInt2 :public SecsItem {
	// Data here
	short Item;
	virtual void  RawCopy(BYTE* pByte);
public:
	virtual SecsItem* CloneSelf();
	//	virtual BOOL  IsInt2();
	virtual BOOL  AddItem(SecsItem* pItem);
	virtual void  PrintBinary(BYTE* pByte);
	virtual void  PrintASCII();
	virtual void  PrintSML();
	virtual void  PrintSML(string&);
	virtual int	  GetLength();
	explicit SecsItemInt2(short Rhs);
	virtual int	  GetSECSLength();
	virtual void  GetData(string&);
	virtual ~SecsItemInt2();
};

class SecsItemInt4 :public SecsItem {
	// Data here
	int Item;
	virtual void  RawCopy(BYTE* pByte);
public:
	virtual SecsItem* CloneSelf();
	//	virtual BOOL  IsInt4();
	virtual BOOL  AddItem(SecsItem* pItem);
	virtual void  PrintBinary(BYTE* pByte);
	virtual void  PrintASCII();
	virtual void  PrintSML();
	virtual void  PrintSML(string&);
	virtual int	  GetLength();
	virtual int	  GetSECSLength();
	explicit SecsItemInt4(int Rhs);
	virtual void  GetData(string&);
	virtual ~SecsItemInt4();
};

class SecsItemFloat4 :public SecsItem {
	// Data here
	float Item;
	virtual void  RawCopy(BYTE* pByte);
public:
	virtual SecsItem* CloneSelf();
	//	virtual BOOL  IsFloat4();
	virtual BOOL  AddItem(SecsItem* pItem);
	virtual void  PrintBinary(BYTE* pByte);
	virtual void  PrintASCII();
	virtual void  PrintSML();
	virtual void  PrintSML(string&);
	virtual int  GetLength();
	virtual int	  GetSECSLength();
	explicit SecsItemFloat4(float Rhs);
	virtual void  GetData(string&);
	virtual ~SecsItemFloat4();
};

class SecsItemFloat8 :public SecsItem {
	// Data here
	double Item;
	virtual void  RawCopy(BYTE* pByte);
public:
	virtual SecsItem* CloneSelf();
	//	virtual BOOL  IsFloat8();
	virtual BOOL  AddItem(SecsItem* pItem);
	virtual void  PrintBinary(BYTE* pByte);
	virtual void  PrintASCII();
	virtual void  PrintSML();
	virtual void  PrintSML(string&);
	virtual int	  GetLength();
	virtual int	  GetSECSLength();
	explicit SecsItemFloat8(double Rhs);
	virtual void  GetData(string&);
	virtual ~SecsItemFloat8();
};

// Windows have no support for 8 byte unsigned integer !
class SecsItemUInt8 :public SecsItem {
	// Data here
	UINT64 Item;
	virtual void  RawCopy(BYTE* pByte);
public:
	virtual SecsItem* CloneSelf();
	//	virtual BOOL  IsUInt8();
	virtual BOOL  AddItem(SecsItem* pItem);
	virtual void  PrintBinary(BYTE* pByte);
	virtual void  PrintASCII();
	virtual void  PrintSML();
	virtual void  PrintSML(string&);
	virtual int	  GetLength();
	virtual int	  GetSECSLength();
	explicit SecsItemUInt8(UINT64 Rhs);
	virtual void  GetData(string&);
	virtual ~SecsItemUInt8();
};

class SecsItemUInt1 :public SecsItem {
	// Data here
	UCHAR Item;
	virtual void  RawCopy(BYTE* pByte);
public:
	virtual SecsItem* CloneSelf();
	//	virtual BOOL  IsUInt1();
	virtual BOOL  AddItem(SecsItem* pItem);
	virtual void  PrintBinary(BYTE* pByte);
	virtual void  PrintASCII();
	virtual void  PrintSML();
	virtual void  PrintSML(string&);
	virtual int	  GetLength();
	virtual int	  GetSECSLength();
	explicit SecsItemUInt1(UCHAR Rhs);
	virtual void  GetData(string&);
	virtual ~SecsItemUInt1();
};

class SecsItemUInt2 :public SecsItem {
	// Data here
	USHORT Item;
	virtual void  RawCopy(BYTE* pByte);
public:
	virtual SecsItem* CloneSelf();
	//	virtual BOOL  IsUInt2();
	virtual BOOL  AddItem(SecsItem* pItem);
	virtual void  PrintBinary(BYTE* pByte);
	virtual void  PrintASCII();
	virtual void  PrintSML();
	virtual void  PrintSML(string&);
	virtual int	  GetLength();
	virtual int	  GetSECSLength();
	explicit SecsItemUInt2(USHORT Rhs);
	virtual void  GetData(string&);
	virtual ~SecsItemUInt2();
};

class SecsItemUInt4 :public SecsItem {
	// Data here
	UINT Item;
	virtual void  RawCopy(BYTE* pByte);
public:
	virtual SecsItem* CloneSelf();
	//	virtual BOOL  IsUInt4();
	virtual BOOL  AddItem(SecsItem* pItem);
	virtual void  PrintBinary(BYTE* pByte);
	virtual void  PrintASCII();
	virtual void  PrintSML();
	virtual void  PrintSML(string&);
	virtual int	  GetLength();
	virtual int	  GetSECSLength();
	explicit SecsItemUInt4(UINT Rhs);
	virtual void  GetData(string&);
	virtual ~SecsItemUInt4();
};

/**
	Array is almost similar to List

**/
class SecsItemArray : public  SecsItemList {
	BOOL  AddItems(const vector<SecsItem*>&);
	explicit SecsItemArray();
public:
	SecsItem* CloneSelf();
	//	virtual BOOL  IsArray();
	virtual void  PrintBinary(BYTE* pByte);
	virtual void  PrintASCII();
	virtual void  PrintSML();
	virtual void  PrintSML(string&);
	virtual void  GetData(string&);
	virtual int	  GetLength();
	virtual int	  GetSECSLength();
	explicit SecsItemArray(vector< SecsItemUInt1* >&);
	explicit SecsItemArray(vector<SecsItemUInt2*>&);
	explicit SecsItemArray(vector<SecsItemUInt4*>&);
	explicit SecsItemArray(vector<SecsItemUInt8*>&);
	explicit SecsItemArray(vector<SecsItemInt1*>&);
	explicit SecsItemArray(vector<SecsItemInt2*>&);
	explicit SecsItemArray(vector<SecsItemInt4*>&);
	explicit SecsItemArray(vector<SecsItemInt8*>&);
	explicit SecsItemArray(vector<SecsItemBoolean*>&);
	explicit SecsItemArray(vector<SecsItemFloat4*>&);
	explicit SecsItemArray(vector<SecsItemFloat8*>&);
	virtual ~SecsItemArray();
};
