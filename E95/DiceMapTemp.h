#pragma once

#include "DiceView.h"
// DiceMapTemp dialog

class DiceMapTemp : public CDialog
{
	DECLARE_DYNAMIC(DiceMapTemp)

public:
	DiceMapTemp(CWnd* pParent = nullptr);   // standard constructor
	virtual ~DiceMapTemp();

	CDiceView	m_cDiceMap; 
	CString m_strSavePath;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DICEMAP_TEMP };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	DECLARE_MESSAGE_MAP()
};
