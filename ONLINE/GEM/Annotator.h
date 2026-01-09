#pragma once

#include "SecsMessage.h"
// #include "CediListCtroller.h"

typedef std::vector<UINT> _VIdVec;
typedef std::map<UINT, _VIdVec> _ReportMap;
typedef bool(Annotator::* pFn)(SecsItem&, std::string&);
typedef std::map<const UINT, pFn> _DecodeMap;

class Annotator {
	_ReportMap ReportMap;
	_DecodeMap DecodeMap;
	bool bInit;
public:
	bool MakeReport(SecsItem& Item, std::string& Data);
	BOOL Annotate(const SecsMessage& Msg, UINT stream, UINT Function, std::string& Data);
	Annotator();
	virtual ~Annotator();

private:
	bool MakeItemDef(UINT SVID, SecsItem& pItem, std::string& Data);
	bool Init();
	bool DecodeDefault(SecsItem&, std::string&);	// 1
	bool AlarmsSet(SecsItem&, std::string&);	// 1
	bool PortList(SecsItem&, std::string&);	// 1
	bool ControlState(SecsItem&, std::string&);		// 2
	bool EquipStat(SecsItem&, std::string&);		// ProcessJobID
	bool CarrierLoc(SecsItem&, std::string&);	//
	bool AccessMode(SecsItem&, std::string&);
	bool TransferStateEnum(SecsItem&, std::string&);
	bool CarrierLocationMatrix(SecsItem&, std::string&);
	bool PortAssociationStateList(SecsItem&, std::string&);
	bool PortStateInfoList(SecsItem&, std::string&);
	bool PortTransferStateList(SecsItem&, std::string&);
	bool TransferState(SecsItem&, std::string&);
	bool AccessModeEnum(SecsItem&, std::string&);
	bool PortStateInfo(SecsItem&, std::string&);
	bool PortAssociationState(SecsItem&, std::string&);
	bool PortTransferState(SecsItem&, std::string&);
	bool PortReserve(SecsItem&, std::string&);
	bool SubstHistory(SecsItem&, std::string&);
	bool SubstProcState(SecsItem&, std::string&);
	bool SubstState(SecsItem&, std::string&);
	bool SubstType(SecsItem&, std::string&);
	bool SubstUsage(SecsItem&, std::string&);
	bool SubstLocState(SecsItem&, std::string&);
	bool EventsEnabled(SecsItem&, std::string&);
	bool Prjobstate(SecsItem&, std::string&);
	bool QueuedCjobList(SecsItem&, std::string&);
	bool PPChangeStatus(SecsItem&, std::string&);
};
extern Annotator Annot;