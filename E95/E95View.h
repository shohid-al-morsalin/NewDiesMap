// E95View.h : interface of the CE95View class
//

#pragma once

class CE95Doc;
class CLPEventDispatcher;

class CE95View : public CFormView {
protected: // create from serialization only
	CE95View();
	DECLARE_DYNCREATE(CE95View)

public:
	enum { IDD = IDD_E95_FORM };

	// Attributes
public:
	CE95Doc* GetDocument() const;

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
	virtual ~CE95View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in E95View.cpp
inline CE95Doc* CE95View::GetDocument() const {
	return reinterpret_cast<CE95Doc*>(m_pDocument);
}
#endif
