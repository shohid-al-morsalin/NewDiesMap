#pragma once

#include "IPC/Smol.h"

#include <map>
#include <vector>
//#include <algorithm>
//#include <string>
//using namespace std;

typedef std::vector<UINT> IDVec;

#include "SecsItem.h"

struct  ParamData {
	string ParamName;
	string ParamVal;
};

struct  EcidData {
	UINT ECID;
	string EcVal;
};
// Structure for report definitions

struct RptData {
	UINT RptId;
	std::vector<UINT> Vids;
};

struct LinkData {
	UINT CeId;
	std::vector<UINT> RptIds;
};

class SecsMessage {
	friend class Annotator;
	SecsItemUndefined m_DoNotCrash;
	SecsItem* MakeBinary(BYTE* pByte, int nLen);
	SecsItem* MakeBoolean(BYTE* pByte, int nLen);
	SecsItem* MakeList(BYTE* pByte, int& nLen);
	SecsItem* MakeInt1(BYTE* pByte, int nLen);
	SecsItem* MakeInt2(BYTE* pByte, int nLen);
	SecsItem* MakeInt4(BYTE* pByte, int nLen);
	SecsItem* MakeInt8(BYTE* pByte, int nLen);
	SecsItem* MakeInt(SecsItem::DataType Typ, BYTE* pByte, int nLen);
	SecsItem* MakeUInt1(BYTE* pByte, int nLen);
	SecsItem* MakeUInt2(BYTE* pByte, int nLen);
	SecsItem* MakeUInt4(BYTE* pByte, int nLen);
	SecsItem* MakeUInt8(BYTE* pByte, int nLen);
	SecsItem* MakeUInt(SecsItem::DataType Typ, BYTE* pByte, int nLen);
	SecsItem* MakeASCII(BYTE* pByte, int nLen);
	SecsItem* MakeJIS(BYTE* pByte, int nLen);
	SecsItem* MakeUnicode(BYTE* pByte, int nLen);
	SecsItem* MakeFloat8(BYTE* pByte, int nLen);
	SecsItem* MakeFloat4(BYTE* pByte, int nLen);
public:

	void ByteSwap(void* pVoid, int nNumBytes);
	void LogFormatError(const CString& src);
	BOOL CheckMessage(BYTE* LenBytes, BYTE* MessageBytes);
	void AddItem(SecsItem* pItem);
	SecsItem* GetItemAt(const std::vector<int>& LocList);
	SecsItem& operator[](int nIndex) {
		if (nIndex<int(Items.size())) {
			return *(Items[nIndex]);
		}
		else {
			return m_DoNotCrash;
		}
	}
	int size() {
		return Items.size();
	};
	// Set of Function for Accessing at perticular location in messsage tree
	/** Get Item from top level items */
	bool GetDataAt(int Itm, SecsItem::DataType Type, string& Data);

	/**Most Generic function for accessing Items data from Message*/
	bool GetDataAt(const std::vector<int>& Itm, SecsItem::DataType Type, string& Data);

	/** Probably not required but wait.... */
	bool GetDataAt(int Itm, int SubItm, SecsItem::DataType Type, string& Data);

	/** Probably not required but wait.... */
	bool GetDataAt(int Itm, int SubItm, int SubSubItm, SecsItem::DataType Type, string& Data);

	// General Utlity
	/** Get size of List at any position in Message */
	bool GetListSizeAt(const std::vector<int>& LocList, int& Num);

	/** Check types of all items in a in a list */
	bool CheckListStruct(SecsItem* pItem, const std::vector<SecsItem::DataType>& Type);

	/** Check types of all Top level items in a in a list */
	bool CheckItemStruct(const std::vector<SecsItem::DataType>& Type);

	/** Check types of all items in a list at any position in Message */
	bool CheckListStruct(const std::vector<int>& LocList, const std::vector<SecsItem::DataType>& Type);

	/**  Check size of List at any position in Message */
	bool CheckListSize(SecsItem* pItem, int Min, int Max);

	/**  Check size of List at any position in Message */
	bool CheckListSize(const std::vector<int>& LocList, int Min, int Max);

	/**  Check size of Test at any position in Message */
	bool CheckTextSize(SecsItem* pItem, int Min, int Max);

	/**  Check size of List May not be used directly */
	bool GetListSize(SecsItem* pItem, int& Num);

	/**  Check size of List May not be used directly */
	bool GetListSize(SecsItem& Item, int& Num) {
		return (GetListSize(&Item, Num));
	}

	/**  Get size of List at any position in Message */
	bool GetNumItemsAt(const std::vector<int>& Location, int& Num);

	/**  Get data from a list of perticular item of interest (at Ind)*/
	bool ExtractDataFromList(SecsItem* pList, int Ind, SecsItem::DataType Type, string& Data);

	/**  Get data from a list when all items are identical */
	bool ExtractSimilarList(SecsItem* pList, const SecsItem::DataType Type, std::vector<string>& Data);

	/**  Get data from a list when all items are identical */
	bool ExtractSimilarList(const std::vector<int>& LocList, const SecsItem::DataType Type, std::vector<string>& Data);

	/**  Get data from a list when all items are different */
	bool ExtractVarList(SecsItem* pList, std::vector<SecsItem::DataType> Type, std::vector<string>& Data);

	/**  Get data from a item with type checking with type checkin may not be used directly */
	bool TypedExtractData(SecsItem* pItm, SecsItem::DataType Type, string& Data);

	UINT BinaryMessageLength(void);
	int MakeBinaryMessage(BYTE* pByte);
	void PrintSML(void);
	void PrintSML(string& Data);
	SecsMessage();
	virtual ~SecsMessage();
	BOOL DecodeMessage(BYTE* pByte, int MessageLen);
	virtual BOOL Validate() {
		return true;
	}
protected:
	void Log(CString mg, short lvl = 5, CString src = "");
	void LogInfo(string& Msg);
	void LogWarn(string& Msg);
	void LogErr(string& Msg);
	void LogFatalErr(string& Msg);
	void ClearItems();
	int GetItemLen(BYTE* pByte, int nNumberOfBytes);
	BYTE TypeOf(BYTE bCheck, int& NumLenBytes);
	std::vector<SecsItem*> Items;
	SecsMessage& MsgRef;
	BOOL bDirty;
};

class MessageS1F3 :public SecsMessage {
	std::vector<UINT> SVIDs;
public:
	BOOL Validate();
	void GetSvidList(std::vector<UINT>& Svids);
};

class MessageS2F13 :public SecsMessage {
	IDVec ECIDs;
public:
	BOOL Validate();
	void GetEcidList(std::vector<UINT>& Ecids);
};

class MessageS3F25 :public SecsMessage {
public:
	UCHAR nPort;
	string PortAction;
	std::vector<string> ParamName;
	std::vector<SecsItem*> ParamValue;

	BOOL Validate();
};

class MessageS3F21 :public SecsMessage {
	string PGName;
	std::vector<BYTE> Ports;
	UCHAR PortAccess;
public:
	BOOL Validate();
	void GetPortGroupName(string& PGName) {
		PGName = this->PGName;
	}
	void GetPortNumbers(std::vector<BYTE>& Ports) {
		Ports.clear();
		Ports = this->Ports;
	};
	void GetPortAccess(string& PortAccess);
};

class MessageS3F23 :public SecsMessage {
	string PGrpAction;
	string PGrpName;
	std::vector<ParamData > Parameters;
public:
	BOOL Validate();
	void GetPortGroupName(string& PGName) {
		PGName = this->PGrpName;
	};
	bool GetParameters(std::vector<ParamData >& Parameters);
	void GetPGrpAction(string& PGRPNAME) {
		PGRPNAME = this->PGrpAction;
	}
};

class MessageS1F11 :public SecsMessage {
	std::vector<UINT> SVIDs;
public:
	BOOL GetSvidList(std::vector<UINT>& SvList);
	BOOL Validate();
};

class MessageS1F13 :public SecsMessage {
public:
	BOOL Validate();
};

class MessageS1F15 :public SecsMessage {
public:
	BOOL Validate();
};

class MessageS1F17 :public SecsMessage {
public:
	BOOL Validate();
};

class MessageS2F15 :public SecsMessage {
	std::vector<EcidData> EcidVals;
public:
	BOOL Validate();
	void GetNewECVal(std::vector<EcidData>& NewVals);
};

class MessageS2F17 :public SecsMessage {
public:
	BOOL Validate();
};

class MessageS2F25 :public SecsMessage {
public:
	BOOL Validate();
};

class MessageS2F33 :public SecsMessage {
	UINT DataId;
	std::vector<RptData> RptList;
public:
	BOOL Validate();
	void GetRptDefs(std::vector<RptData>& RptDefs);
};

class MessageS2F35 :public SecsMessage {
	UINT DataId;
	std::vector<LinkData> LinkList;
public:
	BOOL Validate();
	void GetLinkDefs(std::vector<LinkData>& LinkDefs);
};

class MessageS10F3 :public SecsMessage {
	BYTE TID;
	string Message;
public:
	BOOL Validate();
	BYTE GetTerminalMsg(string& msg);
};

class MessageS10F5 :public SecsMessage {
	BYTE TID;
	string Message;
public:
	BOOL Validate();
	BYTE GetTerminalMsg(string& msg);
};

class MessageS2F21 :public SecsMessage {
	unsigned char RCmd;
public:
	MessageS2F21() {
		RCmd = 0;
	}
	BOOL Validate();
	BYTE GetRemoteCmd();
};

class MessageS2F29 :public SecsMessage {
	std::vector<UINT> Ecids;
public:
	BOOL Validate();
	void GetEcids(std::vector<UINT>& EcidList);
};

class MessageS3F17 :public SecsMessage {
public:
	std::vector<string> AttrIDs;
	std::vector<SecsItem*> AttrValues;
	UINT nPort;
	string Action;
	string CarrierID;
	BOOL Validate();
};

class MessageS16F17 :public SecsMessage {
	std::vector<string> PrJobList;
public:
	BOOL Validate();
	void GetJobList(std::vector<string>& PjIds) {
		PjIds = this->PrJobList;
	};
};

class MessageS16F19 :public SecsMessage {
public:
	BOOL Validate();
};

class MessageS16F21 :public SecsMessage {
public:
	BOOL Validate();
};

struct JobDetails {
	string SlotNum;
	string WaferThickness;
	string filmThickness;
};

class MessageS16F3 :public SecsMessage {
	BYTE PortId;
	UINT DataID;
	BOOL AutoStart;
	int ScanLines;
	int ScanNo;
	BYTE MF;
	std::vector<JobDetails> PrJobDetails;
public:
	BOOL Validate();
	void GetJobDetails(std::vector<JobDetails>& Details) {
		Details = PrJobDetails;
	}
	BYTE GetPortId() {
		return this->PortId;
	};
	BOOL StartMethod() {
		return this->AutoStart;
	};
	int GetScanLines() {
		return this->ScanLines;
	};
	int ScanNumber() {
		return this->ScanNo;
	}
};

class MessageS16F27 :public SecsMessage {
	string CtrlJOB;
	BYTE Command;
	// Action parameter passed with Abort,cancel,Stop command
	string ParamName;
	string ParamVal;
public:
	void GetParam(string& Param) {
		Param = this->ParamName;
	};
	void GetParamVal(string& Val) {
		Val = this->ParamVal;
	};
	BOOL Validate();
	void GetCtrlJobId(string& Id) {
		Id = this->CtrlJOB;
	};
	void GetCommand(UINT& Cmd) {
		Cmd = this->Command;
	};
};

class MessageS6F15 :public SecsMessage {
	UINT Ceid;
public:
	BOOL Validate();
	UINT GetCEID() {
		return Ceid;
	}
};

class MessageMBEnq :public SecsMessage {
	UINT DataLen;
public:
	BOOL Validate();
	UINT GetDataLen() {
		return DataLen;
	}
};

struct ParamNew {
	int SlotNo;
	short ScanNo;
	float FThick;
	float WThick;
};

struct PrjobRepNew {
	BYTE MF;
	string DataID; // Ignored
	string PrJobID;
	string CarrierID;
	string RecipeID;
	std::vector<BYTE> Slots;
	BOOL bAutoStart;
	std::vector<ParamNew> WaferParams;
	std::vector<UINT> PauseEvts;
	std::vector<string> MatlIds;
	UINT NumScanLines;
	UINT ScanNo;
	BYTE RcpMethd;
};

class MessageS16F5 :public SecsMessage {
	string PrJobId;  // Process Job ID
	string Command;  // Command to be processed
	UINT DataID;     // ???
	BOOL SC_OK;
	BOOL SN_OK;
	BOOL WP_OK;
	//	CmdParamMap CmdMap;
public:
	BOOL Validate();
	void GetPrJobId(string& Id) {
		Id = this->PrJobId;
	};
	void GetCommand(string& Cmd) {
		Cmd = this->Command;
	};
	BOOL ParamOk() {
		return (/*(SC_OK)&&(SN_OK)&&*/(WP_OK));
	};
	std::vector<ParamNew> WaferParams;
	UINT NumScanLines;
	UINT ScanNo;
};

class MessageS16F15 :public SecsMessage {
	UINT DataID;
	std::vector<PrjobRepNew> PrJobs;
	BOOL ReadPrJob(SecsItem& PList, PrjobRepNew& PrJob);
public:
	BOOL Validate();
	void GetPrJobList(std::vector<PrjobRepNew>& PrJobList) {
		PrJobList = PrJobs;
	};
};

class MessageS16F11 :public SecsMessage {
	UINT DataID;
public:
	PrjobRepNew PrJob;
	BOOL Validate();
	void GetPrJobList(PrjobRepNew& PrJ) {
		PrJ = PrJob;
	};
};

class MessageS16F13 :public SecsMessage {
	UINT DataID;
	std::vector<PrjobRepNew> PrJobs;
	BOOL ReadPrJob(SecsItem& List, PrjobRepNew& PrJob);
public:
	BOOL Validate();
	void GetPrJobList(std::vector<PrjobRepNew>& PrJobList) {
		PrJobList = PrJobs;
	};
};

//
//		Stream 14 Objects Services
//

struct AttrData {
	string AttrId;
	SecsItem* AttData;
	UCHAR AttrrElen;
};

class MessageS14F1 :public SecsMessage {
	std::vector<string> ObjIds;
	std::vector<AttrData> AtrDat;
	std::vector<string> AttrID;
public:
	string ObjType;
	string ObjSpec;
	BOOL Validate();
	void GetObjIds(std::vector<string>& Objs) {
		Objs = ObjIds;
	}
	void GetAttrData(std::vector<AttrData>& attrs) {
		attrs = AtrDat;
	}
	void GetAttrIds(std::vector<string>& AtIDs) {
		AtIDs = AttrID;
	}
};

class MessageS14F3 :public SecsMessage {
	std::vector<string> ObjIds;
	std::vector<string> AttrID;
	std::vector<SecsItem*> AttrValue;
public:
	string ObjType;
	string ObjSpec;
	BOOL Validate();
	void GetObjIds(std::vector<string>& Objs) {
		Objs = ObjIds;
	}
	void GetAttrIds(std::vector<string>& AtIDs) {
		AtIDs = AttrID;
	}
	SecsItem* GetAttr(const char* szAttr);
	void GetAttrValue(std::vector<SecsItem*>& Value) {
		Value = AttrValue;
	}
};

class MessageS14F5 :public SecsMessage {
public:
	string ObjSpec;
	BOOL Validate();
};

class MessageS14F7 :public SecsMessage {
	std::vector<string> ObjType;
public:
	string ObjSpec;
	BOOL Validate();
	void GetAttrIds(std::vector<string>& types) {
		types = ObjType;
	}
};

typedef map<string, SecsItem*> AttrMap;
typedef AttrMap CmdParamMap;
class MessageS14F9 :public SecsMessage {
public:
	AttrMap Attributes;
	string ObjType;
	string ObjSpec;
	BOOL Validate();
	~MessageS14F9();
};

class MessageS14F11 :public SecsMessage {
	AttrMap Attributes;
	string ObjID;
public:
	BOOL Validate();
	void GetCjobId(string& Id) {
		Id = ObjID;
	}
	~MessageS14F11();
};

struct ErrorMessages :public SecsMessage {
	static BOOL GetErrTxt(int ErrNo, string& ErrText);
	static BOOL GetErrTxtE5(int ErrNo, string& ErrText);
	static BOOL GetErrorTextS16F27(int Service, int ErrNo, string& ErrText);
	static BOOL GetErrorTextS3F18(int ErrNo, string& ErrText);
public:
	static BOOL GetErrTxtS16F15(int ErrNo, string& ErrText);
};

class MessageS3F27 :public SecsMessage {
public:
	std::vector<UINT> Ports;
	UCHAR PortAction;
	BOOL Validate();
};

class MessageS5F3 :public SecsMessage {
public:
	bool bAll;
	bool bEnable;
	UINT AlarmID;
	BOOL Validate();
};

class MessageS5F5 :public SecsMessage {
public:
	std::vector<UINT> ALIDs;
	BOOL Validate();
};

class MessageS7F1 :public SecsMessage {
public:
	string PPID;
	UINT PPLen;
	BOOL Validate();
};

class MessageS7F3 :public SecsMessage {
public:
	string PPID;
	UINT PPLen;
	BYTE* PPBODY;
	BOOL Validate();
	~MessageS7F3() {
		if (PPBODY) delete[] PPBODY;
	}
	MessageS7F3() {
		PPLen = 0;
		PPBODY = NULL;
	}
};

class MessageS7F5 :public SecsMessage {
public:
	string PPID;
	BOOL Validate();
};

class MessageS7F2 :public SecsMessage {
public:
	BYTE PPGNT;
	BOOL Validate();
};

//@@ Vijay#3June$02 New message added
class MessageS7F17 :public SecsMessage {
public:
	std::vector<string> PPIDs;
	BOOL Validate();
};

//@@ Vijay#3June$02 New message added
class MessageS7F19 :public SecsMessage {
public:
	BOOL Validate();
};

// @@ Vijay#1July$02 New message added
class MessageS6F19 :public SecsMessage {
	UINT RptId;
public:
	BOOL Validate();
	UINT GetRptID() {
		return this->RptId;
	};
};

// @@ Vijay#1July$02 New message added
class MessageS6F21 :public MessageS6F19 {
	// Has same structure as of MessageS6F19
	// ... derived Just for Syntactic conveniance
};

class MessageS18F1 :public SecsMessage {
public:
	BOOL Validate();
	string TargetID;
	std::vector<string> AttrIDs;
};

class MessageS18F3 :public SecsMessage {
public:
	BOOL Validate();
	string TargetID;
	std::vector<string> AttrIDs;
	std::vector<string> AttrVals;
};

class MessageS18F5 :public SecsMessage {
public:
	BOOL Validate();
	string TargetID;
	string DatSeg;
	UINT DataLength;
};

class MessageS18F7 :public SecsMessage {
public:
	BOOL Validate();
	string	TargetID;
	string	DatSeg;
	UINT	DataLength;
	string	Data;
};

class MessageS18F9 :public SecsMessage {
public:
	BOOL Validate();
	string	TargetID;
};

class MessageS18F11 :public SecsMessage {
public:
	BOOL Validate();
	string	TargetID;
	string	MID;
};

class MessageS18F13 :public SecsMessage {
public:
	BOOL Validate();
	string	TargetID;
	string	SSCMD;
	std::vector<string> CpList;
};

class MessageS2F37 :public SecsMessage {
public:
	BOOL Validate();
	bool CEED;
	std::vector<UINT> CEIDs;
};

class MessageS2F43 :public SecsMessage {
public:
	BOOL Validate();
	struct SpDat {
		BYTE StreamID;
		std::vector<BYTE> FcnID;
	};
	std::vector<SpDat> StFuncDat;
};

class MessageS6F23 :public SecsMessage {
public:
	BOOL Validate();
	BYTE RSDC;
};

class MessageS10F1 :public SecsMessage {
public:
	BOOL Validate();
	BYTE TID;
	string Text;
};

class MessageS10F10 :public SecsMessage {
public:
	BOOL Validate();
	string Text;
};

class MessageS16F25 :public SecsMessage {
public:
	BOOL Validate();
	std::vector<string> PrJobs;
	bool StartMethod;
};

class MessageS5F7 :public SecsMessage {
public:
	BOOL Validate();
};

extern 	BOOL bIgnorRecipeParameter; //  [9/6/2007 LYF]
