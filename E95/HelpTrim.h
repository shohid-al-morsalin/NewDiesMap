#pragma once

class CHelpTrim : public CDialog {
public:
	CHelpTrim(CWnd* pParent = NULL);   // standard constructor

	enum { IDD = IDD_HELPTRIM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
	DECLARE_MESSAGE_MAP()
};
