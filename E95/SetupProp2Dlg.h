#pragma once

#include "SRC/ResizableDialog.h"
#include "SRC/SortListCtrl.h"

class CSetupProp2Dlg : public CResizableDialog {
public:
	~CSetupProp2Dlg();
	CSetupProp2Dlg(CWnd* pParent = NULL);   // standard constructor

	enum { IDD = IDD_SETUPPROP2 };
	CButton	m_cRptButton;
	CStatic	m_cReport;
	CButton	m_cClose;
	CButton	m_cOpen;
	CButton	m_cProceed;
	CSortListCtrl	m_cRight;
	CTreeCtrl	m_cLeft;
	CString	m_RecipeName;
	CString	m_CarrierID;
	CString	m_Message;

	CEdit	m_cftk[5];
	CEdit	m_cwtk[5];
	BOOL	m_sl[5];

	short Port;

	HANDLE m_hStopEventLeft;
	HANDLE m_hStopEventRight;
	CImageList m_TreeImages;
	CImageList m_ListImages;

	void Unclamp();
	void EnableButtonReport();
	void CloseFoup();
	void OpenFoup();
	void CancelAndQuit();
	void DisplayFiles(LPTSTR Path);
	void OpenFolder(CString CStrPath);
	void OnRightViewFolderSelected(CString strPath, UINT index);
	void ResetFiles();
	void CreateRoots();
	BOOL LoadRecipe(CString& name);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();

protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangedLeftview(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickRightview(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkRightview(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLoadrecipe();
	afx_msg void OnProceed();
	afx_msg void On128module();
	afx_msg void On900module();
	afx_msg void OnOpencarrier();
	afx_msg void OnClosecarrier();
	afx_msg void OnReportview();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLpFoupopenfail();
	afx_msg void OnLpFoupopenok();
	afx_msg void OnComExithome();
	afx_msg void OnComEndopen();
	afx_msg void OnComAbortopen();
	afx_msg void OnComNovacuum();
	afx_msg void OnComWaferout();
	afx_msg void OnComVacuumlost();
	afx_msg void OnComSensortrip();
	afx_msg void OnComTp1timeout();
	afx_msg void OnComTp2timeout();
	afx_msg void OnComTp3timeout();
	afx_msg void OnComTp4timeout();
	afx_msg void OnComTp5timeout();
	afx_msg void OnComTp6timeout();
	afx_msg void OnLpFoupcloseok();
	afx_msg void OnLpFoupclosefail();
	afx_msg void OnComAbortclose();
	afx_msg void OnComEndclose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSaveresult();
	afx_msg void OnDestroy();
	void OnComCarrierremoved();
	long OnTabSelected(WPARAM wP, LPARAM lP);
	long OnComSlotMap(WPARAM wP, LPARAM lP);
	long OnComSlotMapFail(WPARAM wP, LPARAM lP);
	DECLARE_MESSAGE_MAP()
};
