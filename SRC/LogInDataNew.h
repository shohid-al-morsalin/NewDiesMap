// LogInData.h: interface for the CLogInDataNew class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

class CLogInDataNew : public CObject {
public:
	enum ACCLVL {
		READDATA, RUNRCP, CREATERCP, CHGSETTING, FULLACC = CHGSETTING	// 20230512 - FULLACC for CHGSETTING [UMC Customer]
	};
	enum ACCSTATUS {
		LOGIN, LOGOUT
	};
	typedef struct {
		CWordArray   m_lEvelArry;
		CStringArray m_lOgarry;
		CStringArray m_pAsswordArry;
		CStringArray m_Dir;
	}LogArryData;

	LogArryData m_LogArry;
public:
	BOOL check_logon(char* buf, const CString& logid, const CString& pass) const;
	BOOL get_superid(char* buf, CString& user, CString& pas) const;
	BOOL check_superpass(const CString& user, const CString& password) const;
	void set_newfile();
	BOOL check_logging(const CString& logid, const CString& password, short& AuthenLevel);
	void FileClearAll();
	void ClearBuffer();
	void get_logdata();
	BOOL check_logging(short access, const CString& logid, const CString& password);
	void/*CString*/ DeCoding(CString& str)const;
	void/*CString*/ EnCoding(CString& str);
	CFile m_File;
	BOOL Add_Password(short access, CString& logid, CString& password, CString& dir);
	short m_AccessMode;
	CString workdirectory;
	CString m_Password;
	CString m_Loginid;
	CLogInDataNew();
	virtual ~CLogInDataNew();
};