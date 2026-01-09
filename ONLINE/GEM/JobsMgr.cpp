

// JobsMgr.cpp: implementation of the CJobsMgr class.
#include "stdafx.h"




#include "JobsMgr.h"
//

void CJobsMgr::CJAborted(char * pCJ){}
void CJobsMgr::CJStopped(char * pCJ){}
BOOL CJobsMgr::GetPJList(CStringArray & JList){return FALSE;}
BOOL CJobsMgr::GetCJList(CStringArray &JList){return FALSE;}
BOOL CJobsMgr::PJCompleted(){return FALSE;}
CCtrlJob * CJobsMgr::GetActiveCJ(){return NULL;}
CPRJob * CJobsMgr::GetActivePJ(){return NULL;}
CJobsMgr::FSMERRCODE CJobsMgr::DeselectCJ(){return NO_FSM_ERROR;}
CJobsMgr::FSMERRCODE CJobsMgr::SelectCJ(){return NO_FSM_ERROR;}
CJobsMgr::FSMERRCODE CJobsMgr::ResumeCJ(){return NO_FSM_ERROR;}
CJobsMgr::FSMERRCODE CJobsMgr::PauseCJ(){return NO_FSM_ERROR;}
CJobsMgr::FSMERRCODE CJobsMgr::AbortCJ(char *pCJ){return NO_FSM_ERROR;}
CJobsMgr::FSMERRCODE CJobsMgr::StopCJ(char *pCJ){return NO_FSM_ERROR;}
CJobsMgr::FSMERRCODE CJobsMgr::StartCJ(char * pCJ){return NO_FSM_ERROR;}
CJobsMgr::FSMERRCODE CJobsMgr::CancelCJ(char * pCJ){return NO_FSM_ERROR;}

class CCtrlJob;
class CPRJob;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
void CJobsMgr::AddCJ(CCtrlJob * pCJ){}
void CJobsMgr::AddPJ(CPRJob * pPJ){}
CJobsMgr::CJobsMgr()
{

}

CJobsMgr::~CJobsMgr()
{

}
