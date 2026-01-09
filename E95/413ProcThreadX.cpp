// 413ProcThread.cpp: implementation of the C413ProcThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "e95.h"
#include "413ProcThread.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

C413ProcThread::C413ProcThread()
{
	bStop = TRUE;
	pThread = NULL;
	pTCPSvr = NULL;
}

C413ProcThread::~C413ProcThread()
{
	if (pThread)
	{
		ASSERT(0);
	}
}

UINT C413ProcThread::MeasureProc(LPVOID pObject)
{
	short nResult = 2;
	C413TCPServer *pDsptr = (C413TCPServer *)pObject;

// 	CRecipe *rp = AppSystem.GetRecipe();
// 	if (!rp)
// 	{
// 		return 1;
// 	}
// 
// 	pDlg->PostMessage(MSG_FSM_MEASURE_BEGIN);
// 
// 	Global.bStop=0;
// 	
// 	int nMeasureCount = 1;
// 	
// 	pDlg->SetRecipeParameters();  // to be replace [3/11/2010 Valued Customer]
// 
// 	do
// 	{
// 		CString strTitle;
// 		strTitle.Format(_T("Measurement Progress(%u of %i)"), nMeasureCount, pDlg->m_nMeasureCount);
// 		pDlg->SetWindowText(strTitle);
// 		
// 		// ??? [3/11/2010 Valued Customer]
// 		pDlg->SendMessage(MSG_FSM_CLEAR_DATA, 0);	
// 		pDlg->PostMessage(MSG_FSM_CALIBRATE_PROBE, pDlg->m_MPList.GetCount());
// 		
// 		BOOL bIsEcho = rp->IsEcho();
// 		BOOL bIsRough = rp->IsRoughness();
// 		BOOL bIsThinFim = rp->IsThinFilm();
// 
// 		if (bIsEcho || bIsThinFim)  // Temporary include ThinFilm here [6/3/2010 Valued Customer]
// 		{
// 			BOOL bBk2 = pEF2Dlg->bVisible;
// 			AppViews.pS8Dlg->TurnIlluminatorOnOff(TRUE);
// 			lpfnCalculatingEndProc pFunc = NULL;
// 			pFunc = pEcho->m_lpfnCalculatingEndProc;
// 			pEcho->SetCalculatingEndProc(OnCalculatingEnd, pDlg);
// 			BOOL bBk = pPgDsp->bShowPeaks;
// 			pPgDsp->bShowPeaks = TRUE;
// 			pEF2Dlg->bVisible = TRUE;
// 			if (!m_hCalculatingEvent)
// 			{
// 				m_hCalculatingEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
// 			}
// 			if (!m_hMeasureEndEvent)
// 			{
// 				m_hMeasureEndEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
// 			}
// 			
// 			if (rp->IsTotalThickness())
// 			{
// 				SetEvent(m_hMeasureEndEvent);
// 				SetEvent(m_hCalculatingEvent);
// 				pDlg->DoDualProbeCalib();
// 			}
// 			pPgDsp->bShowPeaks = bBk;
// 			pEF2Dlg->bVisible = FALSE;
// 			// Actual measurement [3/11/2010 Valued Customer]
// 			nResult = 0;
// 			
// 			SetEvent(m_hMeasureEndEvent);
// 			SetEvent(m_hCalculatingEvent);
// 			if (!pDlg->MeasureMPListWithRangeChecking(rp, pDlg->m_MPList)) 
// 			{
// 				nResult = 5;
// 			}
// 
// 			if(m_hMeasureEndEvent)
// 			{
// 				CloseHandle(m_hMeasureEndEvent);
// 				m_hMeasureEndEvent = NULL;
// 			}
// 			if(m_hCalculatingEvent)
// 			{
// 				CloseHandle(m_hCalculatingEvent);
// 				m_hCalculatingEvent = NULL;
// 			}
// 			
// 			if (pFunc)
// 			{
// 				pEcho->m_lpfnCalculatingEndProc = pFunc;
// 			}
// 			AppViews.pS8Dlg->TurnIlluminatorOnOff(FALSE);
// 			pDlg->PostMeasurementComputation(rp);  // Calculate total thickness etc [3/11/2010 Valued Customer]
// 			pEF2Dlg->bVisible = bBk2;
// 		}
// 		
// 		if (nResult != 0 ) 
// 		{
// 			break;
// 		}
// 		if (nMeasureCount == 0) 
// 		{
// 			break;
// 		}
// 		if (nMeasureCount == pDlg->m_nMeasureCount)
// 		{
// 			if (pDlg->m_pUserView)
// 			{
// 				pDlg->m_pUserView->SetSystemStatus(CUserView::STATE_MEAS_END);
// 				pDlg->m_pUserView->SetSignalBar();
// 			}
// 		}
// 	} while (Global.bStop == 0 && ++nMeasureCount <= pDlg->m_nMeasureCount);
// 	
// 	if (Global.bStopMotor)
// 	{
// 		AppViews.pS8Dlg->StopMotor();
// 	}
// 
// 	if(Global.bStop == 1 || nResult)
// 	{
// 		pDlg->PostMessage(WM_CLOSE, 0, 0);	
// 		ExitThread(0);
// 	}
// 
// 	if (Global.bMoveStageToCenter)
// 	{
// 		AppViews.pS8Dlg->GotoPos(0, 0);
// 	}
// 	
// 	if(IsWindow(pDlg->m_hWnd))
// 	{
// 		pDlg->PostMessage(MSG_FSM_MEASURE_END);
// 	}
// 	
	return nResult;
}

void C413ProcThread::Stop()
{
	if (pThread)
	{
		bStop = TRUE;
		WaitForSingleObject(pThread->m_hThread,5000);
		delete pThread;
		pThread = NULL;
	}
}

void C413ProcThread::Start()
{
	if (!pTCPSvr)
	{
		// pTCPSvr need to be assigned [7/4/2010 Yuen]
		ASSERT(0);
	}
	pThread = AfxBeginThread(C413ProcThread::MeasureProc,(LPVOID)pTCPSvr);
}
