// WarningDlg.cpp : implementation file
//

#include "stdafx.h"
//#include "Log.h"
#include "WarningDlg.h"
#include "..\Src\DosUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWarningDlg dialog

CWarningDlg::CWarningDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWarningDlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CWarningDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	Title = "";
}

void CWarningDlg::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWarningDlg)
	DDX_Control(pDX, IDC_LIST1, m_clist);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CWarningDlg, CDialog)
	//{{AFX_MSG_MAP(CWarningDlg)
	ON_BN_CLICKED(IDC_CLEAR, OnClear)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWarningDlg message handlers

void CWarningDlg::Message(CString msg) {
	Messages.AddHead(msg);
	Update();
}

BOOL CWarningDlg::OnInitDialog() {
	CDialog::OnInitDialog();

	m_clist.InsertColumn(0, _T("Message"), LVCFMT_LEFT);
	m_clist.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);

	SetWindowText(Title);

	CRect rc;
	CRect rc1;
	GetDesktopWindow()->GetWindowRect(&rc);
	GetWindowRect(&rc1);
	SetWindowPos(&wndTopMost, rc.Width() - rc1.Width(), rc1.Height() / 2, rc1.Width(), rc1.Height(), SWP_HIDEWINDOW);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CWarningDlg::OnClear() {}

void CWarningDlg::Update() {
	CString msg;
	msg = Messages.RemoveHead();

	if (msg.GetLength()) {
		CString str;
		str = msg.Left(msg.Find('\t')) + " " + msg.Mid(msg.ReverseFind('\t') + 1);
		m_clist.InsertItem(0, str);
		if (m_clist.GetItemCount() > 64) {
			for (int i = m_clist.GetItemCount(); i > 52; i--) {
				m_clist.DeleteItem(i);
			}
		}

		ShowWindow(SW_SHOW);
		CString dir;
		DosUtil.GetProgramDir(dir);

		dir += "Log\\";

		int nTemp = ::GetFileAttributes(dir);
		if (!(FILE_ATTRIBUTE_DIRECTORY & nTemp) || (nTemp == -1)) {
			if (!::CreateDirectory(dir, NULL)) {
				return;
			}
		}
		dir += CTime::GetCurrentTime().Format("Y%y%B%d\\");
		nTemp = ::GetFileAttributes(dir);
		if (!(FILE_ATTRIBUTE_DIRECTORY & nTemp) || (nTemp == -1)) {
			if (!::CreateDirectory(dir, NULL)) {
				return;
			}
		}
		dir += "Warning.Log";
		CSingleLock Lock(&CS);
		Lock.Lock();
		FILE* fp;
		fp = fopen(dir, "ab");
		if (fp) {
			str += "\r\n";
			fwrite(str.GetBuffer(str.GetLength()), sizeof(char), str.GetLength(), fp);
			fclose(fp);
		}
	}
}