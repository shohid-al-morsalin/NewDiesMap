#include "stdafx.h"

#include "GlobalDeviceContainer.h"
#include "IncmplPrcsHdlr.h"
#include "InfoPanelEqpMessage.h"
#include "InfoPanelMain.h"
#include "IPC/SLPInit.h"
#include "NavigationPanel.h"
#include "ProcJob413ProcessingDlg.h"
#include "ProcJobCancelCarrierDlg.h"
#include "ProcJobCompleteDlg.h"
#include "ProcJobCompleteManual.h"
#include "ProcJobDlg.h"
#include "ProcJobManDlg.h"
#include "ProcJobManual.h"
#include "ProcJobPWC1DlgBase.h"
#include "ProcJobPWC2Dlg.h"
#include "ProcJobQue413Dlg.h"
#include "ProcJobSMICDlg.h"
#include "ProcJobUnloadCarrierDlg.h"
#include "RecoveryDlg.h"
#include "SYS/CUMMsg.h"
#include "TitlePanel.h"

#include "413App.h"

#include "GlobalUIContainer.h"

CGlobalUIContainer* pUDev = NULL;

CGlobalUIContainer::CGlobalUIContainer() {
	pTitlePanel = NULL;
	pNavigationPanel = NULL;
	pInfoPanelMain = NULL;
	pInfoPanelSetup = NULL;

	pEqpMessage = NULL;
	pHostMessage = NULL;

	Initialize();

	LoadINI();
	SaveINI();
}

CGlobalUIContainer::~CGlobalUIContainer() {}

void CGlobalUIContainer::LoadINI() {}

void CGlobalUIContainer::SaveINI() {}

void CGlobalUIContainer::SetCurrentView(CString ViewName) {
	if (pTitlePanel) {
		pTitlePanel->SetCurrentView(ViewName);
	}
}

BOOL CGlobalUIContainer::PerformAction(int action, short unit, CString& CarrierID) {
	BOOL bNoClose = FALSE;

	short type = 3;
	switch (pGDev->SystemModel) {
	case CGlobalDeviceContainer::FSM413C2C:
	case CGlobalDeviceContainer::FSM413MOT:
	case CGlobalDeviceContainer::FSM413SA:
		type = 3;
		break;
	}
	switch ((CIncmplPrcsHdlr::ACTION)action) {
	case CIncmplPrcsHdlr::STARTRECOVERY:
		if (pNavigationPanel && pInfoPanelMain) {
			pInfoPanelMain->PostMessage(UM_TO_PROCJOBRECOVERY, unit - 1, type);
			pNavigationPanel->PostMessage(WM_COMMAND, ID_E95_BLINKJOBBUTTON, NULL);
		}
		break;
	case CIncmplPrcsHdlr::FORGETANDCLOSEFOUP:
		DosUtil.ForgetPrJob(unit);
		break;
	case CIncmplPrcsHdlr::DONOTCLOSEFOUP:
		bNoClose = TRUE;
		break;
	default:
		ASSERT(0);
		break;
	}
	return bNoClose;
}

// Return the LP number that has incomplete job [3/16/2012 Administrator]
short CGlobalUIContainer::IncompleteProcessDetection(short PortNo) {
	// 	if (p413App->Global.WaferOnStage > 0)
	// 	{
	// 		return p413App->Global.WaferOnStage;
	// 	}

	if (PortNo == 0) {
		short Yes = 0;
		for (int i = 0; i < MAX_LPSUPPORTED; i++) {
			if (CheckIncompleteJob(i)) {
				Yes += (i + 1);
			}
		}
		return Yes;
	}
	else {
		if (CheckIncompleteJob(PortNo - 1)) {
			return PortNo;
		}
	}
	return 0;

	// 	for (int i=0; i<MAX_LPSUPPORTED; i++)
	// 	{
	// 		if (Var.nLoadPort > i)
	// 		{
	// 			LpInit.unit = i+1;
	// 			DosUtil.GetRememberedPrJob(LpInit.unit,LpInit.CarrierID, LpInit.RecipeName);
	// 			if (LpInit.CarrierID.GetLength() > 0)
	// 			{
	// 				// confirm the load port is not in processing state [10/4/2012 Yuen]
	// 				if (!pUDev->pProcJob413ProcessingDlg[i])
	// 				{
	// // 					CIncmplPrcsHdlr dlg;
	// // 					dlg.unit = LpInit.unit;
	// // 					dlg.CarrierID = LpInit.CarrierID;
	// // 					dlg.DoModal();
	// // 					LpInit.action = dlg.Action;
	// // 					// retrieve action here
	// // 					LpInit.bNoClose[i] = PerformAction(LpInit.action,LpInit.unit,LpInit.CarrierID);
	// 					return LpInit.unit;
	// 				}
	// 			}
	// 		}
	// 	}
	// 	return 0;
}

void CGlobalUIContainer::StartUIElements() {
	//	IncompleteProcessDetection();
}

void CGlobalUIContainer::StopUIElements() {}

void CGlobalUIContainer::EqpMessage(CString msg, short lvl) {
	if (msg.GetLength() > 0) {
		if (pEqpMessage) {
			pEqpMessage->Message(msg, lvl);
		}
	}
}

BOOL CGlobalUIContainer::Initialize() {
	for (int i = 0; i < MAX_LPSUPPORTED; i++) {
		pProcJob413ProcessingDlg[i] = NULL;
		pProcJobGENDlg[i] = NULL;
		pProcJobSMICDlg[i] = NULL;
		pProcJobPWCDlg[i] = NULL;
		pProcJobPWC2Dlg[i] = NULL;
		pProcJobQueued413Dlg[i] = NULL;
		pProcJobCompleteDlg[i] = NULL;
		pProcJobCancelCarrierDlg[i] = NULL;
		pProcJobUnloadCarrierDlg[i] = NULL;
		pProcJobRecoveryDlg[i] = NULL;
	}

	pRecoveryDlg = NULL;
	pProcJobManual = NULL;
	pProcJobManDlg = NULL;
	pProcJobManageDlg = NULL;
	pRecipeCreateDlg413 = NULL;
	pProcJobCompleteManual = NULL;

	return TRUE;
}

void CGlobalUIContainer::ClearAll(short i) {
	if (pProcJobGENDlg[i]) {
		pProcJobGENDlg[i]->DestroyWindow();
		pProcJobGENDlg[i] = NULL;
	}
	if (pProcJobSMICDlg[i]) {
		pProcJobSMICDlg[i]->DestroyWindow();
		pProcJobSMICDlg[i] = NULL;
	}
	if (pProcJobPWCDlg[i]) {
		pProcJobPWCDlg[i]->DestroyWindow();
		pProcJobPWCDlg[i] = NULL;
	}
	if (pProcJobPWC2Dlg[i]) {
		pProcJobPWC2Dlg[i]->DestroyWindow();
		pProcJobPWC2Dlg[i] = NULL;
	}
	if (pProcJobQueued413Dlg[i]) {
		pProcJobQueued413Dlg[i]->DestroyWindow();
		pProcJobQueued413Dlg[i] = NULL;
	}
	if (pProcJobCompleteDlg[i]) {
		pProcJobCompleteDlg[i]->DestroyWindow();
		pProcJobCompleteDlg[i] = NULL;
	}
	if (pProcJobCancelCarrierDlg[i]) {
		pProcJobCancelCarrierDlg[i]->DestroyWindow();
		pProcJobCancelCarrierDlg[i] = NULL;
	}
	if (pProcJobUnloadCarrierDlg[i]) {
		pProcJobUnloadCarrierDlg[i]->DestroyWindow();
		pProcJobUnloadCarrierDlg[i] = NULL;
	}
	if (pProcJob413ProcessingDlg[i]) {
		pProcJob413ProcessingDlg[i]->DestroyWindow();
		pProcJob413ProcessingDlg[i] = NULL;
	}
	if (pProcJobRecoveryDlg[i]) {
		pProcJobRecoveryDlg[i]->DestroyWindow();
		pProcJobRecoveryDlg[i] = NULL;
	}
	if (pProcJobManual) {
		pProcJobManual->DestroyWindow();
		pProcJobManual = NULL;
	}
	if (pProcJobCompleteManual) {
		pProcJobCompleteManual->DestroyWindow();
		pProcJobCompleteManual = NULL;
	}
	if (pProcJobManDlg) {
		pProcJobManDlg->DestroyWindow();
		pProcJobManDlg = NULL;
	}
}

BOOL CGlobalUIContainer::UnclampCarrier(short PortNo) {
	if (pProcJobCompleteDlg[PortNo - 1]) {
		pProcJobCompleteDlg[PortNo - 1]->Unclamp();
		return TRUE;
	}
	if (pProcJobCancelCarrierDlg[PortNo - 1]) {
		pProcJobCancelCarrierDlg[PortNo - 1]->Unclamp();
		return TRUE;
	}
	return FALSE;
}

BOOL CGlobalUIContainer::ClampCarrier(short PortNo) {
	if (pProcJobGENDlg[PortNo - 1]) {
		pProcJobGENDlg[PortNo - 1]->LatchFoup();
		return TRUE;
	}
	if (pProcJobSMICDlg[PortNo - 1]) {
		pProcJobSMICDlg[PortNo - 1]->LatchFoup();
		return TRUE;
	}
	return FALSE;
}

BOOL CGlobalUIContainer::CheckIncompleteJob(short i) {
	SLPInit LpInit;
	if (Var.nLoadPort > i) {
		LpInit.unit = i + 1;
		DosUtil.GetRememberedPrJob(LpInit.unit, LpInit.CarrierID, LpInit.RecipeName);
		if (LpInit.CarrierID.GetLength() > 0) {
			// confirm the load port is not in processing state [10/4/2012 Yuen]
			if (pUDev->pProcJob413ProcessingDlg[i] == NULL) {
				return LpInit.unit;
			}
		}
	}
	return FALSE;
}

BOOL CGlobalUIContainer::CanStart(short ID) {
	int i;
	for (i = 0; i < MAX_LPSUPPORTED; i++) {
		if (i != ID) {  // Do not start if other load port is in processing mode
			if (pUDev->pProcJob413ProcessingDlg[i]) {
				if (pUDev->pProcJob413ProcessingDlg[i]->bProcessing) {
					return FALSE;
				}
			}
		}
	}
	return TRUE;
}

int CGlobalUIContainer::UserLevel()
{
	return pTitlePanel->AuthenticateLevel;
}
