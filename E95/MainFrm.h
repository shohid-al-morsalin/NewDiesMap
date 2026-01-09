// MainFrm.h : interface of the CMainFrame class
//

#pragma once

#include "PersistFrames.h"

struct SRptItem {
	int ID;
	CString Desc;

	SRptItem() {
		ID = 0;
		Desc = "";
	}
	void Set(int id, CString desc) {
		ID = id;
		Desc = desc;
	}
};

class ST_SplitterWnd;

class CMainFrame : public CPersistSDIFrame {
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

	// Attributes
public:
	int RptItmCnt;
	SRptItem RptList[128];

	// Operations
public:

	// Overrides
public:
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL DestroyWindow();
protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
//	CStatusBar  m_wndStatusBar;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnToolSwitchinfoview();
	afx_msg void OnOperationSendncas();
	afx_msg void OnOperationSendxfer();
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg void OnAppExit();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClose();
	afx_msg void OnSimuFfuFail();
	afx_msg void OnAltdel();
	afx_msg void OnDestroy();
	afx_msg void OnGenceidlist();
	afx_msg void OnGenecidlist();
	afx_msg void OnGensvidlist();
	afx_msg void OnAlarmAccessviolation();
	afx_msg void OnGenalrmlist();
	afx_msg void OnOperationListCaobjs();
	afx_msg void OnOperationListLpobjs();
	afx_msg void OnGenceidparfile();
	afx_msg void OnGenprogram();
	afx_msg void OnIssuealarm();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	ST_SplitterWnd* m_pSplitterWnd1;
	ST_SplitterWnd* m_pSplitterWnd2;
	int m_nViewNo[16];

	void SwitchPane(short PaneNum);
protected:

public:
	CString GenRemoteCmd(short rcmd);
	CString MakeReport14(short tab);
	CString MakeReport15(short tab);
	CString MakeReport16(short tab);
	CString MakeReport17(short tab);
	CString MakeReport24(short tab);
	CString MakeReport25(short tab);
	CString MakeReport900(short tab);
	CString MakeReport850(short tab);
	CString MakeReport800(short tab);
	CString MakeReport750(short tab);
	CString MakeReport700(short tab);
	CString MakeReport600(short tab);
	CString MakeReport520(short tab);
	CString MakeReport510(short tab);
	CString GetReportDesc(short RptID);
	CString LinkEvent(UINT idx, int RptID[], short tab);
	CString LinkEventReport();
	CString MakeReport310(short tab);
	CString MakeReport300(short tab);
	CString MakeReport38(short tab);
	CString MakeReport36(short tab);
	CString MakeReport34(short tab);
	CString MakeReport30(short tab);
	CString MakeReport20(short tab);
	CString MakeReport12(short tab);
	CString MakeReport11(short tab);
	CString MakeReport10(short tab);
	CString DefineReport();
	CString AllStatusVarNameListReq();
	CString EquipStatusReq();
	CString SVIDReq(UINT SvIdx, short tab);
	CString AllEquipStatusReq();
	CString PJDequeue(CString PJobID);
	CString DeleteAllPJob();
	CString DeleteAllCJob();
	CString DeleteObject(CString JobID);
	CString CJobAction(CString CJobID, short act);
	CString AbortCJob(CString CJob);
	CString StopCJob(CString CJob);
	CString CancelCJob(CString CJob);
	CString ResumeCJob(CString CJob);
	CString PauseCJob(CString CJob);
	CString StartCJob(CString CJobID);
	CString CancelCarrier(CString CarrierID);
	CString CancelCarrieerAtPort(short PortID);
	CString ProcessCtrlSpec(CString PRJob);
	CString CreateCJob(CString CJobID, CString CarrierID);
	CString CreatePRJobMulti(CString CarrierID, CString SM1, CString SM2);
	CString CreatePRJob(CString PRJobID, CString CarrierID, CString SM, CString Recipe, BOOL bStart);
	CString PWCSM(CString CarrierID, short PortID, CString SM);
	CString PWCID(CString CarrierID, short PortID, CString LotID, CString WaferID);
	void Log(CString msg, short lvl = 5);
	//	void OnUM128MsgInQue(WPARAM wP, LPARAM lP);
	//	void OnUM900MsgInQue(WPARAM wP, LPARAM lP);
};
