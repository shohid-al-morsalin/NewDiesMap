// OnlineMgr.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "OnlineMgr.h"
#include "..\ONLINE\INC\Define.h"
#include "..\ONLINE\MFC\establishcomm.h"

#include "GlobalDeviceContainer.h"
#include "..\SRC\DOSUtil.h"

#define MAXLOGLINE	1024
#define LOGDELLEN	256

/////////////////////////////////////////////////////////////////////////////
// COnlineMgr dialog

COnlineMgr::COnlineMgr(CWnd* pParent /*=NULL*/)
	: CResizableDialog(COnlineMgr::IDD, pParent) {
	//{{AFX_DATA_INIT(COnlineMgr)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	pEstComm = NULL;

	fp = NULL;
	LogFileName = "";
	Cnt = 0;
	MsgPtr = 0;

	MakeNewLogFile();
}

COnlineMgr::~COnlineMgr() {}

void COnlineMgr::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COnlineMgr)
	DDX_Control(pDX, IDC_SECSMSG, m_cSecsMsg);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(COnlineMgr, CResizableDialog)
	//{{AFX_MSG_MAP(COnlineMgr)
	ON_COMMAND(ID_MSG_ESTCOMM, OnMsgEstcomm)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_USER_ASYNC_SELECT, OnAsyncSelect)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COnlineMgr message handlers

BOOL COnlineMgr::OnInitDialog() {
	CResizableDialog::OnInitDialog();

	AddAnchor(IDC_SECSMSG, TOP_LEFT, BOTTOM_LEFT);

	StartThreads();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

LONG COnlineMgr::OnAsyncSelect(WPARAM wPram, LPARAM lParam) {
	switch (WSAGETSELECTEVENT(lParam)) {
	case FD_ACCEPT:
		FDAcceptSocket();
		break;

	case FD_CLOSE:
		FDCloseSocket();
		break;
	case FD_WRITE:
		break;
	case FD_ROUTING_INTERFACE_CHANGE:
		break;
	case FD_ADDRESS_LIST_CHANGE:
		break;
	default:
		break;
	}
	return 0L;
}

HWND COnlineMgr::GetHWnd() {
	return m_hWnd;
}

void COnlineMgr::Message(CString msg, short lvl) {
	Log(msg, lvl);
}

void COnlineMgr::Log(CString msg, short lvl) {
	pGDev->LogFile.Log(msg, lvl, "OnlMgr");
	if (lvl <= 1) {
		pGDev->LogFile.LogLocal(msg);
	}
}

void COnlineMgr::OnMsgEstcomm() {
	EstablishConnection();
}

CWnd* COnlineMgr::GetCWnd() {
	return this;
}

void COnlineMgr::ShowMsg(CString msg, short lvl) {
	Log(msg, lvl);
}

void COnlineMgr::SECSMessage(CString msg) {
	int n = m_cSecsMsg.GetCount();
	if (n > MAXLOGLINE) {
		for (int i = n; i >= n - 32; i--) {
			m_cSecsMsg.DeleteString(i);
		}
	}
	msg = pGDev->GetTimeStamp() + msg;
	m_cSecsMsg.InsertString(0, msg);
}

void COnlineMgr::OnDestroy() {
	CResizableDialog::OnDestroy();
}

// Note: Do not modify content of data, it is used elsewhere
// data contains the entire message except message length [8/10/2012 Administrator]
void COnlineMgr::SECSContent(BYTE* data, short len, BOOL bLcl, short* pTime) {
	CLogItem* pI = new CLogItem;
	if (pI) {
		pI->Append(data, len, LogFileName, bLcl, pTime);
		delete pI;
		Cnt++;
	}
	if (Cnt > 12 * MAXLOGLINE) {
		Cnt = 0;
		MakeNewLogFile();
	}
}

void COnlineMgr::MakeNewLogFile() {
	if (fp) {
		fclose(fp);
		fp = NULL;
	}

	CString FileName, Folder;
	FolderName(Folder);

	SYSTEMTIME Time;
	::GetLocalTime(&Time);
	FileName.Format("OL%02d%02d%02d%2d", Time.wHour, Time.wMinute, Time.wSecond, Time.wMilliseconds);

	FileName = Folder + FileName + ".LOG";
	LogFileName = FileName;
}

void COnlineMgr::FolderName(CString& FolderName) {
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	char inifile[_MAX_PATH];

	GetModuleFileName(NULL, inifile, 255);
	_splitpath(inifile, drive, dir, fname, ext);
	FolderName.Empty();
	FolderName = drive;
	FolderName += dir;
	FolderName += "ONL\\";
	if (_mkdir(FolderName) != 0) {
	};

	SYSTEMTIME Time;
	::GetLocalTime(&Time);

	CString YearMonth;
	YearMonth.Format("%04d%02d", Time.wYear, Time.wMonth);
	FolderName += YearMonth + "\\";
	if (_mkdir(FolderName) != 0) {
	}
}

void COnlineMgr::PostNcDestroy() {
	StopThreads();
	if (fp) {
		fclose(fp);
		fp = NULL;
	}
	if (pEstComm) {
		delete pEstComm;
	}
	CResizableDialog::PostNcDestroy();
	delete this;
}

void COnlineMgr::EstablishConnection() {
	if (pEstComm) {
		delete pEstComm;
	}
	pEstComm = new CEstablishComm;
	pEstComm->Create(CEstablishComm::IDD);
	pEstComm->ShowWindow(SW_SHOW);
}

// Record message length and GEM_HEADONLY [8/12/2012 Administrator]
void COnlineMgr::SECSAbnormal(BYTE* pData, short len, char* pMsg, short* pTime) {
	CLogItem* pI = new CLogItem;
	if (pI) {
		pI->Record(pData, len, LogFileName, pMsg, pTime);
		delete pI;
		Cnt++;
	}
	if (Cnt > 12 * MAXLOGLINE) {
		Cnt = 0;
		MakeNewLogFile();
	}
}