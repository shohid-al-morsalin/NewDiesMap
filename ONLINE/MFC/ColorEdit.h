#pragma once

#include "ONLINE/general_utils.h"

class CColorEdit : public CEdit {
	EditCol EditColor;
public:
	CFont m_fBold;

public:
	CColorEdit();

public:
	void SetGreen();
	void SetOrange();
	void SetRed();
	void SetBlue();
	void SetWhite();
	void SetPink();
	void SetCtrlFont();
	virtual ~CColorEdit();

protected:
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()
};
