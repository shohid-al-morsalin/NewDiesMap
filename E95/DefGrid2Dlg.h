#pragma once

class CDefGrid2Dlg : public CDialog {
public:
	CDefGrid2Dlg(CWnd* pParent = NULL);   // standard constructor

	enum { IDD = IDD_DEFINEGRID2 };
	float	m_fX0;
	float	m_fY0;
	int		m_NX;
	int		m_NY;
	float	m_fSx;
	float	m_fSy;
	BOOL	m_DeletePrev;
	BOOL	m_bIgnore;

	BOOL bClearData;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:

	DECLARE_MESSAGE_MAP()
};
