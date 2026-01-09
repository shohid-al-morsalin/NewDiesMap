// DMCMotor.cpp: implementation of the CDMCMotor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DMCMotor.h"
#include "..\E95\413App.h"
#include "..\SRC\DOSUtil.h"

#include "GlobalDeviceContainer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDMCMotor::CDMCMotor() {
	hDmc = NULL;
	FailExePrgm = "Could not execute a program in Echo Motion Controller";
	FailDnldPrgm = "Could not download a program to Echo Motion Controller";
	FailStopPrgm = "Could not stop Echo Motion Controller";
}

CDMCMotor::~CDMCMotor() {}

BOOL CDMCMotor::Initialize(CString Filename, CListBox* pMessage) {
	long rc;
	CString str;
	char szBuffer[256];
	GALILREGISTRY gr;
	unsigned short nController = 1;

	hDmc = NULL;

	// [6/3/2020] ===================
	//rc = DMCGetGalilRegistryInfo(1, &gr);
	//gr.hardwareinfo.serialinfo.usCommPort = p413App->EchoPort;
	//rc = DMCModifyGalilRegistry(1, &gr);

	int nCode = DMCGetGalilRegistryInfo(nController, &gr);
	if (nCode != 0) {
		gr.ulTimeout = 5000;
		gr.fControllerType = 1;
		gr.usCommPort = p413App->EchoPort;
		gr.ulCommSpeed = 19200;

		nCode = DMCAddGalilRegistry(&gr, &nController);
		if (nCode != 0)
			return FALSE;
	}
	rc = DMCGetGalilRegistryInfo(1, &gr);

	if (gr.usCommPort != p413App->EchoPort) {
		gr.usCommPort = p413App->EchoPort;
		rc = DMCModifyGalilRegistry(1, &gr);
	}
	// ==========================

	rc = 1;
	for (int nCount = 0; nCount < 1; nCount++) {
		// @@@ to replace 1 with variable read from INI [10/27/2012 Yuen]
		short CtrlNo = 1;
		rc = DMCOpen(CtrlNo, 0, &hDmc);
		if (rc) {
			Sleep(100);
			DMCClose(hDmc);
			str.Format("Echo Motion Controller: Could not open controller number %d", CtrlNo);
			Log(str, 0);
			pMessage->InsertString(pMessage->GetCount(), str);
			return FALSE;
		}
		else
			break;
	}

	rc = DMCCommand(hDmc, "ST\r", szBuffer, sizeof(szBuffer - 1));
	if (rc) {
		CString str; str.Format("%d", rc);
		Log(FailStopPrgm + str, 0);
		pMessage->InsertString(pMessage->GetCount(), FailStopPrgm + str);
		DMCClose(hDmc);
		return FALSE;
	}

	if (!LoadProgram(Filename)) {
		Log(FailDnldPrgm, 0);
		pMessage->InsertString(pMessage->GetCount(), FailDnldPrgm);
		return FALSE;
	}

	pMessage->InsertString(pMessage->GetCount(), "Echo Motion Controller started OK");
	return TRUE;
}

BOOL CDMCMotor::Deinitialize() {
	if (!hDmc) {
		return TRUE;
	}
	CString str;
	char szBuffer[256];
	long rc;
	rc = DMCCommand(hDmc, "ST\r", szBuffer, sizeof(szBuffer));
	if (rc) {
		str.Format("%d", rc);
		Log(FailStopPrgm + str, 0);
		DMCClose(hDmc);
		return FALSE;
	}
	if (hDmc) {
		DMCClose(hDmc);
	}
	return TRUE;
}

BOOL CDMCMotor::Run() {
	if (!hDmc) {
		return FALSE;
	}
	if (bRun) {
		return TRUE;
	}
	CString dir, str;
	DosUtil.GetProgramDir(dir);
	dir = dir + "ECHOMOTOR.LOG";
	FILE* fp = fopen(dir, "ab");

	long rc;
	char szBuffer[256];
	rc = DMCCommand(hDmc, "ST\r", szBuffer, sizeof(szBuffer));
	if (rc) {
		str.Format("%d", rc);
		Log(FailStopPrgm + str, 0);
		DMCClose(hDmc);
		if (fp) {
			fprintf(fp, "%s Echo Motor Fail ST\r\n", p413App->GetTimeStamp().GetBuffer(0));
			fclose(fp);
		}
		return FALSE;
	}
	Sleep(250);
	rc = DMCCommand(hDmc, "XQ\r", szBuffer, sizeof(szBuffer));
	if (rc) {
		str.Format("%d", rc);
		Log(FailExePrgm + str, 0);
		DMCClose(hDmc);
		if (fp) {
			fprintf(fp, "%s Echo Motor Fail XQ\r\n", p413App->GetTimeStamp().GetBuffer(0));
			fclose(fp);
		}
		return FALSE;
	}
	if (fp) {
		fprintf(fp, "%s Echo Motor Start\r\n", p413App->GetTimeStamp().GetBuffer(0));
		fclose(fp);
	}
	Sleep(1000);  // Start up wait [7/19/2010 C2C]
	bRun = TRUE;
	return TRUE;
}

BOOL CDMCMotor::Stop() {
	if (!hDmc) {
		return FALSE;
	}
	CString dir, str;
	DosUtil.GetProgramDir(dir);
	dir = dir + "ECHOMOTOR.LOG";
	FILE* fp = fopen(dir, "ab");
	long rc;
	char szBuffer[256];
	rc = DMCCommand(hDmc, "ST;MO;\r", szBuffer, sizeof(szBuffer));
	if (rc) {
		str.Format("%d", rc);
		Log(FailStopPrgm + str, 0);
		//DMCClose(hDmc);
		if (fp) {
			fprintf(fp, "%s Echo Motor Fail Stop\r\n", p413App->GetTimeStamp().GetBuffer(0));
			fclose(fp);
		}
		return FALSE;
	}
	bRun = FALSE;
	if (fp) {
		fprintf(fp, "%s Echo Motor Stopped\r\n", p413App->GetTimeStamp().GetBuffer(0));
		fclose(fp);
	}
	return TRUE;
}

BOOL CDMCMotor::Reload(CString Filename) {
	if (!hDmc) {
		return FALSE;
	}
	BOOL bRunBK = bRun;
	if (MotorFile != Filename) {
		if (!Stop()) {
			return FALSE;
		}
		if (!LoadProgram(Filename)) {
			return FALSE;
		}
		if (bRunBK) Run();
	}
	//if (bRunBK)
	//{
	//	bRun = TRUE;	// By now the motor isnot running yet, caller need to chech this value and act appropriately [10/16/2012 FSM413]
	//}
	return TRUE;
}

BOOL CDMCMotor::LoadProgramA() {
	if (MotorFile.GetLength() < 1) {
		ASSERT(0);
		return FALSE;
	}

	CString filename;
	DosUtil.GetProgramDir(filename);

	filename += MotorFile;

	long rc;
	rc = DMCDownloadFile(hDmc, filename.GetBuffer(0), NULL);
	if (rc) {
		Log(FailExePrgm, 0);
		DMCClose(hDmc);
		return FALSE;
	}
	return TRUE;
}

// cmd requires a '\r' termination included [6/17/2013 Yuen]
BOOL CDMCMotor::Send(char* cmd) {
	if (!hDmc) {
		return FALSE;
	}
	long rc;
	char szBuffer[256];
	rc = DMCCommand(hDmc, "cmd", szBuffer, sizeof(szBuffer));
	if (rc) {
		return FALSE;
	}
	return TRUE;
}