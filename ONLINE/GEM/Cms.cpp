// Cms.cpp: implementation of the CCms class.
//
//////////////////////////////////////////////////////////////////////

/***************************************************************************
 * File		: Cms.cpp
 * Revision : A . 1
 * Author	: Alex , Kang
 * Date		: Aug 24.2000
 * Purpose  :
 *			The command to the  Carrier Cancel
 * Arugument :
 *
 *
 * Return value   :
 *			 BOOL	: TRUE; Cancel Success
 *					  FALSE; Cancel Fail
 * Revision History
 *		   A . 1 : April 19 .2001
 *				   Created ..
 * Remark :
 *
 **************************************************************************/

#include "stdafx.h"
#include "strSOp.h"

#include "128/GEM/ObjectsContainer.h"
#include "GEMEvt.h"
#include "GemFactory.h"
#include "GlobalDeviceContainer.h"
#include "IFC/SecsSvrF.h"
#include "ONLINE/INC/Event.h"
#include "ONLINE/ShareMemory/Evt.h"
#include "ONLINE/TCPIP/HsmsParaMeter.h"
#include "SRC/DOSUtil.h"
 // #include "ONLINE/ShareMemory/DataBank.h"
 // #include "ONLINE/ShareMemory/SharedMemQue.h"
 // #include "SecsItem.h"

#include "Cms.h"
#include <E95/413App.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

long CCms::MagicRef = 0;

#define CASSETTEMAXWAFER	25
#define BEGIN_SLOEVENTS		2043

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCms::CCms() {
	PortID = 0;
	m_bBypassAGV = FALSE;
	m_bBypassReadID = FALSE;
}

CCms::~CCms() {}

BOOL CCms::GetAttr(const char* szAttr, SecsItemList* pList) {
	CString strAttr(szAttr);

	if (strAttr == "ObjType") {
		pList->AddItem(new SecsItemASCII(CaObj.ObjType));
		return TRUE;
	}
	else if (strAttr == "ObjID") {
		pList->AddItem(new SecsItemASCII(CaObj.ObjID));
		return  TRUE;
	}
	else if (strAttr == "Capacity") {
		pList->AddItem(new SecsItemUInt1(CaObj.Capacity));
		return TRUE;
	}
	else if (strAttr == "SubstrateCount") {
		pList->AddItem(new SecsItemUInt1(CaObj.SubstrateCount));
		return TRUE;
	}
	else if (strAttr == "CarrierIDStatus") {
		pList->AddItem(new SecsItemUInt1(IDStatEnum()));
		return TRUE;
	}
	else if (strAttr == "CarrierAccessingStatus") {
		pList->AddItem(new SecsItemUInt1(AccessStatEnum()));
		return TRUE;
	}
	else if (strAttr == "ContentMap") {
		SecsItemList* pTemp = new SecsItemList;
		for (int i = 0; i < CaObj.Capacity; i++) {
			SecsItemList* pContent = new SecsItemList;
			pContent->AddItem(new SecsItemASCII(CaObj.CtMap[i].LotID));
			pContent->AddItem(new SecsItemASCII(CaObj.CtMap[i].SubstID));
			pTemp->AddItem(pContent);
		}
		pList->AddItem(pTemp);
		return TRUE;
	}
	else if (strAttr == "LocationID") {
		pList->AddItem(new SecsItemASCII(CaObj.LocationID));
		return TRUE;
	}
	else if (strAttr == "SlotMap") {
		SecsItemList* pTemp = new SecsItemList;
		for (int i = 0; i < CaObj.Capacity; i++) {
			// Yuen  [6/25/2002]
			pTemp->AddItem(new SecsItemUInt1(CaObj.SlotMaP[i]));
		}
		pList->AddItem(pTemp);
		return TRUE;
	}
	else if (strAttr == "SlotMapStatus") {
		pList->AddItem(new SecsItemUInt1(SMStatEnum()));
		return TRUE;
	}
	else if (strAttr == "Usage") {
		pList->AddItem(new SecsItemASCII(CaObj.Usage));
		return TRUE;
	}
	else {
		return FALSE;
	}
}

BYTE CCms::AccessStatEnum() {
	switch (CaObj.AccessingStatus) {
	case CCarrierObj::NOTACCESSED:
		return 0;
		break;
	case CCarrierObj::INACCESS:
		return 1;
		break;
	case CCarrierObj::CARRIERCOMPLETE:
		return 2;
		break;
	case CCarrierObj::CARRIERSTOPPED:
		return 3;
		break;
	}
	return -1;
}

BYTE CCms::TrStatEnum() {
	return LPObj.GetTransferStatus();
}

BYTE CCms::SMStatEnum() {
	switch (CaObj.SlotmapStatus) {
	case CCarrierObj::SMNOTREAD:
		return 0;
		break;
	case CCarrierObj::SMWAITINGFORHOST:
		return 1;
		break;
	case CCarrierObj::SMVERIFICATIONOK:
		return 2;
		break;
	case CCarrierObj::SMVERIFICATIONFAIL:
		return 3;
		break;
	}
	return -1;
}

BYTE CCms::IDStatEnum() {
	switch (CaObj.AccessingStatus) {
	case CCarrierObj::IDNOTREAD:
		return 0;
		break;
	case CCarrierObj::IDWAITINGFORHOST:
		return 1;
		break;
	case CCarrierObj::IDVERIFICATIONOK:
		return 2;
		break;
	case CCarrierObj::IDVERIFICATIONFAIL:
		return 3;
		break;
	}
	return -1;
}

//BOOL CCms::SetAttr(const char* szAttr, SecsItem* pList) {
//	CString strAttr(szAttr);
//
//	if (strAttr == "ObjType") {
//		string temp;
//		if (!pList->GetASCII(temp)) {
//			return FALSE;
//		}
//		strscpy(CaObj.ObjType, OBJTYPELEN, temp.c_str());
//	}
//	else if (strAttr == "ObjID") {
//		if (!pList->IsASCII()) {
//			return FALSE;
//		}
//		string temp;
//		pList->GetData(temp);
//		strscpy(CaObj.ObjID, OBJIDLEN, temp.c_str());
//	}
//	else if (strAttr == "Capacity") {
//		if (!pList->IsUInt1()) {
//			return FALSE;
//		}
//		string temp;
//		pList->GetData(temp);
//		CaObj.Capacity = atoi(temp.c_str());
//	}
//	else if (strAttr == "SubstrateCount") {
//		if (!pList->IsUInt1()) {
//			return FALSE;
//		}
//		string temp;
//		pList->GetData(temp);
//		CaObj.SubstrateCount = atoi(temp.c_str());
//	}
//	else if (strAttr == "CarrierIDStatus") {
//		if (!pList->IsUInt4()) {
//			return FALSE;
//		}
//		string temp;
//		pList->GetData(temp);
//		CaObj.IDStatus = (CCarrierObj::IDSTATUS)atoi(temp.c_str());
//	}
//	else if (strAttr == "CarrierAccessingStatus") {
//		if (!pList->IsUInt4()) {
//			return FALSE;
//		}
//		string temp;
//		pList->GetData(temp);
//		CaObj.AccessingStatus = (CCarrierObj::ACCESSINGSTATUS)atoi(temp.c_str());
//	}
//	else if (strAttr == "ContentMap") {
//		if (!pList->IsList()) {
//			return FALSE;
//		}
//		for (int i = 0; i < pList->NumContainedItems(); i++) {
//			SecsItem* pContent = pList->ContainedItems(i);
//			if (!pContent->IsList() || (pContent->NumContainedItems() != 2)) {
//				return FALSE;
//			}
//
//			string temp;
//			SecsItem* p;
//			p = pContent->ContainedItems(0);
//			if (!p->IsASCII()) {
//				return FALSE;
//			}
//			CaObj.Substrate[i].Preset();
//			CaObj.Substrate[i].SlotNo = i + 1;
//			p->GetData(temp);
//			strscpy(CaObj.CtMap[i].LotID, LOTIDLEN, temp.c_str());
//			strscpy(CaObj.Substrate[i].LotID, LOTIDLEN, temp.c_str());
//
//			p = pContent->ContainedItems(1);
//			if (!p->IsASCII()) {
//				return FALSE;
//			}
//			p->GetData(temp);
//			strscpy(CaObj.CtMap[i].SubstID, SUBSTIDLEN, temp.c_str());
//			strscpy(CaObj.Substrate[i].ObjID, OBJIDLEN, temp.c_str());
//		}
//	}
//	else if (strAttr == "LocationID") {
//		if (!pList->IsASCII()) {
//			return FALSE;
//		}
//		string temp;
//		pList->GetData(temp);
//		strscpy(CaObj.LocationID, SUBSTLOCIDLEN, temp.c_str());
//	}
//	else if (strAttr == "SlotMap") {
//		if (!pList->IsList()) return FALSE;
//		int i;
//		for (i = 0; i < CaObj.Capacity; i++) {
//			CaObj.SlotMaP[i] = CSubstObj::igSLOTMAP;
//		}
//		// Slotmap starts at index 0
//		// Assumed to be of size Capacity.
//		for (i = 0; i < pList->NumContainedItems(); i++) {
//			SecsItem* pTemp = pList->ContainedItems(i);
//			if (pTemp->IsUInt1() || pTemp->IsUInt2()) {
//				string temp;
//				pTemp->GetData(temp);
//				CaObj.SlotMaP[i] = temp[0];
//				// 				switch(atoi(temp.c_str()))
//				// 				{
//				// 				case CSubstObj::SM_EMPTY:
//				// 					CaObj.SlotMap[i] = CSubstObj::SM_EMPTY;
//				// 					break;
//				// 				case CSubstObj::SM_NOT_EMPTY:
//				// 					CaObj.SlotMap[i] = CSubstObj::SM_NOT_EMPTY;
//				// 					break;
//				// 				case CSubstObj::SM_CORRECTLY_OCCUPIED:
//				// 					CaObj.SlotMap[i] = CSubstObj::SM_CORRECTLY_OCCUPIED;
//				// 					break;
//				// 				case CSubstObj::SM_CROSS_SLOTTED:
//				// 					CaObj.SlotMap[i] = CSubstObj::SM_CROSS_SLOTTED;
//				// 					break;
//				// 				case CSubstObj::SM_DOUBLE_SLOTTED:
//				// 					CaObj.SlotMap[i] = CSubstObj::SM_DOUBLE_SLOTTED;
//				// 					break;
//				// 				default:
//				// 					CaObj.SlotMap[i] = CSubstObj::SM_UNDEFINED;
//				// 					break;
//				// 				}
//			}
//			else {
//				return FALSE;
//			}
//		}
//	}
//	else if (strAttr == "SlotMapStatus") {
//		if (!pList->IsASCII()) return FALSE;
//		string temp;
//		pList->GetData(temp);
//		CaObj.SlotmapStatus = (CCarrierObj::SLOTMAPSTATUS)atoi(temp.c_str());
//	}
//	else if (strAttr == "Usage") {
//		if (!pList->IsASCII()) return FALSE;
//		string temp;
//		pList->GetData(temp);
//		strscpy(CaObj.Usage, USAGELEN, temp.c_str());
//	}
//	else {
//		return FALSE;
//	}
//	return TRUE;
//}
BOOL CCms::SetAttr(const char* szAttr, SecsItem* pList) {
	CString strAttr(szAttr);

	if (strAttr == "ObjType") {
		string temp;
		if (!pList->GetASCII(temp)) {
			return FALSE;
		}
		strscpy(CaObj.ObjType, OBJTYPELEN, temp.c_str());
	}
	else if (strAttr == "ObjID") {
		if (!pList->IsASCII()) {
			return FALSE;
		}
		string temp;
		pList->GetData(temp);
		strscpy(CaObj.ObjID, OBJIDLEN, temp.c_str());
	}
	else if (strAttr == "Capacity") {
		if (!pList->IsUInt1()) {
			return FALSE;
		}
		string temp;
		pList->GetData(temp);
		CaObj.Capacity = atoi(temp.c_str());
	}
	else if (strAttr == "SubstrateCount") {
		if (!pList->IsUInt1()) {
			return FALSE;
		}
		string temp;
		pList->GetData(temp);
		CaObj.SubstrateCount = atoi(temp.c_str());
	}
	else if (strAttr == "CarrierIDStatus") {
		if (!pList->IsUInt4()) {
			return FALSE;
		}
		string temp;
		pList->GetData(temp);
		CaObj.IDStatus = (CCarrierObj::IDSTATUS)atoi(temp.c_str());
	}
	else if (strAttr == "CarrierAccessingStatus") {
		if (!pList->IsUInt4()) {
			return FALSE;
		}
		string temp;
		pList->GetData(temp);
		CaObj.AccessingStatus = (CCarrierObj::ACCESSINGSTATUS)atoi(temp.c_str());
	}
	else if (strAttr == "ContentMap") {
		if (!pList->IsList()) {
			return FALSE;
		}
		for (int i = 0; i < pList->NumContainedItems(); i++) {
			SecsItem* pContent = pList->ContainedItems(i);
			if (!pContent->IsList() || (pContent->NumContainedItems() != 2)) {
				return FALSE;
			}

			string temp;
			SecsItem* p;
			p = pContent->ContainedItems(0);
			if (!p->IsASCII()) {
				return FALSE;
			}
			CaObj.Substrate[i].Preset();
			CaObj.Substrate[i].SlotNo = i + 1;
			p->GetData(temp);
			strscpy(CaObj.CtMap[i].LotID, LOTIDLEN, temp.c_str());
			strscpy(CaObj.Substrate[i].LotID, LOTIDLEN, temp.c_str());

			p = pContent->ContainedItems(1);
			if (!p->IsASCII()) {
				return FALSE;
			}
			p->GetData(temp);
			strscpy(CaObj.CtMap[i].SubstID, SUBSTIDLEN, temp.c_str());
			strscpy(CaObj.Substrate[i].ObjID, OBJIDLEN, temp.c_str());
		}
	}
	else if (strAttr == "LocationID") {
		if (!pList->IsASCII()) {
			return FALSE;
		}
		string temp;
		pList->GetData(temp);
		strscpy(CaObj.LocationID, SUBSTLOCIDLEN, temp.c_str());
	}
	else if (strAttr == "SlotMap") {
		if (!pList->IsList()) return FALSE;
		int i;
		//[[08302024/Masum
		p413App->Global.LoadMaxSlot();
		int nSlot = p413App->Global.SlotsPerFOUP;
		int idx = 0;
		if (nSlot == 13)
		{
			for (i = 0; i < nSlot; i++) {
				CaObj.SlotMaP[i] = CSubstObj::igSLOTMAP;
			}
			// Slotmap starts at index 0
			// Assumed to be of size Capacity.
			for (i = 0; i < pList->NumContainedItems(); i++) { 
				if (i != 0)
				{
					if (i % 2 != 0) continue;
				}
				SecsItem* pTemp = pList->ContainedItems(i);
				if (pTemp->IsUInt1() || pTemp->IsUInt2()) {
					string temp;
					pTemp->GetData(temp);
					CaObj.SlotMaP[idx] = temp[0]; //// Host send 25 slots from SML but we take only odd number
					idx++;
				}
				else {
					return FALSE;
				}
			}
		}//]
		else
		{
			for (i = 0; i < CaObj.Capacity; i++) {
				CaObj.SlotMaP[i] = CSubstObj::igSLOTMAP;
			}
			// Slotmap starts at index 0
			// Assumed to be of size Capacity.
			for (i = 0; i < pList->NumContainedItems(); i++) {
				SecsItem* pTemp = pList->ContainedItems(i);
				if (pTemp->IsUInt1() || pTemp->IsUInt2()) {
					string temp;
					pTemp->GetData(temp);
					CaObj.SlotMaP[i] = temp[0];
					// 				switch(atoi(temp.c_str()))
					// 				{
					// 				case CSubstObj::SM_EMPTY:
					// 					CaObj.SlotMap[i] = CSubstObj::SM_EMPTY;
					// 					break;
					// 				case CSubstObj::SM_NOT_EMPTY:
					// 					CaObj.SlotMap[i] = CSubstObj::SM_NOT_EMPTY;
					// 					break;
					// 				case CSubstObj::SM_CORRECTLY_OCCUPIED:
					// 					CaObj.SlotMap[i] = CSubstObj::SM_CORRECTLY_OCCUPIED;
					// 					break;
					// 				case CSubstObj::SM_CROSS_SLOTTED:
					// 					CaObj.SlotMap[i] = CSubstObj::SM_CROSS_SLOTTED;
					// 					break;
					// 				case CSubstObj::SM_DOUBLE_SLOTTED:
					// 					CaObj.SlotMap[i] = CSubstObj::SM_DOUBLE_SLOTTED;
					// 					break;
					// 				default:
					// 					CaObj.SlotMap[i] = CSubstObj::SM_UNDEFINED;
					// 					break;
					// 				}
				}
				else {
					return FALSE;
				}
			}
		}
	}
	else if (strAttr == "SlotMapStatus") {
		if (!pList->IsASCII()) return FALSE;
		string temp;
		pList->GetData(temp);
		CaObj.SlotmapStatus = (CCarrierObj::SLOTMAPSTATUS)atoi(temp.c_str());
	}
	else if (strAttr == "Usage") {
		if (!pList->IsASCII()) return FALSE;
		string temp;
		pList->GetData(temp);
		strscpy(CaObj.Usage, USAGELEN, temp.c_str());
	}
	else {
		return FALSE;
	}
	return TRUE;
}

void CCms::GetAttrName(CStringArray& nArray) {
	nArray.Add("ObjType");
	nArray.Add("ObjID");
	nArray.Add("Capacity");
	nArray.Add("SubstrateCount");
	nArray.Add("CarrierIDStatus");
	nArray.Add("CarrierAccessingStatus");
	nArray.Add("ContentMap");
	nArray.Add("LocationID");
	nArray.Add("SlotMap");
	nArray.Add("SlotMapStatus");
	nArray.Add("Usage");
}

void CCms::SetCurSubStateCount(short count) {
	CaObj.SubstrateCount = count;
}

BOOL CCms::SetCarrierIDStaus(CCarrierObj::IDSTATUS IDStatus) {
	int CEIDs[10];
	int nCEID = 0;
	char Status[52] = { 0 };

	if (!CHsmsParaMeter::IsOnline()) return FALSE;

	switch (IDStatus) {
	case CCarrierObj::IDNOTREAD:
		strscpy(Status, sizeof(Status), "Not Read");
		if (CaObj.IDStatus == CCarrierObj::IDNOSTATE) {
			switch (PortID) {
			case 1:
				CEIDs[nCEID++] = CEID_LIST::CST1NOSTATETOIDNOTREAD;
				break;
			case 2:
				CEIDs[nCEID++] = CEID_LIST::CST2NOSTATETOIDNOTREAD;
				break;
			}
		}
		break;
	case CCarrierObj::IDWAITINGFORHOST:
		strscpy(Status, sizeof(Status), "Waiting for Host");
		if (CaObj.IDStatus == CCarrierObj::IDNOSTATE) {
			switch (PortID) {
			case 1:
				CEIDs[nCEID++] = CEID_LIST::CST1NOSTATETOWAITINGFORHOST;
				break;
			case 2:
				CEIDs[nCEID++] = CEID_LIST::CST2NOSTATETOWAITINGFORHOST;
				break;
			}
		}
		else if (CaObj.IDStatus == CCarrierObj::IDNOTREAD) {
			switch (PortID) {
			case 1:
				CEIDs[nCEID++] = CEID_LIST::CST1IDNOTREADTOWAITINGFORHOST;
				break;
			case 2:
				CEIDs[nCEID++] = CEID_LIST::CST2IDNOTREADTOWAITINGFORHOST;
				break;
			}
		}
		break;
	case CCarrierObj::IDVERIFICATIONOK:
		strscpy(Status, sizeof(Status), "Verification OK");
		if (CaObj.IDStatus == CCarrierObj::IDNOSTATE) {
			switch (PortID) {
			case 1:
				CEIDs[nCEID++] = CEID_LIST::CST1NOSTATETOIDVEROK;
				break;
			case 2:
				CEIDs[nCEID++] = CEID_LIST::CST2NOSTATETOIDVEROK;
				break;
			}
		}
		else if (CaObj.IDStatus == CCarrierObj::IDNOTREAD) {
			if (pGDev->pDataBnk->m_bBypassReadID || m_bBypassReadID) {
				switch (PortID) {
				case 1:
					CEIDs[nCEID++] = CEID_LIST::CST1IDNOTREADTOIDVEROK;
					break;
				case 2:
					CEIDs[nCEID++] = CEID_LIST::CST2IDNOTREADTOIDVEROK;
					break;
				}
			}
			else {
				switch (PortID) {
				case 1:
					CEIDs[nCEID++] = CEID_LIST::CST1IDNOTREADTOIDVEROK;
					break;
				case 2:
					CEIDs[nCEID++] = CEID_LIST::CST2IDNOTREADTOIDVEROK;
					break;
				}
			}
		}
		else if (CaObj.IDStatus == CCarrierObj::IDWAITINGFORHOST) {
			switch (PortID) {
			case 1:
				CEIDs[nCEID++] = CEID_LIST::CST1WAITINGFORHOSTTOIDVEROK;
				break;
			case 2:
				CEIDs[nCEID++] = CEID_LIST::CST2WAITINGFORHOSTTOIDVEROK;
				break;
			}
		}
		break;
	case CCarrierObj::IDVERIFICATIONFAIL:
		strscpy(Status, sizeof(Status), "Verification Fail");
		if (CaObj.IDStatus == CCarrierObj::IDNOSTATE) {
			switch (PortID) {
			case 1:
				CEIDs[nCEID++] = CEID_LIST::CST1NOSTATETOIDVERFAIL;
				break;
			case 2:
				CEIDs[nCEID++] = CEID_LIST::CST2NOSTATETOIDVERFAIL;
				break;
			}
		}
		else if (CaObj.IDStatus == CCarrierObj::IDWAITINGFORHOST) {
			switch (PortID) {
			case 1:
				CEIDs[nCEID++] = CEID_LIST::CST1WAITINGFORHOSTTOIDVERFAIL;
				break;
			case 2:
				CEIDs[nCEID++] = CEID_LIST::CST2WAITINGFORHOSTTOIDVERFAIL;
				break;
			}
		}
		break;
	}

	if (!nCEID) return FALSE;

	CaObj.IDStatus = IDStatus;

	if (CHsmsParaMeter::IsOnline()) // Added online test [9/13/2007 LYF]
	{
		CSharedMemQue EventQueue;
		EventQueue.OpenMemory("EVTMSG");

		for (int i = 0; i < nCEID; i++) {
			if (CEIDs[i] == 0) {
				ASSERT(0);
				continue;
			}
			CEvt* pEvt = new CEvt;
			ASSERT(pEvt);
			ASSERT(PortID);
			pEvt->PortNo = PortID;
			pEvt->SetMsg((BYTE*)CaObj.ObjID, strlen(CaObj.ObjID));
			pEvt->Set(EVT_CMS, CEIDs[i]);
			pGDev->SetActivePort(PortID);
			EventQueue.WriteQue(pEvt);
			pGDev->ClearActivePort();
		}
		EventQueue.CloseMemory();
	}
	return TRUE;
}

void CCms::SetCarrierAccessingStatus(CCarrierObj::ACCESSINGSTATUS status) {
	short nCEID = 0;
	UINT CEID[3] = { 0, 0, 0 };

	if (!CHsmsParaMeter::IsOnline()) return;

	switch (CaObj.AccessingStatus) {
	case CCarrierObj::ASNOSTATE:
		if (status == CCarrierObj::NOTACCESSED) {
			switch (PortID) {
			case 1:
				CEID[nCEID++] = CEID_LIST::CST1NOSTATETONOTACCESSED;
				break;
			case 2:
				CEID[nCEID++] = CEID_LIST::CST2NOSTATETONOTACCESSED;
				break;
			}
		}
		else if (status == CCarrierObj::INACCESS) {
			switch (PortID) {
			case 1:
				CEID[nCEID++] = CEID_LIST::CST1NOSTATETONOTACCESSED;
				CEID[nCEID++] = CEID_LIST::CST1NOTACCESSEDTOINACCESS;
				break;
			case 2:
				CEID[nCEID++] = CEID_LIST::CST2NOSTATETONOTACCESSED;
				CEID[nCEID++] = CEID_LIST::CST2NOTACCESSEDTOINACCESS;
				break;
			}
		}
		break;
	case CCarrierObj::NOTACCESSED:
		if (status == CCarrierObj::INACCESS) {
			switch (PortID) {
			case 1:
				CEID[nCEID++] = CEID_LIST::CST1NOTACCESSEDTOINACCESS;
				break;
			case 2:
				CEID[nCEID++] = CEID_LIST::CST2NOTACCESSEDTOINACCESS;
				break;
			}
		}
		break;
	case CCarrierObj::INACCESS:
		if (status == CCarrierObj::CARRIERCOMPLETE) {
			switch (PortID) {
			case 1:
				CEID[nCEID++] = CEID_LIST::CST1INACCESSTOCARRIERCOMPLETE;
				break;
			case 2:
				CEID[nCEID++] = CEID_LIST::CST2INACCESSTOCARRIERCOMPLETE;
				break;
			}
		}
		else if (status == CCarrierObj::CARRIERSTOPPED) {
			switch (PortID) {
			case 1:
				CEID[nCEID++] = CEID_LIST::CST1INACCESSTOCARRIERSTOPPED;
				break;
			case 2:
				CEID[nCEID++] = CEID_LIST::CST2INACCESSTOCARRIERSTOPPED;
				break;
			}
		}
		break;
	case CCarrierObj::CARRIERCOMPLETE:
		ASSERT(0);
		break;
	case CCarrierObj::CARRIERSTOPPED:
		ASSERT(0);
		break;
	case CCarrierObj::igACCESSINGSTATUS:
		ASSERT(0);
		break;
	}
	CaObj.AccessingStatus = status;
	for (int i = 0; i < nCEID; i++) {
		CSharedMemQue EventQueue;
		EventQueue.OpenMemory("EVTMSG");
		CEvt* pEvt = new CEvt;
		ASSERT(pEvt);
		ASSERT(PortID);
		pEvt->PortNo = PortID;
		pEvt->Set(EVT_CMS, CEID[i]);
		pEvt->SetMsg((BYTE*)CaObj.ObjID, strlen(CaObj.ObjID));
		pGDev->SetActivePort(PortID);
		EventQueue.WriteQue(pEvt);
		pGDev->ClearActivePort();
		EventQueue.CloseMemory();
	}
}

void CCms::SetPortID(char* portid) {
	memcpy(CaObj.LocationID, portid, strlen(portid));
}

void CCms::SetSlotMap(const char* szSlotMap) {
	//Yuen[6/25/2002 12:29]
	for (int i = 0; i < CaObj.Capacity; i++) {
		CaObj.SlotMaP[i] = CSubstObj::GetSMEnum(szSlotMap[i]);
	}
}

void CCms::SetInitialize() {
	int i = 0;
	for (i = 0; i < MAXCAPACITYLEN; i++) {
		ZeroMemory(&CaObj.CtMap[i], sizeof(CContentMap));
		CaObj.SlotMaP[i] = CSubstObj::SM_EMPTY;
	}
}

void CCms::ClearAll() {
	//	int i = 0;

		// ZeroMemory( m_OBjID , sizeof m_OBjID );
		// m_AttrIDName.RemoveAll();

	//	CaObj.Preset();
	//	LPObj.Preset();
}

BOOL CCms::SetSlotMapStatus(CCarrierObj::SLOTMAPSTATUS MapStatus) {
	int CEIDs[10];
	int nCEID = 0;

	if (!CHsmsParaMeter::IsOnline()) return FALSE;

	switch (MapStatus) {
	case CCarrierObj::SMNOTREAD:
		if (CaObj.SlotmapStatus == CCarrierObj::SMNOSTATE) {
			switch (PortID) {
			case 1:
				CEIDs[nCEID++] = CEID_LIST::CST1NOSTATETOSLOTMAPNOTREAD;
				break;
			case 2:
				CEIDs[nCEID++] = CEID_LIST::CST2NOSTATETOSLOTMAPNOTREAD;
				break;
			}
		}
		break;
	case CCarrierObj::SMWAITINGFORHOST:
		if (CaObj.SlotmapStatus == CCarrierObj::SMNOTREAD) {
			switch (PortID) {
			case 1:
				CEIDs[nCEID++] = CEID_LIST::CST1SLOTMAPNOTREADTOWAITINGFORHOST;
				break;
			case 2:
				CEIDs[nCEID++] = CEID_LIST::CST2SLOTMAPNOTREADTOWAITINGFORHOST;
				break;
			}
		}
		break;
	case CCarrierObj::SMVERIFICATIONOK:
		if (CaObj.SlotmapStatus == CCarrierObj::SMNOTREAD) {
			switch (PortID) {
			case 1:
				CEIDs[nCEID++] = CEID_LIST::CST1SLOTMAPNOTREADTOSLOTMAPVEROK;
				break;
			case 2:
				CEIDs[nCEID++] = CEID_LIST::CST2SLOTMAPNOTREADTOSLOTMAPVEROK;
				break;
			}
		}
		else if (CaObj.SlotmapStatus == CCarrierObj::SMWAITINGFORHOST) {
			switch (PortID) {
			case 1:
				CEIDs[nCEID++] = CEID_LIST::CST1WAITINGFORHOSTTOSLOTMAPVEROK;
				break;
			case 2:
				CEIDs[nCEID++] = CEID_LIST::CST2WAITINGFORHOSTTOSLOTMAPVEROK;
				break;
			}
		}
		break;
	case CCarrierObj::SMVERIFICATIONFAIL:
		if (CaObj.SlotmapStatus == CCarrierObj::SMWAITINGFORHOST) {
			switch (PortID) {
			case 1:
				CEIDs[nCEID++] = CEID_LIST::CST1WAITINGFORHOSTTOSLOTMAPVERFAIL;
				break;
			case 2:
				CEIDs[nCEID++] = CEID_LIST::CST2WAITINGFORHOSTTOSLOTMAPVERFAIL;
				break;
			}
		}
		break;
	default:
		ASSERT(0);
		break;
	}
	if (0 == nCEID) return 0;

	CaObj.SlotmapStatus = MapStatus;

	CSharedMemQue EventQueue;
	EventQueue.OpenMemory("EVTMSG");

	for (int i = 0; i < nCEID; i++) {
		if (CEIDs[i] == 0) {
			ASSERT(0);
			continue;
		}
		CEvt* pEvt = new CEvt;
		ASSERT(pEvt);
		ASSERT(PortID);
		pEvt->PortNo = PortID;
		pEvt->SetMsg((BYTE*)CaObj.ObjID, strlen(CaObj.ObjID));
		pGDev->SetActivePort(PortID);
		pEvt->Set(EVT_CMS, CEIDs[i]);
		EventQueue.WriteQue(pEvt);
		pGDev->ClearActivePort();
	}
	EventQueue.CloseMemory();
	return TRUE;
}

int CCms::CarryOutPortAction(const char* szAction) {
	const char szOutOfService[] = "OUTOFSERVICE";		// Port Access = 1
	const char szInService[] = "INSERVICE";			// Port Access = 2
	const char szTransferBlock[] = "TRANSFERBLOCKED";	// Port Access = 3
	const char szReadyToLoad[] = "READYTOLOAD";		// Port Access = 4
	const char szReadyToUnload[] = "READYTOUNLOAD";		// Port Access = 5
	const char szAuto[] = "AUTO";				// Port Access = 6
	const char szManual[] = "MANUAL";			// Port Access = 7
	const char szReserveAtPort[] = "RESERVATIONATPORT";	// Port Access = 8
	const char szCancelReserveAtPort[] = "CANCELRESERVATIONATPORT";// Port Access = 9
	const char szDefine[] = "DEFINE";// Port Access = 10

	if (_stricmp(szAction, szOutOfService) == 0) {
		CLPObj::TRANSFERSTATUS nNewState;
		nNewState = CLPObj::OUTOFSERVICE;
		CGEMEvt Ge;
		Ge.bTransition = TRUE;
		if (!SetTransferState(Ge, nNewState, FALSE)) return 5;
	}
	else if (_stricmp(szAction, szInService) == 0) {
		CLPObj::TRANSFERSTATUS nNewState;
		if (LPObj.GetTransferStatus() == CLPObj::OUTOFSERVICE) {
			// VJTD: Need to know whether there is a carrier on the load port
			nNewState = CLPObj::READYTOLOAD;
		}
		else {
			return 0;
		}
		CGEMEvt Ge;
		Ge.bTransition = TRUE;
		if (!SetTransferState(Ge, nNewState, FALSE)) return 5;
	}
	else if (_stricmp(szAction, szAuto) == 0) {
		CGEMEvt Ge;
		Ge.bTransition = TRUE;
		SetAccessMode(Ge, CLPObj::AUTO, FALSE);
	}
	else if (_stricmp(szAction, szManual) == 0) {
		CGEMEvt Ge;
		Ge.bTransition = TRUE;
		SetAccessMode(Ge, CLPObj::MANUAL, FALSE);
	}
	else if (_stricmp(szAction, szReserveAtPort) == 0) {
		int Res = pGDev->pDataBnk->CanPerformPortAction(PortID, szAction);
		if (Res) {
			return Res;
		}
		CGEMEvt Ge;
		Ge.bTransition = TRUE;
		SetReservation(Ge, CLPObj::RESERVED, FALSE);
	}
	else if (_stricmp(szAction, szCancelReserveAtPort) == 0) {
		int Res = pGDev->CanPerformPortAction(PortID, szAction);
		if (Res) {
			return Res;
		}
		CGEMEvt Ge;
		Ge.bTransition = TRUE;
		SetReservation(Ge, CLPObj::RESERVED, FALSE);
	}
	else {
		return 1;
	}

	return 0;
}

BOOL CCms::SetAccessMode(CGEMEvt& Ge, CLPObj::ACCESSMODE AccessMode, BOOL bChange) {
	int CEIDs[10];
	int nCEID = 0;

	if (!CHsmsParaMeter::IsOnline()) {
		return FALSE;
	}

	if (!Ge.bTransition) {
		// Report state only
		switch (AccessMode) {
		case CLPObj::AUTO:
			switch (PortID) {
			case 1:
				CEIDs[nCEID++] = CEID_LIST::AMSM1MANUALTOAUTO;
				break;
			case 2:
				CEIDs[nCEID++] = CEID_LIST::AMSM2MANUALTOAUTO;
				break;
			}
			break;
		case CLPObj::MANUAL:
			switch (PortID) {
			case 1:
				CEIDs[nCEID++] = CEID_LIST::AMSM1AUTOTOMANUAL;
				break;
			case 2:
				CEIDs[nCEID++] = CEID_LIST::AMSM2AUTOTOMANUAL;
				break;
			}
			break;
		default:
			ASSERT(0);
			return FALSE;
		}
		if (0 < nCEID) {
			CSharedMemQue EventQueue;
			EventQueue.OpenMemory("EVTMSG");

			for (int i = 0; i < nCEID; i++) {
				if (CEIDs[i] == 0) {
					ASSERT(0);
					continue;
				}
				CEvt* pEvt = new CEvt;
				ASSERT(pEvt);
				ASSERT(PortID);
				pEvt->PortNo = PortID;
				pEvt->Set(EVT_CMS, CEIDs[i]);
				pGDev->SetActivePort(PortID);
				EventQueue.WriteQue(pEvt);
				pGDev->ClearActivePort();
			}
			EventQueue.CloseMemory();
		}
		return TRUE;
	}

	if (LPObj.GetAccessMode() == CLPObj::amNOSTATE) {
		switch (AccessMode) {
		case CLPObj::MANUAL:
			switch (PortID) {
			case 1:
				CEIDs[nCEID++] = CEID_LIST::AMSM1NOSTATETOMANUAL;
				break;
			case 2:
				CEIDs[nCEID++] = CEID_LIST::AMSM2NOSTATETOMANUAL;
				break;
			}
			break;
		case CLPObj::AUTO:
			switch (PortID) {
			case 1:
				CEIDs[nCEID++] = CEID_LIST::AMSM1NOSTATETOAUTO;
				break;
			case 2:
				CEIDs[nCEID++] = CEID_LIST::AMSM2NOSTATETOAUTO;
				break;
			}
			break;
		default:
			//ASSERT(0);
			return FALSE;
		}
	}
	else if (LPObj.GetAccessMode() == AccessMode) {
		// Do Nothing
	}
	else {
		switch (PortID) {
		case 1:
			CEIDs[nCEID++] = ((LPObj.GetAccessMode() == CLPObj::AUTO) ? CEID_LIST::AMSM1AUTOTOMANUAL : CEID_LIST::AMSM1MANUALTOAUTO);
			break;
		case 2:
			CEIDs[nCEID++] = ((LPObj.GetAccessMode() == CLPObj::AUTO) ? CEID_LIST::AMSM2AUTOTOMANUAL : CEID_LIST::AMSM2MANUALTOAUTO);
			break;
		}
	}

	if (bChange) {
		//  Yuen[6/25/2002 15:11]
		LPObj.SetAccessMode(AccessMode, PortID);
		if (0 < nCEID) {
			CSharedMemQue EventQueue;
			EventQueue.OpenMemory("EVTMSG");

			for (int i = 0; i < nCEID; i++) {
				if (CEIDs[i] == 0) {
					ASSERT(0);
					continue;
				}
				CEvt* pEvt = new CEvt;
				ASSERT(pEvt);
				ASSERT(PortID);
				pEvt->PortNo = PortID;
				pEvt->Set(EVT_CMS, CEIDs[i]);
				pGDev->SetActivePort(PortID);
				EventQueue.WriteQue(pEvt);
				pGDev->ClearActivePort();
			}
			EventQueue.CloseMemory();
		}
	}
	else {
		// Yuen: Host change access mode [2002/09/23/12:56]
		CSMOL SM;
		SM.cmd = SM.cmdLPSTATUS;
		SM.Lp.PortNo = PortID;
		SM.Lp.act = CLPStatus::WRITE;
		SM.Lp.Association = CLPObj::igASSOCIATION;
		SM.Lp.Occupancy = CLPObj::igOCCUPENCY;
		SM.Lp.Reservation = CLPObj::igRESERVATION;
		SM.Lp.TransferStatus = CLPObj::igTRANSFERSTATUS;
		SM.Lp.AccessMode = ((AccessMode == CLPObj::AUTO) ? CLPObj::AUTO : CLPObj::MANUAL);
		if (pGDev && pGDev->pGEMObjs->pSecsSvr) {
			pGDev->pGEMObjs->pSecsSvr->LoadPortStatusExec(SM);
		}
	}
	return TRUE;
}

BOOL CCms::SetTransferState(CGEMEvt& Ge, CLPObj::TRANSFERSTATUS nNewState, BOOL bChange) {
	int CEIDs[10];
	int nCEID = 0;

	if (!CHsmsParaMeter::IsOnline()) return FALSE;

	if (!Ge.bTransition) {
		switch (nNewState) {
		case CLPObj::OUTOFSERVICE:
			switch (Ge.PortID) {
			case 1:
				CEIDs[nCEID++] = CEID_LIST::LP1OUTOFSERVICE2INSERVICE;
				break;
			case 2:
				CEIDs[nCEID++] = CEID_LIST::LP2OUTOFSERVICE2INSERVICE;
				break;
			default:
				ASSERT(0);
				break;
			}
			break;
		case CLPObj::READYTOLOAD:
			switch (Ge.PortID) {
			case 1:
				CEIDs[nCEID++] = CEID_LIST::LP1READYTOLOAD;
				break;
			case 2:
				CEIDs[nCEID++] = CEID_LIST::LP2READYTOLOAD;
				break;
			default:
				ASSERT(0);
				break;
			}
			break;
		case CLPObj::READYTOUNLOAD:
			switch (Ge.PortID) {
			case 1:
				CEIDs[nCEID++] = CEID_LIST::LP1READYTOUNLOAD;
				break;
			case 2:
				CEIDs[nCEID++] = CEID_LIST::LP2READYTOUNLOAD;
				break;
			default:
				ASSERT(0);
				break;
			}
			break;
		case CLPObj::TRANSFERBLOCKED:
			switch (Ge.PortID) {
			case 1:
				CEIDs[nCEID++] = CEID_LIST::LP1TRANSFERBLOCKED;
				break;
			case 2:
				CEIDs[nCEID++] = CEID_LIST::LP2TRANSFERBLOCKED;
				break;
			default:
				ASSERT(0);
				break;
			}
			break;
		default:
			ASSERT(0);
			return FALSE;
			break;
		}

		if (0 < nCEID) {
			CSharedMemQue EventQueue;
			EventQueue.OpenMemory("EVTMSG");

			for (int i = 0; i < nCEID; i++) {
				if (CEIDs[i] == 0) {
					ASSERT(0);
					continue;
				}
				CEvt* pEvt = new CEvt;
				ASSERT(pEvt);
				ASSERT(PortID);
				pEvt->PortNo = PortID;
				pEvt->SetMsg((BYTE*)CaObj.ObjID, strlen(CaObj.ObjID));
				pEvt->Set(EVT_CMS, CEIDs[i]);
				pGDev->SetActivePort(PortID);
				EventQueue.WriteQue(pEvt);
				pGDev->ClearActivePort();
			}
			EventQueue.CloseMemory();
		}
		return TRUE;
	}
	switch (nNewState) {
	case CLPObj::OUTOFSERVICE:
	{
		switch (LPObj.GetTransferStatus()) {
		case CLPObj::tsNOSTATE:
			switch (Ge.PortID) {
			case 1:
				CEIDs[nCEID++] = CEID_LIST::LP1NOSTATE2OUTOFSERVICE;	// No state to out of service
				break;
			case 2:
				CEIDs[nCEID++] = CEID_LIST::LP2NOSTATE2OUTOFSERVICE;	// No state to out of service
				break;
			default:
				ASSERT(0);
				break;
			}
			break;
		default:
			switch (Ge.PortID) {
			case 1:
				CEIDs[nCEID++] = CEID_LIST::LP1INSERVICE2OUTOFSERVICE;	// In service to out of service
				break;
			case 2:
				CEIDs[nCEID++] = CEID_LIST::LP2INSERVICE2OUTOFSERVICE;	// In service to out of service
				break;
			default:
				ASSERT(0);
				break;
			}
			break;
		}
		break;
	}
	case CLPObj::READYTOLOAD:
	{
		switch (LPObj.GetTransferStatus()) {
		case CLPObj::tsNOSTATE:
			switch (Ge.PortID) {
			case 1:
				CEIDs[nCEID++] = CEID_LIST::LP1NOSTATE2INSERVICE;			// No state to in service
				CEIDs[nCEID++] = CEID_LIST::LP1INSERVICE2TRANSFERREADY;		// In service to transfer ready
				CEIDs[nCEID++] = CEID_LIST::LP1TRANSFERREADY2READYTOLOAD;	// Transfer ready to ready to load
				break;
			case 2:
				CEIDs[nCEID++] = CEID_LIST::LP2NOSTATE2INSERVICE;			// No state to in service
				CEIDs[nCEID++] = CEID_LIST::LP2INSERVICE2TRANSFERREADY;		// In service to transfer ready
				CEIDs[nCEID++] = CEID_LIST::LP2TRANSFERREADY2READYTOLOAD;	// Transfer ready to ready to load
				break;
			default:
				ASSERT(0);
				break;
			}
			break;
		case CLPObj::TRANSFERBLOCKED:
			switch (Ge.PortID) {
			case 1:
				CEIDs[nCEID++] = CEID_LIST::LP1TRANSFERBLOCKED2READYTOLOAD;	// Transfer block to ready to load
				break;
			case 2:
				CEIDs[nCEID++] = CEID_LIST::LP2TRANSFERBLOCKED2READYTOLOAD;	// Transfer block to ready to load
				break;
			default:
				ASSERT(0);
				break;
			}
			break;
		case CLPObj::READYTOUNLOAD:
			switch (Ge.PortID) {
			case 1:
				CEIDs[nCEID++] = CEID_LIST::LP1READYTOUNLOAD2TRANSFERBLOCKED;	// Ready to unload to transfer blocked
				CEIDs[nCEID++] = CEID_LIST::LP1TRANSFERBLOCKED2READYTOLOAD;		// Transfer blocked to ready to load
				break;
			case 2:
				CEIDs[nCEID++] = CEID_LIST::LP2READYTOUNLOAD2TRANSFERBLOCKED;	// Ready to unload to transfer blocked
				CEIDs[nCEID++] = CEID_LIST::LP2TRANSFERBLOCKED2READYTOLOAD;		// Transfer blocked to ready to load
				break;
			default:
				ASSERT(0);
				break;
			}
			break;
		case CLPObj::OUTOFSERVICE:
			switch (Ge.PortID) {
			case 1:
				CEIDs[nCEID++] = CEID_LIST::LP1OUTOFSERVICE2INSERVICE;		// Out of service to in service
				CEIDs[nCEID++] = CEID_LIST::LP1INSERVICE2TRANSFERREADY;		// In service to transfer ready
				CEIDs[nCEID++] = CEID_LIST::LP1TRANSFERREADY2READYTOLOAD;	// Transfer ready to ready to load
				break;
			case 2:
				CEIDs[nCEID++] = CEID_LIST::LP2OUTOFSERVICE2INSERVICE;		// Out of service to in service
				CEIDs[nCEID++] = CEID_LIST::LP2INSERVICE2TRANSFERREADY;		// In service to transfer ready
				CEIDs[nCEID++] = CEID_LIST::LP2TRANSFERREADY2READYTOLOAD;	// Transfer ready to ready to load
				break;
			default:
				ASSERT(0);
				break;
			}
			break;
		}
		break;
	}
	case CLPObj::READYTOUNLOAD:
	{
		switch (LPObj.GetTransferStatus()) {
		case CLPObj::OUTOFSERVICE:
			switch (Ge.PortID) {
			case 1:
				CEIDs[nCEID++] = CEID_LIST::LP1OUTOFSERVICE2INSERVICE;		// Out of service to in service
				CEIDs[nCEID++] = CEID_LIST::LP1INSERVICE2TRANSFERREADY;		// In service to transfer ready
				CEIDs[nCEID++] = CEID_LIST::LP1TRANSFERREADY2READYTOUNLOAD;	// Transfer ready to ready to unload
				break;
			case 2:
				CEIDs[nCEID++] = CEID_LIST::LP2OUTOFSERVICE2INSERVICE;		// Out of service to in service
				CEIDs[nCEID++] = CEID_LIST::LP2INSERVICE2TRANSFERREADY;		// In service to transfer ready
				CEIDs[nCEID++] = CEID_LIST::LP2TRANSFERREADY2READYTOUNLOAD;	// Transfer ready to ready to unload
				break;
			default:
				ASSERT(0);
				break;
			}
			break;
		case CLPObj::tsNOSTATE:
			switch (Ge.PortID) {
			case 1:
				CEIDs[nCEID++] = CEID_LIST::LP1NOSTATE2INSERVICE;			// No state to in service
				CEIDs[nCEID++] = CEID_LIST::LP1INSERVICE2TRANSFERREADY;		// In service to transfer ready
				CEIDs[nCEID++] = CEID_LIST::LP1TRANSFERREADY2READYTOUNLOAD;	// Transfer ready to ready to unload
				break;
			case 2:
				CEIDs[nCEID++] = CEID_LIST::LP2NOSTATE2INSERVICE;			// No state to in service
				CEIDs[nCEID++] = CEID_LIST::LP2INSERVICE2TRANSFERREADY;		// In service to transfer ready
				CEIDs[nCEID++] = CEID_LIST::LP2TRANSFERREADY2READYTOUNLOAD;	// Transfer ready to ready to unload
				break;
			default:
				ASSERT(0);
				break;
			}
			break;
		case CLPObj::TRANSFERBLOCKED:
			switch (Ge.PortID) {
			case 1:
				CEIDs[nCEID++] = CEID_LIST::LP1TRANSFERBLOCKED2READYTOUNLOAD;	// Transfer block to ready to unload
				break;
			case 2:
				CEIDs[nCEID++] = CEID_LIST::LP2TRANSFERBLOCKED2READYTOUNLOAD;	// Transfer block to ready to unload
				break;
			default:
				ASSERT(0);
				break;
			}
			break;
		case CLPObj::READYTOLOAD:
			switch (Ge.PortID) {
			case 1:
				CEIDs[nCEID++] = CEID_LIST::LP1READYTOLOAD2TRANSFERBLOCKED;		// Ready to load to Transfer block
				CEIDs[nCEID++] = CEID_LIST::LP1TRANSFERBLOCKED2READYTOUNLOAD;	// Transfer block to ready to unload
				break;
			case 2:
				CEIDs[nCEID++] = CEID_LIST::LP2READYTOLOAD2TRANSFERBLOCKED;		// Ready to load to Transfer block
				CEIDs[nCEID++] = CEID_LIST::LP2TRANSFERBLOCKED2READYTOUNLOAD;	// Transfer block to ready to unload
				break;
			default:
				ASSERT(0);
				break;
			}
			break;
		}
		break;
	}
	case CLPObj::TRANSFERBLOCKED:
	{
		switch (LPObj.GetTransferStatus()) {
		case CLPObj::OUTOFSERVICE:
			switch (Ge.PortID) {
			case 1:
				CEIDs[nCEID++] = CEID_LIST::LP1OUTOFSERVICE2INSERVICE;		// Out of service to in service
				CEIDs[nCEID++] = CEID_LIST::LP1INSERVICE2TRANSFERBLOCKED;	// In service to transfer blocked
				break;
			case 2:
				CEIDs[nCEID++] = CEID_LIST::LP2OUTOFSERVICE2INSERVICE;		// Out of service to in service
				CEIDs[nCEID++] = CEID_LIST::LP2INSERVICE2TRANSFERBLOCKED;	// In service to transfer blocked
				break;
			default:
				ASSERT(0);
				break;
			}
			break;
		case CLPObj::tsNOSTATE:
			switch (Ge.PortID) {
			case 1:
				CEIDs[nCEID++] = CEID_LIST::LP1NOSTATE2INSERVICE;			// No state to in service
				CEIDs[nCEID++] = CEID_LIST::LP1INSERVICE2TRANSFERBLOCKED;	// In service to transfer blocked
				break;
			case 2:
				CEIDs[nCEID++] = CEID_LIST::LP2NOSTATE2INSERVICE;			// No state to in service
				CEIDs[nCEID++] = CEID_LIST::LP2INSERVICE2TRANSFERBLOCKED;	// In service to transfer blocked
				break;
			default:
				ASSERT(0);
				break;
			}
			break;
		case CLPObj::READYTOLOAD:
			switch (Ge.PortID) {
			case 1:
				CEIDs[nCEID++] = CEID_LIST::LP1READYTOLOAD2TRANSFERBLOCKED;	// Ready to load to Transfer block
				break;
			case 2:
				CEIDs[nCEID++] = CEID_LIST::LP2READYTOLOAD2TRANSFERBLOCKED;	// Ready to load to Transfer block
				break;
			default:
				ASSERT(0);
				break;
			}
			break;
		case CLPObj::READYTOUNLOAD:
			switch (Ge.PortID) {
			case 1:
				CEIDs[nCEID++] = CEID_LIST::LP1READYTOUNLOAD2TRANSFERBLOCKED;// Ready to unload to Transfer block
				break;
			case 2:
				CEIDs[nCEID++] = CEID_LIST::LP2READYTOUNLOAD2TRANSFERBLOCKED;// Ready to unload to Transfer block
				break;
			default:
				ASSERT(0);
				break;
			}
			break;
		}
		break;
	}
	default:
		ASSERT(0);
		break;
	}

	if (bChange) {
		// Note:  Yuen[2002/08/03 20:04]
		LPObj.SetTransferStatus(nNewState, PortID);
		if (0 < nCEID) {
			CSharedMemQue EventQueue;
			EventQueue.OpenMemory("EVTMSG");

			for (int i = 0; i < nCEID; i++) {
				if (CEIDs[i] == 0) {
					ASSERT(0);
					continue;
				}
				CEvt* pEvt = new CEvt;
				ASSERT(pEvt);
				ASSERT(PortID);
				pEvt->PortNo = PortID;
				pEvt->SetMsg((BYTE*)CaObj.ObjID, strlen(CaObj.ObjID));
				pEvt->Set(EVT_CMS, CEIDs[i]);
				pGDev->SetActivePort(PortID);
				EventQueue.WriteQue(pEvt);
				pGDev->ClearActivePort();
			}
			EventQueue.CloseMemory();
		}
	}
	else {
		// Yuen: Host send port action [2002/09/23/10:38]
		CSMOL SM;
		SM.cmd = SM.cmdLPSTATUS;
		SM.Lp.PortNo = PortID;
		SM.Lp.act = CLPStatus::WRITE;
		SM.Lp.Association = CLPObj::igASSOCIATION;
		SM.Lp.Occupancy = CLPObj::igOCCUPENCY;
		SM.Lp.Reservation = CLPObj::igRESERVATION;
		SM.Lp.TransferStatus = nNewState;
		SM.Lp.AccessMode = CLPObj::igACCESSMODE;
		if (pGDev && pGDev->pGEMObjs->pSecsSvr) {
			pGDev->pGEMObjs->pSecsSvr->LoadPortStatusExec(SM);
		}
	}
	return TRUE;
}

//  [7/6/2012 Yuen]
const char* CCms::GetLocationStr(CLPObj::CARRIERLOC locID) {
	switch (locID) {
	case CLPObj::clUNDOCKED:
		return "PLACEMENT";
	case CLPObj::clDOCKED:
		return "DOCKED";
	case CLPObj::clNOSTATE:
		return "AWAY";
	}
	return "";
}

BOOL CCms::SetLocation(CLPObj::CARRIERLOC nLocation) {
	LPObj.CarrierLoc = nLocation;
	return TRUE;
}

int CCms::SetAssociation(CGEMEvt& Ge, CLPObj::ASSOCIATION Association, BOOL bChange) {
	int CEIDs[10];
	int nCEID = 0;

	if (!CHsmsParaMeter::IsOnline()) return FALSE;

	if (!Ge.bTransition) {
		switch (Association) {
		case CLPObj::ASSOCIATED:
			switch (PortID) {
			case 1:
				CEIDs[nCEID++] = CEID_LIST::LPASM1NOTASSOCIATEDTOASSOCIATED;
				break;
			case 2:
				CEIDs[nCEID++] = CEID_LIST::LPASM2NOTASSOCIATEDTOASSOCIATED;
				break;
			}
			break;
		case CLPObj::NOTASSOCIATED:
			switch (PortID) {
			case 1:
				CEIDs[nCEID++] = CEID_LIST::LPASM1ASSOCIATEDTONOTASSOCIATED;
				break;
			case 2:
				CEIDs[nCEID++] = CEID_LIST::LPASM2ASSOCIATEDTONOTASSOCIATED;
				break;
			}
			break;
		default:
			ASSERT(0);
			return FALSE;
			break;
		}
		if (0 < nCEID) {
			CSharedMemQue EventQueue;
			EventQueue.OpenMemory("EVTMSG");

			for (int i = 0; i < nCEID; i++) {
				if (CEIDs[i] == 0) {
					ASSERT(0);
					continue;
				}
				CEvt* pEvt = new CEvt;
				ASSERT(pEvt);
				ASSERT(PortID);
				pEvt->PortNo = PortID;
				pEvt->SetMsg((BYTE*)CaObj.ObjID, strlen(CaObj.ObjID));
				pEvt->Set(EVT_CMS, CEIDs[i]);
				pGDev->SetActivePort(PortID);
				EventQueue.WriteQue(pEvt);
				pGDev->ClearActivePort();
			}
			EventQueue.CloseMemory();
		}
		return TRUE;
	}

	switch (Association) {
	case  CLPObj::ASSOCIATED:
	{
		switch (LPObj.Association) {
		case CLPObj::NOTASSOCIATED:
			switch (PortID) {
			case 1:
				CEIDs[nCEID++] = CEID_LIST::LPASM1NOTASSOCIATEDTOASSOCIATED;
				break;
			case 2:
				CEIDs[nCEID++] = CEID_LIST::LPASM2NOTASSOCIATEDTOASSOCIATED;
				break;
			}
			break;
		case CLPObj::ASSOCIATED:
			switch (PortID) {
			case 1:
				CEIDs[nCEID++] = CEID_LIST::LPASM1ASSOCIATEDTOASSOCIATED;
				break;
			case 2:
				CEIDs[nCEID++] = CEID_LIST::LPASM2ASSOCIATEDTOASSOCIATED;
				break;
			}
			break;
		case CLPObj::asNOSTATE:
			switch (PortID) {
			case 1:
				CEIDs[nCEID++] = CEID_LIST::LPASM1NOSTATETONOTASSOCIATED;
				CEIDs[nCEID++] = CEID_LIST::LPASM1NOTASSOCIATEDTOASSOCIATED;
				break;
			case 2:
				CEIDs[nCEID++] = CEID_LIST::LPASM2NOSTATETONOTASSOCIATED;
				CEIDs[nCEID++] = CEID_LIST::LPASM2NOTASSOCIATEDTOASSOCIATED;
				break;
			}
			break;
		default:
			break;
		}
		break;
	}
	case  CLPObj::NOTASSOCIATED:
	{
		switch (LPObj.Association) {
		case CLPObj::asNOSTATE:
			switch (PortID) {
			case 1:
				CEIDs[nCEID++] = CEID_LIST::LPASM1NOSTATETONOTASSOCIATED;
				break;
			case 2:
				CEIDs[nCEID++] = CEID_LIST::LPASM2NOSTATETONOTASSOCIATED;
				break;
			}
			break;
		case CLPObj::ASSOCIATED:
			switch (PortID) {
			case 1:
				CEIDs[nCEID++] = CEID_LIST::LPASM1ASSOCIATEDTONOTASSOCIATED;
				break;
			case 2:
				CEIDs[nCEID++] = CEID_LIST::LPASM2ASSOCIATEDTONOTASSOCIATED;
				break;
			}
			break;
		default:
			break;
		}
		break;
	}
	default:
		ASSERT(0);
		break;
	}
	if (!nCEID) {
		return FALSE;
	}
	if (bChange) {
		// Note:  Yuen[2002/08/03 20:07]
		LPObj.Association = Association;
		if (0 < nCEID) {
			CSharedMemQue EventQueue;
			EventQueue.OpenMemory("EVTMSG");

			for (int i = 0; i < nCEID; i++) {
				if (CEIDs[i] == 0) {
					ASSERT(0);
					continue;
				}
				CEvt* pEvt = new CEvt;
				ASSERT(pEvt);
				ASSERT(PortID);
				pEvt->PortNo = PortID;
				pEvt->SetMsg((BYTE*)CaObj.ObjID, strlen(CaObj.ObjID));
				pEvt->Set(EVT_CMS, CEIDs[i]);
				pGDev->SetActivePort(PortID);
				EventQueue.WriteQue(pEvt);
				pGDev->ClearActivePort();
			}
			EventQueue.CloseMemory();
		}
	}
	else {
		// Note: Should not update C2C Yuen[2002/08/03 20:25]
		ASSERT(0);
		return FALSE;

		CSMOL SM;
		SM.cmd = SM.cmdLPSTATUS;
		SM.Lp.PortNo = PortID;
		SM.Lp.act = CLPStatus::WRITE;
		SM.Lp.Association = (CLPObj::ASSOCIATION)Association;
		SM.Lp.Occupancy = CLPObj::igOCCUPENCY;
		SM.Lp.Reservation = CLPObj::igRESERVATION;
		SM.Lp.TransferStatus = CLPObj::igTRANSFERSTATUS;
		SM.Lp.AccessMode = CLPObj::igACCESSMODE;
		if (pGDev && pGDev->pGEMObjs->pSecsSvr) {
			pGDev->pGEMObjs->pSecsSvr->LoadPortStatusExec(SM);
		}
	}
	return TRUE;
}

int CCms::SetReservation(CGEMEvt& Ge, CLPObj::RESERVATION Reservation, BOOL bChange) {
	int CEIDs[10];
	int nCEID = 0;

	if (!CHsmsParaMeter::IsOnline()) return FALSE;

	if (!Ge.bTransition) {
		switch (Reservation) {
		case CLPObj::NOTRESERVED:
			switch (PortID) {
			case 1:
				CEIDs[nCEID++] = CEID_LIST::LPRSTM1RESERVEDTONOTRESERVED;
				break;
			case 2:
				CEIDs[nCEID++] = CEID_LIST::LPRSTM2RESERVEDTONOTRESERVED;
				break;
			}
			break;
		case CLPObj::RESERVED:
			switch (PortID) {
			case 1:
				CEIDs[nCEID++] = CEID_LIST::LPRSTM1NOTRESERVEDTORESERVED;
				break;
			case 2:
				CEIDs[nCEID++] = CEID_LIST::LPRSTM2NOTRESERVEDTORESERVED;
				break;
			}
			break;
		default:
			ASSERT(0);
			return FALSE;
			break;
		}

		if (0 < nCEID) {
			CSharedMemQue EventQueue;
			EventQueue.OpenMemory("EVTMSG");

			for (int i = 0; i < nCEID; i++) {
				if (CEIDs[i] == 0) {
					ASSERT(0);
					continue;
				}
				CEvt* pEvt = new CEvt;
				ASSERT(pEvt);
				ASSERT(PortID);
				pEvt->PortNo = PortID;
				pEvt->SetMsg((BYTE*)CaObj.ObjID, strlen(CaObj.ObjID));
				pEvt->Set(EVT_CMS, CEIDs[i]);
				pGDev->SetActivePort(PortID);
				EventQueue.WriteQue(pEvt);
				pGDev->ClearActivePort();
			}
			EventQueue.CloseMemory();
		}
		return TRUE;
	}

	switch (Reservation) {
	case CLPObj::NOTRESERVED:
		if (LPObj.Reservation == CLPObj::rsNOSTATE) {
			switch (PortID) {
			case 1:
				CEIDs[nCEID++] = CEID_LIST::LPRSTM1NOSTATETONOTRESERVED;
				break;
			case 2:
				CEIDs[nCEID++] = CEID_LIST::LPRSTM2NOSTATETONOTRESERVED;
				break;
			}
		}
		else if (LPObj.Reservation == CLPObj::RESERVED) {
			switch (PortID) {
			case 1:
				CEIDs[nCEID++] = CEID_LIST::LPRSTM1RESERVEDTONOTRESERVED;
				break;
			case 2:
				CEIDs[nCEID++] = CEID_LIST::LPRSTM2RESERVEDTONOTRESERVED;
				break;
			}
		}
		break;
	case CLPObj::RESERVED:
		if (LPObj.Reservation == CLPObj::rsNOSTATE) {
			switch (PortID) {
			case 1:
				CEIDs[nCEID++] = CEID_LIST::LPRSTM1NOSTATETONOTRESERVED;
				CEIDs[nCEID++] = CEID_LIST::LPRSTM1NOTRESERVEDTORESERVED;
				break;
			case 2:
				CEIDs[nCEID++] = CEID_LIST::LPRSTM2NOSTATETONOTRESERVED;
				CEIDs[nCEID++] = CEID_LIST::LPRSTM2NOTRESERVEDTORESERVED;
				break;
			}
		}
		else if (LPObj.Reservation == CLPObj::NOTRESERVED) {
			switch (PortID) {
			case 1:
				CEIDs[nCEID++] = CEID_LIST::LPRSTM1NOTRESERVEDTORESERVED;
				break;
			case 2:
				CEIDs[nCEID++] = CEID_LIST::LPRSTM2NOTRESERVEDTORESERVED;
				break;
			}
		}
		break;
	default:
		break;
	}
	if (!nCEID) {
		return FALSE;
	}

	if (bChange) {
		// Note:  Yuen[2002/08/03 20:07]
		LPObj.Reservation = Reservation;
		if (0 < nCEID) {
			CSharedMemQue EventQueue;
			EventQueue.OpenMemory("EVTMSG");

			for (int i = 0; i < nCEID; i++) {
				if (CEIDs[i] == 0) {
					ASSERT(0);
					continue;
				}
				CEvt* pEvt = new CEvt;
				ASSERT(pEvt);
				ASSERT(PortID);
				pEvt->PortNo = PortID;
				pEvt->SetMsg((BYTE*)CaObj.ObjID, strlen(CaObj.ObjID));
				pEvt->Set(EVT_CMS, CEIDs[i]);
				pGDev->SetActivePort(PortID);
				EventQueue.WriteQue(pEvt);
				pGDev->ClearActivePort();
			}
			EventQueue.CloseMemory();
		}
	}
	else {
		// Note: Should not update C2C Yuen[2002/08/03 20:26]
		ASSERT(0);
		return FALSE;

		CSMOL SM;
		SM.cmd = SM.cmdLPSTATUS;
		SM.Lp.PortNo = PortID;
		SM.Lp.act = CLPStatus::WRITE;
		SM.Lp.Association = CLPObj::igASSOCIATION;
		SM.Lp.Occupancy = CLPObj::igOCCUPENCY;
		SM.Lp.Reservation = (CLPObj::RESERVATION)Reservation;
		SM.Lp.TransferStatus = CLPObj::igTRANSFERSTATUS;
		SM.Lp.AccessMode = CLPObj::igACCESSMODE;
		if (pGDev && pGDev->pGEMObjs->pSecsSvr) {
			pGDev->pGEMObjs->pSecsSvr->LoadPortStatusExec(SM);
		}
	}
	return TRUE;
}

int CCms::SetOccupancy(CGEMEvt& Ge, CLPObj::OCCUPANCY Occupancy, BOOL bChange) {
	int CEIDs[10];
	int nCEID = 0;

	if (!CHsmsParaMeter::IsOnline()) return FALSE;

	if (!Ge.bTransition) {
		switch (Occupancy) {
		case CLPObj::CARRIERNOTPRESENT:
			switch (PortID) {
			case 1:
				CEIDs[nCEID++] = CEID_LIST::LPASM1NOTOCCUPIEDTOOCCUPIED;
				break;
			case 2:
				CEIDs[nCEID++] = CEID_LIST::LPASM2NOTOCCUPIEDTOOCCUPIED;
				break;
			}
			break;
		case CLPObj::CARRIERPRESENT:
			switch (PortID) {
			case 1:
				CEIDs[nCEID++] = CEID_LIST::LPASM1OCCUPIEDTONOTOCCUPIED;
				break;
			case 2:
				CEIDs[nCEID++] = CEID_LIST::LPASM2OCCUPIEDTONOTOCCUPIED;
				break;
			}
			break;
		default:
			ASSERT(0);
			return FALSE;
			break;
		}

		if (0 < nCEID) {
			CSharedMemQue EventQueue;
			EventQueue.OpenMemory("EVTMSG");

			for (int i = 0; i < nCEID; i++) {
				if (CEIDs[i] == 0) {
					ASSERT(0);
					continue;
				}
				CEvt* pEvt = new CEvt;
				ASSERT(pEvt);
				ASSERT(PortID);
				pEvt->PortNo = PortID;
				pEvt->Set(EVT_CMS, CEIDs[i]);
				pEvt->SetMsg((BYTE*)CaObj.ObjID, strlen(CaObj.ObjID));
				pGDev->SetActivePort(PortID);
				EventQueue.WriteQue(pEvt);
				pGDev->ClearActivePort();
			}
			EventQueue.CloseMemory();
		}
		return TRUE;
	}
	else {
		switch (Occupancy) {
		case CLPObj::CARRIERNOTPRESENT:
			if (LPObj.Occupancy == CLPObj::ocNOSTATE) {
				switch (PortID) {
				case 1:
					CEIDs[nCEID++] = CEID_LIST::LPASM1NOSTATETONOTOCCUPIED;
					break;
				case 2:
					CEIDs[nCEID++] = CEID_LIST::LPASM2NOSTATETONOTOCCUPIED;
					break;
				}
			}
			else if (LPObj.Occupancy == CLPObj::CARRIERPRESENT) {
				switch (PortID) {
				case 1:
					CEIDs[nCEID++] = CEID_LIST::LPASM1OCCUPIEDTONOTOCCUPIED;
					break;
				case 2:
					CEIDs[nCEID++] = CEID_LIST::LPASM2OCCUPIEDTONOTOCCUPIED;
					break;
				}
			}
			break;
		case CLPObj::CARRIERPRESENT:
			if (LPObj.Occupancy == CLPObj::ocNOSTATE) {
				switch (PortID) {
				case 1:
					CEIDs[nCEID++] = CEID_LIST::LPASM1NOSTATETONOTOCCUPIED;
					CEIDs[nCEID++] = CEID_LIST::LPASM1NOTOCCUPIEDTOOCCUPIED;
					break;
				case 2:
					CEIDs[nCEID++] = CEID_LIST::LPASM2NOSTATETONOTOCCUPIED;
					CEIDs[nCEID++] = CEID_LIST::LPASM2NOTOCCUPIEDTOOCCUPIED;
					break;
				}
			}
			else if (LPObj.Occupancy == CLPObj::CARRIERNOTPRESENT) {
				switch (PortID) {
				case 1:
					CEIDs[nCEID++] = CEID_LIST::LPASM1NOTOCCUPIEDTOOCCUPIED;
					break;
				case 2:
					CEIDs[nCEID++] = CEID_LIST::LPASM2NOTOCCUPIEDTOOCCUPIED;
					break;
				}
			}
			break;
		default:
			ASSERT(0);
			break;
		}
	}
	if (!nCEID) {
		return FALSE;
	}

	if (bChange) {
		LPObj.Occupancy = Occupancy;
		if (0 < nCEID) {
			CSharedMemQue EventQueue;
			EventQueue.OpenMemory("EVTMSG");

			for (int i = 0; i < nCEID; i++) {
				if (CEIDs[i] == 0) {
					ASSERT(0);
					continue;
				}
				CEvt* pEvt = new CEvt;
				ASSERT(pEvt);
				ASSERT(PortID);
				pEvt->PortNo = PortID;
				pEvt->Set(EVT_CMS, CEIDs[i]);
				pEvt->SetMsg((BYTE*)CaObj.ObjID, strlen(CaObj.ObjID));
				pGDev->SetActivePort(PortID);
				EventQueue.WriteQue(pEvt);
				pGDev->ClearActivePort();
			}
			EventQueue.CloseMemory();
		}
	}
	else {
		// Note: Should not update C2C Yuen[2002/08/03 20:27]
		ASSERT(0);
		return FALSE;

		CSMOL SM;
		SM.cmd = SM.cmdLPSTATUS;
		SM.Lp.PortNo = PortID;
		SM.Lp.act = CLPStatus::WRITE;
		SM.Lp.Association = CLPObj::igASSOCIATION;
		SM.Lp.Occupancy = (CLPObj::OCCUPANCY)Occupancy;
		SM.Lp.Reservation = CLPObj::igRESERVATION;
		SM.Lp.TransferStatus = CLPObj::igTRANSFERSTATUS;
		SM.Lp.AccessMode = CLPObj::igACCESSMODE;
		if (pGDev && pGDev->pGEMObjs->pSecsSvr) {
			pGDev->pGEMObjs->pSecsSvr->LoadPortStatusExec(SM);
		}
	}
	return TRUE;
}

int CCms::CarryOutCarrierAction(const char* szCarrierID, const char* szAction,
	vector<string>& AttrIDs, vector < SecsItem*>& AttrValues, SecsItemList* pErrorList, int PortNo) {
	static CSMOL SM;
	int nResult = 0;
	CString strAction(szAction);

	if (!strAction.CompareNoCase("Bind")) {
		ClearCarrierObj(); // Vijay [8\1\2]
		strscpy(CaObj.ObjID, OBJIDLEN, szCarrierID);
		for (int nIndex = 0; nIndex < int(AttrIDs.size()); nIndex++) {
			SetAttr(AttrIDs[nIndex].c_str(), AttrValues[nIndex]);
		}
		SM.CoS.Co = CaObj;
		SM.CoS.portID = PortNo;
		SM.CoS.act = CarrierObjStatus::BIND;

		m_hWait = CreateEvent(NULL, TRUE, FALSE, NULL);
	}
	else if (!strAction.CompareNoCase("CancelBind"))	// CancelBind
	{
		SM.CoS.Co = CaObj;
		SM.CoS.act = CarrierObjStatus::CANCELBIND;
		SM.CoS.portID = PortNo;					//  Yuen [7/25/2002]
		m_hWait = CreateEvent(NULL, TRUE, FALSE, NULL);
	}
	else if (!strAction.CompareNoCase("CancelCarrier"))	// CancelCarrier
	{
		SM.CoS.Co = CaObj;
		SM.CoS.act = CarrierObjStatus::CANCELCARRIER;
		SM.CoS.portID = PortNo;					//  Yuen [7/25/2002]
	}
	else if (!strAction.CompareNoCase("ProceedWithCarrier")) {
		SM.CoS.portID = PortNo;

		if (CaObj.IDStatus == CCarrierObj::IDWAITINGFORHOST) {
			strscpy(CaObj.ObjID, OBJIDLEN, szCarrierID);
			for (int nIndex = 0; nIndex < int(AttrIDs.size()); nIndex++) {
				SetAttr(AttrIDs[nIndex].c_str(), AttrValues[nIndex]);
			}
			SM.CoS.act = CarrierObjStatus::PROCEEDWITHCARRIERID;
			SM.CoS.Co = CaObj;
		}
		else if (CaObj.SlotmapStatus == CCarrierObj::SMWAITINGFORHOST) {
			// Content map must contains at least Slotmap number of entries [12/4/2002]
			for (int nIndex = 0; nIndex < int(AttrIDs.size()); nIndex++) {
				SetAttr(AttrIDs[nIndex].c_str(), AttrValues[nIndex]);
			}
			SM.CoS.act = CarrierObjStatus::PROCEEDWITHCARRIERSM;
			SM.CoS.portID = PortNo;					//  Yuen [7/25/2002]
			SM.CoS.Co = CaObj;
		}
		else {
			return 0;
		}
	}
	else if (!strAction.CompareNoCase("CancelCarrierAtPort"))// CancelCarrierAtPort
	{
		// This is the same except refer to port
		SM.CoS.Co = CaObj;
		SM.CoS.act = CarrierObjStatus::CANCELCARRIERATPORT;
		SM.CoS.portID = PortNo;					//  Yuen [7/25/2002]
	}
	else if (!strAction.CompareNoCase("CancelCarrierOut"))	// CancelCarrierOut
	{
		SM.CoS.Co = CaObj;
		SM.CoS.portID = PortNo;					//  Yuen [7/25/2002]
		SM.CoS.act = CarrierObjStatus::CANCELCARRIEROUT;
	}
	else if (!strAction.CompareNoCase("CarrierIn"))	// CarrierIn
	{
		SM.CoS.Co = CaObj;
		SM.CoS.portID = PortNo;					//  Yuen [7/25/2002]
		SM.CoS.act = CarrierObjStatus::CARRIERIN;
	}
	else if (!strAction.CompareNoCase("CarrierOut"))	// Carrier Out
	{
		SM.CoS.Co = CaObj;
		SM.CoS.portID = PortNo;					//  Yuen [7/25/2002]
		SM.CoS.act = CarrierObjStatus::CARRIEROUT;
	}
	else {
		return 1;
	}

	//	//20120920 SCOTT ADD -------------------------------------------------------------------------------
	//	// Make TRANSFERBLOCK TO READY TO UNLOAD Event
	// 	if (!strAction.CompareNoCase("CancelCarrier") || !strAction.CompareNoCase("CancelCarrierAtPort"))
	// 	{
	// 		pGDev->pGEMObjs->SetLPTransferStatus(PortNo,CLPObj::READYTOUNLOAD);
	// 	}
	//	//--------------------------------------------------------------------------------------------------

	SM.Ack = 0;
	SM.Nak = 0;
	SM.HWin = NULL;
	SM.RetSvr[0] = NULL;

	SM.cmd = CSMOL::cmdCARRIEROBJ;
	if (pGDev && pGDev->pGEMObjs->pSecsSvr) {
		if (!pGDev->pGEMObjs->pSecsSvr->CarrierObjExec(SM)) {
			return 4;
		}
	}
	return 0;
}

BOOL CCms::IsReadyToLoad() {
	return LPObj.GetTransferStatus() == CLPObj::READYTOLOAD;
}

BOOL CCms::STSMChanged(CGEMEvt& Ge) {
	CSubstObj::SUBSTSTATE OldState = CaObj.Substrate[Ge.Slot - 1].SubstState;
	UINT CEID = 0;

	if (!CHsmsParaMeter::IsOnline()) return FALSE;

	switch (OldState) {
	case CSubstObj::NO_TR_STATE:
	{
		switch (Ge.SuObj.SubstState) {
		case  CSubstObj::ATSOURCE:
			CEID = CEID_LIST::SSMNOSTATETOATSOURCE;
			break;
		case CSubstObj::NO_TR_STATE:	// Accept a no state to no state transition
			break;
		default:
			break;
		}
	}
	break;
	case CSubstObj::ATSOURCE:
	{
		switch (Ge.SuObj.SubstState) {
		case  CSubstObj::ATWORK:
			CEID = CEID_LIST::SSMATSOURCETOATWORK;
			break;
		case  CSubstObj::NO_TR_STATE:
			CEID = CEID_LIST::SSMANYSUBSTRATETOEXTINCTION;
			break;
		default:
			break;
		}
	}
	break;
	case CSubstObj::ATWORK:
	{
		switch (Ge.SuObj.SubstState) {
		case  CSubstObj::ATSOURCE:
			CEID = CEID_LIST::SSMATWORKTOATSOURCE;
			break;
		case  CSubstObj::ATWORK:
			CEID = CEID_LIST::SSMATWORKTOATWORK;
			break;
		case  CSubstObj::ATDESTINATION:
			CEID = CEID_LIST::SSMATWORKTOATDESTINATION;
			break;
		case  CSubstObj::NO_TR_STATE:
			CEID = CEID_LIST::SSMANYSUBSTRATETOEXTINCTION;
			break;
		default:
			break;
		}
	}
	break;
	case CSubstObj::ATDESTINATION:
	{
		switch (Ge.SuObj.SubstState) {
		case  CSubstObj::ATSOURCE:
			CEID = CEID_LIST::SSMATDESTINATIONTOATSOURCE;
			break;
		case  CSubstObj::ATWORK:
			CEID = CEID_LIST::SSMATDESTINATIONTOATWORK;
			break;
		case  CSubstObj::NO_TR_STATE:
			CEID = CEID_LIST::SSMANYSUBSTRATETOEXTINCTION;
			break;
		default:
			break;
		}
	}
	break;
	default:
		break;
	}

	if (CEID) {
		CaObj.Substrate[Ge.Slot - 1].SubstState = Ge.SuObj.SubstState;

		CSharedMemQue	EvtQue;
		CEvt* pEvt = new CEvt;
		ASSERT(pEvt);
		pEvt->Set(EVT_EXTRA, CEID);
		pEvt->alid = AddRefObj(CaObj.Substrate[Ge.Slot - 1]);
		pEvt->slotno = Ge.Slot;
		memcpy(pEvt->SubstID, Ge.SubstID, OBJIDLEN);
		memcpy(pEvt->LocID, Ge.LocID, sizeof(pEvt->LocID));
		EvtQue.OpenMemory("EVTMSG");
		pGDev->pDataBnk->SetActivePortSubstrate(PortID, Ge.Slot);
		EvtQue.WriteQue(pEvt);
		pGDev->pDataBnk->ClearActivePortSubstrate();
		EvtQue.CloseMemory();
	}
	return TRUE;
}

BOOL CCms::STPMChanged(CGEMEvt& Ge) {
	CSubstObj::SUBSTPROCSTATE OldState = CaObj.Substrate[Ge.Slot - 1].SubstProcState;
	UINT CEID = 0;

	if (!CHsmsParaMeter::IsOnline()) return FALSE;

	switch (OldState) {
	case  CSubstObj::NO_PROCSTATE:
	{
		switch (Ge.SuObj.SubstProcState) {
		case CSubstObj::NEEDS_PROCESSING:
			CEID = CEID_LIST::SSMNOSTATETONEEDSPROCESSING;
			break;
		default:
			break;
		}
	}
	break;
	case  CSubstObj::NEEDS_PROCESSING:
	{
		switch (Ge.SuObj.SubstProcState) {
		case CSubstObj::IN_PROCESS:
			CEID = CEID_LIST::SSMNEEDSPROCESSINGTOINPROGRESS;
			break;
		default:
			break;
		}
	}
	break;
	case  CSubstObj::IN_PROCESS:
	{
		switch (Ge.SuObj.SubstProcState) {
		case  CSubstObj::PROCESSED:
		case  CSubstObj::ABORTED:
		case  CSubstObj::STOPPED:
		case  CSubstObj::REJECTED:
			CEID = CEID_LIST::SSMINPROCESSTOPROCESSINGCOMPLETE;
			break;
		default:
			break;
		}
	}
	break;
	case  CSubstObj::PROCESSED:
	case  CSubstObj::ABORTED:
	case  CSubstObj::STOPPED:
	case  CSubstObj::REJECTED:
	{
		switch (Ge.SuObj.SubstProcState) {
			// Change the state only no event in this case
		case CSubstObj::NO_PROCSTATE:
			CEID = CEID_LIST::SSMANYSUBSTRATETOEXTINCTION;
			break;
		default:
			break;
		}
	}
	break;
	default:
		ASSERT(0);
		break;
	}

	if (CEID) {
		CaObj.Substrate[Ge.Slot - 1].SubstProcState = Ge.SuObj.SubstProcState;
		if (CEID_LIST::SSMNEEDSPROCESSINGTOINPROGRESS == CEID) {
			char Loc[SUBSTLOCIDLEN + 1] = { 0 };
			sprintf(Loc, "%d:%d", PortID, Ge.Slot);
			strscpy(CaObj.Substrate[Ge.Slot - 1].SubstSource, SUBSTLOCLEN, Loc);
			strscpy(CaObj.Substrate[Ge.Slot - 1].SubstDestination, SUBSTLOCLEN, Loc);
		}
		if (CEID == CEID_LIST::SSMNEEDSPROCESSINGTOINPROGRESS || CEID == CEID_LIST::SSMINPROCESSTONEEDSPROCESSING) {
			char Loc[SUBSTLOCIDLEN + 1] = { 0 };
			sprintf(Loc, "%d:%d", PortID, Ge.Slot);
			Ge.PortID = PortID;
			//Ge.SubstLocState = CSubstLocObj::OCCUPIED;
			memcpy(Ge.LocID, Loc, sizeof(Ge.LocID));
			SubstLocChange(Ge);
			//SubstLocChange(SlotNo,CSubstLocObj::OCCUPIED,Loc);
		}
		if (CEID == CEID_LIST::SSMINPROCESSTOPROCESSINGCOMPLETE) {
			char Loc[SUBSTLOCIDLEN + 1] = { 0 };
			sprintf(Loc, "%d:%d", PortID, Ge.Slot);
			Ge.PortID = PortID;
			//Ge.SubstLocState = CSubstLocObj::UNOCCUPIED;
			memcpy(Ge.LocID, Loc, sizeof(Ge.LocID));
			SubstLocChange(Ge);
			//SubstLocChange(SlotNo,CSubstLocObj::OCCUPIED,Loc);
		}
		CSharedMemQue	EvtQue;
		CEvt* pEvt = new CEvt;
		ASSERT(pEvt);
		pEvt->Set(EVT_EXTRA, CEID);
		pEvt->alid = AddRefObj(CaObj.Substrate[Ge.Slot - 1]);
		pEvt->slotno = Ge.Slot;
		Ge.PortID = PortID;
		memcpy(pEvt->LocID, Ge.LocID, sizeof(pEvt->LocID));
		memcpy(pEvt->SubstID, Ge.SubstID, OBJIDLEN);
		EvtQue.OpenMemory("EVTMSG");
		pGDev->SetActivePortSubstrate(PortID, Ge.Slot);
		EvtQue.WriteQue(pEvt);
		pGDev->ClearActivePortSubstrate();

		EvtQue.CloseMemory();
	}
	return TRUE;
}

BOOL CCms::UpdateSlotmapStatus(CGEMEvt& Ge) {
	if ((Ge.Slot < 1) && (Ge.Slot >= MAXCAPACITYLEN)) {
		return FALSE;
	}

	if (!CHsmsParaMeter::IsOnline()) return FALSE;

	CaObj.SlotMaP[Ge.Slot - 1] = Ge.CaObj.SlotMaP[Ge.Slot - 1];

	UINT CEID = 0;
	switch (Ge.SuObj.SubstState) {
	case CSubstObj::SM_EMPTY:
		CEID = CEID_LIST::SMSNOTOCCUPIED;
		break;
	case CSubstObj::SM_CORRECTLY_OCCUPIED:
		CEID = CEID_LIST::SMSCORRECTLYOCCUPIED;
		break;
	case CSubstObj::SM_CROSS_SLOTTED:
		CEID = CEID_LIST::SMSCROSSSLOTTED;
		break;
	case CSubstObj::SM_DOUBLE_SLOTTED:
		CEID = CEID_LIST::SMSDOUBLESLOTTED;
		break;
	case CSubstObj::SM_NOT_EMPTY:
		CEID = CEID_LIST::SMSNOTEMPTY;
		break;
	default:
		ASSERT(0);
		break;
	}
	if (CEID) {
		CSharedMemQue	EvtQue;
		CEvt* pEvt = new CEvt;
		ASSERT(pEvt);
		pEvt->Set(EVT_CMS, CEID);
		pEvt->alid = AddRefObj(CaObj.Substrate[Ge.Slot - 1]);
		pEvt->slotno = Ge.Slot;
		Ge.PortID = PortID;
		memcpy(pEvt->LocID, Ge.LocID, sizeof(pEvt->LocID));
		memcpy(pEvt->SubstID, Ge.SubstID, OBJIDLEN);
		EvtQue.OpenMemory("EVTMSG");
		pGDev->SetActivePortSubstrate(PortID, Ge.Slot);
		EvtQue.WriteQue(pEvt);
		pGDev->ClearActivePortSubstrate();
	}
	return TRUE;
}

BOOL CCms::SubstLocChange(CGEMEvt& Ge) {
	if ((Ge.Slot < 1) || (Ge.Slot > 25)) {
		ASSERT(0);
		return FALSE;
	}

	if (!CHsmsParaMeter::IsOnline()) return FALSE;

	CSubstObj* pSub1 = &CaObj.Substrate[Ge.Slot - 1];
	CSubstLocObj* pSub2 = NULL;
	CString LocID = Ge.LocID;
	int Loc = 1;	// 1=Carrier Location, 0=Scanner Location [7/5/2012 user]
	if (LocID == "SCANNER") {
		Loc = 0;
		pSub2 = &pGDev->pGEMObjs->StMd.Stage1;
	}
	else if (LocID == "ALIGNER") {
		Loc = 0;
		pSub2 = &pGDev->pGEMObjs->StMd.Algn;
	}

	UINT CEID = 0;
	switch (Ge.SubstLocState) {
	case CSubstLocObj::OCCUPIED:
	{
		if (pSub2) {
			pSub2->SubstLocState = Ge.SubstLocState;
		}
		memcpy(pSub1->SubstLocID, Ge.LocID, sizeof(pSub1->SubstLocID));
		memcpy(Ge.SubstID, pSub1->ObjID, OBJIDLEN);
		for (int nIndex = 0; nIndex < SUBSTHISTITEMLEN; nIndex++) {
			if (0 == strlen(pSub1->History[nIndex].SubstLocID)) {
				memcpy(pSub1->History[nIndex].SubstLocID, Ge.LocID, sizeof(pSub1->History[nIndex].SubstLocID));
				pSub1->History[nIndex].TimeIn = CTime::GetCurrentTime().GetTime();
				break;
			}
		}
	}
	switch (Loc) {
	case 0:
		CEID = CEID_LIST::SLSCANNEROCCUPIED;
		break;
	default:
		CEID = CEID_LIST::SMSCORRECTLYOCCUPIED;
		break;
	}
	break;
	case CSubstLocObj::UNOCCUPIED:
		if (pSub2) {
			pSub2->SubstLocState = Ge.SubstLocState;
		}
		pSub1->SubstLocID[0] = 0;
		strscpy(Ge.SubstID, OBJIDLEN, pSub1->ObjID);
		for (int nIndex = 0; nIndex < SUBSTHISTITEMLEN; nIndex++) {
			if (strcmp(pSub1->History[nIndex].SubstLocID, Ge.LocID) == 0) {
				pSub1->History[nIndex].TimeOut = CTime::GetCurrentTime().GetTime();
				break;
			}
		}
		switch (Loc) {
		case 0:
			CEID = CEID_LIST::SLSCANNERNOTOCCUPIED;
			break;
		default:
			CEID = CEID_LIST::SMSNOTOCCUPIED;
			break;
		}
		break;
	}
	if (CEID) {
		CSharedMemQue	EvtQue;
		CEvt* pEvt = new CEvt;
		ASSERT(pEvt);
		pEvt->Set(EVT_EXTRA, CEID);
		pEvt->alid = AddRefObj(CaObj.Substrate[Ge.Slot - 1]);
		pEvt->PortNo = PortID;
		memcpy(pEvt->LocID, Ge.LocID, LOTIDLEN);
		memcpy(pEvt->SubstID, Ge.SubstID, OBJIDLEN);
		EvtQue.OpenMemory("EVTMSG");
		pGDev->SetActivePortSubstrate(PortID, Ge.Slot);
		EvtQue.WriteQue(pEvt);
		pGDev->ClearActivePortSubstrate();
		EvtQue.CloseMemory();
	}
	return TRUE;
}

BOOL CCms::GetSubstAttr(const string& ObjId, string& Attrib, SecsItemList& AttrList) {
	CSubstObj Subst;
	BOOL bRes = FALSE;
	string Attr;
	if (FindSubst(ObjId, Subst)) {
		if (Subst.GetAttr((char*)Attrib.c_str(), Attr)) {
			bRes = TRUE;
		}
	}
	AttrList.AddItem(new SecsItemASCII(Attr));
	return bRes;
}

BOOL CCms::FindSubst(const string& SubstID, CSubstObj& Subst) {
	if (CaObj.SlotmapStatus == CCarrierObj::SMVERIFICATIONOK) {
		for (int nIndex = 0; nIndex < MAXCAPACITYLEN; nIndex++) {
			if (0 == strcmp(CaObj.Substrate[nIndex].ObjID, SubstID.c_str())) {
				Subst = CaObj.Substrate[nIndex];
				return TRUE;
			}
		}
	}
	return FALSE;
}

void CCms::ClearCarrierObj() {
	CaObj.Preset();
}

BOOL CCms::GetRefObj(long MagicRef, BOOL bRemove, CSubstObj& Ref) {
	map<long, CSubstObj>::iterator iter = TempSubstMap.find(MagicRef);
	if (iter != TempSubstMap.end()) {
		Ref = iter->second;
		if (bRemove) {
			TempSubstMap.erase(MagicRef);
		}
		return TRUE;
	}
	return FALSE;
}

long CCms::AddRefObj(const CSubstObj& Ref) {
	long Temp = ::InterlockedIncrement(&CCms::MagicRef);
	TempSubstMap.insert(SubRefMap::value_type(Temp, Ref));
	return Temp;
}

char CCms::SubstStateEnum(CSubstObj::SUBSTSTATE SubsSt) {
	switch (SubsSt) {
	case CSubstObj::ATSOURCE:
		return 0;
		break;
	case CSubstObj::ATWORK:
		return 1;
		break;
	case CSubstObj::ATDESTINATION:
		return 2;
		break;
	}
	return -1;
}
char CCms::SubstProcStateEnum(CSubstObj::SUBSTPROCSTATE ProcSt) {
	switch (ProcSt) {
	case CSubstObj::NEEDS_PROCESSING:
		return 0;
		break;
	case CSubstObj::IN_PROCESS:
		return 1;
		break;
	case CSubstObj::PROCESSED:
		return 2;
		break;
	case CSubstObj::ABORTED:
		return 3;
		break;
	case CSubstObj::STOPPED:
		return 4;
		break;
	case CSubstObj::REJECTED:
		return 5;
		break;
	}
	return -1;
}

void CCms::SetLPObjID(short port) {
	PortID = port;
	CString str;
	str.Format("LP%d", PortID);
	strscpy(LPObj.ObjID, OBJIDLEN, str.GetBuffer(0));
}

void CCms::GetCarrierAttrIds(vector<string>& AttrIds) {
	CStringArray nTemp;
	CCms::GetAttrName(nTemp);
	for (int nIndex = 0; nIndex<int(nTemp.GetSize()); nIndex++) {
		AttrIds.push_back(nTemp[nIndex].GetBuffer(0));
	}
}

void CCms::GetSubstAttrIds(vector<string>& AttrIds) {
	AttrIds.push_back("ObjID");
	AttrIds.push_back("ObjType");
	AttrIds.push_back("LotID");
	AttrIds.push_back("SubstLocID");
	AttrIds.push_back("SubstSource");
	AttrIds.push_back("SubstDestination");
	AttrIds.push_back("SubstType");
	AttrIds.push_back("SubstState");
	AttrIds.push_back("SubstUsage");
	AttrIds.push_back("SubstProcState");
}

void CCms::GetSubstLocAttrIds(vector<string>& AttrIds) {
	AttrIds.push_back("ObjID");
	AttrIds.push_back("ObjType");
	AttrIds.push_back("SubstID");
	AttrIds.push_back("SubstLocState");
}

BOOL CCms::IsDocked() {
	return (LPObj.CarrierLoc == CLPObj::clDOCKED);
}

BOOL CCms::IsIdVerificationOk() {
	return (CaObj.IDStatus == CCarrierObj::IDVERIFICATIONOK);
}

BOOL CCms::IsSMVerificationOk() {
	return (CaObj.SlotmapStatus == CCarrierObj::SMVERIFICATIONOK);
}

BOOL CCms::IsSMMatched(CString& SlotMap) {
	BOOL bRes = FALSE;
	if (SlotMap.GetLength() == MAXCAPACITYLEN) {
		for (UINT nIndex = 0; nIndex < MAXCAPACITYLEN; nIndex++) {
			if (SlotMap.GetAt(nIndex) != CSubstObj::GetSMChar(CaObj.SlotMaP[nIndex])) {
				bRes = FALSE;
			}
		}
	}
	return bRes;
}

BOOL CCms::ProcessCancelCarrier() {
	// Id Verification & Slotmap Verification are mutually exclusive so...
	if (CaObj.IDStatus == CCarrierObj::IDWAITINGFORHOST) {
		SetCarrierIDStaus(CCarrierObj::IDVERIFICATIONFAIL);
	}
	else if (CaObj.SlotmapStatus == CCarrierObj::SMWAITINGFORHOST) {
		SetSlotMapStatus(CCarrierObj::SMVERIFICATIONFAIL);
	}
	return TRUE;
}

void CCms::Init(short port) {
	PortID = port;
	LPObj.Preset();
	strscpy(LPObj.ObjType, OBJTYPELEN, "Load Port");
	LPObj.SetAccessMode(CLPObj::MANUAL);
	LPObj.SetAccessMode(LPObj.GetAccessMode(PortID), PortID);
	LPObj.Reservation = CLPObj::NOTRESERVED;
	LPObj.Association = CLPObj::NOTASSOCIATED;
	LPObj.SetTransferStatus(CLPObj::tsNOSTATE);
	//LPObj.SetTransferStatus(LPObj.GetTransferStatus(PortID));
	LPObj.Occupancy = CLPObj::ocNOSTATE;

	CaObj.Preset();

	CString str;
	CString sect;
	CString winini;
	CString AcsMode;
	DosUtil.GetLocalCfgFile(winini);

	sect.Format("LP%d", PortID);
	strscpy(LPObj.ObjID, OBJIDLEN, sect.GetBuffer(0));

	BCR.SetServiceStatus(CIDRObj::INSERVICE);
	BCR.SetServiceStatus(BCR.GetServiceStatus(PortID), PortID);
	BCR.SetBypassStatus(CIDRObj::INUSE);
	BCR.SetBypassStatus(BCR.GetBypassStatus(PortID), PortID);

	sect.Format("IDR%d", PortID);
	strscpy(BCR.ObjID, OBJIDLEN, sect.GetBuffer(0));
}

void CCms::LoadComplete(short PortID) {
	if (!CHsmsParaMeter::IsOnline()) return;

	CSharedMemQue EventQueue;
	EventQueue.OpenMemory("EVTMSG");
	CEvt* pEvt = new CEvt;
	pEvt->PortNo = PortID;
	switch (PortID) {
	case 1:
		pEvt->Set(EVT_CMS, CEID_LIST::LP1LOADCOMPLETE);
		break;
	case 2:
		pEvt->Set(EVT_CMS, CEID_LIST::LP2LOADCOMPLETE);
		break;
	}
	pGDev->SetActivePort(PortID);
	EventQueue.WriteQue(pEvt);
	pGDev->ClearActivePort();
	EventQueue.CloseMemory();
}