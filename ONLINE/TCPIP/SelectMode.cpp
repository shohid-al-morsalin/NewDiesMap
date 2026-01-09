// SelectMode.cpp: implementation of the CSelectMode class.
//
//////////////////////////////////////////////////////////////////////

#include "afxmt.h"
#include "stdafx.h"
#include "SelectMode.h"
#include "HsmsParameter.h"
// #include "resource.h"
#include "..\INC\define.h"
#include "..\ShareMemory\DataBank.h"

#include "OnlineMgr.h"
#include "GlobalDeviceContainer.h"

// #define  STYPE_POS  5

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSelectMode::CSelectMode() {
	// 	m_STypeRec = SELECTREQ ;
	// 	m_STypeSnd = SELECTRSP ;
	memset(m_RecvData, 0, sizeof m_RecvData);
	// 	m_STypeArcnt = 5;
}

CSelectMode::~CSelectMode() {}

BOOL CSelectMode::IsSelectReq(BYTE* nData) {
	// 	BOOL bRet = FALSE;
	if (nData[STYPE_POS] == SELECTREQ) {
		memcpy(m_RecvData, nData, 10);
		return TRUE;
	}
	return /*bRet*/FALSE;
}

void CSelectMode::SelectRep(BYTE* nData) {
	m_RecvData[STYPE_POS] = SELECTRSP;
	memcpy(nData, m_RecvData, 10);
}

BOOL CSelectMode::Pass_SelectCedure(DWORD T7) {
	BOOL bRet = FALSE;
	ULONGLONG dwStartTime, dwCurTime;
	BYTE dwData[200];
	int  dwLen = 0;
	CString dd("");

	//	Sleep(500);  // Patch work, CCS isn't that fast to accept selreq acknowledge [8/1/2010 XPMUser]
	hmHSMSSS = CHsmsParaMeter::NOTSELECTED;

	memset(dwData, 0, sizeof dwData);
	dwStartTime = dwCurTime = GetTickCount64();
	while (dwCurTime - dwStartTime < T7) {
		//dwCurTime = ::GetCurrentTime();
		short MsgLen = CHsmsParaMeter::Pass_RecvString(dwData, DATALENSIZE, NULL);
		if (MsgLen == 0) {
			continue;
		}
		if (MsgLen != 4) {
			TRACE("Character timeout\n");
			continue;;
		}
		if (MsgLen) {
			dwLen = CHsmsParaMeter::Pass_GetDataLen(dwData);
			if (dwLen) {
				if (CHsmsParaMeter::Pass_RecvString(&dwData[4], dwLen, NULL)) {
					pGDev->pOLnMgr->SECSContent(&dwData[4], dwLen, FALSE, NULL);
					if (IsSelectReq(&dwData[4])) {
						hmHSMSSS = CHsmsParaMeter::SELECTED;
						memset(dwData, 0, sizeof(dwData));
						CHsmsParaMeter::Pass_SetDataLen(dwData, 10);
						SelectRep(&dwData[4]);
						pGDev->pOLnMgr->SECSContent(&dwData[4], 10, TRUE, NULL);
						CHsmsParaMeter::Pass_SndString(dwData, 14);
						if (pGDev->pDataBnk->bAutoConnect) {
							if (pWnd) {
								pWnd->PostMessage(WM_COMMAND, ID_MSG_ESTCOMM, NULL);
							}
							else {
								AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_MSG_ESTCOMM);
							}
						}
						return TRUE;
					}
					return FALSE;
				}
				else {
				}
			}
		}
		dwCurTime = ::GetCurrentTime();
	}
	return bRet;
}

BOOL CSelectMode::IsSeperateReq(BYTE* nData) {
	BOOL bRet = FALSE;
	if (nData[STYPE_POS] == SEPERATEREQ) {
		memcpy(m_RecvData, nData, 10);
		return TRUE;
	}
	return bRet;
}