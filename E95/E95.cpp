// E95.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "E95.h"
#include "MainFrm.h"

#include "E95Doc.h"
#include "E95View.h"

#include "..\SYS\Var.h"
#include "..\SRC\sinstance.h"
#include "..\SRC\FileVersion.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CE95App

BEGIN_MESSAGE_MAP(CE95App, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

// CE95App construction
CE95App::CE95App() {}

// The one and only CE95App object
CE95App theApp;

// CE95App initialization
BOOL CE95App::InitInstance() {
	CInstanceChecker instanceChecker;
	if (instanceChecker.PreviousInstanceRunning()) {
		instanceChecker.ActivatePreviousInstance();
		return FALSE;
	}

	if (!AfxSocketInit()) {
		AfxMessageBox("SOCKETS INIT FAILED");
		return FALSE;
	}

	if (!SocketStartUp()) {
		ASSERT(0);
		AfxMessageBox("Socket startup failed , App cannot continue !");
		return FALSE;
	}
	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

	CWinApp::InitInstance();

	// Initialize OLE libraries
	if (!AfxOleInit()) {
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)
	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CSingleDocTemplate* pDocTemplate;
	if (Var.UIMode == 1) {
		pDocTemplate = new CSingleDocTemplate(
			IDR_MAINFRAME,
			RUNTIME_CLASS(CE95Doc),
			RUNTIME_CLASS(CMainFrame),       // main SDI frame window
			RUNTIME_CLASS(CE95View));
	}
	else if (Var.UIMode == 2) {
		pDocTemplate = new CSingleDocTemplate(
			IDR_UIMODE2,
			RUNTIME_CLASS(CE95Doc),
			RUNTIME_CLASS(CMainFrame),       // main SDI frame window
			RUNTIME_CLASS(CE95View));
	}
	else {
		pDocTemplate = new CSingleDocTemplate(
			IDR_UIMODE2,
			RUNTIME_CLASS(CE95Doc),
			RUNTIME_CLASS(CMainFrame),       // main SDI frame window
			RUNTIME_CLASS(CE95View));
	}
	if (!pDocTemplate) {
		return FALSE;
	}
	AddDocTemplate(pDocTemplate);
	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// The one and only window has been initialized, so show and update it
	m_pMainWnd->SetWindowText("FSM Control Module");
	if (Var.UIMode == 0) {
		m_pMainWnd->ShowWindow(SW_SHOWMAXIMIZED);
		m_pMainWnd->SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}
	else if (Var.UIMode == 1) {
		m_pMainWnd->ShowWindow(SW_SHOWMAXIMIZED);
		//	m_pMainWnd->ShowWindow(SW_SHOW);
	}
	else {
		// m_pMainWnd->ShowWindow(SW_SHOWMAXIMIZED);
		m_pMainWnd->ShowWindow(SW_SHOW);
	}
	m_pMainWnd->UpdateWindow();
	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand

	// If this is the first instance of our App then track it so any other instances can find it.
	if (!instanceChecker.PreviousInstanceRunning()) {
		instanceChecker.TrackFirstInstanceRunning();
	}
	return TRUE;
}

// BOOL CE95App::IsNTBasedOS()
// {
// 	OSVERSIONINFOEX OSV;
//
// 	ZeroMemory(&OSV, sizeof(OSVERSIONINFOEX));
// 	OSV.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
//
// 	if( !(GetVersionEx ((OSVERSIONINFO *) &OSV)) )
// 	{
// 		OSV.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
// 		if (! GetVersionEx ( (OSVERSIONINFO *) &OSV) )
// 		{
// 			return FALSE;
// 		}
// 	}
// 	return OSV.dwPlatformId == VER_PLATFORM_WIN32_NT;
// }

BOOL CE95App::SocketStartUp() {
	// 	WORD VersionRequested;
	// 	WSADATA WsaData;
	// 	int SockErr;
	//
	// 	VersionRequested = MAKEWORD( 2, 2 );
	//
	// 	SockErr = WSAStartup( VersionRequested, &WsaData );
	// 	if ( SockErr != 0 )
	// 	{
	// 		return FALSE;
	// 	}
	//
	// 	if ( LOBYTE( WsaData.wVersion ) != 2 ||
	// 			HIBYTE( WsaData.wVersion ) != 2 )
	// 	{
	// 		WSACleanup( );
	// 		return FALSE;
	// 	}
	return TRUE;
}

int CE95App::ExitInstance() {
	// 	WSACleanup( );
	return CWinApp::ExitInstance();
}

// CAboutDlg dialog used for App About

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD) {
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDC_VERSION, m_cversion);
	DDX_Control(pDX, IDC_PROGDIR, m_cprogdir);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CE95App::OnAppAbout() {
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

BOOL CAboutDlg::OnInitDialog() {
	CDialog::OnInitDialog();

	char inifile[_MAX_PATH];
	GetModuleFileName(NULL, inifile, _MAX_PATH);

	m_cprogdir.SetWindowText(inifile);

	CFileVersion fv;

	if (fv.Open(inifile)) {
		CString ver = "";
		ver.Format("%s:\r\nVer:%s (build %s)\r\n%s\r\n%s",
			fv.GetFileDescription(), fv.GetFileVersion(),
			fv.GetSpecialBuild(), fv.GetLegalCopyright(), fv.GetCompanyName());
		m_cversion.SetWindowText(ver);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}