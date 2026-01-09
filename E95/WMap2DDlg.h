#pragma once
#include "afxdialogex.h"
#include <atlImage.h> //20251012_Mohir
// CWMap3DDlg dialo
#include "resource.h"
#include "E95/Glut2DMapView.h"

class CGlut2DMapView;
class CRecipe413;

class CWMap2DDlg : public CDialog
{
	CGlut2DMapView* pView = nullptr;
	CRecipe413* pRcp = nullptr;
	int nDataIdx = 0;
	BOOL bReadyExport = FALSE;
	DECLARE_DYNAMIC(CWMap2DDlg)

public:
	void SetParam(CRecipe413* recipe, int dataIdx);
	CWMap2DDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CWMap2DDlg();

	enum { IDD = IDD_WAFERMAP2D_DLG };
	void ExportToImage(CString exportPath);//20251210_Mohir
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	//void ExportTo(CString exportPath);


	void Yld(ULONGLONG dly);
};
