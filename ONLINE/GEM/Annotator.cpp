#include "stdafx.h"

#pragma warning( disable : 4701 4100 4244 4189 4245 4310 4786 )

#include "CediListCtroller.h"
#include "IPC/SArams.h"
#include "SRC/DOSUtil.h"
// #include "SVID_LIST.h"

#include "Annotator.h"

Annotator Annot;

Annotator::Annotator() {
	bInit = false;
}

Annotator::~Annotator() {}

BOOL Annotator::Annotate(const SecsMessage& Msg, UINT Stream, UINT Function, string& Data) {
	if (!(Stream == 6 && Function == 11)) {
		return FALSE;
	}
	if (!bInit) {
		Init();
	}
	string TempStr;
	UINT CEID;
	SecsMessage& Ref = Msg.MsgRef;
	if ((1 != Ref.size()) ||
		(!Ref[0].IsList()) ||
		(3 != Ref[0].size()) ||
		(!Ref[0][0].IsAInt()) ||
		(!Ref[0][1].GetAInt(CEID))) {
		return FALSE; // Incorrect Format
	}
	Data += "\r\n";
	Data += "Annotated Event Output\r\n";
	Ref[0][0].GetData(TempStr);
	Data += "DataID: ";
	Data += TempStr;
	Data += "\t";
	CEIDCtlr.CeidDesc(CEID, TempStr);
	Data += TempStr;
	Data += "\r\n";

	MakeReport(Ref[0][2], Data);
	return TRUE;
}

bool Annotator::Init() {
	// Read in the Reports Definitions
	FILE* fp = NULL;
	_VIdVec Temp;
	CString Dir;
	DosUtil.GetProgramDir(Dir);
	Dir += "default.rpt";
	fp = fopen(Dir.GetBuffer(0), "rb");
	if (fp) {
		UINT Data[2];
		while (2 == fread(Data, sizeof(UINT), 2, fp)) {
			UINT* pSVID = new UINT[Data[1]];
			if (!pSVID) break;
			if (Data[1] == fread(pSVID, sizeof(UINT), Data[1], fp)) {
				for (UINT iSVID = 0; iSVID < Data[1]; iSVID++) {
					Temp.push_back(pSVID[iSVID]);
				}
				ReportMap.insert(_ReportMap::value_type(Data[0], Temp));
				Temp.clear();
			}
			delete[] pSVID;
			pSVID = NULL;
		}
		fclose(fp);
		bInit = true;
	}

	//	SVID_LIST *p = &CEIDCtlr.m_SVList;
	SVID_ITEM* p = CEIDCtlr.m_SVList.m_Svid;

	DecodeMap.clear();
	DecodeMap.insert(_DecodeMap::value_type(0, &Annotator::DecodeDefault));	// Default
	DecodeMap.insert(_DecodeMap::value_type((p + SVID_LIST::CONTROLSTATELINEMODE)->m_svidno, &Annotator::ControlState)); // Control State
	DecodeMap.insert(_DecodeMap::value_type((p + SVID_LIST::EQUIPMENTSTATUS)->m_svidno, &Annotator::EquipStat));    // Equipment Status
	DecodeMap.insert(_DecodeMap::value_type(102, &Annotator::CarrierLoc));
	DecodeMap.insert(_DecodeMap::value_type(103, &Annotator::CarrierLoc));
	DecodeMap.insert(_DecodeMap::value_type((p + SVID_LIST::LP1CARRIERACCESSSTATUS)->m_svidno, &Annotator::AccessMode));
	DecodeMap.insert(_DecodeMap::value_type((p + SVID_LIST::LP2CARRIERACCESSSTATUS)->m_svidno, &Annotator::AccessMode));
	DecodeMap.insert(_DecodeMap::value_type((p + SVID_LIST::LP1TRANSFERSTATUS)->m_svidno, &Annotator::TransferState));
	DecodeMap.insert(_DecodeMap::value_type((p + SVID_LIST::LP2TRANSFERSTATUS)->m_svidno, &Annotator::TransferState));
	DecodeMap.insert(_DecodeMap::value_type(4100, &Annotator::CarrierLocationMatrix));
	DecodeMap.insert(_DecodeMap::value_type((p + SVID_LIST::LPASSOCIATESTATEENUM)->m_svidno, &Annotator::PortAssociationStateList));
	DecodeMap.insert(_DecodeMap::value_type(4102, &Annotator::PortStateInfoList));
	DecodeMap.insert(_DecodeMap::value_type((p + SVID_LIST::LPTRANSFERSTATEENUM)->m_svidno, &Annotator::PortTransferStateList));
	DecodeMap.insert(_DecodeMap::value_type(4109, &Annotator::TransferStateEnum));
	DecodeMap.insert(_DecodeMap::value_type(4124, &Annotator::AccessModeEnum));
	DecodeMap.insert(_DecodeMap::value_type(4126, &Annotator::PortStateInfo));
	DecodeMap.insert(_DecodeMap::value_type(4127, &Annotator::PortAssociationState));
	DecodeMap.insert(_DecodeMap::value_type(4128, &Annotator::PortTransferState));
	DecodeMap.insert(_DecodeMap::value_type(4129, &Annotator::PortReserve));
	DecodeMap.insert(_DecodeMap::value_type(4144, &Annotator::AccessModeEnum));
	DecodeMap.insert(_DecodeMap::value_type(4146, &Annotator::PortStateInfo));
	DecodeMap.insert(_DecodeMap::value_type(4147, &Annotator::PortAssociationState));
	DecodeMap.insert(_DecodeMap::value_type(4148, &Annotator::PortTransferState));
	DecodeMap.insert(_DecodeMap::value_type(4149, &Annotator::PortReserve));
	DecodeMap.insert(_DecodeMap::value_type(5003, &Annotator::SubstHistory));
	DecodeMap.insert(_DecodeMap::value_type(5006, &Annotator::SubstLocState));
	DecodeMap.insert(_DecodeMap::value_type(5008, &Annotator::SubstProcState));
	DecodeMap.insert(_DecodeMap::value_type(5010, &Annotator::SubstState));
	DecodeMap.insert(_DecodeMap::value_type(5011, &Annotator::SubstType));
	DecodeMap.insert(_DecodeMap::value_type(5012, &Annotator::SubstUsage));
	DecodeMap.insert(_DecodeMap::value_type(4001, &Annotator::EventsEnabled));
	DecodeMap.insert(_DecodeMap::value_type(4002, &Annotator::ControlState));
	DecodeMap.insert(_DecodeMap::value_type(4004, &Annotator::QueuedCjobList));
	DecodeMap.insert(_DecodeMap::value_type(4204, &Annotator::PPChangeStatus));
	DecodeMap.insert(_DecodeMap::value_type(4105, &Annotator::PortList));
	DecodeMap.insert(_DecodeMap::value_type(2002, &Annotator::AlarmsSet));

	return (bInit);
}

bool  Annotator::DecodeDefault(SecsItem& Item, std::string& Data) {
	string Temp;
	Item.GetData(Temp);
	Data += Temp;
	return true;
}

bool  Annotator::AlarmsSet(SecsItem& Item, std::string& Data) {
	if (!Item.IsList()) {
		Data += "(Invalid Alarms Format)";
		return false;
	}
	UINT nItems = Item.size();
	string Str;
	Data += "[ ";
	for (UINT iItems = 0; iItems < nItems; iItems++) {
		if (Item[iItems].IsAInt()) {
			Item[iItems].GetData(Str);
			Data += Str;
			Data += ", ";
			Str.erase();
		}
	}
	Data += " ]";
	return true;
}

bool  Annotator::PortList(SecsItem& Item, std::string& Data) {
	if (!Item.IsList()) {
		Data += "(Invalid Port list Format)";
		return false;
	}
	UINT nItems = Item.size();
	string Str;
	Data += "[ ";
	for (UINT iItems = 0; iItems < nItems; iItems++) {
		if (Item[iItems].IsAInt()) {
			Item[iItems].GetData(Str);
			Data += Str;
			Data += ", ";
			Str.erase();
		}
	}
	Data += " ]";
	return true;
}

bool  Annotator::ControlState(SecsItem& Item, std::string& Data) {
	UINT Temp = 0;
	string Str;
	Item.GetAInt(Temp);
	switch (Temp) {
	case 1: Str = "Online/Remote";
		break;
	case 2: Str = "Online/Local";
		break;
	case 3: Str = "Offline/Equip Offline";
		break;
	case 4: Str = "Offline/Host Offline";
		break;
	case 5: Str = "Attempt Online";
		break;
	default:
		Str = "<Unknown control state>";
		break;
	}
	Data += Str;
	return true;
}

bool  Annotator::EquipStat(SecsItem& Item, std::string& Data) {
	UINT Temp = 0;
	string Str;
	Item.GetAInt(Temp);
	switch (Temp) {
	case SArams::STANDBY:
	case SArams::NONSCHEDULED:
		Str = "Idle";
		break;
	case SArams::PRODUCTIVE:
		Str = "Working";
		break;
	case SArams::SCHEDULEDDOWN:
	case SArams::UNSCHEDULEDDOWN:
		Str = "Maintenance";
		break;
	default:
		Str = "Unknown";
		break;
	}

	Data += Str;
	return true;
}

bool  Annotator::CarrierLoc(SecsItem& Item, std::string& Data) {
	UINT Temp = 0;
	string Str;
	Item.GetAInt(Temp);
	switch (Temp) {
	case /*CCms::PLACEMENT*/ 0:
		Str = "NOT PRESENT";
		break;
	case /*CCms::DOCKED*/ 1:
		Str = "PRESENT";
		break;
	default:
		Str = "NO CARRIER LOCATION";
		break;
	}
	Data += Str;
	return true;
}

bool  Annotator::AccessMode(SecsItem& Item, std::string& Data) {
	UINT Temp = 0;
	string Str;
	Item.GetAInt(Temp);
	switch (Temp) {
	case /*CCms::MANUAL_ACCESS*/1:
		Str = "MANUAL";
		break;
	case /*CCms::AUTO_ACCESS*/2:
		Str = "AUTO";
		break;
	default:
		Str = "NO ACCESSMODE";
		break;
	}

	Data += Str;
	return true;
}

bool  Annotator::TransferState(SecsItem& Item, std::string& Data) {
	UINT Temp = 0;
	string Str;
	Item.GetAInt(Temp);
	switch (Temp) {
	case /*CCms::TRANSFER_BLOCKED*/1:
		Str = "READY_TO_LOAD";
		break;
	case /*CCms::TRANSFER_READY*/2:
		Str = "TRANSFER_BLOCKED";
		break;
	case /*CCms::TRANSFER_READY*/3:
		Str = "READY_TO_UNLOAD";
		break;
	default:
		Str = "NO TRANSFER STATE";
		break;
	}
	Data += Str;
	return true;
}
bool  Annotator::CarrierLocationMatrix(SecsItem& Item, std::string& Data) {
	char Buf[126];
	if ((!Item.IsList()) || (0 == Item.size())) {
		Data += "(Improper format of CarrierLocationMatrix)";
	}
	UINT nNumLp = Item.size();
	string Loc, CarrierID;
	UINT nNumLoc;
	sprintf(Buf, "[%d Loadports]", nNumLp);
	Data += Buf;
	for (UINT iLp = 0; iLp < nNumLp; iLp++) {
		if (!Item[iLp].IsList()) {
			Data += "(Incorrect location info)";
			continue;
		}
		nNumLoc = Item[iLp].size();
		if (!nNumLoc) {
			Data += "(No Locations specified)";
			continue;
		}
		sprintf(Buf, "[Lp%d %d Locations]", iLp + 1, nNumLoc);
		Data += Buf;
		for (UINT iLoc = 0; iLoc < nNumLoc; iLoc++) {
			if ((!Item[iLp][iLoc].IsList()) ||
				(2 != Item[iLp][iLoc].size()) ||
				(!Item[iLp][iLoc][0].GetASCII(Loc)) ||
				(!Item[iLp][iLoc][1].GetASCII(CarrierID))) {
				Data += "(Incorrect location information)";
				break;
			}
			sprintf(Buf, "[Loc %d (%s) Carrier (%s)]", iLoc + 1, Loc.c_str(), CarrierID.c_str());
			Data += Buf;
		}
	}
	return true;
}

bool  Annotator::PortAssociationStateList(SecsItem& Item, std::string& Data) {
	char Buf[126];
	if ((!Item.IsList()) || (0 == Item.size())) {
		Data += "(Invalid PortAssociatioList Format)";
		return false;
	}
	UINT nNumLp = Item.size();
	sprintf(Buf, "[%d Loadports]", nNumLp);
	Data += Buf;
	UINT State;
	string StateStr;
	for (UINT iLp = 0; iLp < nNumLp; iLp++) {
		if (!Item[iLp].GetUInt4(State)) {
			Data += "(Invalid PortAssociationState format)";
		}
		switch (State) {
		case 0:StateStr = "Not Associated"; break;
		case 1:StateStr = "Associated"; break;
		default:StateStr = "No Association state"; break;
		}
		sprintf(Buf, "[LP%d %s]", iLp + 1, StateStr.c_str());
		Data += Buf;
	}
	return true;
}

bool  Annotator::PortStateInfoList(SecsItem& Item, std::string& Data) {
	char Buf[126];
	if ((!Item.IsList()) || (0 == Item.size())) {
		Data += "(Invalid PortStateInfoList Format)";
		return false;
	}
	UINT nNumLp = Item.size();
	sprintf(Buf, "[%d Loadports]", nNumLp);
	Data += Buf;

	string Temp;
	for (UINT iLp = 0; iLp < nNumLp; iLp++) {
		if (!PortStateInfo(Item[iLp], Temp)) {
			sprintf(Buf, "(Invalid state info for Lp%d)", iLp + 1);
			Data += Buf;
			continue;
		}
		sprintf(Buf, "[Lp%d %s]", iLp + 1, Temp.c_str());
		Data += Buf;
		Temp.erase();
	}
	return true;
}

bool  Annotator::PortTransferStateList(SecsItem& Item, std::string& Data) {
	char Buf[126];
	if ((!Item.IsList()) || (0 == Item.size())) {
		Data += "(Invalid PortTransferStateList Format)";
		return false;
	}
	UINT nNumLp = Item.size();
	sprintf(Buf, "[%d Loadports]", nNumLp);
	Data += Buf;

	string Temp;
	for (UINT iLp = 0; iLp < nNumLp; iLp++) {
		if (!TransferState(Item[iLp], Temp)) {
			sprintf(Buf, "(Invalid Transfer info for Lp%d)", iLp + 1);
			Data += Buf;
			continue;
		}
		sprintf(Buf, "[Lp%d %s]", iLp + 1, Temp.c_str());
		Data += Buf;
		Temp.erase();
	}
	return true;
}

bool  Annotator::TransferStateEnum(SecsItem& Item, std::string& Data) {
	UINT AssocState;
	string Str;
	if (!Item.GetAInt(AssocState)) {
		Data += "Invalid format for Transferstate";
		return false;
	}
	switch (AssocState) {
	case 0:
		Str = "NOT ASSOCIATED";
		break;
	case 1:
		Str = "ASSOCIATED";
		break;
	default:
		Str = "NO ASSOCIATION STATE";
		break;
	}
	Data += Str;
	return true;
}

bool  Annotator::AccessModeEnum(SecsItem& Item, std::string& Data) {
	UINT State;
	string StateStr;
	if (!Item.GetUInt4(State)) {
		Data += "(Invalid AccessMode Format)";
		return false;
	}
	switch (State) {
	case 1:
		StateStr = "MANUAL";
		break;
	case 2:
		StateStr = "AUTO";
		break;
	default:
		StateStr = "NO ACCESS STATE";
		break;
	}
	Data += StateStr;
	return true;
}

bool  Annotator::PortStateInfo(SecsItem& Item, std::string& Data) {
	UINT AssoState, TransState;
	string AssoStateStr, TransStateStr;
	if ((!Item.IsList()) ||
		(2 != Item.size()) ||
		(!Item[0].GetUInt4(AssoState)) ||
		(!Item[1].GetUInt4(TransState))) {
		return false;
	}

	switch (AssoState) {
	case 0:
		AssoStateStr = "Not ASSOCIATED";
		break;
	case 1:
		AssoStateStr = "ASSOCIATED";
		break;
	default:
		AssoStateStr = "NO ASSOCIATION STATE";
		break;
	}
	Data += AssoStateStr;
	Data += "-";
	switch (TransState) {
	case 0:
		TransStateStr = "No Transfer State";
		break;
	case 1:
		TransStateStr = "Ready To Load";
		break;
	case 2:
		TransStateStr = "Transfer Blocked";
		break;
	case 3:
		TransStateStr = "Ready To Unload";
		break;
	case 4:
		TransStateStr = "Out Of Service";
		break;
	case 5:
		TransStateStr = "In Service";
		break;
	case 6:
		TransStateStr = "Transfer Ready";
		break;
	default:
		TransStateStr = "Illegal value for TransferStatus";
		break;
	}
	Data += TransStateStr;

	return true;
}
bool  Annotator::PortAssociationState(SecsItem& Item, std::string& Data) {
	UINT State;
	string StateStr;
	if (!Item.GetUInt4(State)) {
		Data += "(Invalid PortAssociationState Format)";
		return false;
	}
	switch (State) {
	case 0:
		StateStr = "NOT ASSOCIATED";
		break;
	case 1:
		StateStr = "ASSOCIATED";
		break;
	default:
		StateStr = "NO ASSOCIATION STATE";
		break;
	}
	Data += StateStr;
	return true;
}

bool  Annotator::PortTransferState(SecsItem& Item, std::string& Data) {
	UINT State;
	string StateStr;
	if (!Item.GetUInt4(State)) {
		Data += "(Invalid PortTransferState Format)";
		return false;
	}
	switch (State) {
	case 4:
		StateStr = "Out Of Service";
		break;
	case 1:
		StateStr = "Ready To Load ";
		break;
	case 2:
		StateStr = "Transfer Blocked";
		break;
	case 3:
		StateStr = "Ready To Unload";
		break;
	default:
		StateStr = "Invalid Transfer State";
		break;
	}
	Data += StateStr;
	return true;
}

bool  Annotator::PortReserve(SecsItem& Item, std::string& Data) {
	UINT State;
	string StateStr;
	if (!Item.GetUInt4(State)) {
		Data += "(Invalid PortReserve Format)";
		return false;
	}
	switch (State) {
	case 0:
		StateStr = "NOT RESERVED";
		break;
	case 1:
		StateStr = "RESERVKED";
		break;
	default:
		StateStr = "NO RESERVATION STATE";
		break;
	}
	Data += StateStr;
	return true;
}

bool  Annotator::SubstHistory(SecsItem& Item, std::string& Data) {
	if (!Item.IsList()) {
		return false;
	}
	char Buf[126];
	UINT nItems = Item.size();
	string LocID, TimeIn, TimeOut;
	for (UINT iItems = 0; iItems < nItems; iItems++) {
		if ((!Item[iItems].IsList()) ||
			(3 != Item[iItems].size()) ||
			(!Item[iItems][0].GetASCII(LocID)) ||
			(!Item[iItems][1].GetASCII(TimeIn)) ||
			(!Item[iItems][2].GetASCII(TimeOut))) {
			Data += "Invalid format for History;";
			return false;
		}
		sprintf(Buf, "[Loc(%s) In(%s) Out(%s)]", LocID.c_str(), TimeIn.c_str(), TimeOut.c_str());
		Data += Buf;
	}
	return true;
}

bool  Annotator::SubstLocState(SecsItem& Item, std::string& Data) {
	UINT bOcc;
	if (!Item.GetAInt(bOcc)) {
		Data += "Invalid SubstLocState Format";
		return false;
	}
	Data += bOcc ? "Occupied" : "UnOccupied";
	return true;
}

bool  Annotator::SubstProcState(SecsItem& Item, std::string& Data) {
	UINT State;
	string StateStr;
	if (!Item.GetUInt4(State)) {
		Data += "(Invalid SubstProcState Format)";
		return false;
	}
	switch (State) {
	case 1:
		StateStr = "NEEDS PROCESSING";
		break;
	case 2:
		StateStr = "IN PROCESS";
		break;
	case 3:
		StateStr = "PROCESSED";
		break;
	case 4:
		StateStr = "ABORTED";
		break;
	case 5:
		StateStr = "STOPPED";
		break;
	case 6:
		StateStr = "REJECTED";
		break;
	default:
		StateStr = "NO SUBST PROCESSING STATE";
		break;
	}
	Data += StateStr;
	return true;
}
bool  Annotator::SubstState(SecsItem& Item, std::string& Data) {
	UINT State;
	string StateStr;
	if (!Item.GetUInt4(State)) {
		Data += "(Invalid SubstState Format)";
		return false;
	}
	switch (State) {
	case 1:
		StateStr = "AT SOURCE";
		break;
	case 2:
		StateStr = "AT WORK";
		break;
	case 3:
		StateStr = "AT DESTINATION";
		break;
	default:
		StateStr = "NO SUBST TR STATE";
		break;
	}
	Data += StateStr;
	return true;
}

bool  Annotator::SubstType(SecsItem& Item, std::string& Data) {
	UINT State;
	string StateStr;
	if (!Item.GetUInt4(State)) {
		Data += "(Invalid SubstType Format)";
		return false;
	}
	switch (State) {
	case 0:
		StateStr = "WAFER";
		break;
	case 1:
		StateStr = "FLAT PANNEL";
		break;
	case 2:
		StateStr = "CD";
		break;
	case 3:
		StateStr = "MASK";
		break;
	default:
		StateStr = "UNKNOWN SUBSTRATE TYPE";
		break;
	}
	Data += StateStr;
	return true;
}

bool  Annotator::SubstUsage(SecsItem& Item, std::string& Data) {
	UINT State;
	string StateStr;
	if (!Item.GetUInt4(State)) {
		Data += "(Invalid SubstType Format)";
		return false;
	}
	switch (State) {
	case 0:
		StateStr = "PRODUCT";
		break;
	case 1:
		StateStr = "TEST";
		break;
	case 2:
		StateStr = "FILTER";
		break;
	case 3:
		StateStr = "CLEANING";
		break;
	default:
		StateStr = "UNKNOWN SUBSTRATE USAGE";
		break;
	}
	Data += StateStr;
	return true;
}

bool  Annotator::EventsEnabled(SecsItem& Item, std::string& Data) {
	if (!Item.IsList()) {
		Data += "(Invalid EventsEnabled Format)";
		return false;
	}
	UINT nItems = Item.size();
	string Str;
	Data += "[ ";
	for (UINT iItems = 0; iItems < nItems; iItems++) {
		if (Item[iItems].IsAInt()) {
			Item[iItems].GetData(Str);
			Data += Str;
			Data += ", ";
			Str.erase();
		}
	}
	Data += " ]";
	return true;
}

bool  Annotator::QueuedCjobList(SecsItem& Item, std::string& Data) {
	if (!Item.IsList()) {
		Data += "(Invalid QueuedCjobList Format)";
		return false;
	}
	UINT nItems = Item.size();
	string Str;
	Data += "[ ";
	for (UINT iItems = 0; iItems < nItems; iItems++) {
		if (Item[iItems].GetASCII(Str)) {
			Data += Str;
			Data += ", ";
			Str.erase();
		}
	}
	Data += " ]";
	return true;
}
bool  Annotator::PPChangeStatus(SecsItem& Item, std::string& Data) {
	UINT Temp = 0;
	string Str;
	Item.GetAInt(Temp);
	switch (Temp) {
	case 1:
		Str = "RECIPE CREATED";
		break;
	case 2:
		Str = "RECIPE EDITED";
		break;
	case 3:
		Str = "RECIPE DELETED";
		break;
	default:
		Str = "INVALID RECIPE OPERATION";
		break;
	}
	Data += Str;
	return true;
}

bool Annotator::MakeReport(SecsItem& RptRef, string& Data) {
	UINT nReports = RptRef.size(), ReportID, VID;
	_VIdVec TmpVec;
	_ReportMap::iterator rit;

	string TempStr;
	for (UINT iReport = 0; iReport < nReports; iReport++) {
		if ((RptRef[iReport].IsList()) &&
			(2 == RptRef[iReport].size()) &&
			(RptRef[iReport][0].GetAInt(ReportID)) &&
			(RptRef[iReport][1].IsList())) {
			RptRef[iReport][0].GetData(TempStr);
			Data += "\r\nReport ID:";
			Data += TempStr;
			Data += "\r\n";
			rit = ReportMap.find(ReportID);
			if (rit != ReportMap.end()) {
				TmpVec = rit->second;
			}
			UINT nVIDs = RptRef[iReport][1].size();
			for (UINT iVIDs = 0; iVIDs < nVIDs; iVIDs++) {
				VID = 0;
				if (TmpVec.size() > iVIDs) {
					VID = TmpVec[iVIDs];
					CEIDCtlr.SvidDesc(VID, TempStr);
					Data += TempStr;
					Data += ": ";
				}
				TempStr.erase();
				MakeItemDef(VID, RptRef[iReport][1][iVIDs], Data);
				Data += "\r\n";
			}
		}
	}
	return true;
}

bool Annotator::MakeItemDef(UINT SVID, SecsItem& Item, string& Data) {
	_DecodeMap::iterator dit = DecodeMap.find(SVID);
	if (dit != DecodeMap.end()) {
		(this->*(dit->second))(Item, Data);
	}
	else {
		DecodeDefault(Item, Data);
	}
	return true;
}