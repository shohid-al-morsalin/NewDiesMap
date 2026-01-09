#include "stdafx.h"
#include "resource.h"

#include "128/GEM/ObjectsContainer.h"
#include "128/GEM/OLnClt.h"
#include "128/GEM/PrJobList.h"
#include "GlobalDeviceContainer.h"
#include "IPC/SMHdr.h"
#include "IPC/Smol.h"
#include "ONLINE/GEM/CediListCtroller.h"
#include "ONLINE/GEM/CtrlJob.h"
#include "ONLINE/GEM/CtrlJobMgr.h"

#include "SecsSvrF.h"

// Used by E95 only [11/14/2007 YUEN]

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//IMPLEMENT_DYNCREATE(CSecsSvr, CSvrThread)

CSecsSvr::CSecsSvr(void) {
	for (int i = 0; i < 4; i++) {
		pActWin[i] = NULL;
	}
}

CSecsSvr::~CSecsSvr(void) {
	PrJobs.Clear();
}

BOOL CSecsSvr::ProcessCmd(CSMOL& SM) {
	switch (SM.cmd) {
	case CSMOL::cmdSTDACTION:
		StdActionsExec(SM);
		break;
	case CSMOL::cmdLPACTION:
		LoadPortActionsExec(SM);
		break;
	case CSMOL::cmdLPSTATUS:
		LoadPortStatusExec(SM);
		break;
	case CSMOL::cmdLINEMODEACTION:
		return LineModeActionsExec(SM);
		break;
	case CSMOL::cmdGEMEVENTS:
		GEMEventsExec(SM);
		break;
	case CSMOL::cmdWAFERPARAM:
		// Use by non Online IPC only
		WaferParamExec(SM);
		break;
	case CSMOL::cmdCARRIERACTION:
		CarrierActionsExec(SM);
		break;
	case CSMOL::cmdCARRIERIDINFO:
		CarrierIDInfoExec(SM);
		break;
	case CSMOL::cmdCARRIEROBJ:
		CarrierObjExec(SM);
		break;
	case CSMOL::cmdPCJOBEXEC:
		// YF Online Define C2C style Processing Job
		PrJobExec(SM);
		break;
	case CSMOL::cmdPRCSMGRSERVICES:
		ASSERT(0);
		//PrcsMgrService(SM);
		break;
	case CSMOL::cmdBCRSTATUS:
		BCRStatusExec(SM);
		break;
	}

	return TRUE;
}

BOOL CSecsSvr::LineModeActionsExec(CSMOL& SM) {
	switch (SM.Lm.act) {
	case LineModeAction::GOREMOTE:
		pGDev->pOLnClt->GoRemote();
		break;
	case LineModeAction::GOLOCAL:
		pGDev->pOLnClt->GoLocal();
		break;
	case LineModeAction::GOEQPOFFLINE:
		pGDev->pOLnClt->GoOfflineEqp();
		break;
	case LineModeAction::GOHOSTOFFLINE:
		pGDev->pOLnClt->GoOfflineHost();
		break;
	}

	return TRUE;
}

BOOL CSecsSvr::LoadPortStatusExec(CSMOL& SM) {
	switch (SM.Lp.act) {
	case CLPStatus::READ:
		return LoadPortStatusRead(SM);
		break;
	case CLPStatus::WRITE:
		return LoadPortStatusWrite(SM);
		break;
	}

	return TRUE;
}

BOOL CSecsSvr::WaferParamExec(CSMOL& SM) {
	switch (SM.Wp.act) {
	case SWaferParam::wpWRITE:
		break;
	case SWaferParam::wpREAD:
		break;
	}

	return TRUE;
}

BOOL CSecsSvr::CarrierIDInfoExec(CSMOL& SM) {
	switch (SM.Ci.act) {
	case CarrierIDInfo::WRITE:
		break;
	case CarrierIDInfo::READ:
		break;
	}

	return TRUE;
}

BOOL CSecsSvr::LoadPortStatusRead(CSMOL& SM) {
	ASSERT(0);
	return TRUE;
}

BOOL CSecsSvr::LoadPortStatusWrite(CSMOL& SM) {
	pGDev->pOLnClt->SetLPStatus(SM.Lp);
	return TRUE;
}

BOOL CSecsSvr::CarrierActionsExec(CSMOL& SM) {
	int i;
	CWnd* pWnd = NULL;

	switch (SM.Ca.act) {
	case CarrierAction::SETCARRIERARRIVED:
		if (SM.Ca.PortNo >= 1 && SM.Ca.PortNo <= 4) {
		}
		break;
	case CarrierAction::CLEARCARRIERARRIVED:
		if (SM.Ca.PortNo >= 1 && SM.Ca.PortNo <= 4) {
		}
		break;

	case CarrierAction::STARTPROCESSING:
		switch (SM.Ca.PortNo) {
		case 1:
			if (pActWin[0]) {
				pWnd = pActWin[0];
			}
			break;
		case 2:
			if (pActWin[1]) {
				pWnd = pActWin[1];
			}
			break;
			// 			case 3:
			// 				if (pActWin[2])
			// 				{
			// 					pWnd = pActWin[2];
			// 				}
			// 				break;
			// 			case 4:
			// 				if (pActWin[3])
			// 				{
			// 					pWnd = pActWin[3];
			// 				}
			// 				break;
		default:
			ASSERT(0);
			break;
		}
		if (pWnd) {
			CSMOL* pSM = new CSMOL;
			if (!pSM) {
				ASSERT(0);
				return FALSE;
			}
			*pSM = SM;
			pWnd->PostMessage(ID_PRCS_PROCESSINGSTART, 0, (long)pSM);
		}
		break;

	case CarrierAction::PROCESSINGCOMPLETE:
		// Message originated from online telling equipment to close port Yuen [7/5/2002]
		for (i = 1; i <= 4; i++) {
			CCarrierObj* pCarrier = pGDev->pGEMObjs->GetCarrierAtPort(i);
			if (pCarrier) {
				if (strcmp(pCarrier->ObjID, SM.Ca.CarrierID) == 0) {
					switch (i) {
					case 1:
						if (pActWin[0]) {
							pActWin[0]->PostMessage(WM_COMMAND, ID_SVR_CLOSECOMPLETE1, NULL);
						}
						return TRUE;
						break;
					case 2:
						if (pActWin[1]) {
							pActWin[1]->PostMessage(WM_COMMAND, ID_SVR_CLOSECOMPLETE2, NULL);
						}
						return TRUE;
						break;
					case 3:
						if (pActWin[2]) {
							pActWin[2]->PostMessage(WM_COMMAND, ID_SVR_CLOSECOMPLETE3, NULL);
						}
						return TRUE;
						break;
					case 4:
						if (pActWin[3]) {
							pActWin[3]->PostMessage(WM_COMMAND, ID_SVR_CLOSECOMPLETE4, NULL);
						}
						return TRUE;
						break;
					default:
						ASSERT(0);
						return TRUE;
						break;
					}
				}
			}
		}
		break;
	case CarrierAction::ABORTPROCESSING:
		// Message originated from online telling equipment to abort Yuen [7/5/2002]
		switch (SM.Ca.PortNo) {
		case 1:
			if (pActWin[0]) {
				pActWin[0]->PostMessage(WM_COMMAND, ID_PRCS_PROCESSINGABORT, NULL);
			}
			break;
		case 2:
			if (pActWin[1]) {
				pActWin[1]->PostMessage(WM_COMMAND, ID_PRCS_PROCESSINGABORT, NULL);
			}
			break;
		case 3:
			if (pActWin[2]) {
				pActWin[2]->PostMessage(WM_COMMAND, ID_PRCS_PROCESSINGABORT, NULL);
			}
			break;
		case 4:
			if (pActWin[3]) {
				pActWin[3]->PostMessage(WM_COMMAND, ID_PRCS_PROCESSINGABORT, NULL);
			}
			break;
		default:
			ASSERT(0);
			break;
		}
		break;
	case CarrierAction::PAUSEPROCESSING:
		// Message originated from online telling equipment to pause Yuen [7/5/2002]
		switch (SM.Ca.PortNo) {
		case 1:
			if (pActWin[0]) {
				pActWin[0]->PostMessage(WM_COMMAND, ID_PRCS_PROCESSINGPAUSE, NULL);
			}
			break;
		case 2:
			if (pActWin[1]) {
				pActWin[1]->PostMessage(WM_COMMAND, ID_PRCS_PROCESSINGPAUSE, NULL);
			}
			break;
		case 3:
			if (pActWin[2]) {
				pActWin[2]->PostMessage(WM_COMMAND, ID_PRCS_PROCESSINGPAUSE, NULL);
			}
			break;
		case 4:
			if (pActWin[3]) {
				pActWin[3]->PostMessage(WM_COMMAND, ID_PRCS_PROCESSINGPAUSE, NULL);
			}
			break;
		default:
			ASSERT(0);
			break;
		}
		break;

		/*	case CarrierAction::STARTPROCESSING:
		break; */

	case CarrierAction::RESUMEPROCESSING:
		// Message originated from onlie telling equipment to resume Yuen [7/5/2002]
		switch (SM.Ca.PortNo) {
		case 1:
			if (pActWin[0]) {
				pActWin[0]->PostMessage(WM_COMMAND, ID_PRCS_PROCESSINGRESUME, NULL);
			}
			break;
		case 2:
			if (pActWin[1]) {
				pActWin[1]->PostMessage(WM_COMMAND, ID_PRCS_PROCESSINGRESUME, NULL);
			}
			break;
		case 3:
			if (pActWin[2]) {
				pActWin[2]->PostMessage(WM_COMMAND, ID_PRCS_PROCESSINGRESUME, NULL);
			}
			break;
		case 4:
			if (pActWin[3]) {
				pActWin[3]->PostMessage(WM_COMMAND, ID_PRCS_PROCESSINGRESUME, NULL);
			}
			break;
		default:
			ASSERT(0);
			break;
		}
		break;
	case CarrierAction::STOPPROCESSING:
		// Message originated from onlie telling equipment to stop Yuen [7/5/2002]
		switch (SM.Ca.PortNo) {
		case 1:
			if (pActWin[0]) {
				pActWin[0]->PostMessage(WM_COMMAND, ID_PRCS_PROCESSINGSTOP, NULL);
			}
			break;
		case 2:
			if (pActWin[1]) {
				pActWin[1]->PostMessage(WM_COMMAND, ID_PRCS_PROCESSINGSTOP, NULL);
			}
			break;
		case 3:
			if (pActWin[2]) {
				pActWin[2]->PostMessage(WM_COMMAND, ID_PRCS_PROCESSINGSTOP, NULL);
			}
			break;
		case 4:
			if (pActWin[3]) {
				pActWin[3]->PostMessage(WM_COMMAND, ID_PRCS_PROCESSINGSTOP, NULL);
			}
			break;
		default:
			ASSERT(0);
			break;
		}
		break;
	default:
		ASSERT(0);
		break;
	}

	return TRUE;
}

BOOL CSecsSvr::GEMEventsExec(CSMOL& SM) {
	return TRUE;
}

BOOL CSecsSvr::StdActionsExec(CSMOL& SM) {
	switch (SM.Sa.cmd) {
	case StdAction::HELLO:
		strcpy(SM.Sa.text, "SecsSvr:Hello");
		if (SM.exe == CSMHdr::REQ) {
		}
		break;
	case StdAction::STARTUNIT:
		break;
	default:
		ASSERT(0);
		break;
	}

	return TRUE;
}

BOOL CSecsSvr::LoadPortActionsExec(CSMOL& SM) {
	ASSERT(0);
	return FALSE;
}

BOOL CSecsSvr::CarrierObjExec(CSMOL& SM) {
	CString str;
	switch (SM.CoS.act) {
	case CarrierObjStatus::BIND:
	{
		CSMOL* pSM;
		pSM = new CSMOL;
		*pSM = SM;
		switch (SM.CoS.portID) {
		case 1:
			if (pActWin[0]) {
				pActWin[0]->PostMessage(ID_SVR_BIND, 0, (long)pSM);
			}
			break;
		case 2:
			if (pActWin[1]) {
				pActWin[1]->PostMessage(ID_SVR_BIND, 0, (long)pSM);
			}
			break;
		case 3:
			if (pActWin[2]) {
				pActWin[2]->PostMessage(ID_SVR_BIND, 0, (long)pSM);
			}
			break;
		case 4:
			if (pActWin[3]) {
				pActWin[3]->PostMessage(ID_SVR_BIND, 0, (long)pSM);
			}
			break;
		default:
			ASSERT(0);
			break;
		}
	}
	break;
	case CarrierObjStatus::CANCELBIND:
	{
		CSMOL* pSM;
		pSM = new CSMOL;
		*pSM = SM;
		switch (SM.CoS.portID) {
		case 1:
			if (pActWin[0]) {
				pActWin[0]->PostMessage(ID_SVR_CANCELBIND, 0, (long)pSM);
			}
			break;
		case 2:
			if (pActWin[1]) {
				pActWin[1]->PostMessage(ID_SVR_CANCELBIND, 0, (long)pSM);
			}
			break;
		case 3:
			if (pActWin[2]) {
				pActWin[2]->PostMessage(ID_SVR_CANCELBIND, 0, (long)pSM);
			}
			break;
		case 4:
			if (pActWin[3]) {
				pActWin[3]->PostMessage(ID_SVR_CANCELBIND, 0, (long)pSM);
			}
			break;
		default:
			ASSERT(0);
			break;
		}
	}
	break;
	case CarrierObjStatus::CANCELCARRIER:
	{
		CSMOL* pSM;
		pSM = new CSMOL;
		*pSM = SM;
		switch (SM.CoS.portID) {
		case 1:
			if (pActWin[0]) {
				pActWin[0]->PostMessage(ID_SVR_CANCELCARRIER, 0, (long)pSM);
			}
			break;
		case 2:
			if (pActWin[1]) {
				pActWin[1]->PostMessage(ID_SVR_CANCELCARRIER, 0, (long)pSM);
			}
			break;
		case 3:
			if (pActWin[2]) {
				pActWin[2]->PostMessage(ID_SVR_CANCELCARRIER, 0, (long)pSM);
			}
			break;
		case 4:
			if (pActWin[3]) {
				pActWin[3]->PostMessage(ID_SVR_CANCELCARRIER, 0, (long)pSM);
			}
			break;
		default:
			if (pSM) {
				delete pSM;
			}
			break;
		}
	}
	break;
	case CarrierObjStatus::CANCELCARRIERATPORT:
	{
		CSMOL* pSM;
		pSM = new CSMOL;
		*pSM = SM;
		switch (SM.CoS.portID) {
		case 1:
			if (pActWin[0]) {
				pActWin[0]->PostMessage(ID_SVR_CANCELCARRIERATPORT, 0, (long)pSM);
			}
			break;
		case 2:
			if (pActWin[1]) {
				pActWin[1]->PostMessage(ID_SVR_CANCELCARRIERATPORT, 0, (long)pSM);
			}
			break;
		case 3:
			if (pActWin[2]) {
				pActWin[2]->PostMessage(ID_SVR_CANCELCARRIERATPORT, 0, (long)pSM);
			}
			break;
		case 4:
			if (pActWin[3]) {
				pActWin[3]->PostMessage(ID_SVR_CANCELCARRIERATPORT, 0, (long)pSM);
			}
			break;
		default:
			if (pSM) {
				delete pSM;
			}
			break;
		}
	}
	break;
	case CarrierObjStatus::PROCEEDWITHCARRIERID:
	{
		CSMOL* pSM;
		pSM = new CSMOL;
		*pSM = SM;
		switch (SM.CoS.portID) {
		case 1:
			if (pActWin[0]) {
				pActWin[0]->PostMessage(ID_SVR_PROCEEDWITHCARRIERID, 0, (long)pSM);
			}
			break;
		case 2:
			if (pActWin[1]) {
				pActWin[1]->PostMessage(ID_SVR_PROCEEDWITHCARRIERID, 0, (long)pSM);
			}
			break;
		case 3:
			if (pActWin[2]) {
				pActWin[2]->PostMessage(ID_SVR_PROCEEDWITHCARRIERID, 0, (long)pSM);
			}
			break;
		case 4:
			if (pActWin[3]) {
				pActWin[3]->PostMessage(ID_SVR_PROCEEDWITHCARRIERID, 0, (long)pSM);
			}
			break;
		default:
			if (pSM) {
				delete pSM;
			}
			break;
		}
	}
	break;
	case CarrierObjStatus::PROCEEDWITHCARRIERSM:
	{
		CSMOL* pSM = new CSMOL;
		*pSM = SM;
		switch (SM.CoS.portID) {
		case 1:
			if (pActWin[0]) {
				pActWin[0]->PostMessage(ID_SVR_PROCEEDWITHCARRIERSM, 0, (long)pSM);
			}
			break;
		case 2:
			if (pActWin[1]) {
				pActWin[1]->PostMessage(ID_SVR_PROCEEDWITHCARRIERSM, 0, (long)pSM);
			}
			break;
			// 			case 3:
			// 				if (pActWin[2])
			// 				{
			// 					pActWin[2]->PostMessage(ID_SVR_PROCEEDWITHCARRIERSM, 0, (long)pSM);
			// 				}
			// 				break;
			// 			case 4:
			// 				if (pActWin[3])
			// 				{
			// 					pActWin[3]->PostMessage(ID_SVR_PROCEEDWITHCARRIERSM, 0, (long)pSM);
			// 				}
			// 				break;
		default:
			if (pSM) {
				delete pSM;
			}
			//ASSERT(0);
			break;
		}
	}
	break;
	case CarrierObjStatus::CARRIEROUT:
	{
		CSMOL* pSM = new CSMOL;
		*pSM = SM;
		switch (SM.CoS.portID) {
		case 1:
			if (pActWin[0]) {
				pActWin[0]->PostMessage(ID_SVR_CLOSECOMPLETE1, 0, (long)pSM);
			}
			break;
		case 2:
			if (pActWin[1]) {
				pActWin[1]->PostMessage(ID_SVR_CLOSECOMPLETE2, 0, (long)pSM);
			}
			break;
		case 3:
			if (pActWin[2]) {
				pActWin[2]->PostMessage(ID_SVR_CLOSECOMPLETE3, 0, (long)pSM);
			}
			break;
		case 4:
			if (pActWin[3]) {
				pActWin[3]->PostMessage(ID_SVR_CLOSECOMPLETE4, 0, (long)pSM);
			}
			break;
		default:
			if (pSM) {
				delete pSM;
			}
			break;
		}
	}
	break;
	default:
		ASSERT(0);
		break;
	}

	return TRUE;
}

BOOL CSecsSvr::PrJobExec(CSMOL& SM) {
	CWnd* pWnd = NULL;
	int PortNo;
	switch (SM.Pj.act) {
	case SPcJobExec::DEFINEPJ:
	{
		CString CarrierID = SM.Pj.CarrierID;
		PortNo = pGDev->pGEMObjs->GetPortOfCarrier(CarrierID);
		switch (PortNo) {
		case 1:
			pWnd = pActWin[0];
			break;
		case 2:
			pWnd = pActWin[1];
			break;
		case 3:
			pWnd = pActWin[2];
			break;
		case 4:
			pWnd = pActWin[3];
			break;
		default:
			CEIDCtlr.m_alarm.SetAlarm(CAlrmList::PRJOBEXEERROR);
			//ASSERT(0); //  [9/6/2007 LYF]
			break;
		}
		if (pWnd) {
			CSMOL* pSM1;
			pSM1 = new CSMOL;
			*pSM1 = SM;	// need to persist
			pWnd->PostMessage(ID_SVR_PJDEFINE, 0, (long)pSM1);
		}
	}
	break;
	case SPcJobExec::DEFINECJ:
	{
		CCtrlJob* pJ = CJobMgr.GetCJob(SM.Pj.PrJID);
		if (pJ) {
			for (int n = 0; n < pJ->CarrierInputSpec.GetSize(); n++) {
				CString str;
				str = pJ->CarrierInputSpec.GetAt(n);
				PortNo = pGDev->pGEMObjs->GetPortOfCarrier(str);
				switch (PortNo) {
				case 1:
					pWnd = pActWin[0];
					break;
				case 2:
					pWnd = pActWin[1];
					break;
				case 3:
					pWnd = pActWin[2];
					break;
				case 4:
					pWnd = pActWin[3];
					break;
				default:
					ASSERT(0);
					break;
				}
				if (pWnd) {
					CSMOL* pSM1;
					pSM1 = new CSMOL;
					*pSM1 = SM;	// need to persist
					pWnd->PostMessage(ID_SVR_CJDEFINE, 0, (long)pSM1);
				}
			}
		}
	}
	break;
	case SPcJobExec::ADD:
	{
		// 		SPcJobExec *p = new SPcJobExec;
		// 		*p = SM.Pj;
		// 		PrJobs.Add(p);
	}
	break;
	case SPcJobExec::REMOVE:
		PrJobs.Remove(SM.Pj);
		break;
	case SPcJobExec::PAUSE:
		break;
	case SPcJobExec::RESUME:
		break;
	case SPcJobExec::ABORT:
		break;
	case SPcJobExec::STOP:
		break;
	default:
		ASSERT(0);
		break;
	}
	return TRUE;
}

BOOL CSecsSvr::BCRStatusExec(CSMOL& SM) {
	switch (SM.Bs.act) {
	case BCRStatus::READ:
		return BCRRead(SM.Bs);
		break;
	case BCRStatus::WRITE:
		return BCRWrite(SM.Bs);
		break;
	case BCRStatus::OPERATIONSTATUSCHANGE:
		return BCROperationStatusChange(SM.Bs);
		break;
	case BCRStatus::ALARMSTATUSCHANGE:
		return BCRAlarmStatusChange(SM.Bs);
		break;
	case BCRStatus::ONOFFCHANGE:
		return BCROnOffChange(SM.Bs);
		break;
	case BCRStatus::HEADSTATUSCHANGE:
		return BCRHeadStatusChange(SM.Bs);
		break;
	case BCRStatus::HEATCONDITIONCHANGE:
		return BCRHeadConditionChange(SM.Bs);
		break;
	default:
		ASSERT(0);
		break;
	}
	return TRUE;
}

BOOL CSecsSvr::BCROperationStatusChange(BCRStatus& SM) {
	return TRUE;
}

BOOL CSecsSvr::BCRRead(BCRStatus& As) {
	return TRUE;
}

BOOL CSecsSvr::BCRWrite(BCRStatus& As) {
	return TRUE;
}

BOOL CSecsSvr::BCRAlarmStatusChange(BCRStatus& As) {
	return TRUE;
}

BOOL CSecsSvr::BCROnOffChange(BCRStatus& As) {
	return TRUE;
}

BOOL CSecsSvr::BCRHeadStatusChange(BCRStatus& As) {
	return TRUE;
}

BOOL CSecsSvr::BCRHeadConditionChange(BCRStatus& As) {
	return TRUE;
}

void CSecsSvr::CleanUp() {}