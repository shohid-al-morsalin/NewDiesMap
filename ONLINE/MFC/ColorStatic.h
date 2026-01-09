#pragma once

class CColorStatic : public CStatic {
public:
	CColorStatic();

public:
	CFont m_fBold;

public:
	void Setting_Font();
	virtual ~CColorStatic();

protected:
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnInitMenu(CMenu* pMenu);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()
};
