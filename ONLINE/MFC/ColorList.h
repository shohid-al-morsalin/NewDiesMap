#pragma once

class CColorList : public CListBox {
public:
	CColorList();

public:
	CFont m_fBold;

public:
	void Add_String(CString data);
	void SetCtrlFont();
	virtual ~CColorList();

protected:
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	DECLARE_MESSAGE_MAP()
};
