#pragma once

#include "IPC/Smol.h"
// #include "OSSBASE.h"
// #include "GEM.h"
// #include "LPObj.h"
// #include "CarrierObj.h"
#include "IPC/SubstObj.h"

class SecsItem;
class SecsItemList;

#define LOT_ID_SIZE (33)
#define SUBSTRATE_ID_SIZE (64)

/*
 * This is  Carrier Management service
 */
typedef map <long, CSubstObj> SubRefMap;

class CCms /*: public COSSBASE*/
{
public:
	CIDRObj BCR;
	CCarrierObj CaObj;
	CLPObj LPObj;
	short PortID; // Relate the object instant to a port Yuen [7/3/2002]
	HANDLE m_hWait;
	static long MagicRef;
	SubRefMap TempSubstMap;
	BOOL m_bBypassReadID; // Each load port has one copy of this Yuen[6/26/2002 12:12]
	BOOL m_bBypassAGV; // Each load port has one copy of this Yuen[6/26/2002 12:12]

private:
	// static CString m_strCarrier;

	// m_OBjID is Carrier ID
	// m_OBjType is "Carrier"
	BOOL FindSubst(const string& SubstID, CSubstObj& Subst);
	BYTE AccessStatEnum();
	BYTE IDStatEnum();
	BYTE SMStatEnum();
	long AddRefObj(const CSubstObj& Ref);
public:
	static char SubstProcStateEnum(CSubstObj::SUBSTPROCSTATE st);
	static char SubstStateEnum(CSubstObj::SUBSTSTATE st);
	BOOL GetRefObj(long MagicRef, BOOL bRemove, CSubstObj&);

	//=====================================================
	// Pure Virtual function Override
	BOOL GetAttr(const char* szAttr, SecsItemList* pList);
	BOOL SetAttr(const char* szAttr, SecsItem* pItem);
	static void GetAttrName(CStringArray& nArray);

public:
	//====================================================
	// Basic function
public:
	BYTE TrStatEnum();
	// CSMOL SM;
	// Yuen  [6/25/2002]
	//	short m_nPort;
	//	short	m_Capacity;					// Maximum number of substrates in carrier
	//	short	m_SubstrateCount;			// The number of substrates currently in the carrier

	//  Yuen[6/25/2002 16:46]
	//	enum
	//	{
	//		VERIFICATION_NEEDED = 0,
	//		VERIFICATION_BY_EQUIPMENT,
	//		UNSUCCESSFUL,
	//		READ_FAIL,
	//		IMPROPER_WAFER_POSITION,
	//	} m_Reason;

	// Yuen  [6/25/2002]
	//	enum CARRIERIDSTATUS {
	//		ID_NO_STATUS = 0,
	//		ID_NOT_READ,
	//		ID_READ ,
	//		ID_WAIT_FOR_HOST,
	//		ID_VERIFICATION_OK ,
	//		ID_VERIFICATION_FAIL,
	//	} m_CarrierIDStatus;			// Current state of the Carrier ID verification
	BOOL SetCarrierIDStaus(CCarrierObj::IDSTATUS IDStatus);

	// Yuen  [6/25/2002]
	//	enum CARRIERACCESSINGSTATUS
	//	{
	//		NOT_ACCESSED = 0,
	//		IN_ACCESS,
	//		CARRIER_COMPLETE,
	//		CARRIER_STOPPED
	//	};
	void SetCarrierAccessingStatus(CCarrierObj::ACCESSINGSTATUS status);

	// Yuen  [6/25/2002]
	//	enum SLOTMAP
	//	{
	//		UNDEFINED = 0,
	//		EMPTY ,
	////		NOT_EMPTY ,
	//		CORRECTLY_OCCUPIED ,
	//		DOUBLE_SLOTTED,
	//		CROSS_SLOTTED
	//	};

	// Yuen  [6/25/2002]
	//	enum SLOTMAPSTATUS
	//	{
	//		SLOTMAP_NO_STATUS = 0,
	//		SLOTMAP_NOT_READ,
	//		SLOTMAP_WTG_HOST,
	//		SLOTMAP_VERIFICATION_OK ,
	//		SLOTMAP_VERIFICATION_FAIL
	//	};
	BOOL SetSlotMapStatus(CCarrierObj::SLOTMAPSTATUS MapStatus);

	// Yuen  [6/25/2002]
	//	CARRIERACCESSINGSTATUS  m_CarrierAccessingStatus;	// The current accessing state of the carrier by
	// the equipment .The current substarate of the
	// CarrierAccessingStatus state model.
	// Yuen  [6/25/2002]
	//#if 1
	//	CSubST  m_ContentMap[25];
	//#else
	//	struct
	//	{
	//		char szLotID[LOT_ID_SIZE];
	//		char szSubstrateID[SUBSTRATE_ID_SIZE];
	//	} m_ContentMap[25];
	//#endif
	// The list of lot and substrate identifiers,
	//	corresponding to slot 1, 2, 3.....n
	// Yuen  [6/25/2002]
	//	char		m_LocationID[33];						// Identifier of current loction. PORT NO
	//	SLOTMAP		m_SlotMap[26];						    // List of slot status as provided by the host
	//														//	until a successful slot map read , then as
	//														//	read by the equipment
	//
	//	SLOTMAPSTATUS m_SlotMapStatus;						// Current state of slot map verification
	//
	//	char		m_Usage[32];							// The type of material contained in the carrier
	//														// ( i.e., TEST , DUMMY , PRODUCT , FILTER , etc )

//	AGVObj AGV;
//	BCRObj BCR;

	// Yuen  [6/25/2002]
	//	enum ACCESSMODE
	//	{
	//		MANUAL_ACCESS = 0,
	//		AUTO_ACCESS,
	//		NO_ACCESS,
	//	} m_AccessMode;

	// Yuen  [6/25/2002]
//	LoadPortObj::ACCESSMODE GetAccessMode()
//	{
//		return LpObj.AccessMode;
//	}

	BOOL SetAccessMode(CGEMEvt& Ge, CLPObj::ACCESSMODE AccessMode, BOOL bChange = TRUE);

	// Yuen  [6/25/2002]
	//	enum LOCATION
	//	{
	//		PLACEMENT = 0,
	//		DOCKED,
	//		N_LOCATION
	//	} m_LocationStatus;
	BOOL SetLocation(CLPObj::CARRIERLOC nLocation);
	const char* GetLocationStr(CLPObj::CARRIERLOC loc);

	// Yuen  [6/25/2002]
	//	enum ASSOCIATION
	//	{
	//		NOT_ASSOCIATED = 0,
	//		ASSOCIATED,
	//		NO_ASSOCIATION,
	//	} m_AssociationState;
	//CLPObj::ASSOCIATION GetAssociationState();
	BOOL SetAssociation(CGEMEvt& Ge, CLPObj::ASSOCIATION Association, BOOL bChange = TRUE);

	// Yuen  [6/25/2002]
	//	enum RESERVATION
	//	{
	//		NOT_RESERVED = 0,
	//		RESERVED,
	//		NO_RESERVATION,
	//	} m_ReservationStatus;
	int SetReservation(CGEMEvt& Ge, CLPObj::RESERVATION Reservation, BOOL bChange = TRUE);

	int SetOccupancy(CGEMEvt& Ge, CLPObj::OCCUPANCY Occupancy, BOOL bChange = TRUE);

	//  Yuen[6/25/2002 14:13]
	//	enum SERVICESTATE
	//	{
	//		OUT_OF_SERVICE = 0,
	//		IN_SERVICE,
	//		NO_SERVICE_STATE
	//	} m_ServiceState;

	// Yuen  [6/25/2002]
	//	enum TRANSFERSTATE
	//	{
	//		TRANSFER_BLOCKED = IN_SERVICE, // 1
	//		TRANSFER_READY,
	//	} m_TransferState;

	// Yuen  [6/25/2002]
	//	enum READYSTATE
	//	{
	//		READY_TO_LOAD = TRANSFER_READY, // 2
	//		READY_TO_UNLOAD,
	//	} m_ReadyState;

	//CLPObj::TRANSFERSTATUS GetTransferState();
	BOOL SetTransferState(CGEMEvt& Ge, CLPObj::TRANSFERSTATUS nNewState, BOOL bChange = TRUE);

	//	const char *GetCarrierID();

	int CarryOutPortAction(const char* szAction /*, CCms *pTemp*/);
	int CarryOutCarrierAction(const char* szCarrierID, const char* szAction, vector < string >& AttrIDs,
		vector < SecsItem* >& AttrValues, SecsItemList* pErrorList, int PortNo);

public:
	void LoadComplete(short PortID);
	void Init(short port);
	//Already defined in CLPObj
	//short AccessModeEnum();
	void ClearCarrierObj();
	BOOL ProcessCancelCarrier();
	BOOL IsSMMatched(CString& SlotMap);
	BOOL IsSMVerificationOk();
	BOOL IsIdVerificationOk();
	BOOL IsDocked(void);
	static void GetSubstLocAttrIds(vector<string>& AttrIds);
	static void GetSubstAttrIds(vector<string>& AttrIds);
	static void GetCarrierAttrIds(vector<string>& AttriDs);
	void SetLPObjID(short port);
	BOOL SubstLocChange(CGEMEvt& Ge);
	//BOOL SubstLocChange(short SlotNo, SubstLocObj::SUBSTLOCSTATE NewState, char *LocId);
	BOOL UpdateSlotmapStatus(CGEMEvt& Ge);
	/// Following section implements all Substreate related functions in terms of CMS
	BOOL STSMChanged(CGEMEvt& Ge);
	//BOOL STSMChanged(short SlotNo, CSubstObj::SUBSTSTATE);
	BOOL STPMChanged(CGEMEvt& Ge);
	//BOOL STPMChanged(short SlotNo, CSubstObj::SUBSTPROCSTATE NewState);

	// Get attr for Substrate implemented in terms of CMS
	BOOL GetSubstAttr(const string& ObjId, string&, SecsItemList&);

	BOOL IsReadyToLoad(void);
	BOOL CanCelCarrier();
	void ClearContentMap();
	void SetSlotMapVerifyEvent();
	void SetCarrierIDVerifyEvent();
	void ClearAll();
	void SetInitialize();
	void SetOBjectID(char* objid, int sz);
	void SetSlotMap(const char* szSlotMap);
	void SetPortID(char* portid);
	// void SetContentMap( int arryno , char* mappstatus);
	// void SetCarrierIDStaus(short status );
	void SetCurSubStateCount(short count);
	//	BOOL SetCarrierID( char * carrierid );
	CCms();
	virtual ~CCms();
};
