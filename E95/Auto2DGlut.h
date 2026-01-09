#pragma once

class CWMapView;
class CRecipe413;
class CGlut2DMapView;

class Auto2DGlut : public CDialog
{
	CWMapView* pView = nullptr;
	CRecipe413* pRcp = nullptr;
	int nDataIdx = 0;
	BOOL bReadyExport = FALSE;
	DECLARE_DYNAMIC(Auto2DGlut)

public:
	void SetParam(CRecipe413* recipe, int dataIdx);
	Auto2DGlut(CWnd* pParent = nullptr);   // standard constructor
	virtual ~Auto2DGlut();

	enum { IDD = IDD_2DAUTO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	void ExportToCImage(CString exportPath);
	void ExportTo(CString exportPath);
	void Yld(ULONGLONG dly);
};