// AlignerSwitcher.cpp: implementation of the CAlignerSwitcher class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\..\SRC\Comm.h"
#include "..\..\SRC\DosUtil.h"
#include "AlignerSwitcher.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CAlignerSwitcher::CAlignerSwitcher()
{
}

CAlignerSwitcher::~CAlignerSwitcher()
{

}

BOOL CAlignerSwitcher::Swith128()
{
	CComm Com;
	short port = DosUtil.GetAlignerCtrlPort();
	if (port > 0)
	{
		Com.SetParam((BYTE)port,9600,8,NOPARITY,1,0);
		if (Com.Open()) {
			Com.SendCommandEcho("SO2H\r\n",6);
			Com.Close();
			Sleep(1000);	// stabilization
			return TRUE;
		}
		ASSERT(0);
	}
// 	if (DosUtil.GetEnaAlignerCtrl()) {
// 		AfxMessageBox("EnaAlignerCtrl is TRUE but no comm port specified");
// 		return FALSE;
// 	}
// 	else {
// 		return TRUE;
// 	}
	return FALSE;
}

BOOL CAlignerSwitcher::Switch900()
{
	CComm Com;	
	short port = DosUtil.GetAlignerCtrlPort();
	if (port > 0)
	{
		Com.SetParam((BYTE)port,9600,8,NOPARITY,1,0);
		if (Com.Open()) {
			Com.SendCommandEcho("SO2L\r\n",6);
			Com.Close();
			Sleep(1000);	// stabilization
			return TRUE;
		}
		ASSERT(0);
	}
	return FALSE;
}

int CAlignerSwitcher::GetWaferPresent()
{
	int status = 0;
	CComm Com;
	short port = DosUtil.GetAlignerCtrlPort();
	if (port > 0)
	{
		Com.SetParam((BYTE)port,9600,8,NOPARITY,1,0);
		if (Com.Open()) {
			CString Cmd;
			Cmd.Format("IS\r\n");  // Just in case QuerryData modify the string
			if (Com.QuerryData(Cmd)) 
			{
				if (Cmd.GetLength() < 12)
				{
					return status;
				}
				Cmd.TrimLeft();
				Cmd.TrimRight();
				Cmd = Cmd.Mid(3);
				if (Cmd.GetAt(3) == '0')		// aligner
				{
					status += 1;
				}
				if (Cmd.GetAt(4) == '0')		// scale
				{
					status += 2;
				}
				if (Cmd.GetAt(5) == '0')		// scan stage
				{
					status += 4;;
				}
				return status;
			}
		}
	}
	return status;
}
