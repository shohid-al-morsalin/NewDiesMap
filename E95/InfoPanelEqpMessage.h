#pragma once
// InfoPanelEqpMessage.h : header file
//

#include "resource.h"

#include "ResizableFormView.h"
#include "xtabctrl.h"

/////////////////////////////////////////////////////////////////////////////
// CInfoPanelEqpMessage form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

struct SEQPMSG {
	CString msg;
	short lvl;
};

class CInfoPanelEqpMessage : public CResizableFormView {
protected:
	CInfoPanelEqpMessage();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CInfoPanelEqpMessage)

	// Form Data
public:
	//{{AFX_DATA(CInfoPanelEqpMessage)
	enum { IDD = IDD_INFOPANELEQPMESSAGE };
	CListBox	m_List;
	//}}AFX_DATA

// Attributes
public:
	std::vector <SEQPMSG> m_Msgs;

	// Operations
public:
	// 	CString GetTimeStamp();
	void DspMessage(CString msg, short lvl = 5);
	void Message(CString msg, short lvl = 5);
	void Log(CString msg, short lvl = 5);

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CInfoPanelEqpMessage)
public:
	virtual void OnInitialUpdate();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CInfoPanelEqpMessage();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CInfoPanelEqpMessage)
	afx_msg void OnLocalMsg();
	//}}AFX_MSG
	LRESULT OnSTSelected(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
