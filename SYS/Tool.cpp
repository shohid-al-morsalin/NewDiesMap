// Tool.cpp: implementation of the CTool class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Tool.h"
#include "..\SRC\DosUtil.h"
#include "..\128\Cas\NCas.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTool Tool;

CTool::CTool() {
	IDRCarrierID1 = "";
	IDRCarrierID2 = "";
	IDRCarrierID3 = "";
	IDRCarrierID4 = "";

	pCasMgr = &CasMgr1;

	LoadParam();
	SaveParam();
}

CTool::~CTool() {
	SaveParam();
}

void CTool::LoadParam() {}

void CTool::SaveParam() {}

CCasMgr* CTool::GetCurCasMgr() {
	return pCasMgr;
}

BOOL CTool::IsCassetteSelected(CString& id) {
	CNCas* pCas;
	if (CasMgr1.State != CCasMgr::NOTSELECTED) {
		pCas = CasMgr1.GetCurCas();
		if (pCas && (pCas->GetCasID() == id)) return TRUE;
	}
	if (CasMgr2.State != CCasMgr::NOTSELECTED) {
		pCas = CasMgr2.GetCurCas();
		if (pCas && (pCas->GetCasID() == id)) return TRUE;
	}
	if (CasMgr3.State != CCasMgr::NOTSELECTED) {
		pCas = CasMgr3.GetCurCas();
		if (pCas && (pCas->GetCasID() == id)) return TRUE;
	}
	if (CasMgr4.State != CCasMgr::NOTSELECTED) {
		pCas = CasMgr4.GetCurCas();
		if (pCas && (pCas->GetCasID() == id)) return TRUE;
	}
	return FALSE;
}

CCasMgr* CTool::GetCasMgr(short unit) {
	switch (unit) {
	case 0:
		pCasMgr = &CasMgr1;
		return &CasMgr1;
		break;
	case 1:
		pCasMgr = &CasMgr1;
		return &CasMgr1;
		break;
	case 2:
		pCasMgr = &CasMgr2;
		return &CasMgr2;
		break;
	case 3:
		pCasMgr = &CasMgr3;
		return &CasMgr3;
		break;
	case 4:
		pCasMgr = &CasMgr4;
		return &CasMgr4;
		break;
	default:
		//		ASSERT(0);
		return NULL;
		break;
	}
	return NULL;
}

BOOL CTool::IsAnyCasInProcessState(short unit) {
	switch (unit) {
	case 1:
		if ((CasMgr2.State == CCasMgr::PROCESSING) ||
			(CasMgr3.State == CCasMgr::PROCESSING) ||
			(CasMgr4.State == CCasMgr::PROCESSING)) {
			return TRUE;
		}
		break;
	case 2:
		if ((CasMgr1.State == CCasMgr::PROCESSING) ||
			(CasMgr3.State == CCasMgr::PROCESSING) ||
			(CasMgr4.State == CCasMgr::PROCESSING)) {
			return TRUE;
		}
		break;
	case 3:
		if ((CasMgr1.State == CCasMgr::PROCESSING) ||
			(CasMgr2.State == CCasMgr::PROCESSING) ||
			(CasMgr4.State == CCasMgr::PROCESSING)) {
			return TRUE;
		}
		break;
	case 4:
		if ((CasMgr1.State == CCasMgr::PROCESSING) ||
			(CasMgr2.State == CCasMgr::PROCESSING) ||
			(CasMgr3.State == CCasMgr::PROCESSING)) {
			return TRUE;
		}
		break;
	}
	return FALSE;
}

BOOL CTool::IsCasInSelectedState(short unit) {
	switch (unit) {
	case 1:
		if (CasMgr1.State == CCasMgr::SELECTED) {
			return TRUE;
		}
		break;
	case 2:
		if (CasMgr2.State == CCasMgr::SELECTED) {
			return TRUE;
		}
		break;
	case 3:
		if (CasMgr3.State == CCasMgr::SELECTED) {
			return TRUE;
		}
		break;
	case 4:
		if (CasMgr4.State == CCasMgr::SELECTED) {
			return TRUE;
		}
		break;
	default:
		ASSERT(0);
		return FALSE;
	}
	return FALSE;
}

BOOL CTool::SetCasStateQueued(short unit) {
	switch (unit) {
	case 1:
		CasMgr1.State = CCasMgr::QUEUED;
		break;
	case 2:
		CasMgr2.State = CCasMgr::QUEUED;
		break;
	case 3:
		CasMgr3.State = CCasMgr::QUEUED;
		break;
	case 4:
		CasMgr4.State = CCasMgr::QUEUED;
		break;
	default:
		ASSERT(0);
		return FALSE;
		break;
	}
	return TRUE;
}

BOOL CTool::SetCasStateSelected(short unit) {
	switch (unit) {
	case 1:
		if (CasMgr1.State != CCasMgr::QUEUED) {
			if (CasMgr1.State != CCasMgr::PROCESSING) {
				ASSERT(0);
				return FALSE;
			}
		}
		if (CasMgr1.State == CCasMgr::QUEUED) {
			CasMgr1.State = CCasMgr::SELECTED;
		}
		break;
	case 2:
		if (CasMgr2.State != CCasMgr::QUEUED) {
			if (CasMgr2.State != CCasMgr::PROCESSING) {
				ASSERT(0);
				return FALSE;
			}
		}
		if (CasMgr2.State == CCasMgr::QUEUED) {
			CasMgr2.State = CCasMgr::SELECTED;
		}
		break;
	case 3:
		if (CasMgr3.State != CCasMgr::QUEUED) {
			if (CasMgr3.State != CCasMgr::PROCESSING) {
				ASSERT(0);
				return FALSE;
			}
		}
		if (CasMgr3.State == CCasMgr::QUEUED) {
			CasMgr3.State = CCasMgr::SELECTED;
		}
		break;
	case 4:
		if (CasMgr4.State != CCasMgr::QUEUED) {
			if (CasMgr4.State != CCasMgr::PROCESSING) {
				ASSERT(0);
				return FALSE;
			}
		}
		if (CasMgr4.State == CCasMgr::QUEUED) {
			CasMgr4.State = CCasMgr::SELECTED;
		}
		break;
	default:
		ASSERT(0);
		return FALSE;
		break;
	}
	return TRUE;
}

BOOL CTool::SetCasStateProcessing(short unit) {
	switch (unit) {
	case 1:
		if (CasMgr1.State != CCasMgr::SELECTED) {
			if (CasMgr1.State != CCasMgr::PROCESSING) {
				ASSERT(0);
				return FALSE;
			}
		}
		CasMgr1.State = CCasMgr::PROCESSING;
		break;
	case 2:
		if (CasMgr2.State != CCasMgr::SELECTED) {
			if (CasMgr2.State != CCasMgr::PROCESSING) {
				ASSERT(0);
				return FALSE;
			}
		}
		CasMgr2.State = CCasMgr::PROCESSING;
		break;
	case 3:
		if (CasMgr3.State != CCasMgr::SELECTED) {
			if (CasMgr2.State != CCasMgr::PROCESSING) {
				ASSERT(0);
				return FALSE;
			}
		}
		CasMgr3.State = CCasMgr::PROCESSING;
		break;
	case 4:
		if (CasMgr4.State != CCasMgr::SELECTED) {
			if (CasMgr2.State != CCasMgr::PROCESSING) {
				ASSERT(0);
				return FALSE;
			}
		}
		CasMgr4.State = CCasMgr::PROCESSING;
		break;
	default:
		ASSERT(0);
		return FALSE;
		break;
	}
	return TRUE;
}

BOOL CTool::SetCasStateCompleted(short unit) {
	switch (unit) {
	case 1:
		// 		if (CasMgr1.State != CCasMgr::PROCESSING)
		// 		{
		// 			ASSERT(0);
		// 			return FALSE;
		// 		}
		CasMgr1.State = CCasMgr::COMPLETED;
		break;
	case 2:
		// 		if (CasMgr2.State != CCasMgr::PROCESSING)
		// 		{
		// 			ASSERT(0);
		// 			return FALSE;
		// 		}
		CasMgr2.State = CCasMgr::COMPLETED;
		break;
	case 3:
		// 		if (CasMgr3.State != CCasMgr::PROCESSING)
		// 		{
		// 			ASSERT(0);
		// 			return FALSE;
		// 		}
		CasMgr3.State = CCasMgr::COMPLETED;
		break;
	case 4:
		// 		if (CasMgr4.State != CCasMgr::PROCESSING)
		// 		{
		// 			ASSERT(0);
		// 			return FALSE;
		// 		}
		CasMgr4.State = CCasMgr::COMPLETED;
		break;
	default:
		ASSERT(0);
		return FALSE;
		break;
	}
	return TRUE;
}

BOOL CTool::SetCasStateAborted(short unit) {
	switch (unit) {
	case 1:
		// 		if (CasMgr1.State != CCasMgr::PROCESSING)
		// 		{
		// 			ASSERT(0);
		// 			return FALSE;
		// 		}
		CasMgr1.State = CCasMgr::ABORTED;
		break;
	case 2:
		// 		if (CasMgr2.State != CCasMgr::PROCESSING)
		// 		{
		// 			ASSERT(0);
		// 			return FALSE;
		// 		}
		CasMgr2.State = CCasMgr::ABORTED;
		break;
	case 3:
		// 		if (CasMgr3.State != CCasMgr::PROCESSING)
		// 		{
		// 			ASSERT(0);
		// 			return FALSE;
		// 		}
		CasMgr3.State = CCasMgr::ABORTED;
		break;
	case 4:
		// 		if (CasMgr4.State != CCasMgr::PROCESSING)
		// 		{
		// 			ASSERT(0);
		// 			return FALSE;
		// 		}
		CasMgr4.State = CCasMgr::ABORTED;
		break;
	default:
		ASSERT(0);
		return FALSE;
		break;
	}
	return TRUE;
}

CScanDataMgr* CTool::GetCurScanDataMgr() {
	if (!pCasMgr) {
		return NULL;
	}
	CNCas* cas = pCasMgr->GetCurCas();
	if (cas) {
		ASSERT(0);
	}
	return NULL;
}

CNCas* CTool::GetCurCas() {
	if (!pCasMgr) {
		return NULL;
	}
	return pCasMgr->GetCurCas();
}

CWaferInfoMgr* CTool::GetCurWaferInfoMgr() {
	CNCas* cas = GetCurCas();
	if (cas) {
		return cas->GetWaferInfoMgr();
	}
	return NULL;
}

CCasMgr::STATE CTool::GetCasState(short unit) {
	switch (unit) {
	case 1:
		return CasMgr1.State;
		break;
	case 2:
		return CasMgr2.State;
		break;
	case 3:
		return CasMgr3.State;
		break;
	case 4:
		return CasMgr4.State;
		break;
	default:
		ASSERT(0);
		return CCasMgr::NOTSELECTED;
		break;
	}
	return CCasMgr::NOTSELECTED;
}