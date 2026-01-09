#include "stdafx.h"

// #include <direct.h>
// #include "CLogMsgItem.h"
#include "resource.h"
#include "IPC/SMHdr.h"
#include "SRC/DOSUtil.h"
#include "SRC/FolderUtils.h"

#include "LogSvr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;

#endif
/////////////////////////////////////////////////////////////////////////////
// CLogSvr
//
// Service Provided via SM.cmd (SMHdr)
// CTLOGMSG
// SM.text: message text
// SM.sub[0]: log level
// SM.sub[1]: Sender's ID for filtering purposes
// Operation
// Message received via this interface will be store in LogMsgList.
// This is a fix buffer (1024) list of CLogMsgItem. When the buffer
// is full, the oldest entry will be deleted.
// Message will be save to disk as soon as it arrived in text format.
// New text file will be created every 2K messages

CLogSvr::CLogSvr() {
	hWnd = NULL;
	type = 2;
	MsgCount = 0;
	SvrName = "LogE95";
	LogFileName = MakeNewLogFileName(".LOG");
	ErrFileName = MakeNewLogFileName(".ERR");
	WarnFileName = MakeNewLogFileName(".WRN");
	NoticeFileName = MakeNewLogFileName(".NOT");

	FILE* fp;
	fp = fopen(NoticeFileName, "ab");
	if (fp) {
		CLogMsgItem pMsgItem;
		pMsgItem.AddTimeStamp();
		pMsgItem.Source = "LogSvr";
		pMsgItem.Destination = "";
		pMsgItem.Level = 5;
		pMsgItem.ID = 0;
		pMsgItem.MessageText = "Application start";
		pMsgItem.Save(fp);
		fclose(fp);
	}
}

CString CLogSvr::MakeNewLogFileName(CString ext) {
	CString dir;
	CString FileName = "";

	DosUtil.GetProgramDir(FileName);
	FileName += "LOG";

	CString month = CTime::GetCurrentTime().Format("%Y%m");
	FileName += "\\" + month;
	CFolderUtils::CreateFolder(FileName);

	CString name;
	name.Format("L%s%s", CTime::GetCurrentTime().Format("%H%M%S"), ext);
	FileName += "\\" + name;

	return FileName;
}

int CLogSvr::ChgDirec(CString dir) {
	if (_chdir(dir) == -1) {
		if (_mkdir(dir) != -1) {
			if (_chdir(dir) != -1) {
				return 0;
			}
			else {
				return -1;
			}
		}
		else {
			return -1;
		}
	}
	return 0;
}

CLogSvr::~CLogSvr() {
	CleanUp();
}

BOOL CLogSvr::InitInstance() {
	CWinThread* pThread;
	pThread = AfxGetThread();
	pThread->SetThreadPriority(THREAD_PRIORITY_BELOW_NORMAL);
	return CSvrThread::InitInstance();
}

BEGIN_MESSAGE_MAP(CLogSvr, CSvrThread)
	//{{AFX_MSG_MAP(CLogSvr)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLogSvr message handlers

BOOL CLogSvr::ProcessCmd(CSMHdr& SM) {
	if (SM.cmd == CSMHdr::CTLOGMSG) {
		CLogMsgItem* p = new CLogMsgItem;
		p->Level = SM.sub[0];
		p->ID = SM.sub[1];
		// Crack text field
		{
			int m, n;
			CString msg = SM.text;
			m = msg.Find('\t');
			if (m < 1) {
				return FALSE; // Parser error
			}
			p->DateTime = msg.Left(m);
			m++;
			n = msg.Find('\t', m);
			if (n < 1) {
				return FALSE; // Parser error
			}
			p->Source = msg.Mid(m, n - m);
			m = n + 1;
			n = msg.Find('\t', m);
			if (n < 1) {
				return FALSE; // Parser error
			}
			p->Destination = msg.Mid(m, n - m);
			m = n + 1;
			p->MessageText = msg.Mid(m);
		}
		Save(p);
		LogMsgList.Set(p);
		if (hWnd) {
			::PostMessage(hWnd, ID_LOG_REFRESH, 0, (LPARAM)p);
		}
	}
	return TRUE;
}

void CLogSvr::Save(CLogMsgItem* pMsgItem) {
	MsgCount++;
	if (MsgCount > MAX_LOGFILEENTRY) {
		MsgCount = 0;
		LogFileName = MakeNewLogFileName(".LOG");
		ErrFileName = MakeNewLogFileName(".ERR");
		WarnFileName = MakeNewLogFileName(".WRN");
		NoticeFileName = MakeNewLogFileName(".NOT");
	}
	FILE* fp;
	if (pMsgItem->Level == 0) {
		fp = fopen(ErrFileName, "ab");
		if (fp) {
			pMsgItem->Save(fp);
			fclose(fp);
		}
	}
	else if (pMsgItem->Level == 1) {
		fp = fopen(WarnFileName, "ab");
		if (fp) {
			pMsgItem->Save(fp);
			fclose(fp);
		}
	}
	else if (pMsgItem->Level == 2) {
		fp = fopen(NoticeFileName, "ab");
		if (fp) {
			pMsgItem->Save(fp);
			fclose(fp);
		}
	}

	fp = fopen(LogFileName, "ab");
	if (fp) {
		pMsgItem->Save(fp);
		fclose(fp);
	}
}

void CLogSvr::CleanUp() {
	LogMsgList.Clear();
}