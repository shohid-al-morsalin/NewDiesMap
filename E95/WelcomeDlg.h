#pragma once

// CWelcomeDlg dialog

class CWelcomeDlg : public CDialog {
	DECLARE_DYNAMIC(CWelcomeDlg)

public:
	CWelcomeDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CWelcomeDlg();

	// Dialog Data
	enum { IDD = IDD_WELCOMEDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
