#pragma once
#include "afxdialogex.h"
#include <atlImage.h> //20251012_Mohir
// CWMap3DDlg dialog

class CWMapView;
class CRecipe413;

class CWMap3DDlg : public CDialog
{
	CWMapView* pView = nullptr;
	CRecipe413* pRcp = nullptr;
	int nDataIdx = 0;
	BOOL bReadyExport = FALSE;
	DECLARE_DYNAMIC(CWMap3DDlg)

public:
	void SetParam(CRecipe413* recipe, int dataIdx);
	CWMap3DDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CWMap3DDlg();

	enum { IDD = IDD_WAFERMAP3D_DLG };
	void ExportToImage(CString exportPath);//20251210_Mohir
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	//void ExportTo(CString exportPath);

	afx_msg void OnClose();
	void Yld(ULONGLONG dly);
};
