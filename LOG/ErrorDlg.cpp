// ErrorDlg.cpp : implementation file
//

#include "stdafx.h"
//#include "Log.h"
#include "ErrorDlg.h"
#include "..\Src\DosUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CErrorDlg dialog

CErrorDlg::CErrorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CErrorDlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CErrorDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	Title = "";
}

void CErrorDlg::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CErrorDlg)
	DDX_Control(pDX, IDC_LIST1, m_clist);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CErrorDlg, CDialog)
	//{{AFX_MSG_MAP(CErrorDlg)
	ON_BN_CLICKED(IDC_CLEAR, OnClear)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CErrorDlg message handlers

void CErrorDlg::Message(CString msg) {
	Messages.AddTail(msg);
	Update();
}

BOOL CErrorDlg::OnInitDialog() {
	CDialog::OnInitDialog();

	m_clist.InsertColumn(0, _T("Message"), LVCFMT_LEFT);
	m_clist.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);

	SetWindowText(Title);

	CRect rc;
	CRect rc1;
	GetDesktopWindow()->GetWindowRect(&rc);
	GetWindowRect(&rc1);
	SetWindowPos(&wndTopMost, 0, 0, rc1.Width(), rc1.Height(), SWP_HIDEWINDOW);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CErrorDlg::OnClear() {}

void CErrorDlg::Update() {
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
		dir += "Error.Log";
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