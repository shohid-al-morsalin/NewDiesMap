#ifndef _SUBSTOBJ_H_
#define _SUBSTOBJ_H_
#include "Constant.h"
#include "OSSMemS.h"
#define SUBSTLOCIDLEN		32
#define SUBSTIDLEN			32
#define SUBSTLOCLEN			32
#define SUBSTHISTITEMLEN	5

struct SubstHistory {
	char SubstLocID[SUBSTLOCIDLEN + 1];

	long TimeIn;

	long TimeOut;

	void Preset();
};

class CSubstObj : public OSSMemS {
public:
	enum SLOTMAP {
		SM_UNDEFINED = '0', SM_EMPTY = '1', SM_NOT_EMPTY = '2', SM_CORRECTLY_OCCUPIED = '3', SM_DOUBLE_SLOTTED = '4', SM_CROSS_SLOTTED = '5', igSLOTMAP = '6'
	};

	enum SUBSTPROCSTATE {
		NO_PROCSTATE, NEEDS_PROCESSING, IN_PROCESS, PROCESSED, ABORTED, STOPPED, REJECTED, igPROCSTATE
	};

	enum SUBSTTYPE {
		WAFER, FLAT_PANEL, CD, MASK
	};

	enum SUBSTUSAGE {
		sPRODUCT, sTEST, sFILTER, sCLEANING
	};

	enum SUBSTSTATE {
		NO_TR_STATE, ATSOURCE, ATWORK, ATDESTINATION, igSUBSTSTATE
	};

	SUBSTTYPE SubstType;
	SUBSTSTATE SubstState;
	SUBSTUSAGE SubstUsage;
	SUBSTPROCSTATE SubstProcState;

	short SlotNo;

	char LotID[LOTIDLEN + 1];
	char SubstLocID[SUBSTLOCIDLEN + 1];
	char SubstSource[SUBSTLOCLEN + 1];
	char SubstDestination[SUBSTLOCLEN + 1];
	SubstHistory History[SUBSTHISTITEMLEN];

	CString GetSubstStateStr(SUBSTSTATE state);
	CString GetSubstProcStateStr(SUBSTPROCSTATE state);
	static char GetSMChar(char sm);
	static SLOTMAP GetSMEnum(char ch);
	void Preset();
	void SetState(CSubstObj::SUBSTSTATE state);
	void SetProcState(CSubstObj::SUBSTPROCSTATE state);
	BOOL GetAttr(char* attrname, std::string& attr);
	BOOL SetAttr(char* attrname, std::string attr);

	// 	CSubstObj ();
	// 	virtual ~CSubstObj();
};
#endif