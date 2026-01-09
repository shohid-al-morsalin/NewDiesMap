#pragma once

#include <string>
#include <vector>

/////////////////////////////////////////////////////////////////////////////
// CVarDisp dialog

class CVarDisp : public CDialog {
	// Construction
public:
	static std::string typestr(short type);
	static std::string tostring(short val);
	// 	static int get_ecidx(short ECID);
	static void MakeECIDReport(short ECID, std::string& data);
	void GetECIDData(std::string& data);
	static void MakeCEIDReport(short CEID, std::string& data);
	std::string m_VarName;
	static void MakeSVIDReport(short SVID, std::string& data);
	void GetCEIDData(std::string& data);
	void GetSVIDData(std::string& str);
	CVarDisp(CWnd* pParent = NULL);   // standard constructor
	short m_VarNo;
	short m_Type;
	// Dialog Data
		//{{AFX_DATA(CVarDisp)
	enum { IDD = IDD_ONLINE_VAR_DISP };
	CEdit	m_VarData;
	//}}AFX_DATA
	void WriteDataToList(CListCtrl& List, char* data);

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CVarDisp)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CVarDisp)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void PostReq();
};