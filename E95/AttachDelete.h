#pragma once

class CAttachDelete : public CDialog {
public:
	CAttachDelete(CWnd* pParent = NULL);   // standard constructor

	short option;

	enum { IDD = IDD_ATTACHDELETE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:

	afx_msg void OnDelete();
	DECLARE_MESSAGE_MAP()
};
