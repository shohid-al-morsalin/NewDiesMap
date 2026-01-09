#pragma once
// InfoPanelHostMsg.h : header file
//

#include "ResizableFormView.h"
#include "xtabctrl.h"

/////////////////////////////////////////////////////////////////////////////
// CInfoPanelHostMsg form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CHostMsg {
public:
	CString DateTime;
	CString Msg;
};

class CHostMsgList {
	CPtrList m_MsgList;
public:
	void ClearList();
	void Remove(short n);
	CHostMsgList();
	~CHostMsgList();
	// 	CString GetTimeStamp();
	CHostMsg* Get(short idx);
	void Add(CString msg);
	short GetCount() { return m_MsgList.GetCount(); }
};

class CInfoPanelHostMsg : public CResizableFormView {
protected:
	CInfoPanelHostMsg();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CInfoPanelHostMsg)

	// Form Data
public:
	short idx;
	CString m_Message;
	CHostMsgList m_HostMsg;

	//{{AFX_DATA(CInfoPanelHostMsg)
	enum { IDD = IDD_INFOHOSTMSG };
	CStatic	m_cMsgCount;
	CEdit	m_cMessage;
	//}}AFX_DATA

// Attributes
public:

	// Operations
public:
	void Update();
	void Message(CString msg);

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CInfoPanelHostMsg)
public:
	virtual void OnInitialUpdate();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CInfoPanelHostMsg();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CInfoPanelHostMsg)
	afx_msg void OnAccept();
	afx_msg void OnUp();
	afx_msg void OnDown();
	afx_msg LRESULT OnSTSelected(WPARAM wParam, LPARAM lParam);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	LRESULT OnHostMessage(WPARAM wP, LPARAM lP);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
