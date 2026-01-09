// RecipeDlg.cpp : implementation file
//

#include "stdafx.h"

#include "RecipeDlg.h"
#include "GetPW.h"
#include "..\..\SRC\Logindatanew.h"
#include "..\..\RCP\RecipeNSMgr.h"
#include "..\inc\event.h"
#include "..\ShareMemory\evt.h"
#include "..\ShareMemory\SharedMemQue.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRecipeDlg dialog


CRecipeDlg::CRecipeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRecipeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRecipeDlg)
	m_RecipeName = _T("T1");
	m_CreatorName = _T("T2");
	m_ScanSize = 280;
	m_ScanLines = 1;
	m_ScanNo = 1;
	//}}AFX_DATA_INIT
	bEditing = FALSE;
}


void CRecipeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	CString WFD;
	UINT MaxScan;
	// The controls are not created initially during WM_INIT_DIALOG message so.....
	if(m_WaferDia.m_hWnd){
		m_WaferDia.GetWindowText(WFD);
		MaxScan = atoi(WFD.GetBuffer(0));
	}else{
		MaxScan = 280;
	}
	//{{AFX_DATA_MAP(CRecipeDlg)
	DDX_Control(pDX, IDC_WAFER_DIA, m_WaferDia);
	DDX_Text(pDX, IDC_EDIT_RECIPENAME, m_RecipeName);
	DDV_MaxChars(pDX, m_RecipeName, 25);
	DDX_Text(pDX, IDC_EDIT_CREATOR, m_CreatorName);
	DDV_MaxChars(pDX, m_CreatorName, 25);
	DDX_Text(pDX, IDC_EDIT_SCANSIZE, m_ScanSize);
	DDV_MinMaxUInt(pDX, m_ScanSize, 1, MaxScan);
	DDX_Text(pDX, IDC_SCAN_LINES, m_ScanLines);
	DDV_MinMaxUInt(pDX, m_ScanLines, 1, 25);
	DDX_Text(pDX, IDC_SCAN_NO, m_ScanNo);
	DDV_MinMaxUInt(pDX, m_ScanNo, 1, 25);
	//}}AFX_DATA_MAP
	if(pDX->m_bSaveAndValidate){
		CString Msg;
		Rcp.SetRecipeName(m_RecipeName.GetBuffer(0));
		strcpy(Rcp.Creator, m_CreatorName);
		Rcp.scansize = m_ScanSize;
		Rcp.nlines = m_ScanLines;
		Rcp.scanno = m_ScanNo;
		Rcp.laser = (((CButton*)GetDlgItem(IDC_LASER_750))->GetCheck()==BST_CHECKED)?750:810;
		m_WaferDia.GetLBText(m_WaferDia.GetCurSel(),Msg);
		Rcp.wafersize= atoi(Msg.GetBuffer(0));
	}
}


BEGIN_MESSAGE_MAP(CRecipeDlg, CDialog)
	//{{AFX_MSG_MAP(CRecipeDlg)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_LOAD, OnButtonLoad)
	ON_BN_CLICKED(IDC_BUTTON_HOST_UPLOAD, OnButtonHostUpload)
	ON_BN_CLICKED(IDC_LOGIN, OnLogin)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRecipeDlg message handlers

BOOL CRecipeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	// Laser
	if((this->Rcp.laser == 750) || (this->Rcp.laser == 780)){
		((CButton *)GetDlgItem(IDC_LASER_810))->SetCheck(FALSE);
		((CButton *)GetDlgItem(IDC_LASER_750))->SetCheck(TRUE);
	}
	else{
		((CButton *)GetDlgItem(IDC_LASER_810))->SetCheck(TRUE);
		((CButton *)GetDlgItem(IDC_LASER_750))->SetCheck(FALSE);
	}
	// Recipe Name
	m_RecipeName = Rcp.GetRecipeName();
	// Creator
	m_CreatorName = Rcp.Creator;
	// WaferSize
	// changes in the data of the control should be reflected in initialization
	// .. Vijay 14 Aug 03
	this->m_WaferDia.SetCurSel(2);
	// ScanSize
	this->m_ScanSize = Rcp.scansize;
	// Scan Number
	this->m_ScanNo =Rcp.scanno;
	// ScanLines
	this->m_ScanLines = Rcp.nlines;
	UpdateData(FALSE);
	if(bEditing){
		GetDlgItem(IDC_EDIT_RECIPENAME)->EnableWindow(FALSE);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CRecipeDlg::OnButtonSave() 
{
	// Check if Exists if so thenit is edit
	CString sRCP;
	GetDlgItem(IDC_EDIT_RECIPENAME)->GetWindowText(sRCP);
	string sPPID = sRCP.GetBuffer(0);
	BOOL bEdit = FALSE;
	if(RcpMgr.IsRecipeAvailable(sPPID)){		// Edit Operation
		bEdit = TRUE;
	}
	// Else it is Create operation
	if(!UpdateData(TRUE)){
		return;
	}
	CString RcpName;
	if(!CheckRCPFolder(RcpName)){
		return;
	}
	// Open a file
	RcpName += Rcp.GetRecipeName();
	RcpName += ".RCP";
	CFile theFile;
	CFileException Ex;
	CString FMsg;
	GetLocalTime(&Rcp.ModifiedTime);
	if (theFile.Open(RcpName, CFile::modeCreate|CFile::modeWrite,&Ex)) {
		CArchive archive(&theFile, CArchive::store);
		Rcp.Serialize(archive);
		archive.Close();
		theFile.Close();
		//return ;
	}else{
		FMsg.Format("   File Error \n%s",FileExText(Ex.m_cause));
		AfxMessageBox(FMsg,MB_OK|MB_ICONSTOP);
		return;
	}
	// Changes in scheme of recipe should have reflected everywhere to avoid the 
	// .. confusion to user -Vijay 14 Aug 03
	FMsg.Format("Recipe (%s) Saved Succesfully",/*Rcp.GetRecipeName()*/sRCP); 
	AfxMessageBox(FMsg,MB_OK);
	// Check if file exists
	// Serialize

//	OnOK();
	// Send event here
	CEvt *pEvt = new CEvt;
	ASSERT(pEvt);
	short len = strlen(Rcp.GetRecipeName());
	//memset(pEvt->msg,0,sizeof(pEvt->msg));
	pEvt->SetMsg((BYTE *)Rcp.GetRecipeName(),len);
	//pEvt->msg[len] = 0;
	pEvt->Set( EVT_OTHER , EVT_RCP_CHANGE); // Equip Terminal Service Msg to Host

	if(bEdit){
		pEvt->no = 1;
	}else{
		pEvt->no = 2;
	}
	CSharedMemQue	MemQue;
	MemQue.OpenMemory("EVTMSG");
	MemQue.WriteQue( pEvt );
	MemQue.CloseMemory();		
}

void CRecipeDlg::OnButtonLoad() 
{
	CString RcpName;
	if(!CheckRCPFolder(RcpName)){
		return;
	}
	// 
	CFileException Ex;
	CString FMsg;
	CFileDialog Dlg(TRUE);
	Dlg.m_ofn.lpstrInitialDir = RcpName.GetBuffer(0);
	Dlg.m_ofn.lpstrFilter = "RecipeFiles\0*.RCP";
	if(IDOK==Dlg.DoModal()){
		CFile theFile;
		RcpName = Dlg.GetPathName();
		if (theFile.Open(RcpName, CFile::modeRead,&Ex)) {
			CArchive archive(&theFile, CArchive::load);
			Rcp.Serialize(archive);
			archive.Close();
			theFile.Close();
			//return ;
		}else{
			FMsg.Format("   File Error \n%s",FileExText(Ex.m_cause));
			AfxMessageBox(FMsg,MB_OK|MB_ICONSTOP);
			return;
		}
		// Why we store version name as recipe name ??????
		// this creates inconsistancy. User assigns a name to the recipe
		// .. saves the same & when the user reopens the same, finds the 
		// .. recipe name changed ! Vijay 14 Aug 03

		m_RecipeName = /*Rcp.GetRecipeName()*/Dlg.GetFileTitle();
		// Vijay 14 Aug 03
		m_CreatorName = Rcp.Creator;
		m_ScanSize = Rcp.scansize;
		m_ScanLines = Rcp.nlines;
		m_ScanNo = Rcp.scanno;
		if((Rcp.laser== 750)|| (Rcp.laser== 780))
		{
			((CButton *)GetDlgItem(IDC_LASER_810))->SetCheck(FALSE);
			((CButton *)GetDlgItem(IDC_LASER_750))->SetCheck(TRUE);
		}
		else
		{
			((CButton *)GetDlgItem(IDC_LASER_810))->SetCheck(TRUE);
			((CButton *)GetDlgItem(IDC_LASER_750))->SetCheck(FALSE);
		}
		int Sel= 1;
		// changes in the data of the control should be reflected here !
		// .. Vijay 14 Aug 03
		switch(Rcp.wafersize)
		{
		case 150:Sel = 0;break;
		case 200:Sel = 1;break;
		case 300:Sel = 2;break;
		}
		m_WaferDia.SetCurSel(Sel);;
		UpdateData(FALSE);
	}
}

BOOL CRecipeDlg::CheckRCPFolder(CString &RcpFolder)
{
	// Get module file name
	RcpFolder = "";
	char strDir[_MAX_PATH] = {0},Dir[_MAX_PATH],Drive[52],Fname[_MAX_PATH],Ext[26];
	CString strTemp;

	::GetModuleFileName(NULL,strDir,_MAX_PATH);
	::_splitpath(strDir,Drive,Dir,Fname,Ext);
	strTemp = Drive;
	strTemp += Dir;
	strTemp += _T("Recipe\\");
	int nTemp = ::GetFileAttributes(strTemp);
	if(!(FILE_ATTRIBUTE_DIRECTORY & nTemp)){
		if (NULL  ==::CreateDirectory(strTemp,NULL)){
		    AfxMessageBox("Cannot create default Recipe directory. Exiting",MB_OK);
			return FALSE;
		}
	}
	RcpFolder= strTemp;
	return TRUE;
	// Check recipe folder exists
	// If not create one
}

CString CRecipeDlg::FileExText(int Cause)
{	
	CString ErrStr;
	switch(Cause){
	case CFileException::none: ErrStr= " No error occurred";break;
	case CFileException::generic: ErrStr= "An unspecified error occurred";break;
	case CFileException::fileNotFound: ErrStr= "The file could not be located";break;
	case CFileException::badPath: ErrStr= "All or part of the path is invalid";break;
	case CFileException::tooManyOpenFiles: ErrStr= "The permitted number of open files was exceeded";break;
	case CFileException::accessDenied: ErrStr= "The file could not be accessed";break;
	case CFileException::invalidFile: ErrStr= "There was an attempt to use an invalid file handle";break;
	case CFileException::removeCurrentDir: ErrStr= "The current working directory cannot be removed";break;
	case CFileException::directoryFull: ErrStr= "There are no more directory entries";break;
	case CFileException::badSeek: ErrStr= "There was an error trying to set the file pointer";break;
	case CFileException::hardIO: ErrStr= "There was a hardware error";break;
	case CFileException::sharingViolation: ErrStr= "SHARE.EXE was not loaded, or a shared region was locked";break;
	case CFileException::lockViolation: ErrStr= "There was an attempt to lock a region that was already locked";break;
	case CFileException::diskFull: ErrStr= "The disk is full";break;
	case CFileException::endOfFile: ErrStr= "The end of file was reached";break;
	default:ErrStr= "Unknown error occurred";break;
	}
	return ErrStr;
}

void CRecipeDlg::OnButtonHostUpload() 
{
// Dirty work
	CEvt *pEvt = new CEvt;
	ASSERT(pEvt);
	//memset(pEvt->msg,0,sizeof(pEvt->msg));
	int RcpLen = Rcp.GetBinarySize();
	short len = strlen(Rcp.GetRecipeName());
	pEvt->SetMsg((BYTE *)Rcp.GetRecipeName(),len);
	//pEvt->msg[len] = 0;
	if(RcpLen >244){
		pEvt->Set( EVT_OTHER , EVT_REQ_MB_RECIPE); // Equip Terminal Service Msg to Host
	}else{
		pEvt->Set( EVT_OTHER , EVT_UPLOAD_RECIPE); // Equip Terminal Service Msg to Host
	}
	CSharedMemQue	MemQue;
	MemQue.OpenMemory("EVTMSG");
	MemQue.WriteQue( pEvt );
	MemQue.CloseMemory();	
}

void CRecipeDlg::OnLogin() 
{
	if (pGDev->AuthenticateUser()) {
		EnableDlgItems();
	}
//	CGetPW pw;
//	if (pw.DoModal() == IDOK) {
//		CLogInDataNew login;
//		if (login.check_logging(pw.m_username,pw.m_password)) {
//			if (login.check_superpass(pw.m_username,pw.m_password)) {
//				EnableDlgItems();
//			}
//			else {
//				AfxMessageBox("User not authorized");
//			}
//		}
//		else {
//			AfxMessageBox("User not found");
//		}
//	}
}

void CRecipeDlg::EnableDlgItems()
{
	((CButton*)GetDlgItem(IDC_LASER_750))->EnableWindow();
	((CButton*)GetDlgItem(IDC_LASER_810))->EnableWindow();
	((CButton*)GetDlgItem(IDC_EDIT_RECIPENAME))->EnableWindow();
	((CButton*)GetDlgItem(IDC_EDIT_CREATOR))->EnableWindow();
	((CButton*)GetDlgItem(IDC_WAFER_DIA))->EnableWindow();
	((CButton*)GetDlgItem(IDC_EDIT_SCANSIZE))->EnableWindow();
	((CButton*)GetDlgItem(IDC_SCAN_NO))->EnableWindow();
	((CButton*)GetDlgItem(IDC_SCAN_LINES))->EnableWindow();
	((CButton*)GetDlgItem(IDC_BUTTON_SAVE))->EnableWindow();
	((CButton*)GetDlgItem(IDC_BUTTON_HOST_UPLOAD))->EnableWindow();
}
