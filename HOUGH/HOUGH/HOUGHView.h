
// HOUGHView.h : interface of the CHOUGHView class
//

#pragma once


class CHOUGHView : public CFormView
{
protected: // create from serialization only
	CHOUGHView() noexcept;
	DECLARE_DYNCREATE(CHOUGHView)

public:
#ifdef AFX_DESIGN_TIME
	enum{ IDD = IDD_HOUGH_FORM };
#endif

// Attributes
public:
	CHOUGHDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct

// Implementation
public:
	virtual ~CHOUGHView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in HOUGHView.cpp
inline CHOUGHDoc* CHOUGHView::GetDocument() const
   { return reinterpret_cast<CHOUGHDoc*>(m_pDocument); }
#endif

