#pragma once

#include "ONLINE/GEM/CtrlJob.h"
#include "ONLINE/GEM/PRJOB.H"
#include "IPC/SubstObj.h"
#include "IPC/Smol.h"
// #include "ONLINE/INC/Status.h"
#include <list>

class CPRJob;
class CCtrlJob;

/*struct PortStat{
	BOOL BindOk;
	BOOL CarrierPlaced;
	BOOL Reserved;
	char CarierId[65];
	bool SlotsToBeProcessed[25];
	bool IsProcessing;
	bool IdVerified;
	bool SlotMapVerified;
};*/
struct PortGroup {
	string PGPNAME;
	vector<BYTE> PortList;
};

// Current SVID values
// Whenever there is a SVID value change, it is going to be recorded here
struct CURSVIDVAL {
	short SlotNo;	// Note: This value is changed by a specific CE only Yuen[2002/07/28 14:06]
	short PortNo;	// Note: Instantaneous port number Yuen[2002/07/28 16:53]
	char SubstID[SUBSTIDLEN + 1];
	char SubstLocID[SUBSTLOCIDLEN + 1];
	char CarrierID[OBJIDLEN + 1];
	char CarrierLocID[CARRIERLOCIDLEN + 1];
	sScanData ScnDta;

	CURSVIDVAL() {
		SlotNo = 0;
		PortNo = 0;
		ZeroMemory(CarrierID, OBJIDLEN + 1);
		ZeroMemory(SubstID, SUBSTIDLEN + 1);
		ZeroMemory(SubstLocID, SUBSTLOCIDLEN + 1);
		ZeroMemory(CarrierLocID, CARRIERLOCIDLEN + 1);

		ScnDta.Preset();
	}
};

#define	MAX_CONSOLE_MSG 4096

class CDataBank {
	HANDLE hMutex_Abort;
	HANDLE hMutex_Stop;
	vector<PortGroup> PGrpList;
	list<CCarrierObj> Notified;// Should be static member of CCtrlJob
	int CanReserveAtPort(BYTE PortID);
	BOOL CheckAttribute(string AttrName, vector<string>& Set);
	string GetCarrierID(BYTE PortID);
	int CanProceedWithCarrierID(BYTE PortID, string& CarrierID, vector<string>& Attributes);
	int CanCarrierOut(string& CarrierID, vector<string>& Attributes);
	int CanCarrierNotification(string& CarrierID, vector<string>& Attributes);
	int CanCarrierIn(string& CarrierID, vector<string>& Attributes);
	int CanCancelReservationAtPort(BYTE PortID);
	int CanCancelCarrierOut(string& CarrierID, vector<string>& Attributes);
	int CanCancelCarrierNotification(string& CarrierID, vector<string>& Attributes);
	int CanCancelCarrierAtPort(BYTE PortID, vector<string>& Attributes);
	int CanCancelCarrier(string& CarrierID, vector<string>& Attributes);
	int CanCancelBindCarrier(string& CarrierID, vector<string>& Attributes);
	int CanCancelBindPort(BYTE PortID, vector<string>& Attributes);
	int CanCancelAllCarrierOut(vector<string>& Attributes);
	int CanBindCarrier(string& CarrierID, BYTE PortID, vector<string>& Attributes);
	CSubstObj* GetSubstrateObj(const string& SubstID);
	// Event
	int ActivePort;
	int ActiveSubst;
	std::string		Prjob;
	std::string		CJob;

public:
	char LastSM1[SLOTMAPLEN + 1];
	char LastSM2[SLOTMAPLEN + 1];
	char m_HostMsg1[MAX_CONSOLE_MSG + 16];	// Intermediate buffer for sending terminal message only
//	char m_HostMsg2[MAX_CONSOLE_MSG+16];
	// Note: Added Yuen[2002/08/03 19:16]
	CURSVIDVAL CurSVID;
	CPRJob* GetActivePrJob();
	CCtrlJob* GetActiveCtrlJob();
	void	SetActivePrJob(const std::string& PrjobID);
	void	SetActiveCtrlJob(const std::string& CtlJobID);
	void	SetActiveSubstrate(int SubstNo);
	int		GetActiveSubstrate();
	void	ClearActiveSubstrate();
	void	SetActivePort(int PortNo);
	void	ClearActivePort();
	int		GetActivePort();
	BOOL    GetCurrentPRJobID(string& Id);
	CPRJob* GetPrjobObj(std::string& PrJobID);
public:
	int GetPortID(const string& CarrierID);
	BOOL GetRefObj(long MagicRef, BOOL bRemove, CSubstObj&);
	//	CCidrw		CidrSystem;
	BOOL		m_bBypassReadID;		// I think is a duplicate Yuen[6/26/2002 12:23]??
//	CCms		m_Carrier[4];			// Carrier Management Service
//	CCms        m_TempCarrier;
//	EqpStatus	m_EStatus;

//============================================================
// Control Job Decaration

//============================================================
// OSS
	int CreateObject(const string& ObjSpec, const string& ObjType, map<string, SecsItem*>&, vector<int>& Errcodes, string&);
	CCtrlJob* CreateCJob(map<string, SecsItem*>&, vector<int>& Errcodes);
	CSubstObj* CreateSubst(map<string, SecsItem*>&, vector<int>& Errcodes);
	int SetSubstAttr(CSubstObj* pSubst, const string& AttrID, SecsItem* pItem, BOOL bCreating = FALSE);

	//============================================================
	//  PROCESS JOB DECARATION
public:
	void SetActiveCJPJ(const std::string& CtlJobID, const std::string& PrjobID);
	void ClearActivePortSubstrate();
	void SetActivePortSubstrate(short PortID, short Slot);
	BOOL IsCarrierOnLp(CString& CarrierId);
	short GetActiveSubstrateNo();
	CSubstObj* GetActiveSubstrateObj();
	BOOL CanProcessCarrier(CString& CarrierID, CString& SlotMap);
	BOOL CjobValidityCheck(const char*);
	BOOL IsLPInService(UCHAR PortNo);
	BOOL IsEqpIdle();
	BOOL GetSubstLocList(vector<string>& Locs);
	BOOL GetAllSubstIDS(vector<string>& SubstIds);
	BOOL bAutoOnline;
	BOOL bAutoConnect;
	BOOL bCJJobControl;

	//============================================================
	// Constructor and destructor

	void Log(CString msg, short lvl = 5);
	void GetAllCJobIDs(vector<string>& IdList);
	void GetAllPRJobIDs(vector<string>& IdList);
	BOOL GetCurrentCJobID(string& Id);
	BOOL GetCurrentPRJobID(short PortNo, string& Id);
	int DeleteObject(string& ObjID);
	int GetSubstAttr(const string& SubstID, const string& AttrID, SecsItemList*);
	int GetSubstLocAttr(const string& SubstLocID, const string& AttrID, SecsItemList*);
	BOOL IsSubstrate(const string& SubstID);
	BOOL IsSubstrateLoc(const string& SubstID);
	BOOL IsEqSubstrateLoc(const string& SubstID);
	int CanPerformPortAction(int PortID, const string&);
	int PerformE87Service(const char* szCarrierID, const char* szAction, vector < string >& AttrIDs,
		vector < SecsItem* >& AttrValues, SecsItemList* pErrorList, BYTE PortNo);
	int CanPerformService(string& Service, string& CarrierID, BYTE PortID, vector<string>& Attributes);
	void GetCurPrjobDetails(short PortNo, char* PrjobId, CPRJob::PRJOBSTATE* PrJobState);
	void ClearJobs();                 // Clear all Processjobs,ControlJobs,other datastructures after going offline
	bool GetAssociatedPorts(const string& PGrpName, vector<BYTE>& PortList);
	void AddPortGroup(PortGroup& PGrp);
	// 	BOOL SetRcpData(short PortNo, const string &PPID,short ScanNo,short nLine,short ScanLen);
	// 	BOOL SetRcpData(short PortNo, CString &PPID,short ScanNo,short nLine,short ScanLen);
	short GetPortNumber(string& CarrierName);
	BOOL CarrierPresent(string& ProcessJobName);
	CPRJob* GetPrjobObj(CString& PrJobID);
	CCtrlJob* GetCtrlJobObj(const char*);
	BOOL IsCarrierPresent(const string& Carrier);
	BOOL IsCarrierPresent(const char* Carrier);
	void GetCarrierIds(vector<string>& CIds);
	//int GetAvailableQSpace();//  [9/12/2007 LYF]
	BOOL GetCarrierID(const string& SubstID, CString& CarrierID);
	void CleanUp();
	//void AbortProcessing();
	//void StopProcessing();
	void ClearAll();						// Reset the load ports

	void RemovePRJobInCtrJob(char* pObjid);

	void GetQueuedCJobsList(vector<string>& CjList);

	BOOL RemoveCtrlJob(CCtrlJob* pCJob, BOOL bSave = TRUE);

	CCtrlJob* GetCompletedCjobObj(char*);
	CDataBank();
	virtual ~CDataBank();

private:
	BOOL CheckCJobAttr(map<string, SecsItem*>& Map);
	BOOL ValidatePrjobMatlInCJ(const char* CJobID);
	BOOL IsCJobPjobsPresent(const char* CjobID);
	BOOL IsCjobCarrierPresent(const char* CJobID);
	BOOL IsObjectSupported(const char* ObjType);
	int IsCarrierLocation(const string& LocID);
};

// extern CDataBank m_DataBank;
