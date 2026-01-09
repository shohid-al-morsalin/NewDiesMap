#pragma once
#include "Constant.h"
#include "AlrmItem.h"
#include "SubstObj.h"
#include "EqpStatus.h"
#include "StdAction.h"
#include "LPStatus.h"
#include "LPObj.h"
#include "SubstObj.h"
#include "CarrierObj.h"
#include "SubstLocObj.h"

#include <map>
#include <vector>

using namespace std;

struct ComMsg {
	HWND HWin;
	int Ack;
	int Nak;
	int Val;
	CString text;
};

struct CarrierObjStatus {
	enum ACTION {
		BIND, CANCELBIND, CANCELCARRIER, CANCELCARRIERATPORT, CARRIERIN, CANCELCARRIEROUT, CARRIEROUT, PROCEEDWITHCARRIERID,
		PROCEEDWITHCARRIERSM, CARRIEROBJUPDATE // Tell Online about new carrier object status
	};

	ACTION act;
	short portID; // use in BIND to pass in port ID
	CCarrierObj Co;
};

struct C128Obj {
	enum SERVICESTATUS {
		INSERVICE, OUTOFSERVICE
	};

	SERVICESTATUS Service;

	enum OPERATIONSTATUS {
		oOFFLINE, oIDLE, oPREPROCESSING, oPROCESSING, oPOSTPROCESSING
	};

	OPERATIONSTATUS Status;

	C128Obj::C128Obj() {
		Status = oOFFLINE;
		Service = OUTOFSERVICE;
	}

	CString GetOpStatusStr() {
		switch (Status) {
		case oOFFLINE:
			return "OFFLINE";
			break;
		case oIDLE:
			return "IDLE";
			break;
		case oPREPROCESSING:
			return "PREPORCESSING";
			break;
		case oPROCESSING:
			return "PROCESSING";
			break;
		case oPOSTPROCESSING:
			return "POSTPROCESSING";
			break;
		default:
			ASSERT(0);
			break;
		}
		return "";
	}
};

struct C900Obj : public C128Obj {
	int CurTempr; // Current temperature
	float VacPressure; // Current vacuum pressure
	float LampStatus[12]; // Current lamp status
};

struct C413Obj : public C128Obj {};

struct CRbtObj {
	enum SERVICESTATUS {
		INSERVICE, OUTOFSERVICE
	};

	SERVICESTATUS Service;
};

struct CIDRObj {
	enum SERVICESTATUS {
		INSERVICE, OUTOFSERVICE
	};

	enum BYPASSSTATUS {
		INUSE, BYPASSED
	};

	char ObjID[OBJIDLEN + 1];
public:
	void SetBypassStatus(BYPASSSTATUS status, short port);
	void SetServiceStatus(SERVICESTATUS status, short port);
	BYPASSSTATUS GetBypassStatus(short port);
	SERVICESTATUS GetServiceStatus(short port);
	void SetBypassStatus(BYPASSSTATUS status);
	void SetServiceStatus(SERVICESTATUS status);
	BYPASSSTATUS GetBypassStatus();
	SERVICESTATUS GetServiceStatus();
	int GGetServiceStatus();
	CString GetServiceStatusStr();
private:
	BYPASSSTATUS Bypass;
	SERVICESTATUS Service;
};

struct CScaleObj {
	enum SERVICESTATUS {
		INSERVICE, OUTOFSERVICE
	};

	SERVICESTATUS Service;
};

struct CarrierSubstLocObj : public CSubstLocObj {
	CSubstObj Subst[MAXCAPACITYLEN];
};

struct EquipSubstLocObj : public CSubstLocObj {
	CSubstObj Subst;
};

struct CarrierLocObj : public OSSMemS {
	CarrierSubstLocObj CarrierSubstLoc;
};

struct CarrierAction {
	enum CARRIERACTION {
		BINDSERVICE, READYTOLOAD, TRANSFERBLOCKED, READYTOUNLOAD, CANCELCARRIER, CLAMPCOMPLETE,
		CLAMPFAILED, OPENCOMPLETE, OPENFAILED, CARRIERIDREAD, CARRIERIDREADOK, CARRIERIDREADFAIL,
		UNCLAMPCOMPLETE, UNCLAMPFAILED, CLOSECOMPLETE, CLOSEFAILED, PROCESSINGCOMPLETE, IDWAITINGFORHOST,
		CARRIERIDVERIFICATION, CARRIERIDVERIFICATIONOK, CARRIERIDVERIFICATIONFAIL, UNLOADCOMPLETE,
		CARRIERARRIVED, CARRIERREMOVED, SETCARRIERARRIVED, CLEARCARRIERARRIVED, RESERVEATPORT,
		RESERVEATPORTOK, RESERVEATPORTFAIL, CANCELRESERVEATPORT, CANCELRESERVEATPORTOK,
		CANCELRESERVEATPORTFAIL, COSMCHANGED, LPCASMCHANGED, LPRSMCHANGED, STARTPROCESSING,
		STOPPROCESSING, PAUSEPROCESSING, RESUMEPROCESSING, ABORTPROCESSING, CLOSECARRIER,
		PROCESSINGSTARTED, UNLOADREQUEST, LOADREQUEST
	};

	CARRIERACTION act;
	short PortNo;
	BOOL bSaveJob;
	char CarrierID[OBJIDLEN + 1];
	char PrJobID[PRJIDLEN + 1];
	char CJobID[CRJIDLEN + 1];
};

struct LPAction {
	enum ACTION {
		UNKNOWN, MANUALARRIVED, MANUALREMOVED, AUTOARRIVED, AUTOREMOVED, NOTPRESENT, PRESENT,
		SETOUTOFSERVICE, SETACCESSMODEAUTO, SETACCESSMODEMANUAL
	};

	ACTION act;
	short PortNo;
};

struct OnOffActions {
	enum ONOFF {
		OFF, ON
	};

	enum DEVICES {
		BCR, AGV
	};

	ONOFF act;
	DEVICES dev;
};

struct CarrierIDInfo {
	enum ACTION {
		READ, WRITE, CARRIERIDREAD
	};

	ACTION act;
	short PortNo;
	char CarrierID[OBJIDLEN + 1];
};

struct SlotmapInfo {
	short PortNo;
	char slotmap[MAXCAPACITYLEN + 1];
};

struct WaferStatus {
	enum ACTION {
		READ, WRITE
	};

	ACTION act;

	short SlotNo;
	char WaferID[SUBSTIDLEN + 1];
};

struct RecipeData {
	enum ACTION {
		NORCPACT, CREATED, EDITED, DELETED,
	};

	ACTION Act;
	char RecipeName[OBJIDLEN + 1];

	void Preset() {
		Act = NORCPACT;
		memset(RecipeName, 0, sizeof(RecipeName));
	}
};

struct sScanData {
	enum FTUNIT {
		NANOMETER, ANSTRONG
	};

	enum SUNIT {
		DYNES, MPA
	};

	enum ACTION {
		READ, WRITE, INIT
	};

	ACTION act;
	short PortNo;
	short SlotNo;
	short WaferNo;
	short ScanNo;
	short LineNo;
	float WaferThick;
	float FilmThick;
	float Stress;
	float Radius;
	float BowHeight;
	long StartTime;
	long EndTime;
	float SConst;
	SUNIT SUnit;
	FTUNIT FTUnit;
	short Temperature;
	short Laser; //  [11/2/2009 Yuen]
	char SubstID[SUBSTIDLEN + 1];
	char CarrierID[OBJIDLEN + 1];
	char LotID[LOTIDLEN + 1];

	void Preset() {
		act = INIT;
		PortNo = 0;
		SlotNo = 0;
		WaferNo = 0;
		ScanNo = 0;
		LineNo = 0;
		WaferThick = 0;
		FilmThick = 0;
		Stress = 0;
		Radius = 0;
		BowHeight = 0;
		StartTime = 0;
		EndTime = 0;
		SConst = 0;
		SUnit = DYNES;
		FTUnit = ANSTRONG;
		Temperature = 0;
		Laser = 0;
		ZeroMemory(SubstID, SUBSTIDLEN + 1);
		ZeroMemory(CarrierID, OBJIDLEN + 1);
		ZeroMemory(LotID, LOTIDLEN + 1);
	}
};

struct ScanParam {
	enum SCANNUMBER {
		FIRSTSCAN = 1, SECONDSCAN
	};

	enum SCANTYPE {
		NONETYPE, LINETYPE, MAPTYPE
	};

	enum SCANMODE {
		NONEMODE, RTMODE, HTMODE
	};

	enum ACTION {
		READ, WRITE
	};

	ACTION act;
	SCANNUMBER scanno;
	short laser;
	float angle;
	short nline;

	float scansize;
	float wafersize;
	short waferno;
	SCANMODE mode;
	SCANTYPE type;
	short phase;
	char waferID[SUBSTIDLEN + 1];
};

struct SWaferParam {
	enum ACTIONWP {
		wpREAD, wpWRITE, wpNONE
	};

	ACTIONWP act;
	short PortNo;
	short SlotNo;
	float WaferThick;
	float FilmThick;
	char ID[SUBSTIDLEN + 1];

	void Preset() {
		act = wpNONE;
		PortNo = 0;
		SlotNo = 0;
		WaferThick = 0;
		FilmThick = 0;
		memset(ID, 0, SUBSTIDLEN + 1);
	}
};

struct BCRHeadStatus {
	enum HEADSTATUS {
		bIDLE, bBUSY, bNOTOPERATED
	};

	enum HEADCONDITION {
		NOALARMS, NEEDMAINTENANCE, READWRITEFAULT, READWRITERATEFAULT, NOPOWER
	};

	char HeadID[BCRHEADIDLEN + 1];

	HEADSTATUS Status;
	HEADCONDITION Condition;
};

struct BCRObj {
	enum ONOFF {
		OFF, ON
	};

	enum ALARMSTATUS {
		NOALARMS, ALARMS
	};

	enum OPERATIONSTATUS {
		bIDLE, bBUSY, bMAINTENANCE
	};

	short PortNo;
	char Configuration[BCRCONFIGLEN + 1];

	char SoftwareRevisionLevel[REVISIONLEVELLEN + 1];
	char DeviceType[DEVICETYPELEN + 1];

	char Manufacturer[MANUFACTURERLEN + 1];
	char ModelNo[MODELNUMLEN + 1];
	char SerialNo[SERIALNUMLEN + 1];
	BCRHeadStatus HeadStatus[HEADSTATUSLEN + 1];
	ONOFF Status;

	ALARMSTATUS AlarmStatus;
	OPERATIONSTATUS OperationStatus;
	void Preset(short PortNo);
	short GGetStatus();
};

struct BCRStatus {
	enum ACTION {
		READ, WRITE, OPERATIONSTATUSCHANGE, ALARMSTATUSCHANGE, ONOFFCHANGE, HEADSTATUSCHANGE,
		HEATCONDITIONCHANGE
	};

	ACTION act;
	CIDRObj BCR;
};

struct AGVObj {
	enum ONOFF {
		OFF, ON
	};

	ONOFF Status;
	short PortNo;
	void Preset(short PortNo);
	short GGetStatus();
};

struct AGVStatus {
	enum ACTION {
		READ, WRITE, ONOFFCHANGE
	};
	ACTION act;
	AGVObj AGV; // esOFF, esON
};

struct LineModeAction {
	enum ACTION {
		GOREMOTE, GOLOCAL, GOHOSTOFFLINE, GOEQPOFFLINE
	};

	ACTION act;
};

struct CarrierManagementServices {
public:
	enum ACTION {
		CREATECARRIEROBJECT, DESTROYCARRIEROBJECT, UPDATESLOTMAP
	};

public:
	ACTION act;
	short PortNo;
	short capacity;
	char lotID[LOTIDLEN + 1];
	char slotmap[MAXCAPACITYLEN + 1];
	char carrierID[OBJIDLEN + 1];
};

struct SubstTrackingServices {
public:
	enum SUBSTLOC {
		SOURCE, ALIGNER, SCALE, SCANNER, DESTINATION
	};

	enum ACTION {
		CHANGESUBSTSTATE, REMOVESUBST, REGISTERSUBST
	};

	ACTION act;
	SUBSTLOC substloc;

	union {
		EquipSubstLocObj EquipSubstLoc;
		CarrierSubstLocObj CarrierSubstLoc;
	};
};

struct PrcsMgrServices {
public:
	enum ACTION {
		HELLO, REGISTERWIN, STARTALLUNITS, STOPALLUNITS, OPENFOUP, CLOSEFOUP, LATCHFOUP, UNLATCHFOUP,
		READCARRIERID, WRITECARRIERID, READTAGDATA, WRITETAGDATA, MAP, AUTO, RESET, NOVACUUM, XFER,
		HOMEROBOT, HOMEALGNER, STARTUNIT, CLEARALARM, WEIGHWAFER, CLEARSCALE, SCANONELINE, GETLPSTATUS,
		GETLPISTATUS, AUTOCALIBRATE, SETOUTOFSERVICE, SETINSERVICE, ACCESSMODEAUTO, ACCESSMODEMANUAL,
		LEDMANUAL, LEDRESERVED, LEDCLEARALL, GOTOSTARTPOS, SCALECHECK, WAFERCHECK, TINYEYECHECK,
		CLOSENORELEASE, SETDEFAULT, PUSHBUTTON2, BASELINECALIBRATION, NEEDVACUUM, LEDLOADUNLOAD,
		ENABLEHOAVBL, ENABLEBUTTONREPORT, ABORT, WAFERONARM, LP2HC, HC2LP, LP2SCL, SCL2SCN, SCN2LP,
		XFER1, REGISTERWINEX, UNREGISTERWINEX, RESETWAFERTABLE
	};

	//Note: Do not change the sequence, it is use by its rank
	enum LPBSTATE {
		UNDOCKFAILED, DOCKFAILED, CLAMPFAILED, FOUPERROR,
		//BUTTONPRESSED2,
		UNCLAMPPED, UNCLAMPFAILED, NOFOUP, FOUPPRESENT,
		//BUTTONPRESSED1,
		CLAMPPING, CLAMPPED, DOCKING, DOCKED, OPENING, OPENED, OPENFAILED, UNDOCKING, UNDOCKED,
		UNCLAMPPING, BUTTONPRESSED
	};

	ACTION act;

	int Ret;
	short unit, port, nunit;
	short laser;
	int STSFrom, STSTo;
	LPBSTATE LpBState;
	float scansize, angle;
	BOOL bInPlace, bReady, bMode;

	char text[TEXTLEN + 1];

	void Preset() {
		unit = 0;
		port = 0;
		nunit = 0;
		laser = 0;
		LpBState = NOFOUP;
		scansize = 0;
		angle = 0;
		bInPlace = FALSE;
		bReady = FALSE;
		bMode = FALSE;
		text[0] = 0;
		STSTo = 0;
		STSFrom = 0;
	}
};

struct SlotInfo {
	short slotno;
};

struct SPcJobExec {
	enum ACTION {
		NOACT, ADD, REMOVE, PAUSE, RESUME, STOP, ABORT, DEFINEPJ, DEFINECJ
	};

	ACTION act;
	char PrJID[PRJIDLEN + 1];
	char CarrierID[OBJIDLEN + 1];
	short PortNo;
	SlotInfo slot[MAXCAPACITYLEN + 1];
	CContentMap CtMap[MAXCAPACITYLEN + 1];
	char RecipeName[RECIPEIDLEN + 1];

	void Preset() {
		act = NOACT;
		memset(PrJID, 0, sizeof(PrJID));
		memset(CarrierID, 0, sizeof(CarrierID));
		memset(slot, 0, sizeof(slot));
		memset(CtMap, 0, sizeof(CtMap));
		PortNo = 0;
	}
};

struct StartInfo {
	char lotid[LOTIDLEN + 1];
	char carrierid[OBJIDLEN + 1];
	short portno;
	short scanline;
	short scanno;
	SlotInfo slotinfo[MAXCAPACITYLEN + 1];
};

struct RcpVarList {
	char AttrName[OBJIDLEN + 1];
	float AttrList[MAXCAPACITYLEN + 1];
};

// Specific PRJob acceptable by FSM
struct PRJob : public OSSMemS {
	enum PRCSJOBSM {
		NOSTATE, QUEUED, SETTINGUP, WAITINGFORSTART, PROCESSING, PROCESSCOMPLETE, STOPPING,
		PAUSING, PAUSED, ABORTING,
	};

	enum STARTMETHOD {
		USER, AUTO
	};

	enum RCPMETHOD {
		RECIPEONLU = 1, RECIPEWITHVARIABLE
	};

	RCPMETHOD RecipeMethod; // 1= Recipe only. 2= Recipe with variable tuning
	char RecID[RECIPEIDLEN + 1]; // Recipe ID
	char carrierid[OBJIDLEN + 1];
	CSubstObj::SLOTMAP slotlist[MAXCAPACITYLEN + 1];
	RcpVarList FThick;
	STARTMETHOD StartMethod;
	short PauseEvent[MAXPAUSEEVENT + 1];
	PRCSJOBSM State;
};

struct PRJobStatus {
	enum ACTION {
		ADD, REMOVE, PAUSE, RESUME, STOP, ABORT
	};

	ACTION act;
	PRJob pJob;
};

struct PrcsCtrlSpec {
	char prjobid[PRJIDLEN + 1];
	char CtrlRule[CTRLRULELEN + 1];
	char OutPRule[OUTPRULELEN + 1];
};

struct CRJob : public OSSMemS {
	char CurrentPRJob[OBJIDLEN + 1];
	char CarrierInputSpec[OBJIDLEN + 1]; // carrierID for material that will be used
	char MtrlOutSpec[OBJIDLEN + 1]; // Not use, should be empty
	short PauseEvent[MAXPAUSEEVENT + 1];
	PrcsCtrlSpec PCSpec[MAXPRCSCTRLSPEC + 1];

	enum PRCSORDERMGMT {
		ARRIVAL = 1, OPTIMIZE
	};

	PRCSORDERMGMT PrcsOrder;

	enum STARTMETHOD {
		USER, AUTO
	};

	STARTMETHOD StartMethod;

	enum CTRLJOBSM {
		NOSTATE, QUEUED, SELECTED, WAITINGFORSTART, EXECUTING, PAUSED, COMPLETED
	};

	CTRLJOBSM State;
};

struct CRJobStatus {
	enum ACTION {
		ADD, START, PAUSE, STOP, ABORT, REMOVE
	};

	ACTION act;
	CRJob cJob;
};

struct RPLY {
	HWND HWin;

	int Ack, Nak;

	void Preset() {
		HWin = NULL;
		Ack = Nak = 0;
	}

	void Set(HWND hWn, int Ak, int Nk) {
		HWin = hWn;
		Ack = Ak;
		Nak = Nk;
	}

	void Reply(int ans);
};

struct CjPjCmd {
	enum ACTION {
		ABORTING, STOPPING
	};

	ACTION act;
	short unit;
};

struct DeviceCmd {
	enum ACTION {
		CONNECT, DISCONNECT, RESET, AUTO, NOVAC, NEEDVAC, ABORT
	};

	ACTION act;
	short unit;
	BOOL success;
	char msg[256];

	void Preset() {
		unit = 0;
		success = FALSE;
		memset(msg, 0, 256);
	}
};

struct RbtServices {
	enum ACTION {
		GOTO, MOVE
	};

	ACTION act;

	enum CMDTYPE {
		NOCMDTYPE, PORT2STAGE, STAGE2PORT, STAGE2STAGE180, STAGE2STAGE0, PORT2ALGN, ALGN2STAGE
	};

	CMDTYPE type;
	short iSTN; // Initial Station
	short iSLOT; // Initial Slot #
	short fSTN; // Final Station
	short fSLOT; // Final Slot #

	void Preset() {
		type = NOCMDTYPE;
		iSTN = iSLOT = fSTN = fSLOT = 1;
	}
};

struct CSMOL {
public:
	int Ack;
	int Nak;

	enum USRMSG {
		ILLEGALSTATION, ALLOCFAIL, INCASEMPTY, SENDFAIL, CHAMBER_CLOSE_FAIL, CHAMBER_OPEN_FAIL,
		umNOERROR = 0
	};

	enum PRCSSTATUS {
		LOADREQ, PROCESSREQ, DONEREQ, UNLOADREQ, PORTBUSY, COMPLETED
	};

	enum STATIONS {
		INCAS = 1, PREALGN, SCALE, SCALE1, SCANNER, OUTCAS, ALLCAS, PREMEASURE
	};

public:
	enum COMMD {
		cmdTERMINATE = 0, cmdLPSTATUS, cmdCARRIEROBJSTATUS, cmdBCRSTATUS, cmdPRJOBSTATUS, cmdCRJOBSTATUS,
		cmdSCANDATA, cmdSCANPARAM, cmdWAFERPARAM, cmdWAFERACTION, cmdCARRIERACTION, cmdCARRIERIDINFO,
		cmdSTDACTION, cmdLPACTION, cmdLINEMODEACTION, cmdPCJOBEXEC, cmdCARRIEROBJ, cmdPRCSMGRSERVICES,
		cmdALARM, cmdGEMEVENTS, cmdECHO, cmdDEVICE, cmdCJPJACTION, cmdRBTSERVICES, cmdRESULTREADY,
		cmdRCPSTATUSCHANGE
	};

	enum EXECTN {
		SND, REQ
	};

public:
	void InitLoadPortObj(CLPObj& Lp);
	COMMD cmd;

	EXECTN exe;

	HWND HWin;
	char RetSvr[32];

	unsigned long threadID;

	union {
		EqpStatus Eq; // cmdEQPSTATUS
		CLPStatus Lp; // cmdLPSTATUS
		CarrierObjStatus CoS; // cmdCARRIEROBJSTATUS
		BCRStatus Bs; // cmdBCRSTATUS
		PRJobStatus PrJ; // cmdPRJOBSTATUS
		CRJobStatus Cj; // cmdCRJOBSTATUS
		sScanData Sd; // cmdSCANDATA
		SWaferParam Wp; // cmdWAFERPARAM
		ScanParam Sp; // cmdSCANPARAM
		CarrierAction Ca; // cmdCARREIERACTION
		CarrierIDInfo Ci; // cmdCARRIERIDINFO
		StdAction Sa; // cmdSTDACTION
		LPAction La; // cmdLPACTION
		LineModeAction Lm; // cmdLINEMODEACTION
		SPcJobExec Pj; // cmdPCJOBEXEC
		CCarrierObj C0; // cmdCARRIEROBJ
		PrcsMgrServices Pm; // cmdPRCSMGRSERVICES
		CAlrmItem Ar; // cmdALARM
		DeviceCmd Dv; // cmdDEVICECMD
		CjPjCmd Cp;
		RbtServices Rs; // Robot commands
	};
	CSMOL(void);

	virtual ~CSMOL(void);
};

struct EventReport {
	enum REPORTNUM {
		EQPOFFLINE, HOSTOFFLINE, LOCALONLINE, REMOTEONLINE, BCR1ON, BCR1OFF, BCR2ON, BCR2OFF, LOADREQUEST,
		LOADCOMPLETED, SLOTMAPINFO, STARTED, WAFERPICKUP, WAFERMEASURED, WAFERPUTBACK, PROCESSCOMPLETED,
		UNLOADREQUEST, UNLOADCOMPLETED, ECID_BYPASSREADID_GLOBAL = 100, ECID_BYPASSREADID_LP1,
		ECID_BYPASSREADID_LP2, ECID_SETUP_NAME_E94 = 200
	};

	REPORTNUM RptNo;
	CSMOL SM;
};

// Used in NTC9
struct WaferActions {
	enum ACTION {
		LOCATION, PICKUP, PROCESSINGSTART, PROCESSINGCOMPLETE, PUTBACK, MEASURED
	};

	ACTION act;
	short PortNo;
	short SlotNo;
	char SubstID[SUBSTIDLEN + 1];
};

// Used in NTC9
struct CarrierActions {
	enum CARRIERACTION {
		BINDSERVICE, READYTOLOAD, TRANSFERBLOCKED, READYTOUNLOAD, PROCEEDWITHCARRIER, CANCELCARRIER,
		CLAMPCOMPLETE, CLAMPFAILED, OPENCOMPLETE, OPENFAILED, CARRIERIDREAD, CARRIERIDREADOK,
		CARRIERIDREADFAIL, UNCLAMPCOMPLETE, UNCLAMPFAILED, CLOSECOMPLETE, CLOSEFAILED, PROCESSINGCOMPLETE,
		IDWAITINGFORHOST, CARRIERIDVERIFICATION, CARRIERIDVERIFICATIONOK, CARRIERIDVERIFICATIONFAIL,
		UNLOADCOMPLETE, CARRIERARRIVED, CARRIERREMOVED, SETCARRIERARRIVED, CLEARCARRIERARRIVED,
		RESERVEATPORT, RESERVEATPORTOK, RESERVEATPORTFAIL, CANCELRESERVEATPORT, CANCELRESERVEATPORTOK,
		CANCELRESERVEATPORTFAIL, COSMCHANGED, LPCASMCHANGED, LPRSMCHANGED, STARTPROCESSING, STOPPROCESSING,
		PAUSEPROCESSING, RESUMEPROCESSING, ABORTPROCESSING, CLOSECARRIER, PROCESSINGSTARTED, UNLOADREQUEST,
		LOADREQUEST
	};

	CARRIERACTION act;
	short PortNo;
	char carrierID[OBJIDLEN + 1];
	char PrJobID[PRJIDLEN + 1];
};

// Used in NTC9
struct LPActions {
	enum ACTION {
		UNKNOWN, MANUALARRIVED, MANUALREMOVED, AUTOARRIVED, AUTOREMOVED, NOTPRESENT, PRESENT, SETOUTOFSERVICE,
		SETACCESSMODEAUTO, SETACCESSMODEMANUAL
	};

	ACTION act;
	short PortNo;
};
