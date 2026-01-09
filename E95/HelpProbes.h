#pragma once

class CHelpProbes : public CDialog {
public:
	void SetItem(short Prb, CString Desc);
	CHelpProbes(CWnd* pParent = NULL);   // standard constructor

	enum { IDD = IDD_PROBEHELP };
	CListCtrl	m_cList;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
};
