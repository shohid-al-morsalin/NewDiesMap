//20251011_Mohir....
#pragma once
#include "afxdialogex.h"
#include "hpdf.h"
#include "gdiplus.h"
#include "atlimage.h" //20251210_Mohir
#include "stdafx.h"
// CExportPDF dialog
//[20251215_Mohir
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
//]]
class CExportPDF : public CDialogEx
{
	DECLARE_DYNAMIC(CExportPDF)

public:
	CExportPDF(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CExportPDF();
	void DrawMixedFormatText(HPDF_Page page, HPDF_Doc pdf,
		float x, float y,
		const CString& fullText,
		float fontSize = 10.0f); //20251215_Mohir

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PDF };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()

public:
	void ResetPDFState(); //20251215_Mohir
	// PDF-related members
	typedef struct _HPDF_RGBColor {
		HPDF_REAL r;
		HPDF_REAL g;
		HPDF_REAL b;
	} HPDF_RGBColor;
	HPDF_Doc  PDF;
	HPDF_Page page;
	HPDF_Font font;
	jmp_buf env;
	CString str;
	int pageNo = 1;
	int ExtraPage = 0;
	int Image3dCount = 0;
	float currentY = 0;
	_HPDF_PageDirection pageOrientation = HPDF_PAGE_PORTRAIT;
	_HPDF_PageSizes pageSize = HPDF_PAGE_SIZE_A4;

	CString HeaderCenter = _T("FSM-MOT Confidential");
	CString RecipeText = _T("Recipe Name: ");
	CString CassatteIdText;  
	CString LotIdText = _T("Lot ID: ");
	CString UserIdText = _T("User ID: ");
	CString WaferId;
	CString WaferType;
	CString WaferSize;
	CString Station;
	CString Operator;
	CString DateTime;
	CString StartTime;
	CString EndTime;
	CString FolderPath; //2d3d folder path

	CString m_csvFilePath;// file path
	HTREEITEM CurSelItem = NULL;
	HTREEITEM hTreeItemParent = NULL;

private:
	CArray<CArray<CString, CString>*, CArray<CString, CString>*> dataArray;
	CArray<CString, CString>* pCurrentRow;
	int statIndex = 0;

public:
	afx_msg void LoadFile();
	CString FormatLabelValue(const CString& label, const CString& value);
	afx_msg void ReadFileInfo();
	afx_msg void OnBnClickedExportPdf();
	void SetCSVFilePath(const CString& csvFilePath);
	CString GetLastSpecificBmpName(const CString& folderPath, const CString& baseName);
	void LOAD_2D_3D_IMAGE(HPDF_Doc& pdf, HPDF_Page& page);
	CString GetLastSpecific_Png_ImageName(const CString& folderPath, const CString& probe, const CString& extension); //20251210_Mohir
	HPDF_Image ConvertPngToHpdfImage(HPDF_Doc pdf, const CString& pngPath); //20251210_Mohir
	void ExportAsPDF(CString &filename);
	bool SaveAndClosePDF(); //20251215_Mohir
	char fname[256]; //20251210_Mohir
 	void InsertFileInfo(HPDF_Doc &pdf, HPDF_Page &page, HPDF_Font &font, float boxTopY);
	void SetupColumnWidths(float pageWidth, float marginLeft, float marginRight, float colWidths[], int& colCount);
	float GetColumnWidthMultiplier(int columnIndex);
	float InsertTableHeader(HPDF_Doc &pdf, HPDF_Page &page, HPDF_Font &font,
		const CString headers[], const float colWidths[],
		float startX, float startY, int colCount);
	void InsertTableData(HPDF_Doc &pdf, HPDF_Page &page, HPDF_Font font, float startY);
	void DrawPDHeader(HPDF_Doc &pdf, HPDF_Page &page, int &pageNo);
	void DrawPDFooter(HPDF_Doc &pdf, HPDF_Page &page, int &pageNo);
	 
	void InsertMeasurementInfo(HPDF_Doc &pdf, HPDF_Page &page, HPDF_Font &font, float boxTopY);
 	CString GetOutputFolderFilePath(LPCTSTR fileName);
	HPDF_Image ConvertBmpToHpdfImage(HPDF_Doc &pdf, const CString& bmpPath);
	HPDF_Image CreateHpdfImageFromBitmap(HPDF_Doc& pdf, Gdiplus::Bitmap* pBitmap, UINT width, UINT height);
	void DrawImageScaled(HPDF_Page page, HPDF_Image image, float x, float y, float maxWidth, float maxHeight);
	void AddImageCaptions(HPDF_Page page, float leftX, float rightX, float leftY, float rightY, float columnWidth, CString probeName);
	HPDF_Image ConvertCImageToHpdfImage(CImage& cImage);
	void InsertTopImage(HPDF_Page &page, float startY, CString title, CString probeName, CImage& image, CString caption);
	void InsertFullPageImage(HPDF_Page& page, CImage& image);
	void  InsertTopImageM(HPDF_Page& page, float startY, CString title, CString probeName, HPDF_Image image, CString caption);
	void InsertBottomImageM(HPDF_Page& page, float startY, CString title, CString probeName, HPDF_Image image, CString caption);
	void InsertTwoImagesDirect(HPDF_Doc& pdf, HPDF_Page& page, float startY, CString title, CString probeName, HPDF_Image leftImage, HPDF_Image rightImage);
	void Insert_2d_3d_Image(HPDF_Doc& pdf, HPDF_Page& page, CString probe, HPDF_Image leftImage, HPDF_Image rightImage);
    //[20251215_Mohir
	void InsertBottomImage(HPDF_Page& page, float startY, CString title, CString probeName, CImage& image, CString caption);
	CArray<HPDF_Page, HPDF_Page> m_pages; // Store page references
	void AddMultiPages(int NumberOfPage);
	HPDF_Page GetPage(int pageNumber); // Get page by number
	//int GetEncoderClsid(const WCHAR* format, CLSID* pClsid); //20251215_Mohir
	HPDF_Image  CreateHpdfImageFromStream(IStream* pStream, bool isJpeg); //20251215_Mohir
	//Gdiplus::Bitmap* OptimizeBitmapForPng(Gdiplus::Bitmap* pSource); //20251215_Mohir
	Gdiplus::Bitmap* OptimizeBitmapForPng(Gdiplus::Bitmap* pSource, ULONG_PTR gdiplusToken);
	//]
};
