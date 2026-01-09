#pragma once
// ViewFileDlg.h : header file
//
#include "..\SRC\SortListCtrl.h"
#include "..\SRC\ResizableDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CViewFileDlg dialog
class CInfoPanelView;

class CViewFileDlg : public CResizableDialog {
	// Construction
public:
	CString strFile;
	CString strSearchPath;

	void LoadFolders();
	CInfoPanelView* m_Parent;
	CViewFileDlg(CWnd* pParent = NULL);   // standard constructor
	~CViewFileDlg();

	// Dialog Data
		//{{AFX_DATA(CViewFileDlg)
	enum { IDD = IDD_VIEWFILEDLG };
	CStatic	m_cDirectory;
	CTreeCtrl m_cLeft;
	CSortListCtrl m_cRight;
	//}}AFX_DATA

	HANDLE m_hStopEventLeft;
	HANDLE m_hStopEventRight;
	CImageList m_TreeImages;
	CImageList m_ListImages;

	void CreateRoots();
	void ResetFiles();
	void DisplayFiles(LPTSTR Path);
	void OpenFolder(CString CStrPath);
	void OnRightViewFolderSelected(CString strPath, UINT index);

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CViewFileDlg)
protected:
	virtual void PostNcDestroy();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CViewFileDlg)
	afx_msg void OnLoad();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangedLeftview(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickRightview(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkRightview(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRefresh();
	afx_msg void OnDestroy();
	afx_msg void OnLoad2();
	//}}AFX_MSG
	afx_msg long OnTabSelected(WPARAM wP, LPARAM lP);
	DECLARE_MESSAGE_MAP()
};
