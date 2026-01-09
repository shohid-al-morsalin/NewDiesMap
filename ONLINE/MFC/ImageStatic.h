#pragma once
// ImageStatic.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CImageStatic window

class CImageStatic : public CStatic {
public:
	int m_nImageID;
	HBITMAP hBmp;
	HICON hIcon;
	// Construction
public:
	CImageStatic();

	// Attributes
public:

	// Operations
public:

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CImageStatic)
		//}}AFX_VIRTUAL

	// Implementation
public:
	void LoadImage_Icon(int nImageID = -1);
	void LoadImage_Bmp(int nImageID = -1);

	void UnLoadImage_Icon();
	void UnLoadImage_Bmp();

	virtual ~CImageStatic();

	// Generated message map functions
protected:
	//{{AFX_MSG(CImageStatic)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
