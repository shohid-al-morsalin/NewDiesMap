#include "stdafx.h"

#include "ComBase.h"
#include "SRC/Comm.h"

#include "ComSMIF.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;

#define new DEBUG_NEW
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
// Not tread save [5/8/2002]

BOOL CComSMIF::SetDefault() {
	SendHCA("HCS AUTO", 6000);
	SendHCA("EDER ON", 6000);
	return TRUE;
}

void CComSMIF::Decode(CString& msg) {
	//	if (!hWnd)
	//	{
	//		ASSERT(0);
	//		return;
	//	}
	//
	//	if (msg.Find("HCA") != -1)
	//	{
	//		if (msg.Find("ALARM") != -1)
	//		{
	//			::PostMessage(hWnd, WM_COMMAND, 40008, NULL);
	//			return;
	//		}
	//		if (msg.Find("RMIDA NO") != -1)
	//		{
	//			::PostMessage(hWnd, WM_COMMAND, 40009, NULL);
	//			return;
	//		}
	//	}
	//	if (msg.Find("AERS") != -1)
	//	{
	//		if (msg.Find("E84_L_REQ_SUBMITTED") != -1)
	//		{
	//			::PostMessage(hWnd, WM_COMMAND, 32867, NULL);
	//			return;
	//		}
	//		if (msg.Find("E84_L_REQ_COMPLETED") != -1)
	//		{
	//			::PostMessage(hWnd, WM_COMMAND, 32853, NULL);
	//			return;
	//		}
	//		if (msg.Find("E84_LOAD_COMPLETED") != -1)
	//		{
	//			::PostMessage(hWnd, WM_COMMAND, 32854, NULL);
	//			return;
	//		}
	//		if (msg.Find("E84_U_REQ_COMPLETED") != -1)
	//		{
	//			::PostMessage(hWnd, WM_COMMAND, 32855, NULL);
	//			return;
	//		}
	//		if (msg.Find("E84_UNLOAD_COMPLETED") != -1)
	//		{
	//			::PostMessage(hWnd, WM_COMMAND, 32856, NULL);
	//			return;
	//		}
	//		if (msg.Find("E84_LOST_E84_SIGNAL") != -1)
	//		{
	//			::PostMessage(hWnd, WM_COMMAND, 32857, NULL);
	//			return;
	//		}
	//		if (msg.Find("E84_UNEXPECTED_E84_SIGNAL") != -1)
	//		{
	//			::PostMessage(hWnd, WM_COMMAND, 32858, NULL);
	//			return;
	//		}
	//		if (msg.Find("E84_TP1_TIMEOUT") != -1)
	//		{
	//			::PostMessage(hWnd, WM_COMMAND, 32859, NULL);
	//			return;
	//		}
	//		if (msg.Find("E84_TP2_TIMEOUT") != -1)
	//		{
	//			::PostMessage(hWnd, WM_COMMAND, 32860, NULL);
	//			return;
	//		}
	//		if (msg.Find("E84_TP3_TIMEOUT") != -1)
	//		{
	//			::PostMessage(hWnd, WM_COMMAND, 32863, NULL);
	//			return;
	//		}
	//		if (msg.Find("E84_TP4_TIMEOUT") != -1)
	//		{
	//			::PostMessage(hWnd, WM_COMMAND, 32864, NULL);
	//			return;
	//		}
	//		if (msg.Find("E84_TP5_TIMEOUT") != -1)
	//		{
	//			::PostMessage(hWnd, WM_COMMAND, 32865, NULL);
	//			return;
	//		}
	//		if (msg.Find("E84_TP6_TIMEOUT") != -1)
	//		{
	//			::PostMessage(hWnd, WM_COMMAND, 32866, NULL);
	//			return;
	//		}
	//		if (msg.Find("E84_NON_AMHS_CARRIER_ARRIVED") != -1)
	//		{
	//			::PostMessage(hWnd, WM_COMMAND, 40010, NULL);
	//			return;
	//		}
	//		if (msg.Find("E84_NON_AMHS_CARRIER_REMOVED") != -1)
	//		{
	//			::PostMessage(hWnd, WM_COMMAND, 40011, NULL);
	//			return;
	//		}
	//		if (msg.Find("CARRIER_ARRIVED") != -1)
	//		{
	//			::PostMessage(hWnd, WM_COMMAND, 40012, NULL);
	//			return;
	//		}
	//		if (msg.Find("CARRIER_REMOVED") != -1)
	//		{
	//			::PostMessage(hWnd, WM_COMMAND, 40013, NULL);
	//			return;
	//		}
	//		if (msg.Find("END_OPEN") != -1)
	//		{
	//			ReadMap();
	//			::PostMessage(hWnd, WM_COMMAND, 40005, NULL);
	//			return;
	//		}
	//		if (msg.Find("END_CLOSE") != -1)
	//		{
	//			ReadMap();
	//			::PostMessage(hWnd, WM_COMMAND, 40004, NULL);
	//			return;
	//		}
	//		if (msg.Find("ABORT_CLOSE") != -1)
	//		{
	//			::PostMessage(hWnd, WM_COMMAND, 40002, NULL);
	//			return;
	//		}
	//		if (msg.Find("ABORT_OPEN") != -1)
	//		{
	//			::PostMessage(hWnd, WM_COMMAND, 40003, NULL);
	//			return;
	//		}
	//		if (msg.Find("CMPL_APHD_LATCH") != -1)
	//		{
	//			::PostMessage(hWnd, WM_COMMAND, 32803, NULL);
	//			return;
	//		}
	//		if (msg.Find("CMPL_APHD_RELEASE") != -1)
	//		{
	//			::PostMessage(hWnd, WM_COMMAND, 32804, NULL);
	//			return;
	//		}
	//		if (msg.Find("CMPL_CLOSE_NO_RELEASE") != -1)
	//		{
	//			ReadMap();
	//			::PostMessage(hWnd, WM_COMMAND, 32805, NULL);
	//			return;
	//		}
	//		if (msg.Find("BUTTON_2_PUSHED") != -1)
	//		{
	//			::PostMessage(hWnd, WM_COMMAND, 32806, NULL);
	//			return;
	//		}
	//	}
	//	if (msg.Find("ARS") != -1)
	//	{
	//		if (msg.Find("FATAL") != -1)
	//		{
	//			if (msg.Find("Excessive wafer out") != -1)
	//			{
	//				::PostMessage(hWnd, WM_COMMAND, 40007, NULL);
	//				Reset();
	//				return;
	//			}
	//			if (msg.Find("Perimeter sensor is tripped") != -1)
	//			{
	//				::PostMessage(hWnd, WM_COMMAND, 40006, NULL);
	//				Reset();
	//				return;
	//			}
	//			return;
	//		}
	//	}
}

CComSMIF::CComSMIF() {}

CComSMIF::~CComSMIF() {}

BOOL CComSMIF::Connect(short port) {
	if (port == 0) {
		if (pCom) {
			pCom->Close();
			delete pCom;
		}
		pCom = NULL;
		return TRUE;
	}

	CSingleLock Lock(&CS);
	Lock.Lock();
	if (!pCom) {
		pCom = new CComm;
		if (!pCom) {
			ASSERT(0);
			return FALSE;
		}
	}
	pCom->SetParam(char(port), 9600, 8, 0, 1, 0);
	if (pCom->Open()) {
		SetDefault();
		return TRUE;
	}
	delete pCom;
	pCom = NULL;
	return FALSE;
}

BOOL CComSMIF::Disconnect() {
	if (pCom) {
		pCom->Close();
		delete pCom;
		pCom = NULL;
		return TRUE;
	}
	return TRUE;
}