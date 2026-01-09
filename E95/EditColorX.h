#if !defined(AFX_EDITCOLOR_H__2C3F480B_83DE_410B_9493_F9987DB9360E__INCLUDED_)
#define AFX_EDITCOLOR_H__2C3F480B_83DE_410B_9493_F9987DB9360E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditColor.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditColor dialog

class CEditColor : public CDialog
{
// Construction
public:
	CEditColor(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEditColor)
	enum { IDD = IDD_EDITCOLOR };
	float	m_Blue;
	float	m_Green;
	float	m_Red;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditColor)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEditColor)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITCOLOR_H__2C3F480B_83DE_410B_9493_F9987DB9360E__INCLUDED_)
