// OnlineLogsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "E95.h"
#include "OnlineLogsDlg.h"
#include ".\onlinelogsdlg.h"
#include "..\ONLINE\TCPIP\LogMessage.h"
#include "..\ONLINE\TCPIP\comlog.h"
#include "..\ONLINE\MFC\ur_resource.h"

using std::string; using std::vector;

extern CComLog m_Log;

// COnlineLogsDlg dialog

IMPLEMENT_DYNAMIC(COnlineLogsDlg, CDialog)
COnlineLogsDlg::COnlineLogsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COnlineLogsDlg::IDD, pParent) {
	m_CurrMessage = 0;
	m_Recordcnt = 0;
	m_CurRecordcnt = 0;
}

COnlineLogsDlg::~COnlineLogsDlg() {}

void COnlineLogsDlg::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_KEY, m_CodeKey);
	DDX_Control(pDX, IDC_EDIT_MESSAGE, m_Msg);
	DDX_Control(pDX, IDC_MSG_TYPE, m_Type);
	DDX_Control(pDX, IDC_EDIT_SYSTEM, m_System);
	DDX_Control(pDX, IDC_LIST_DATA, m_ListData);
	DDX_Control(pDX, IDC_LIST_LOG, m_LogFiles);
	DDX_Control(pDX, IDC_DATETIMEPICKER1, m_STimePicker);
	DDX_Control(pDX, IDC_DATETIMEPICKER2, m_ETimePicker);
	DDX_Control(pDX, IDC_SPIN_EHOUR, m_EHour_Spin);
	DDX_Control(pDX, IDC_SPIN_SHOUR, m_SHour_Spin);
	DDX_Control(pDX, IDC_SPIN_SMIN, m_SMin_Spin);
	DDX_Control(pDX, IDC_WBIT, m_Wbit);
	DDX_Control(pDX, IDC_TIME, m_Time);
	DDX_Control(pDX, IDC_FUNCTION, m_Function);
	DDX_Control(pDX, IDC_SYSBYTE, m_Sysbyte);
	DDX_Control(pDX, IDC_STREAM, m_Stream);
	DDX_Control(pDX, IDC_SPIN_EMIN, m_EMin_Spin);
	DDX_Control(pDX, IDC_RECORDCNT, m_RecordCount);
}

BEGIN_MESSAGE_MAP(COnlineLogsDlg, CDialog)
	ON_BN_CLICKED(IDC_LOGDOWN, OnBnClickedLogdown)
	ON_BN_CLICKED(IDC_FIRST, OnBnClickedFirst)
	ON_BN_CLICKED(IDC_PREV_10, OnBnClickedPrev10)
	ON_BN_CLICKED(IDC_PREV_5, OnBnClickedPrev5)
	ON_BN_CLICKED(IDC_PREV, OnBnClickedPrev)
	ON_BN_CLICKED(IDC_NEXT, OnBnClickedNext)
	ON_BN_CLICKED(IDC_NEXT_5, OnBnClickedNext5)
	ON_BN_CLICKED(IDC_NEXT_10, OnBnClickedNext10)
	ON_BN_CLICKED(IDC_LAST, OnBnClickedLast)
END_MESSAGE_MAP()

// COnlineLogsDlg message handlers

void COnlineLogsDlg::OnBnClickedLogdown() {
	// TODO: Add your control notification handler code here
	CFile nLogfile;
	CFileDialog fd(FALSE, "log", NULL,
		OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY,
		"SecsLog file|*.log|");
	if (fd.DoModal() == IDOK) {
		CString path = fd.GetPathName();
		nLogfile.Open(path, CFile::modeCreate | CFile::modeWrite);
		string Data, strTemp, Stream, Funct;
		bool bFound = false;
		UINT Temp;
		CLogMessage LMsg;
		for (int i = 0; i < m_Recordcnt; i++) {
			int nIndex;
			Temp = i;
			for (nIndex = 0; nIndex<int(m_MsgDat.size()); nIndex++) {
				if (Temp <= (m_MsgDat[nIndex].Pos)) {
					bFound = true;
					break;
				}
				Temp -= (m_MsgDat[nIndex].Pos);
			}
			char Buf[25];
			if (bFound) {
				if (::m_Log.ReadMessage(m_MsgDat[nIndex].FileName.c_str(), LMsg, Temp, m_MsgDat[nIndex].OffSet)) {
					strTemp = _itoa(i + 1, Buf, 10);
					strTemp += "/";
					strTemp += _itoa(m_Recordcnt, Buf, 10);
					Data = "(" + strTemp + ") ";
					LMsg.MessageType(strTemp);
					Data += " " + strTemp;
					LMsg.TimeStamp(strTemp);
					Data += " " + strTemp;
					LMsg.SysByte(strTemp);
					Data += " System Byte:" + strTemp;
					Data += "\r\n";
					LMsg.MessageSystem(strTemp);
					Data += "System:" + strTemp;
					LMsg.SreamFunction(Stream, Funct, strTemp);
					Data += " Stream:" + Stream + " Function:" + Funct + " WaitBit:" + strTemp + "\r\n";
					LMsg.Decode(strTemp);
					Data += strTemp + "\r\n";
					nLogfile.Write(Data.data(), Data.size());
				}
			}
		}
		nLogfile.Close();
	}
}

void COnlineLogsDlg::OnBnClickedFirst() {
	m_CurrMessage = 1;
	Display_OnScreen();
}

void COnlineLogsDlg::OnBnClickedPrev10() {
	int Temp = m_CurrMessage - 10;
	m_CurrMessage = (Temp < 0) ? (m_Recordcnt + Temp) : (Temp);
	Display_OnScreen();
}

void COnlineLogsDlg::OnBnClickedPrev5() {
	int Temp = m_CurrMessage - 5;
	m_CurrMessage = (Temp < 0) ? (m_Recordcnt + Temp) : (Temp);
	Display_OnScreen();
}

void COnlineLogsDlg::OnBnClickedPrev() {
	if (m_CurrMessage > 1) {
		m_CurrMessage--;
	}
	else {
		m_CurrMessage = m_Recordcnt;
	}
	Display_OnScreen();
}

void COnlineLogsDlg::OnBnClickedNext() {
	if (int(m_CurrMessage) < m_Recordcnt) {
		m_CurrMessage++;
	}
	else {
		m_CurrMessage = 1;
	}
	Display_OnScreen();
}

void COnlineLogsDlg::OnBnClickedNext5() {
	long Temp = m_CurrMessage + 5;
	m_CurrMessage = (Temp > m_Recordcnt) ? (Temp % m_Recordcnt) : Temp;
	Display_OnScreen();
}

void COnlineLogsDlg::OnBnClickedNext10() {
	long Temp = m_CurrMessage + 10;
	m_CurrMessage = (Temp > m_Recordcnt) ? (Temp % m_Recordcnt) : Temp;
	Display_OnScreen();
}

void COnlineLogsDlg::OnBnClickedLast() {
	m_CurrMessage = m_Recordcnt;
	Display_OnScreen();
}

void COnlineLogsDlg::Display_OnScreen() {
	if (int(m_CurrMessage) > m_Recordcnt) {
		AfxMessageBox("No More Log Messages");
		return;
	}
	UINT Temp = m_CurrMessage;
	int nIndex;
	bool bFound = false;
	for (nIndex = 0; nIndex<int(m_MsgDat.size()); nIndex++) {
		if (Temp <= (m_MsgDat[nIndex].Pos)) {
			bFound = true;
			break;
		}
		Temp -= (m_MsgDat[nIndex].Pos);
	}
	if (!bFound) {
		return;
	}
	m_LogFiles.SetHotItem(nIndex);

	CLogMessage LMsg;
	if (!::m_Log.ReadMessage(m_MsgDat[nIndex].FileName.c_str(), LMsg, Temp, m_MsgDat[nIndex].OffSet)) {
		return;
	}
	string Data, Funct, WBit;
	SetCol(LMsg.MessageType());
	LMsg.MessageType(Data);
	SetDlgItemText(IDC_MSG_TYPE, Data.c_str());
	SetDlgItemText(IDC_EDIT_KEY, LMsg.m_Key.c_str());
	LMsg.MessageSystem(Data);
	SetDlgItemText(IDC_EDIT_SYSTEM, Data.c_str());
	LMsg.SreamFunction(Data, Funct, WBit);
	SetDlgItemText(IDC_STREAM, Data.c_str());
	SetDlgItemText(IDC_FUNCTION, Funct.c_str());
	SetDlgItemText(IDC_WBIT, WBit.c_str());
	LMsg.Decode(Data, false);
	//	SetDlgItemText(IDC_EDIT_MESSAGE,Data.c_str());
	WriteDataToList(m_ListData, Data.c_str());
	LMsg.TimeStamp(Data);
	SetDlgItemText(IDC_TIME, Data.c_str());
	LMsg.SysByte(Data);
	SetDlgItemText(IDC_SYSBYTE, Data.c_str());
	char Buf[25];
	Data = _itoa(m_CurrMessage, Buf, 10);
	Data += "/";
	Data += _itoa(m_Recordcnt, Buf, 10);
	SetDlgItemText(IDC_RECORDCNT, Data.c_str());
}

void COnlineLogsDlg::SetCol(int MsgType) {
	switch (MsgType) {
	case SECS_MESSAGE:
	{
		m_Function.SetWhite();
		m_Time.SetWhite();
		m_Wbit.SetWhite();
		m_Sysbyte.SetWhite();
		m_Stream.SetWhite();
		m_Function.SetWhite();
		m_RecordCount.SetWhite();
		m_Type.SetWhite();
		m_System.SetWhite();
		m_Msg.SetWhite();
		m_CodeKey.SetWhite();
		m_ListData.SetTextColor(RGB(255, 255, 255));
	}
	break;
	case INFO_MESSAGE:
	{
		m_Function.SetGreen();
		m_Time.SetGreen();
		m_Wbit.SetGreen();
		m_Sysbyte.SetGreen();
		m_Stream.SetGreen();
		m_Function.SetGreen();
		m_RecordCount.SetGreen();
		m_Type.SetGreen();
		m_System.SetGreen();
		m_Msg.SetGreen();
		m_CodeKey.SetGreen();
		m_ListData.SetTextColor(RGB(0, 255, 0));
	}
	break;
	case WARNING_MESSAGE:
	{
		m_Function.SetOrange();
		m_Time.SetOrange();
		m_Wbit.SetOrange();
		m_Sysbyte.SetOrange();
		m_Stream.SetOrange();
		m_Function.SetOrange();
		m_RecordCount.SetOrange();
		m_Type.SetOrange();
		m_System.SetOrange();
		m_Msg.SetOrange();
		m_CodeKey.SetOrange();
		m_ListData.SetTextColor(RGB(255, 150, 50));
	}
	break;
	case ERROR_MESSAGE:
	{
		m_Function.SetPink();
		m_Time.SetPink();
		m_Wbit.SetPink();
		m_Sysbyte.SetPink();
		m_Stream.SetPink();
		m_Function.SetPink();
		m_RecordCount.SetPink();
		m_Type.SetPink();
		m_System.SetPink();
		m_Msg.SetPink();
		m_CodeKey.SetPink();
		m_ListData.SetTextColor(RGB(255, 125, 125));
	}
	break;
	case FATAL_ERROR_MESSAGE:
	{
		m_Function.SetRed();
		m_Time.SetRed();
		m_Wbit.SetRed();
		m_Sysbyte.SetRed();
		m_Stream.SetRed();
		m_Function.SetRed();
		m_RecordCount.SetRed();
		m_Type.SetRed();
		m_System.SetRed();
		m_Msg.SetRed();
		m_CodeKey.SetRed();
		m_ListData.SetTextColor(RGB(255, 0, 0));
	}
	break;
	default:
	{
		m_Function.SetGreen();
		m_Time.SetGreen();
		m_Wbit.SetGreen();
		m_Sysbyte.SetGreen();
		m_Stream.SetGreen();
		m_Function.SetGreen();
		m_RecordCount.SetGreen();
		m_Type.SetGreen();
		m_System.SetGreen();
		m_ListData.SetTextColor(RGB(0, 255, 0));
	}
	break;
	}
}

void COnlineLogsDlg::WriteDataToList(CListCtrl& List, const  char* Data) {
	List.DeleteAllItems();
	int nLen = strlen(Data);
	string Line;
	Line.reserve(nLen);
	int Row = 0;
	for (int i = 0; i < nLen; i++) {
		if (Data[i] == '\t') {
			Line += "    ";
			continue;
		}
		if (Data[i] == '\n') {
			List.InsertItem(Row++, Line.c_str());
			Line.erase();
			continue;
		}
		Line.append(1, Data[i]);
	}
	List.InsertItem(Row++, Line.c_str());
}

BOOL COnlineLogsDlg::OnInitDialog() {
	CDialog::OnInitDialog();

	m_ListData.SetBkColor(RGB(0, 0, 0));
	m_ListData.SetTextBkColor(RGB(0, 0, 0));

	m_Time.SetCtrlFont();
	m_Wbit.SetCtrlFont();
	m_Sysbyte.SetCtrlFont();
	m_Stream.SetCtrlFont();
	m_Function.SetCtrlFont();
	m_RecordCount.SetCtrlFont();
	m_Msg.SetCtrlFont();
	m_Type.SetCtrlFont();
	m_System.SetCtrlFont();
	m_CodeKey.SetCtrlFont();
	SYSTEMTIME TM;
	::GetLocalTime(&TM);
	// SPIN CONTROLL
	m_SHour_Spin.SetRange(0, 23);
	m_SHour_Spin.SetPos(0);

	m_EHour_Spin.SetRange(0, 23);
	m_EHour_Spin.SetPos(TM.wHour);

	m_SMin_Spin.SetRange(0, 59);
	m_SMin_Spin.SetPos(0);

	m_EMin_Spin.SetRange(0, 59);
	m_EMin_Spin.SetPos(TM.wMinute + 1);

	// Searching Button Creation
	HRGN cr;
	HDC hDC = GetDC()->GetSafeHdc();

	BeginPath(hDC);
	MoveToEx(hDC, 0, 20, NULL);
	LineTo(hDC, 50, 0);
	LineTo(hDC, 100, 20);
	LineTo(hDC, 100, 30);
	LineTo(hDC, 50, 50);
	LineTo(hDC, 0, 30);
	LineTo(hDC, 0, 20);
	EndPath(hDC);
	cr = CreateRectRgn(0, 0, 63, 63);
	cr = PathToRegion(hDC);
	m_LogSearch.Create("Search", WS_CHILD | WS_VISIBLE, CPoint(250, 200), cr, this,
		IDC_BUT_LOGSEARCH, 1, RGB(255, 139, 83), // 254, 247, 211),
		RGB(211, 247, 254), RGB(211, 247, 254), GetSysColor(COLOR_BTNFACE));

	BeginPath(hDC);
	MoveToEx(hDC, 0, 0, NULL);
	MoveToEx(hDC, 90, 0, NULL);
	LineTo(hDC, 90, 40);
	LineTo(hDC, 50, 20);
	LineTo(hDC, 90, 0);
	EndPath(hDC);
	cr = CreateRectRgn(0, 0, 63, 63);
	cr = PathToRegion(hDC);
	COleDateTime nStart(2000, 1, 1, 0, 1, 1);
	COleDateTime nEnd(2020, 12, 31, 23, 59, 59);
	m_STimePicker.SetRange(&nStart, &nEnd);
	m_ETimePicker.SetRange(&nStart, &nEnd);
	m_ETimePicker.SetTime(&TM);
	m_LogFiles.DeleteAllItems();
	char* dwCjName[] =
	{
		("  NO"), ("  Files")
	};
	int nSize[] = { 25, 450 };
	m_LogFiles.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	LV_COLUMN nListColumn;

	int i;
	for (i = 0; i < 2; i++) {
		nListColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		nListColumn.fmt = LVCFMT_LEFT;
		nListColumn.cx = nSize[i];
		nListColumn.iSubItem = 0;
		nListColumn.pszText = dwCjName[i];
		m_LogFiles.InsertColumn(i, &nListColumn);
	}

	char* dwName2[] =
	{
		("  Data ")
	};
	int nSize2[] = { 550 };

	m_ListData.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	for (i = 0; i < 1; i++) {
		nListColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		nListColumn.fmt = LVCFMT_LEFT;
		nListColumn.cx = nSize2[i];
		nListColumn.iSubItem = 0;
		nListColumn.pszText = dwName2[i];
		m_ListData.InsertColumn(i, &nListColumn);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void COnlineLogsDlg::PostNcDestroy() {
	CDialog::PostNcDestroy();
	delete this;
}