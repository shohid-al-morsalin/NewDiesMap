// LogInData.cpp: implementation of the CLogInDataNewNew class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LogInDataNew.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLogInDataNew::CLogInDataNew() {}

CLogInDataNew::~CLogInDataNew() {}

BOOL CLogInDataNew::Add_Password(short access, CString& logid, CString& password, CString& direc) {
	CString dd("");
	CString data("");
	char drive[_MAX_DRIVE];
	char dir[256];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	char inifile[_MAX_PATH];
	CString pFile("kang"), strDirectory;

	GetModuleFileName(NULL, inifile, 255);
	// Get the directory

	_splitpath(inifile, drive, dir, fname, ext);
	strDirectory += drive;
	strDirectory += dir;

	GetWindowsDirectory(inifile, _MAX_PATH);
	ASSERT(strlen(inifile) > 0);
	strcat(inifile, "\\usr.par");

	if (!check_logging(access, logid, password)) {   //"c:\\1.txt"
		// User does not exists
		m_File.Open(inifile, CFile::modeReadWrite | CFile::typeBinary | CFile::shareDenyNone);
		m_File.SeekToEnd();
		dd.Format("%c", (char)access);
		data = dd + "," + logid + "," + password + "," + direc;
		/*data =*/ EnCoding(data);
		m_File.Write(data, data.GetLength());
		m_File.Write("\n", 1);
		m_File.Close();
		// Try to create a dirctory
		if (!::CreateDirectory(strDirectory + "\\data\\" + logid, NULL)) {
			//TRACE("%s : Directory Exists\n",logid);
		}
		return TRUE;
	}
	else {
		//TRACE("Already Exist %s \n",logid);
		return FALSE;
	}
}

void/*CString*/ CLogInDataNew::EnCoding(CString& str) {
	for (int i = 0; i < str.GetLength(); i++) {
		str.SetAt(i, (char)~str.GetAt(i));
	}
	return /*str*/;
}

void/*CString*/ CLogInDataNew::DeCoding(CString& str)const {
	for (int i = 0; i < str.GetLength() - 1; i++) {
		str.SetAt(i, (char)~str.GetAt(i));
	}
	return /*str*/;
}

BOOL CLogInDataNew::check_logging(short access, const CString& logid, const CString& password) {
	access;
	CString dd("");
	CString data("");
	CString deco("");
	char buf[101];

	char inifile[_MAX_PATH];

	GetWindowsDirectory(inifile, _MAX_PATH);
	ASSERT(strlen(inifile) > 0);
	strcat(inifile, "\\usr.par");
	memset(buf, 0, sizeof buf);

	FILE* f;
	if ((f = fopen(inifile, "r")) == NULL) {
		// Actually this should not happen in any case !
		AfxMessageBox("OPEN FAIL");
		return FALSE;
	}
	else {
		while (fgets(buf, 100, f)) {
			if (check_logon(buf, logid, password)) {
				fclose(f);
				return TRUE;
			}
			memset(buf, 0, sizeof buf);
		}
		fclose(f);
	}
	return FALSE;
}

void CLogInDataNew::get_logdata() {
	CString dd("");
	char buf[101], nBuf[101];
	int spoint = 0, commacnt = 0;

	char inifile[_MAX_PATH];

	GetWindowsDirectory(inifile, _MAX_PATH);
	ASSERT(strlen(inifile) > 0);
	strcat(inifile, "\\user.par");

	memset(nBuf, 0, sizeof buf);
	FILE* f;
	ClearBuffer();

	if ((f = fopen(inifile, "r")) == NULL) {
		AfxMessageBox("OPEN FAIL");
		return; 
	}
	else {
		while (fgets(nBuf, 100, f)) {
			dd.Format("%s", nBuf);
			/*dd = */DeCoding(dd);
			memset(buf, 0, sizeof buf);
			memcpy(buf, dd.operator const char* (), dd.GetLength());
			spoint = commacnt = 0;
			for (unsigned int i = 0; i < strlen(buf); i++) {
				if (buf[i] == ',') {
					switch (commacnt) {
					case 0:
						// OPERATOR LEVEL
						memset(nBuf, 0, sizeof nBuf);
						memcpy(nBuf, buf, i);
						m_LogArry.m_lEvelArry.Add((BYTE)nBuf[0]);
						spoint = i + 1;
						commacnt++;
						break;
					case 1:
						// OPERATOR LOGID
						memset(nBuf, 0, sizeof nBuf);
						memcpy(nBuf, &buf[spoint], i - spoint);
						m_LogArry.m_lOgarry.Add(nBuf);
						spoint = i + 1;
						commacnt++;
						break;
					case 2:
						memset(nBuf, 0, sizeof nBuf);
						memcpy(nBuf, &buf[spoint], i - spoint);
						m_LogArry.m_pAsswordArry.Add(nBuf);
						spoint = i + 1;
						// OPERATOR PASSWORD
						memset(nBuf, 0, sizeof nBuf);
						memcpy(nBuf, &buf[spoint], (strlen(buf) - spoint) - 1);
						m_LogArry.m_Dir.Add(nBuf);
						break;
					}
				}
			}
			memset(nBuf, 0, sizeof nBuf);
		}
		fclose(f);
	}
}

void CLogInDataNew::ClearBuffer() {
	m_LogArry.m_lEvelArry.RemoveAll();
	m_LogArry.m_lOgarry.RemoveAll();
	m_LogArry.m_pAsswordArry.RemoveAll();
	m_LogArry.m_Dir.RemoveAll();
}

void CLogInDataNew::FileClearAll() {
	char inifile[_MAX_PATH];
	CString pFile("kang");

	GetWindowsDirectory(inifile, _MAX_PATH);
	ASSERT(strlen(inifile) > 0);
	strcat(inifile, "\\usr.par");

	m_File.Open(inifile, CFile::modeCreate | CFile::shareDenyNone);
	m_File.Close();
}

BOOL CLogInDataNew::check_logging(const CString& logid, const CString& password, short& AuthenLevel) {
	get_logdata();

	CString sLogon;
	CString sPassword;
	int cnt = m_LogArry.m_lEvelArry.GetSize();
	for (int i = 0; i < cnt; i++) {
		sLogon = m_LogArry.m_lOgarry.GetAt(i);
		if (sLogon == logid) {
			sPassword = m_LogArry.m_pAsswordArry.GetAt(i);
			if (sPassword == password) {
				AuthenLevel = m_LogArry.m_lEvelArry.GetAt(i);
				return TRUE;
			}
		}
	}
	return FALSE;
}

void CLogInDataNew::set_newfile() {
	CString dd("");
	CString data("");
	char inifile[_MAX_PATH];

	GetWindowsDirectory(inifile, _MAX_PATH);
	ASSERT(strlen(inifile) > 0);
	strcat(inifile, "\\usr.par");

	m_File.Open(inifile, CFile::modeCreate | CFile::modeReadWrite | CFile::typeBinary | CFile::shareDenyNone);
	dd.Format("%c", 1);
	data = dd + "," + "FSM" + "," + "123" + "," + "C:\\";
	/*data = */EnCoding(data);
	m_File.Write(data, data.GetLength());
	m_File.Write("\n", 1);
	m_File.Close();
}

BOOL CLogInDataNew::check_superpass(const CString& user, const CString& password) const {
	CString nUser(""), nPass("");

	char inifile[_MAX_PATH];
	char buf[300];

	GetWindowsDirectory(inifile, _MAX_PATH);
	ASSERT(strlen(inifile) > 0);
	strcat(inifile, "\\usr.par");

	memset(buf, 0, sizeof buf);

	FILE* f;

	if ((f = fopen(inifile, "r")) == NULL) {
		AfxMessageBox("OPEN FAIL");
		return FALSE;
	}
	else {
		while (fgets(buf, 100, f)) {
			if (get_superid(buf, nUser, nPass)) {
				if (user == nUser && password == nPass) {
					return TRUE;
				}
			}
			memset(buf, 0, sizeof buf);
		}
		fclose(f);
	}
	return FALSE;
}

BOOL CLogInDataNew::get_superid(char* buf, CString& user, CString& pas) const {
	CString cBuf("");
	int commacnt = 0, spoint = 0;
	char nBuf[256];

	cBuf.Format("%s", buf);
	/*cBuf =*/ DeCoding(cBuf);
	memset(buf, 0, strlen(buf));
	memcpy(buf, cBuf.operator const char* (), cBuf.GetLength());
	for (unsigned int i = 0; i < strlen(buf); i++) {
		if (buf[i] == ',') {
			switch (commacnt) {
			case 0:
				// OPERATOR LEVEL
				memset(nBuf, 0, sizeof nBuf);
				memcpy(nBuf, buf, i);
				spoint = i + 1;
				commacnt++;
				if (nBuf[0] != 1)
					return FALSE;
				break;
			case 1:
				// OPERATOR LOGID
				memset(nBuf, 0, sizeof nBuf);
				memcpy(nBuf, &buf[spoint], i - spoint);
				spoint = i + 1;
				commacnt++;
				user.Format("%s", nBuf);
				break;

			case 2:
				memset(nBuf, 0, sizeof nBuf);
				memcpy(nBuf, &buf[spoint], i - spoint);
				spoint = i + 1;
				pas.Format("%s", nBuf);
				// OPERATOR PASSWORD
				memset(nBuf, 0, sizeof nBuf);
				memcpy(nBuf, &buf[spoint], (strlen(buf) - spoint) - 1);
				break;
			}
		}
	}
	return TRUE;
}
// Returens FALSE if this line does not contain the user or the password does not match !
BOOL CLogInDataNew::check_logon(char* buf, const CString& logid, const CString& pass) const {
	CString cBuf(""), csBuf("");
	int commacnt = 0, spoint = 0;
	char nBuf[256];

	cBuf.Format("%s", buf);
	/*cBuf = */DeCoding(cBuf);
	memset(buf, 0, strlen(buf));
	memcpy(buf, cBuf.operator const char* (), cBuf.GetLength());
	for (unsigned int i = 0; i < strlen(buf); i++) {
		if (buf[i] == ',') {
			switch (commacnt) {
			case 0:
				// OPERATOR LEVEL
				memset(nBuf, 0, sizeof nBuf);
				memcpy(nBuf, buf, i);
				spoint = i + 1;
				commacnt++;
				break;
			case 1:
				// OPERATOR LOGID
				memset(nBuf, 0, sizeof nBuf);
				memcpy(nBuf, &buf[spoint], i - spoint);
				spoint = i + 1;
				commacnt++;
				csBuf.Format("%s", nBuf);
				if (csBuf != logid) {
					//TRACE("Copmparing Logid %s with %s : FALSE \n",csBuf,logid);
					return FALSE;
				}
				break;

			case 2:
				memset(nBuf, 0, sizeof nBuf);
				memcpy(nBuf, &buf[spoint], i - spoint);
				spoint = i + 1;
				csBuf.Format("%s", nBuf);
				if (csBuf != pass) {
					//TRACE("Copmparing Pw %s with %s : FALSE\n",csBuf,pass);
					return FALSE;
				}

				memset(nBuf, 0, sizeof nBuf);
				memcpy(nBuf, &buf[spoint], (strlen(buf) - spoint) - 1);
				break;
			}
		}
	}
	//TRACE("Copmparing %s with %s : TRUE\n",cBuf,logid);
	return TRUE;
}