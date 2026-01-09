//20251011_Mohir....file for generate pdf automically
// ExportPDF.cpp : implementation file
#include "stdafx.h"
#include "afxdialogex.h"
#include <sstream>   
#include <algorithm>
#include <string>

#include "E95.h"               // Your main app header
#include "ExportPDF.h"
#include "Recipe413.h"
#include "afxdialogex.h"
#include <math.h>
#include <setjmp.h>
#include "stdafx.h"
#include "hpdf.h"
#include "math.h"
 

CExportPDF::CExportPDF(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_PDF, pParent)
{
}

CExportPDF::~CExportPDF()
{
}
//[20251215_Mohir
void CExportPDF::ResetPDFState()
{
    // Reset PDF-related variables
    pageNo = 1;
    ExtraPage = 0;
    Image3dCount = 0;
    currentY = 0;
    PDF = nullptr;
    page = nullptr;
    font = nullptr;
    // Clear data arrays
    for (int i = 0; i < dataArray.GetSize(); i++)
    {
        delete dataArray[i];
    }
    dataArray.RemoveAll();
    pCurrentRow = nullptr;
}
void CExportPDF::AddMultiPages(int NumberOfPage)
{
    for (int i = 0; i < NumberOfPage; i++)
    {
        // Create new page
        HPDF_Page newPage = HPDF_AddPage(PDF);
        HPDF_Page_SetSize(newPage, pageSize, pageOrientation);
        // Set font for this page
        HPDF_Page_SetFontAndSize(newPage, font, 10);
        // Store page reference
        m_pages.Add(newPage);
        // Get page dimensions
        float pageWidth = HPDF_Page_GetWidth(newPage);
        float pageHeight = HPDF_Page_GetHeight(newPage);
        pageNo++;
        // ========== ADD HEADER ==========
        DrawPDHeader(PDF, newPage, pageNo);

        // ========== ADD FOOTER ==========
        DrawPDFooter(PDF, newPage, pageNo);
    }
}
HPDF_Page CExportPDF::GetPage(int pageNumber)
{
    if (pageNumber >= 0 && pageNumber < m_pages.GetSize())
        return m_pages[pageNumber];
    return nullptr;
}
//]



void CExportPDF::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}
IMPLEMENT_DYNAMIC(CExportPDF, CDialogEx)   // <-- must be here
BEGIN_MESSAGE_MAP(CExportPDF, CDialogEx)
    ON_BN_CLICKED(IDC_EXPORT_PDF, &CExportPDF::OnBnClickedExportPdf)
END_MESSAGE_MAP()

// ---------------- Header ----------------
void CExportPDF::DrawPDHeader(HPDF_Doc &pdf, HPDF_Page &page, int &pageNo)
{
    if (pageNo == 1)
        return;

    const float pageWidth = HPDF_Page_GetWidth(page);
    const float headerY = HPDF_Page_GetHeight(page) - 20;
    const float fontSize = 6.0f;

    HPDF_Page_BeginText(page);
    HPDF_Page_SetFontAndSize(page, HPDF_GetFont(pdf, "Helvetica", NULL), fontSize);

    HPDF_Page_TextOut(page, 30, headerY, RecipeText);

    const char* centerText = HeaderCenter;
    float centerWidth = HPDF_Page_TextWidth(page, centerText);
    HPDF_Page_TextOut(page, (pageWidth - centerWidth) / 2, headerY, centerText);

    const char* rightText = CassatteIdText;
    float rightWidth = HPDF_Page_TextWidth(page, rightText);
    HPDF_Page_TextOut(page, pageWidth - rightWidth - 30, headerY, rightText);

    HPDF_Page_EndText(page);
}

void CExportPDF::DrawPDFooter(HPDF_Doc &pdf, HPDF_Page &page, int &pageNo)
{
    const float pageWidth = HPDF_Page_GetWidth(page);
    const float footerY = 20;
    const float logoSize = 20;

    // Logo path
    CString logoPath = GetOutputFolderFilePath(_T("fsmlogo.png"));
    std::string logoPathA = CT2A(logoPath);
    HPDF_Image logo = nullptr;
    try {
        logo = HPDF_LoadPngImageFromFile(pdf, logoPathA.c_str());
    }
    catch (...) {
        logo = nullptr;
    }

    if (logo)
        HPDF_Page_DrawImage(page, logo, 30, footerY - 5, logoSize, logoSize);

    // Company name
    HPDF_Page_BeginText(page);
    HPDF_Page_SetFontAndSize(page, HPDF_GetFont(pdf, "Helvetica", NULL), 6.0f);
    HPDF_Page_TextOut(page, 55, footerY, "Frontier Semiconductor Ltd");
    HPDF_Page_EndText(page);

    // Page number
    CString pageStr;
    pageStr.Format(_T("Page %d"), pageNo);
    std::string pageStrA = CT2A(pageStr);

    HPDF_Page_BeginText(page);
    HPDF_Page_SetFontAndSize(page, HPDF_GetFont(pdf, "Helvetica", NULL), 6.0f);
    HPDF_Page_TextOut(page, pageWidth - 80, footerY, pageStrA.c_str());
    HPDF_Page_EndText(page);
}
void CExportPDF::LoadFile() {
    CFileDialog dlg(TRUE, _T("csv"), NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
        _T("CSV Files (*.csv)|*.csv|All Files (*.*)|*.*||"), this);
    if (dlg.DoModal() == IDOK)
    {
        m_csvFilePath = dlg.GetPathName();  // Save the file path
       // AfxMessageBox(_T("Selected File: ") + m_csvFilePath);
    }
}
CString CExportPDF::FormatLabelValue(const CString& label, const CString& value) {
    const int LABEL_WIDTH = 20;
    CString formatted = label;
    formatted += _T(": ") + value;
    return formatted;
}
void CExportPDF::ReadFileInfo() {
    // Clear previous data
    for (int i = 0; i < dataArray.GetSize(); i++) {
        delete dataArray[i];
    }
    dataArray.RemoveAll();
    if (m_csvFilePath.IsEmpty()) {
        AfxMessageBox(_T("No CSV file selected!"));
        return;
    }
    CStdioFile file;
    if (!file.Open(m_csvFilePath, CFile::modeRead | CFile::typeText)) {
        AfxMessageBox(_T("Failed to open CSV file!"));
        return;
    }
    CString line;
    int rowCount = 0;
    while (file.ReadString(line)) {
        rowCount++;
        // Split CSV line into columns
        CString token;
        int curPos = 0;
        int colIndex = 0;
        CString secondColValue;
        token = line.Tokenize(_T(","), curPos);
        BOOL rowFlag = FALSE;
        pCurrentRow = new CArray<CString, CString>();
        while (!token.IsEmpty()) {
            colIndex++;

            if (rowCount > 10) {
                pCurrentRow->Add(token);
                if (!rowFlag) {
                    rowFlag = TRUE;
                }
            }
            // Get second column value
            if (colIndex == 2) {
                secondColValue = token;
            }
            // Store measurement information for next use
            if (token == _T("Statistics")) {
                statIndex = dataArray.GetSize();
            }
            token = line.Tokenize(_T(","), curPos);
        }
        // FIX 1: Check if row has data before adding
        if (rowFlag && pCurrentRow->GetSize() > 1) {
            dataArray.Add(pCurrentRow);
        }
        else {
            delete pCurrentRow; // Clean up if not used
        }
        // Use the corrected formatting function
        switch (rowCount) {
        case 1:
            RecipeText = FormatLabelValue(_T("Recipe Name"), secondColValue);
            break;
        case 2:
            LotIdText = FormatLabelValue(_T("Lot ID"), secondColValue);
            break;
        case 3:
            WaferId = FormatLabelValue(_T("Wafer ID"), secondColValue);
            break;
        case 4:
            WaferType = FormatLabelValue(_T("Wafer Type"), secondColValue);
            break;
        case 5:
            WaferSize = FormatLabelValue(_T("Wafer Size"), secondColValue);
            break;
        case 6:
            Station = FormatLabelValue(_T("Station"), secondColValue);
            break;
        case 7:
            Operator = FormatLabelValue(_T("Operator"), secondColValue);
            break;
        case 8:
            DateTime = FormatLabelValue(_T("Date/Time"), secondColValue);
            break;
        case 9:
            StartTime = FormatLabelValue(_T("Time Start"), secondColValue);
            break;
        case 10:
            EndTime = FormatLabelValue(_T("End Time"), secondColValue);
            break;
        default:
            break;
        }
    }
    file.Close();
}
// ---------------- Utility ----------------
CString CExportPDF:: GetOutputFolderFilePath(LPCTSTR fileName)
{
    TCHAR exePath[MAX_PATH];
    GetModuleFileName(NULL, exePath, MAX_PATH);
    PathRemoveFileSpec(exePath);  // remove the exe name, keep only folder
    CString fullPath(exePath);
    fullPath += _T("\\");
    fullPath += fileName;
    return fullPath;
}
//[20251215_Mohir
void CExportPDF::DrawImageScaled(HPDF_Page page, HPDF_Image image, float x, float y, float maxWidth, float maxHeight)
{
    if (!image) return;
    HPDF_REAL imgWidth = HPDF_Image_GetWidth(image);
    HPDF_REAL imgHeight = HPDF_Image_GetHeight(image);
    float scaleByWidth = maxWidth / imgWidth;
    float scaleByHeight = maxHeight / imgHeight;
    float scale = min(scaleByWidth, scaleByHeight);
    float scaledWidth = imgWidth * scale;
    float scaledHeight = imgHeight * scale;
    float centeredX = x + (maxWidth - scaledWidth) / 2;
    float centeredY = y + (maxHeight - scaledHeight) / 2;
    HPDF_Page_DrawImage(page, image, centeredX, centeredY, scaledWidth, scaledHeight);
}

void CExportPDF::AddImageCaptions(HPDF_Page page, float leftX, float rightX, float leftY, float rightY, float columnWidth, CString probeName)
{
    HPDF_Page_BeginText(page);
    HPDF_Page_SetFontAndSize(page, HPDF_GetFont(PDF, "Helvetica", NULL), 7);
    // Left caption
    CStringA leftCaptionA(probeName + _T("_2D"));
    float leftCaptionWidth = HPDF_Page_TextWidth(page, leftCaptionA);
    HPDF_Page_TextOut(page, leftX + (columnWidth - leftCaptionWidth) / 2,
        leftY - 10, leftCaptionA);
    // Right caption  
    CStringA rightCaptionA(probeName + _T("_3D"));
    float rightCaptionWidth = HPDF_Page_TextWidth(page, rightCaptionA);
    HPDF_Page_TextOut(page, rightX + (columnWidth - rightCaptionWidth) / 2,
        rightY - 10, rightCaptionA);
    HPDF_Page_EndText(page);
}
//]
////////////////////////////////////////////////////////////////////////////////////////////////////////INSERT TOP IMAGE //////////////////////////////////////////////////////////////////////////////////////
//[20251215_Mohir
void CExportPDF::InsertTopImageM(HPDF_Page& page, float startY, CString title,
    CString probeName, HPDF_Image image, CString caption)
{
    if (page == nullptr || image == nullptr) return;

    // Get page dimensions
    const float pageWidth = HPDF_Page_GetWidth(page);
    const float pageHeight = HPDF_Page_GetHeight(page); // A4 = 842 points

    // HEADER SPACE: Top 70 points for header
    const float HEADER_HEIGHT = 70.0f;

    // FOOTER SPACE: Bottom 70 points for footer  
    const float FOOTER_HEIGHT = 70.0f;

    const float MARGIN = 25.0f;
    const float PADDING = 10.0f;

    // Calculate top section position (below header)
    float rectHeight = 305.0f; // Keep your original height
    float rectWidth = pageWidth - (2 * MARGIN);
    float rectX = MARGIN;
    float rectY = pageHeight - HEADER_HEIGHT - rectHeight; // Below header

    // If startY is provided, use it (adjust for header)
    if (startY > 0) {
        rectY = startY - rectHeight;
        // Ensure it's below header
        if (rectY > pageHeight - HEADER_HEIGHT - rectHeight) {
            rectY = pageHeight - HEADER_HEIGHT - rectHeight;
        }
    }
    // Draw top rectangle
    HPDF_Page_SetLineWidth(page, 1.0f);
    HPDF_Page_Rectangle(page, rectX, 50/*rectY*/, rectWidth, 740/*rectHeight*/);
    HPDF_Page_Stroke(page);

    // Title
    float titleY = rectY + rectHeight - PADDING;
    HPDF_Page_BeginText(page);
    HPDF_Page_SetFontAndSize(page, HPDF_GetFont(PDF, "Helvetica-Bold", NULL), 12);
    CStringA titleTextA(title);
    float titleWidth = HPDF_Page_TextWidth(page, titleTextA);
    float titleX = rectX + (rectWidth - titleWidth) / 2;
    HPDF_Page_TextOut(page, titleX, titleY+8, titleTextA);
    HPDF_Page_EndText(page);

    // Image - scaled to fit in rectangle
    float imageHeight = 250.0f; // Fixed height as you wanted
    float imageWidth = rectWidth - 20; // 10px padding each side
    float imageY = titleY - 25 - imageHeight;
    float imageX = rectX + 10;

    DrawImageScaled(page, image, imageX, 438 /*imageY+5*/, imageWidth, 340/*imageHeight+50*/);

    // Caption
    HPDF_Page_BeginText(page);
    HPDF_Page_SetFontAndSize(page, HPDF_GetFont(PDF, "Helvetica-Bold", NULL), 10);
    CStringA captionA(caption);
    float captionWidth = HPDF_Page_TextWidth(page, captionA);
    float captionX = imageX + (imageWidth - captionWidth) / 2;
    float captionY = imageY - 48;
    HPDF_Page_TextOut(page, captionX, captionY, captionA);
    HPDF_Page_EndText(page);

    // Return the Y position for bottom section
    // Bottom section should start 20px below this rectangle
    currentY = rectY - 20.0f;

    // Ensure bottom section doesn't go into footer area
    if (currentY < FOOTER_HEIGHT + 100) { // 100px minimum for bottom section
        currentY = FOOTER_HEIGHT + 100;
    }
}

//HPDF_Image CExportPDF::ConvertCImageToHpdfImage(CImage& cImage)
//{
//    if (PDF == nullptr)
//        return nullptr;
//    // Get image dimensions
//    int width = cImage.GetWidth();
//    int height = cImage.GetHeight();
//    if (width <= 0 || height <= 0)
//        return nullptr;
//    // Create a memory stream
//    IStream* pStream = nullptr;
//    HRESULT hr = CreateStreamOnHGlobal(NULL, TRUE, &pStream);
//
//    if (SUCCEEDED(hr) && pStream)
//    {
//        // Save CImage to stream as PNG
//        hr = cImage.Save(pStream, Gdiplus::ImageFormatPNG);
//
//        if (SUCCEEDED(hr))
//        {
//            // Get stream size
//            STATSTG stat;
//            pStream->Stat(&stat, STATFLAG_NONAME);
//            ULARGE_INTEGER size = stat.cbSize;
//
//            // Read stream data
//            HGLOBAL hGlobal = NULL;
//            GetHGlobalFromStream(pStream, &hGlobal);
//
//            if (hGlobal)
//            {
//                BYTE* pData = (BYTE*)GlobalLock(hGlobal);
//                if (pData)
//                {
//                    // Create HPDF image from memory
//                    HPDF_Image image = HPDF_LoadPngImageFromMem(PDF, pData, (HPDF_UINT)size.QuadPart);
//                    GlobalUnlock(hGlobal);
//                    pStream->Release();
//                    return image;
//                }
//            }
//        }
//        pStream->Release();
//    }
//
//    return nullptr;
//}

///////////////////////////////////////////////////////
int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken = 0;

    if (Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL) != Gdiplus::Ok)
        return -1;

    UINT num = 0;
    UINT size = 0;
    Gdiplus::GetImageEncodersSize(&num, &size);

    if (size == 0)
    {
        Gdiplus::GdiplusShutdown(gdiplusToken);
        return -1;
    }

    Gdiplus::ImageCodecInfo* pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
    if (!pImageCodecInfo)
    {
        Gdiplus::GdiplusShutdown(gdiplusToken);
        return -1;
    }

    Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);
    int result = -1;

    for (UINT i = 0; i < num; ++i)
    {
        if (wcscmp(pImageCodecInfo[i].MimeType, format) == 0)
        {
            *pClsid = pImageCodecInfo[i].Clsid;
            result = i;
            break;
        }
    }

    free(pImageCodecInfo);
    Gdiplus::GdiplusShutdown(gdiplusToken);
    return result;
}

//HPDF_Image CExportPDF::ConvertCImageToHpdfImage(CImage& cImage)
//{
//    if (PDF == nullptr)
//        return nullptr;
//
//    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
//    ULONG_PTR gdiplusToken = 0;
//
//    if (Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL) != Gdiplus::Ok)
//        return nullptr;
//
//    bool useJpeg = true;
//    int jpegQuality = 75;
//    int width = cImage.GetWidth();
//    int height = cImage.GetHeight();
//
//    if (width <= 0 || height <= 0)
//    {
//        Gdiplus::GdiplusShutdown(gdiplusToken);
//        return nullptr;
//    }
//
//    Gdiplus::Bitmap* pBitmap = Gdiplus::Bitmap::FromHBITMAP(cImage, NULL);
//    if (!pBitmap)
//    {
//        Gdiplus::GdiplusShutdown(gdiplusToken);
//        return nullptr;
//    }
//
//    IStream* pStream = nullptr;
//    HRESULT hr = CreateStreamOnHGlobal(NULL, TRUE, &pStream);
//
//    if (FAILED(hr) || !pStream)
//    {
//        delete pBitmap;
//        Gdiplus::GdiplusShutdown(gdiplusToken);
//        return nullptr;
//    }
//
//    HPDF_Image result = nullptr;
//
//    if (useJpeg)
//    {
//        CLSID encoderClsid;
//        if (GetEncoderClsid(L"image/jpeg", &encoderClsid) != -1)
//        {
//            Gdiplus::EncoderParameters encoderParams;
//            encoderParams.Count = 1;
//            encoderParams.Parameter[0].Guid = Gdiplus::EncoderQuality;
//            encoderParams.Parameter[0].Type = Gdiplus::EncoderParameterValueTypeLong;
//            encoderParams.Parameter[0].NumberOfValues = 1;
//
//            ULONG quality = (ULONG)min(max(jpegQuality, 1), 100);
//            encoderParams.Parameter[0].Value = &quality;
//
//            if (pBitmap->Save(pStream, &encoderClsid, &encoderParams) == Gdiplus::Ok)
//            {
//                result = CreateHpdfImageFromStream(pStream, true);
//            }
//        }
//    }
//    else
//    {
//        CLSID encoderClsid;
//        if (GetEncoderClsid(L"image/png", &encoderClsid) != -1)
//        {
//            Gdiplus::Bitmap* pOptimized = OptimizeBitmapForPng(pBitmap, gdiplusToken);
//            if (pOptimized)
//            {
//                delete pBitmap;
//                pBitmap = pOptimized;
//            }
//
//            if (pBitmap->Save(pStream, &encoderClsid, NULL) == Gdiplus::Ok)
//            {
//                result = CreateHpdfImageFromStream(pStream, false);
//            }
//        }
//    }
//
//    delete pBitmap;
//    pStream->Release();
//    Gdiplus::GdiplusShutdown(gdiplusToken);
//
//    return result;
//}


// Helper function to crop a bitmap
Gdiplus::Bitmap* CropBitmap(Gdiplus::Bitmap* pSource, int margin)
{
    if (!pSource)
        return nullptr;

    int originalWidth = pSource->GetWidth();
    int originalHeight = pSource->GetHeight();

    // Calculate crop dimensions
    int cropX = margin;
    int cropY = margin;
    int cropWidth = originalWidth - (2 * margin);
    int cropHeight = originalHeight - (2 * margin);

    // Validate dimensions - if image is too small, don't crop
    if (cropWidth <= 10 || cropHeight <= 10)  // Keep minimum 10px
        return pSource;

    // Create cropped bitmap
    Gdiplus::Bitmap* pCropped = new Gdiplus::Bitmap(cropWidth, cropHeight, pSource->GetPixelFormat());
    Gdiplus::Graphics graphics(pCropped);

    // CORRECTED: Use proper DrawImage overload
    graphics.DrawImage(pSource,
        Gdiplus::Rect(0, 0, cropWidth, cropHeight),          // Destination rectangle
        cropX, cropY, cropWidth, cropHeight,                // Source rectangle
        Gdiplus::UnitPixel);

    return pCropped;
}

// Modified ConvertCImageToHpdfImage using the crop function
HPDF_Image CExportPDF::ConvertCImageToHpdfImage(CImage& cImage)
{
    if (PDF == nullptr)
        return nullptr;

    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken = 0;

    if (Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL) != Gdiplus::Ok)
        return nullptr;

    bool useJpeg = true;
    int jpegQuality = 75;
    int width = cImage.GetWidth();
    int height = cImage.GetHeight();

    if (width <= 0 || height <= 0)
    {
        Gdiplus::GdiplusShutdown(gdiplusToken);
        return nullptr;
    }

    Gdiplus::Bitmap* pOriginalBitmap = Gdiplus::Bitmap::FromHBITMAP(cImage, NULL);
    if (!pOriginalBitmap)
    {
        Gdiplus::GdiplusShutdown(gdiplusToken);
        return nullptr;
    }

    // Crop 30 pixels from each side
    Gdiplus::Bitmap* pBitmap = CropBitmap(pOriginalBitmap, 30);
    delete pOriginalBitmap;  // Delete original

    if (!pBitmap)  // If crop failed, use original
    {
        pBitmap = Gdiplus::Bitmap::FromHBITMAP(cImage, NULL);
        if (!pBitmap)
        {
            Gdiplus::GdiplusShutdown(gdiplusToken);
            return nullptr;
        }
    }

    IStream* pStream = nullptr;
    HRESULT hr = CreateStreamOnHGlobal(NULL, TRUE, &pStream);

    if (FAILED(hr) || !pStream)
    {
        delete pBitmap;
        Gdiplus::GdiplusShutdown(gdiplusToken);
        return nullptr;
    }

    HPDF_Image result = nullptr;

    if (useJpeg)
    {
        CLSID encoderClsid;
        if (GetEncoderClsid(L"image/jpeg", &encoderClsid) != -1)
        {
            Gdiplus::EncoderParameters encoderParams;
            encoderParams.Count = 1;
            encoderParams.Parameter[0].Guid = Gdiplus::EncoderQuality;
            encoderParams.Parameter[0].Type = Gdiplus::EncoderParameterValueTypeLong;
            encoderParams.Parameter[0].NumberOfValues = 1;

            ULONG quality = (ULONG)min(max(jpegQuality, 1), 100);
            encoderParams.Parameter[0].Value = &quality;

            if (pBitmap->Save(pStream, &encoderClsid, &encoderParams) == Gdiplus::Ok)
            {
                result = CreateHpdfImageFromStream(pStream, true);
            }
        }
    }
    else
    {
        CLSID encoderClsid;
        if (GetEncoderClsid(L"image/png", &encoderClsid) != -1)
        {
            Gdiplus::Bitmap* pOptimized = OptimizeBitmapForPng(pBitmap, gdiplusToken);
            if (pOptimized)
            {
                delete pBitmap;
                pBitmap = pOptimized;
            }

            if (pBitmap->Save(pStream, &encoderClsid, NULL) == Gdiplus::Ok)
            {
                result = CreateHpdfImageFromStream(pStream, false);
            }
        }
    }

    delete pBitmap;
    pStream->Release();
    Gdiplus::GdiplusShutdown(gdiplusToken);

    return result;
}



Gdiplus::Bitmap* CExportPDF::OptimizeBitmapForPng(Gdiplus::Bitmap* pSource, ULONG_PTR gdiplusToken)
{
    if (!pSource)
        return nullptr;

    int width = pSource->GetWidth();
    int height = pSource->GetHeight();

    Gdiplus::Bitmap* pDest = new Gdiplus::Bitmap(width, height, PixelFormat8bppIndexed);
    if (!pDest)
        return nullptr;

    Gdiplus::BitmapData srcData;
    Gdiplus::Rect rect(0, 0, width, height);

    if (pSource->LockBits(&rect, Gdiplus::ImageLockModeRead,
        pSource->GetPixelFormat(), &srcData) == Gdiplus::Ok)
    {
        Gdiplus::BitmapData destData;
        if (pDest->LockBits(&rect, Gdiplus::ImageLockModeWrite,
            PixelFormat8bppIndexed, &destData) == Gdiplus::Ok)
        {
            for (int y = 0; y < height; y++)
            {
                BYTE* pSrcRow = (BYTE*)srcData.Scan0 + y * srcData.Stride;
                BYTE* pDestRow = (BYTE*)destData.Scan0 + y * destData.Stride;

                for (int x = 0; x < width; x++)
                {
                    if (srcData.PixelFormat == PixelFormat24bppRGB)
                    {
                        BYTE r = pSrcRow[x * 3 + 2];
                        BYTE g = pSrcRow[x * 3 + 1];
                        BYTE b = pSrcRow[x * 3 + 0];
                        pDestRow[x] = (BYTE)((r * 0.299f + g * 0.587f + b * 0.114f));
                    }
                    else if (srcData.PixelFormat == PixelFormat32bppARGB)
                    {
                        BYTE r = pSrcRow[x * 4 + 2];
                        BYTE g = pSrcRow[x * 4 + 1];
                        BYTE b = pSrcRow[x * 4 + 0];
                        pDestRow[x] = (BYTE)((r * 0.299f + g * 0.587f + b * 0.114f));
                    }
                }
            }

            pDest->UnlockBits(&destData);
        }
        pSource->UnlockBits(&srcData);
    }
    else
    {
        delete pDest;
        return nullptr;
    }

    return pDest;
}

HPDF_Image CExportPDF::CreateHpdfImageFromStream(IStream* pStream, bool isJpeg)
{
    STATSTG stat;
    if (FAILED(pStream->Stat(&stat, STATFLAG_NONAME)))
        return nullptr;

    ULARGE_INTEGER size = stat.cbSize;

    HGLOBAL hGlobal = NULL;
    if (FAILED(GetHGlobalFromStream(pStream, &hGlobal)) || !hGlobal)
        return nullptr;

    BYTE* pData = (BYTE*)GlobalLock(hGlobal);
    if (!pData)
        return nullptr;

    HPDF_Image image = nullptr;
    if (isJpeg)
    {
        image = HPDF_LoadJpegImageFromMem(PDF, pData, (HPDF_UINT)size.QuadPart);
    }
    else
    {
        image = HPDF_LoadPngImageFromMem(PDF, pData, (HPDF_UINT)size.QuadPart);
    }

    GlobalUnlock(hGlobal);
    return image;
}
////////////////////////////////////////////////////////
//]
//[20251215_Mohir
void CExportPDF::InsertTopImage(HPDF_Page &page, float startY, CString title, CString probeName, CImage& image, CString caption)
{
    if (page == nullptr) return;
    HPDF_Image hpdfImage = ConvertCImageToHpdfImage(image);
    if (hpdfImage == nullptr)
    {
        return;
    }
    // Now call the original InsertTopImage with HPDF_Image
    InsertTopImageM(page, startY, title, probeName, hpdfImage, caption);
}

//20251219_Mahmudul
void CExportPDF::InsertFullPageImage(HPDF_Page& page, CImage& image)
{
    if (page == nullptr) return;

    // 1. Convert Image
    HPDF_Image hpdfImage = ConvertCImageToHpdfImage(image);
    if (hpdfImage == nullptr) return;

    // 2. Get Page Dimensions
    float pageWidth = HPDF_Page_GetWidth(page);
    float pageHeight = HPDF_Page_GetHeight(page);

    // 3. Define Margins (Optional: set to 0.0f for absolute full page)
    const float MARGIN_X = 20.0f;
    const float MARGIN_Y = 20.0f;

    // Calculate usable area
    float usableWidth = pageWidth - (2 * MARGIN_X);
    float usableHeight = pageHeight - (2 * MARGIN_Y);

    // 4. Get Original Image Dimensions
    HPDF_Point imgSize = HPDF_Image_GetSize(hpdfImage);
    float imgW = imgSize.x;
    float imgH = imgSize.y;

    if (imgW <= 0 || imgH <= 0) return;

    // 5. Calculate Scale to Fit (Aspect Ratio)
    float scaleX = usableWidth / imgW;
    float scaleY = usableHeight / imgH;

    // Use the smaller scale factor to ensure the whole image fits
    float scale = (scaleX < scaleY) ? scaleX : scaleY;

    // 6. Calculate Final Dimensions
    float finalWidth = imgW * scale;
    float finalHeight = imgH * scale;

    // 7. Calculate Centered Position
    // (PageDim - ImageDim) / 2 gives the offset to center
    float x = (pageWidth - finalWidth) / 2.0f;
    float y = (pageHeight - finalHeight) / 2.0f;

    // 8. Draw the Image
    HPDF_Page_DrawImage(page, hpdfImage, x, y, finalWidth, finalHeight);

    // Update global Y tracker to bottom of page (optional)
    currentY = MARGIN_Y;
}


/////////////////////////////////////////////////////////////////////Insert Bottom Image ////////////////////////////////////////
void CExportPDF::InsertBottomImageM(HPDF_Page& page, float startY, CString title, CString probeName, HPDF_Image image, CString caption)
{
    if (page == nullptr || image == nullptr) return;
    // Get page dimensions
    const float pageWidth = HPDF_Page_GetWidth(page);
    const float pageHeight = HPDF_Page_GetHeight(page);

    // FOOTER SPACE: Bottom 70 points for footer  
    const float FOOTER_HEIGHT = 70.0f;

    const float MARGIN = 25.0f;
    const float PADDING = 10.0f;

    // Calculate bottom section position (above footer)
    float rectHeight = 305.0f; // Same as top section
    float rectWidth = pageWidth - (2 * MARGIN);
    float rectX = MARGIN;
    float imageHeight = 250.0f;
    float imageWidth = rectWidth - 20;
    float imageY = 72;
    float imageX = rectX + 5;
    DrawImageScaled(page, image, imageX, imageY, imageWidth, 340/*imageHeight*/);
    // Caption
    HPDF_Page_BeginText(page);
    HPDF_Page_SetFontAndSize(page, HPDF_GetFont(PDF, "Helvetica-Bold", NULL), 10);
    CStringA captionA(caption);
    float captionWidth = HPDF_Page_TextWidth(page, captionA);
    float captionX = imageX + (imageWidth - captionWidth) / 2;
   /* float captionY = imageY - 10;*/
    HPDF_Page_TextOut(page, captionX, 62, captionA);
    HPDF_Page_EndText(page);
}
void CExportPDF::InsertBottomImage(HPDF_Page& page, float startY, CString title, CString probeName, CImage& image, CString caption)
{
    if (page == nullptr) return;
    HPDF_Image hpdfImage = ConvertCImageToHpdfImage(image);
    if (hpdfImage == nullptr)
    {
        return;
    }
    
    InsertBottomImageM(page, startY, title, probeName, hpdfImage, caption);
}
 //]]
void CExportPDF::InsertTwoImagesDirect(HPDF_Doc& pdf, HPDF_Page& page, float startY, CString title, CString probeName, HPDF_Image leftImage, HPDF_Image rightImage)
{
    const float pageWidth = HPDF_Page_GetWidth(page);
    const float marginLeft = 25;
    const float marginRight = 25;
    const float padding = 5;
    float contentWidth = pageWidth - marginLeft - marginRight;
    float imageWidth = contentWidth;
    float availableHeight = startY - 40;
    float imageHeight = availableHeight / 2 - 30; // Changed from -20 (makes images 10px shorter)
    float captionHeight = 10;
    float titleHeight = 10;
    float verticalGap = 5;
    float totalSectionHeight = (imageHeight * 2) + captionHeight * 2 + titleHeight + verticalGap + (2 * padding);
    // INCREASE RECTANGLE BORDER BY 30PX AT BOTTOM
    float rectHeight = totalSectionHeight + 18;  
    float rectWidth = contentWidth;
    float rectX = marginLeft;
    float rectY = startY - rectHeight;
    // Draw rectangle border (now taller by 30px)
    HPDF_Page_SetLineWidth(page, 0.5f);
    HPDF_Page_Rectangle(page, rectX, rectY, rectWidth, rectHeight);
    HPDF_Page_Stroke(page);
    // Add title
    HPDF_Page_BeginText(page);
    HPDF_Page_SetFontAndSize(page, HPDF_GetFont(pdf, "Helvetica-Bold", NULL), 12);
    CStringA titleTextA(title);
    float titleWidth = HPDF_Page_TextWidth(page, titleTextA);
    float titleX = rectX + (rectWidth - titleWidth) / 2;
    float titleY = rectY + rectHeight - padding - 10;
    HPDF_Page_TextOut(page, titleX, titleY, titleTextA);
    HPDF_Page_EndText(page);

    // Calculate positions for TOP image
    float topImageY = rectY + rectHeight - padding - titleHeight - imageHeight - 5;
    float topImageX = rectX;

    // Calculate positions for BOTTOM image
    float bottomImageY = topImageY - imageHeight - verticalGap - captionHeight;
    float bottomImageX = rectX;

    // Draw TOP image
    if (leftImage) {
        DrawImageScaled(page, leftImage, topImageX, topImageY, imageWidth, imageHeight);

        // Top image caption - moved up slightly to fit in larger rectangle
        HPDF_Page_BeginText(page);
        HPDF_Page_SetFontAndSize(page, HPDF_GetFont(pdf, "Helvetica", NULL), 8);
        CStringA topCaption("2D_VIEW");
        float topCaptionWidth = HPDF_Page_TextWidth(page, topCaption);
        float topCaptionX = topImageX + (imageWidth - topCaptionWidth) / 2;
        float topCaptionY = topImageY - 8;
        HPDF_Page_TextOut(page, topCaptionX, topCaptionY, topCaption);
        HPDF_Page_EndText(page);
    }

    // Draw BOTTOM image
    if (rightImage) {
        DrawImageScaled(page, rightImage, bottomImageX, bottomImageY, imageWidth, imageHeight);

        // Bottom image caption - moved up slightly to fit in larger rectangle
        HPDF_Page_BeginText(page);
        HPDF_Page_SetFontAndSize(page, HPDF_GetFont(pdf, "Helvetica", NULL), 8);
        CStringA bottomCaption("3D_VIEW");
        float bottomCaptionWidth = HPDF_Page_TextWidth(page, bottomCaption);
        float bottomCaptionX = bottomImageX + (imageWidth - bottomCaptionWidth) / 2;
        float bottomCaptionY = bottomImageY - 8;
        HPDF_Page_TextOut(page, bottomCaptionX, bottomCaptionY, bottomCaption);
        HPDF_Page_EndText(page);
    }

    currentY -= (totalSectionHeight + 20); // Changed from +30
}
void CExportPDF::SetCSVFilePath(const CString& csvFilePath) {
    m_csvFilePath = csvFilePath;
}
void CExportPDF::InsertFileInfo(HPDF_Doc& pdf, HPDF_Page& page, HPDF_Font& font, float boxTopY)
{
    const float pageWidth = HPDF_Page_GetWidth(page);

    float marginLeft = 25;
    float marginRight = 25;
    float boxHeight = 100;

    float boxX = marginLeft;
    float boxY = boxTopY + 20;
    float boxW = pageWidth - marginLeft - marginRight;

    // Draw rectangle border
    HPDF_Page_Rectangle(page, boxX, boxY, boxW, boxHeight);
    HPDF_Page_Stroke(page);

    int textPaddingX = 10;
    int textPaddingY = 25;
    int lineHeight = 12;
    int columnWidth = boxW / 2 - textPaddingX;
    float topMargin = 5;
    float fontSize = 10.0f;

    // Starting Y position
    float startY = boxY + boxHeight - textPaddingY - topMargin;

    // Remove HPDF_Page_BeginText/EndText wrapper since we're not using TextRect anymore
    // Column 1 (Left side)
    DrawMixedFormatText(page, pdf, boxX + textPaddingX, startY, RecipeText, fontSize);
    DrawMixedFormatText(page, pdf, boxX + textPaddingX, startY - lineHeight, LotIdText, fontSize);
    DrawMixedFormatText(page, pdf, boxX + textPaddingX, startY - 2 * lineHeight, WaferId, fontSize);
    DrawMixedFormatText(page, pdf, boxX + textPaddingX, startY - 3 * lineHeight, WaferType, fontSize);
    DrawMixedFormatText(page, pdf, boxX + textPaddingX, startY - 4 * lineHeight, WaferSize, fontSize);

    // Column 2 (Right side)
    DrawMixedFormatText(page, pdf, boxX + columnWidth + textPaddingX, startY, Station, fontSize);
    DrawMixedFormatText(page, pdf, boxX + columnWidth + textPaddingX, startY - lineHeight, Operator, fontSize);
    DrawMixedFormatText(page, pdf, boxX + columnWidth + textPaddingX, startY - 2 * lineHeight, DateTime, fontSize);
    DrawMixedFormatText(page, pdf, boxX + columnWidth + textPaddingX, startY - 3 * lineHeight, StartTime, fontSize);
    DrawMixedFormatText(page, pdf, boxX + columnWidth + textPaddingX, startY - 4 * lineHeight, EndTime, fontSize);

    // Logo code remains the same...
    CString logoPath = GetOutputFolderFilePath(_T("fsmlogo_M.png"));
    HPDF_Image logo = nullptr;

    try {
        if (PathFileExists(logoPath)) {
            logo = HPDF_LoadPngImageFromFile(pdf, CT2A(logoPath));

            if (logo) {
                float logoWidth = 100;
                float logoHeight = 40;
                float logoX = boxX + boxW - logoWidth - 10;
                float logoY = boxY + (boxHeight - logoHeight) / 2;
                HPDF_Page_DrawImage(page, logo, logoX, logoY, logoWidth, logoHeight);
            }
        }
    }
    catch (...) {
        // Silent catch
    }

    DrawPDFooter(pdf, page, pageNo);
    DrawPDHeader(pdf, page, pageNo);
}
// In CExportPDF.cpp
void CExportPDF::DrawMixedFormatText(HPDF_Page page, HPDF_Doc pdf,
    float x, float y,
    const CString& fullText,
    float fontSize)
{
    // Your FormatLabelValue creates "Label: Value"
    // Split at ": " to separate label from value
    int colonPos = fullText.Find(_T(": "));

    if (colonPos == -1) {
        // Fallback: draw everything as normal
        HPDF_Page_BeginText(page);
        HPDF_Page_SetFontAndSize(page, HPDF_GetFont(pdf, "Helvetica", NULL), fontSize);
        HPDF_Page_TextOut(page, x, y, CT2A(fullText));
        HPDF_Page_EndText(page);
        return;
    }

    // Label includes ": " (e.g., "Recipe Name: ")
    CString label = fullText.Left(colonPos + 2);
    // Value is everything after ": " (e.g., "MyRecipe123")
    CString value = fullText.Mid(colonPos + 2);

    // Draw LABEL in BOLD
    HPDF_Page_BeginText(page);
    HPDF_Page_SetFontAndSize(page, HPDF_GetFont(pdf, "Helvetica-Bold", NULL), fontSize);
    HPDF_Page_TextOut(page, x, y, CT2A(label));
    HPDF_Page_EndText(page);

    // Calculate where value should start
    float labelWidth = HPDF_Page_TextWidth(page, CT2A(label));

    // Draw VALUE in NORMAL
    HPDF_Page_BeginText(page);
    HPDF_Page_SetFontAndSize(page, HPDF_GetFont(pdf, "Helvetica", NULL), fontSize);
    HPDF_Page_TextOut(page, x + labelWidth, y, CT2A(value));
    HPDF_Page_EndText(page);
}

 

void CExportPDF::InsertMeasurementInfo(HPDF_Doc& pdf, HPDF_Page& page, HPDF_Font &font, float startY)
{
    const float pageWidth = HPDF_Page_GetWidth(page);
    const float pageHeight = HPDF_Page_GetHeight(page);

    const float marginLeft = 25;
    const float marginRight = 25;
    const float marginBottom = 30;

    float currentY1 = (startY == -1) ? (pageHeight - 50) : startY;
    // ========== ADD "STATISTICS" TEXT ON LEFT SIDE ==========
    HPDF_Page_BeginText(page);
    HPDF_Page_SetFontAndSize(page, HPDF_GetFont(pdf, "Helvetica-Bold", NULL), 10);
    HPDF_Page_TextOut(page, marginLeft, currentY1 - 5, "Statistics");
    HPDF_Page_EndText(page);
    currentY1 -= 10;
    //=========================================================

    const float fontSize = 10.0f;
    const float cellPadding = 4.0f;
    const float lineWidth = 0.5f;
    const float rowHeight = fontSize + 2 * cellPadding;

    int colCount = dataArray[statIndex + 1]->GetSize();
    float colWidths[25] = { 0 };
    SetupColumnWidths(pageWidth, marginLeft, marginRight, colWidths, colCount);

    if (colCount == 0 || dataArray.GetSize() == 0) return;

    // Create headers from first row of dataArray
    CString headers[25];
    if (dataArray.GetSize() > 0) {
        CArray<CString, CString>* pFirstRow = dataArray[statIndex];
        for (int i = 0; i < min(pFirstRow->GetSize(), 25); i++) {
            headers[i] = pFirstRow->GetAt(i);
        }
    }

    else {
        // Fallback to generic headers
        for (int i = 0; i < colCount; i++) {
            headers[i].Format(_T("Column %d"), i + 1);
        }
    }

    // Draw header on first page
    float headerHeight = InsertTableHeader(pdf, page, font, headers, colWidths, marginLeft, currentY1, colCount);
    currentY1 -= headerHeight;
    bool isFirstPage = true;

    // Start from row 1 (skip header row)
    for (int i = statIndex + 1; i < dataArray.GetSize(); i++)
    {
        CArray<CString, CString>* pRow = dataArray[i];
        if (!pRow || pRow->GetSize() == 0) continue;

        // Check for page break
        if (currentY1 - rowHeight - 20 < marginBottom)
        {
            page = HPDF_AddPage(pdf);
            HPDF_Page_SetSize(page, pageSize, pageOrientation);
            DrawPDFooter(pdf, page, ++pageNo);
            DrawPDHeader(pdf, page,   pageNo);

            // Reset Y position and draw header on new page
            currentY1 = pageHeight - 50;
            headerHeight = InsertTableHeader(pdf, page, font, headers, colWidths, marginLeft, currentY1, colCount);
            currentY1 -= headerHeight;
        }

        HPDF_Page_SetLineWidth(page, lineWidth);
        HPDF_Page_SetFontAndSize(page, font, fontSize);

        float x = marginLeft;

        for (int col = 0; col <  colCount; col++)
        {
            CString val;
            if (pRow->GetSize() > col)
                val = pRow->GetAt(col);
            else
                val = "";
  
            CStringA valA(val);

            // ========== CENTER THE TEXT ==========
            // Calculate text width
            HPDF_Page_BeginText(page);
            float textWidth = HPDF_Page_TextWidth(page, valA);
            HPDF_Page_EndText(page);

            // Calculate centered X position
            float centeredX = x + (colWidths[col] - textWidth) / 2;

            // Ensure text doesn't go outside cell
            if (centeredX < x + cellPadding) {
                centeredX = x + cellPadding;
            }

            // Draw the text
            HPDF_Page_BeginText(page);
            HPDF_Page_TextOut(page, centeredX, currentY1 - fontSize - 2, valA);
            HPDF_Page_EndText(page);

            // Draw the rectangle
            HPDF_Page_Rectangle(page, x, currentY1 - rowHeight, colWidths[col], rowHeight);
            HPDF_Page_Stroke(page);

            x += colWidths[col];
        }
        currentY1 -= rowHeight;
    }
    currentY = currentY1;
}
void CExportPDF::SetupColumnWidths(float pageWidth, float marginLeft, float marginRight, float colWidths[], int& colCount)
{
    if (colCount == 0) return;

    double tableWidth = pageWidth - marginLeft - marginRight;

    // Use double for precision
    double colWidthsDbl[25] = { 0 };
    double totalWidth = 0.0;

    // Calculate proportional widths
    for (int k = 0; k < colCount; k++)
    {
        // Get multiplier (implement your logic here)
        double multiplier = GetColumnWidthMultiplier(k);
        colWidthsDbl[k] = (tableWidth / colCount) * multiplier;
        totalWidth += colWidthsDbl[k];
    }

    // Normalize to exactly fit table width
    double scale = tableWidth / totalWidth;
    double accumulated = 0.0;

    for (int k = 0; k < colCount; k++)
    {
        if (k < colCount - 1)
        {
            // For all but last column
            colWidthsDbl[k] *= scale;
            colWidths[k] = (float)colWidthsDbl[k];
            accumulated += colWidths[k];
        }
        else
        {
            // Last column gets remaining width to avoid rounding errors
            colWidths[k] = (float)(tableWidth - accumulated);
        }
    }
}

// AND update your GetColumnWidthMultiplier to return proper values:
float CExportPDF::GetColumnWidthMultiplier(int columnIndex)
{
    // Example: Make first column wider, others equal
    if (columnIndex == 0) return 1.5f;    // 50% wider
    if (columnIndex == 1) return 1.2f;    // 20% wider  
    return 1.0f;                          // Normal width
}
// ---------------- Table Header ----------------
float CExportPDF::InsertTableHeader(HPDF_Doc& pdf, HPDF_Page& page, HPDF_Font &font,
    const CString headers[], const float colWidths[], float startX, float startY, int colCount)
{
    const float fontSize = 10.0f;
    const float cellPadding = 2.0f;
    const float lineWidth = 0.5f;
    HPDF_Page_SetLineWidth(page, lineWidth);
    HPDF_Page_SetFontAndSize(page, HPDF_GetFont(pdf, "Helvetica-Bold", NULL), fontSize);
    float x = startX;
    float maxHeaderHeight = 0;
    float lineHeight = fontSize + 1;
    // Calculate header heights for all columns
    float headerHeights[25] = { 0 };
    for (int i = 0; i < colCount; i++)
    {
        CString text = headers[i];
        CStringArray lines;
        CString temp;

        // Split text into multiple lines if too wide
        for (int j = 0; j < text.GetLength(); j++)
        {
            temp += text[j];
            HPDF_Page_BeginText(page);
            HPDF_REAL w = HPDF_Page_TextWidth(page, CT2A(temp));
            HPDF_Page_EndText(page);

            if (w + 2 * cellPadding > colWidths[i])
            {
                if (!temp.IsEmpty())
                {
                    lines.Add(temp);
                    temp.Empty();
                }
            }
        }
        if (!temp.IsEmpty()) lines.Add(temp);

        float h = lineHeight * lines.GetCount() + 2 * cellPadding;
        headerHeights[i] = h;
        if (h > maxHeaderHeight) maxHeaderHeight = h;
    }
    // Draw headers
    x = startX;
    for (int i = 0; i < colCount; i++)
    {
        CString text = headers[i];
        CStringArray lines;
        CString temp;
        // Split text into multiple lines if too wide
        for (int j = 0; j < text.GetLength(); j++)
        {
            temp += text[j];
            HPDF_Page_BeginText(page);
            HPDF_REAL w = HPDF_Page_TextWidth(page, CT2A(temp));
            HPDF_Page_EndText(page);

            if (w + 2 * cellPadding > colWidths[i])
            {
                if (!temp.IsEmpty())
                {
                    lines.Add(temp);
                    temp.Empty();
                }
            }
        }
        if (!temp.IsEmpty()) lines.Add(temp);
        // ========== CENTER THE TEXT LINES ==========
        float textY = startY - cellPadding - fontSize;
        for (int l = 0; l < lines.GetCount(); l++)
        {
            HPDF_Page_BeginText(page);

            // Calculate text width for this line
            CStringA lineA(lines[l]);
            float lineWidth = HPDF_Page_TextWidth(page, lineA);
            // Calculate centered X position
            float centeredX = x + (colWidths[i] - lineWidth) / 2;
            // Ensure text doesn't go outside cell
            if (centeredX < x + cellPadding) {
                centeredX = x + cellPadding;
            }
            HPDF_Page_TextOut(page, centeredX, textY, lineA);
            HPDF_Page_EndText(page);
            textY -= lineHeight;
        }
        // ========== END CENTERING ==========
        // Draw cell border
        HPDF_Page_Rectangle(page, x, startY - maxHeaderHeight, colWidths[i], maxHeaderHeight);
        HPDF_Page_Stroke(page);
        x += colWidths[i];
    }
    return maxHeaderHeight;
}

void CExportPDF::InsertTableData(HPDF_Doc& pdf, HPDF_Page& page, HPDF_Font font, float startY)
{
    const float pageWidth = HPDF_Page_GetWidth(page);
    const float pageHeight = HPDF_Page_GetHeight(page);

    const float marginLeft = 25;
    const float marginRight = 25;
    const float marginBottom = 30;

    float currentY1 = (startY == -1) ? (pageHeight - 80) : startY;

    const float fontSize = 10.0f;
    const float cellPadding = 4.0f;
    const float lineWidth = 0.5f;
    const float rowHeight = fontSize + 2 * cellPadding;
    //=================Table title=============================
    HPDF_Page_BeginText(page);
    HPDF_Page_SetFontAndSize(page, HPDF_GetFont(pdf, "Helvetica-Bold", NULL), 10);
    HPDF_Page_TextOut(page, marginLeft, currentY1 - 10, "Data Information");
    HPDF_Page_EndText(page);
    currentY1 -= 15;
    //====================================================================
    int colCount = dataArray[0]->GetSize();
    float colWidths[25] = { 0 };
    SetupColumnWidths(pageWidth, marginLeft, marginRight, colWidths, colCount);
    if (colCount == 0 || dataArray.GetSize() == 0) return;
    // Create headers from first row of dataArray
    CString headers[25];
    if (dataArray.GetSize() > 0) {
        CArray<CString, CString>* pFirstRow = dataArray[0];
        for (int i = 0; i < min(pFirstRow->GetSize(), 25); i++) {
            headers[i] = pFirstRow->GetAt(i);
        }
    }
    else {
        // Fallback to generic headers
        for (int i = 0; i < colCount; i++) {
            headers[i].Format(_T("Column %d"), i + 1);
        }
    }
    // Draw header on first page
    float headerHeight = InsertTableHeader(pdf, page, font, headers, colWidths, marginLeft, currentY1, colCount);
    currentY1 -= headerHeight;
    bool isFirstPage = true;

    // Start from row 1 (skip header row)
    for (int i = 1; i < statIndex; i++)
    {
        CArray<CString, CString>* pRow = dataArray[i];
        if (!pRow || pRow->GetSize() == 0) continue;
        // Check for page break
        if (currentY1 - rowHeight - 20 < marginBottom)
        {
            page = HPDF_AddPage(pdf);
            HPDF_Page_SetSize(page, pageSize, pageOrientation);
            DrawPDFooter(pdf, page, ++pageNo);
            DrawPDHeader(pdf, page, pageNo);

            // Reset Y position and draw header on new page
            currentY1 = pageHeight - 50;
            headerHeight = InsertTableHeader(pdf, page, font, headers, colWidths, marginLeft, currentY1, colCount);
            currentY1 -= headerHeight;
        }

        HPDF_Page_SetLineWidth(page, lineWidth);
        HPDF_Page_SetFontAndSize(page, font, fontSize);

        float x = marginLeft;
        HPDF_Page_BeginText(page);

        for (int col = 0; col < colCount; col++)
        {
            CString val;
            if (pRow->GetSize() > col)
                val = pRow->GetAt(col);
            else
                val = "";

            CStringA valA(val);
            // ========== CENTER THE TEXT ==========
            HPDF_Page_EndText(page); // End text first to calculate width

            // Calculate text width
            float textWidth = HPDF_Page_TextWidth(page, valA);

            // Calculate centered X position
            float centeredX = x + (colWidths[col] - textWidth) / 2;

            // Ensure text doesn't go outside cell
            if (centeredX < x + cellPadding) {
                centeredX = x + cellPadding;
            }

            HPDF_Page_BeginText(page); // Begin text again for drawing
            HPDF_Page_TextOut(page, centeredX, currentY1 - fontSize - 2, valA);
            // ========== END CENTERING ==========

            HPDF_Page_EndText(page);
            HPDF_Page_Rectangle(page, x, currentY1 - rowHeight, colWidths[col], rowHeight);
            HPDF_Page_Stroke(page);

            x += colWidths[col];
            HPDF_Page_BeginText(page);
        }
        HPDF_Page_EndText(page);
        currentY1 -= rowHeight;
    }
    currentY = currentY1;
}
// 20251210_Mohir
void CExportPDF::LOAD_2D_3D_IMAGE(HPDF_Doc& pdf, HPDF_Page& page) {
    if (dataArray.GetSize() < statIndex + 1) return;

    CString twoDFolder = FolderPath + _T("2D\\");
    CString threeDFolder = FolderPath + _T("3D\\");
    CreateDirectory(twoDFolder, NULL);
    CreateDirectory(threeDFolder, NULL);

    // Process each probe
    for (int i = 1; i < dataArray[statIndex]->GetSize(); i++) {
        CString probe = dataArray[statIndex]->GetAt(i);
        if (probe.IsEmpty()) continue;

        HPDF_Image leftImage = NULL;
        HPDF_Image rightImage = NULL;

        // Load 2D image directly from BMP
        CString last2DBmpName = GetLastSpecificBmpName(twoDFolder, probe);
        if (!last2DBmpName.IsEmpty()) {
            CString bmpPath = twoDFolder + last2DBmpName + _T(".bmp");
            leftImage = ConvertBmpToHpdfImage(pdf, bmpPath);
        }

        // Load 3D image directly from BMP  
        CString last3DBmpName = GetLastSpecificBmpName(threeDFolder, probe);
        if (!last3DBmpName.IsEmpty()) {
            CString bmpPath = threeDFolder + last3DBmpName + _T(".bmp");
            rightImage = ConvertBmpToHpdfImage(pdf, bmpPath);
        }
        // Insert images using the efficient method
        Insert_2d_3d_Image(pdf,  page,  probe,leftImage, rightImage);

    }
}
 
CString CExportPDF::GetLastSpecific_Png_ImageName(const CString& folderPath, const CString& probe, const CString& extension = _T(""))
{
    CString searchPattern;
    if (extension.IsEmpty()) {
        // Look for any image file
        searchPattern = folderPath + probe + _T("_*.*");
    }
    else {
        searchPattern = folderPath + probe + _T("_*.") + extension;
    }

    WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile(searchPattern, &findFileData);

    CString lastName;
    CTime lastTime(0);

    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                CString filename = findFileData.cFileName;

                // Check if it's an image file (PNG or BMP)
                CString ext = filename.Right(4).MakeLower();
                if (ext == _T(".png") || ext == _T(".bmp") || extension.IsEmpty()) {
                    CTime fileTime(findFileData.ftLastWriteTime);
                    if (fileTime > lastTime) {
                        lastTime = fileTime;
                        lastName = filename;
                    }
                }
            }
        } while (FindNextFile(hFind, &findFileData));

        FindClose(hFind);
    }

    // Remove extension if needed
    if (!extension.IsEmpty() && !lastName.IsEmpty()) {
        lastName = lastName.Left(lastName.GetLength() - 4); // Remove .ext
    }

    return lastName;
}
HPDF_Image CExportPDF::ConvertPngToHpdfImage(HPDF_Doc pdf, const CString& pngPath)
{
 
    // Now try libharu
    CT2A pngPathAscii(_T("C:\\DATA\\FSM\\bot+dep\\2025\\12\\TXT\\2D\\BotCD.png"));
    HPDF_Image image = HPDF_LoadPngImageFromFile(pdf, pngPathAscii);
    return image;
}
//]]
void CExportPDF::Insert_2d_3d_Image(HPDF_Doc& pdf, HPDF_Page& page, CString probe, HPDF_Image leftImage, HPDF_Image rightImage)
{

    const float pageHeight = HPDF_Page_GetHeight(page);
    const float pageWidth = HPDF_Page_GetWidth(page);
    // Check if we need a new page for images
    float imagesSectionHeight = 150; // Approximate height needed for images
    if (currentY - imagesSectionHeight < 400) { // Not enough space on current page
        page = HPDF_AddPage(pdf);
        HPDF_Page_SetSize(page, pageSize, pageOrientation);
        DrawPDFooter(pdf, page, ++pageNo);
        DrawPDHeader(pdf, page,  pageNo);
        currentY = pageHeight - 30; // Start from top of new page
    }
    else {
        currentY = currentY - 30; // Leave some space after table
    }

    InsertTwoImagesDirect(pdf, page, currentY, probe, probe, leftImage, rightImage);
    currentY -= 30;
}
CString CExportPDF :: GetLastSpecificBmpName(const CString& folderPath, const CString& baseName)
{
    CString searchPath = folderPath + _T("\\*.bmp");
    CString lastBmpName;
    int maxNumber = -1;

    CFileFind finder;
    BOOL bWorking = finder.FindFile(searchPath);

    while (bWorking)
    {
        bWorking = finder.FindNextFile();

        if (finder.IsDots() || finder.IsDirectory())
            continue;

        CString fileTitle = finder.GetFileTitle(); // Name without extension

        // Check if file starts with our base name
        if (fileTitle.Find(baseName) == 0)
        {
            int currentNumber = -1;
            CString remainingPart = fileTitle.Mid(baseName.GetLength());

            if (remainingPart.IsEmpty())
            {
                currentNumber = 0; // Exact match like "aa"
            }
            else if (remainingPart.GetLength() > 1 && remainingPart[0] == '_')
            {
                CString numberStr = remainingPart.Mid(1);
                if (_ttoi(numberStr) > 0 || numberStr == _T("0"))
                {
                    currentNumber = _ttoi(numberStr);
                }
            }

            if (currentNumber >= 0 && currentNumber > maxNumber)
            {
                maxNumber = currentNumber;
                lastBmpName = fileTitle; // Name without extension
            }
        }
    }
    finder.Close();
    return lastBmpName; // Returns something like "aa_3" or "aa"
}
HPDF_Image CExportPDF::CreateHpdfImageFromBitmap(HPDF_Doc& pdf, Gdiplus::Bitmap* pBitmap, UINT width, UINT height)
{
    if (!pBitmap || width == 0 || height == 0) {
        return NULL;
    }

    // Create temporary buffer for RGB data
    BYTE* rgbData = new BYTE[width * height * 3]; // 3 bytes per pixel (RGB)

    if (!rgbData) {
        return NULL;
    }

    HPDF_Image image = NULL;

    try {
        // Lock bitmap bits for direct access
        Gdiplus::BitmapData bitmapData;
        Gdiplus::Rect rect(0, 0, width, height);

        if (pBitmap->LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat24bppRGB, &bitmapData) == Gdiplus::Ok) {

            BYTE* source = (BYTE*)bitmapData.Scan0;
            BYTE* dest = rgbData;

            // Convert from BGR to RGB (Windows bitmaps are BGR)
            for (UINT y = 0; y < height; y++) {
                BYTE* line = source + (y * bitmapData.Stride);
                for (UINT x = 0; x < width; x++) {
                    // BMP is BGR, we need RGB
                    dest[2] = line[0]; // Blue -> Red
                    dest[1] = line[1]; // Green -> Green  
                    dest[0] = line[2]; // Red -> Blue

                    line += 3;
                    dest += 3;
                }
            }

            // Create HPDF image from RGB data
            image = HPDF_LoadRawImageFromMem(pdf, rgbData, width, height,
                HPDF_CS_DEVICE_RGB, 8);

            pBitmap->UnlockBits(&bitmapData);
        }
    }
    catch (...) {
        TRACE(_T("Error during bitmap conversion\n"));
    }

    delete[] rgbData;
    return image;
}

HPDF_Image CExportPDF::ConvertBmpToHpdfImage(HPDF_Doc& pdf, const CString& bmpPath)
{
    if (!PathFileExists(bmpPath)) {
        TRACE(_T("BMP file does not exist: %s\n"), (LPCTSTR)bmpPath);
        return NULL;
    }

    // Initialize GDI+
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    HPDF_Image image = NULL;

    // Load BMP directly
    Gdiplus::Bitmap* pBitmap = Gdiplus::Bitmap::FromFile(CT2W(bmpPath));

    if (pBitmap && pBitmap->GetLastStatus() == Gdiplus::Ok) {
        // Get image dimensions
        UINT width = pBitmap->GetWidth();
        UINT height = pBitmap->GetHeight();

        // Create HPDF image directly from bitmap data
        HPDF_Image pdfImage = CreateHpdfImageFromBitmap(pdf, pBitmap, width, height);
        image = pdfImage;
        delete pBitmap;
    }
    else {
        AfxMessageBox(_T("Failed to load BMP image!"));
    }

    Gdiplus::GdiplusShutdown(gdiplusToken);
    return image;
}

//  implemented this function for ui update
void CExportPDF::OnBnClickedExportPdf()
{
    LoadFile(); //Load File
    ReadFileInfo(); // Read data from csv
    CTime t = CTime::GetCurrentTime();
    CString timeStr = t.Format(_T("%Y%m%d_%H%M%S")); // Example: 20251105_152530
    CString fileName;
    fileName.Format(_T("Test_Pdf_%s"), timeStr);
    ExportAsPDF(fileName);
}
void CExportPDF::ExportAsPDF(CString& csvFilePath)
{
    ResetPDFState(); //20251215_Mohir
    ReadFileInfo(); // Read data from csv
    // ======================================== CREATE PDF FOLDER IN SAME LOCATION AS CSV ===================================================
    CString pdfFilePath;
    if (!csvFilePath.IsEmpty()) {
        // Extract directory from CSV file path
        CString csvDirectory = csvFilePath;
        int lastSlash = csvDirectory.ReverseFind('\\');
        if (lastSlash != -1) {
            csvDirectory = csvDirectory.Left(lastSlash + 1); // Include the backslash
            FolderPath = csvDirectory; // store folder path...............

            // Go back one folder level
            // Find the previous slash (parent folder)
            CString parentDirectory = csvDirectory.Left(lastSlash); // Remove trailing slash
            int prevSlash = parentDirectory.ReverseFind('\\');
            if (prevSlash != -1) {
                // Get parent folder path
                parentDirectory = parentDirectory.Left(prevSlash + 1);

                // Create PDF folder in parent directory
                CString pdfFolder = parentDirectory + _T("PDF\\");
                CreateDirectory(pdfFolder, NULL);

                // Extract filename without extension from CSV
                CString csvFileName = csvFilePath;
                int dotPos = csvFileName.ReverseFind('.');
                if (dotPos != -1) {
                    csvFileName = csvFileName.Left(dotPos);
                }

                // Remove path from filename if present
                int lastSlashInFile = csvFileName.ReverseFind('\\');
                if (lastSlashInFile != -1) {
                    csvFileName = csvFileName.Mid(lastSlashInFile + 1);
                }

                // Add timestamp to make filename unique
                CTime t = CTime::GetCurrentTime();
                CString timeStr = t.Format(_T("%Y%m%d_%H%M%S"));

                // Generate PDF file path in PDF folder
                pdfFilePath = pdfFolder + csvFileName + _T("_") + timeStr + _T(".pdf");
            }
            else {
                // If no parent folder found, use original directory
                CString pdfFolder = csvDirectory + _T("PDF\\");
                CreateDirectory(pdfFolder, NULL);

                // ... rest of filename generation code
                // [Keep your existing filename generation code here]
            }
        }
    }
    else {
        // Fallback to original method if no filename provided
        CString folderPath = _T("C:\\DATA\\FSM\\PDF");
        CreateDirectory(folderPath, NULL);
        CTime t = CTime::GetCurrentTime();
        CString timeStr = t.Format(_T("%Y%m%d_%H%M%S"));
        CString fileNameOriginal;
        fileNameOriginal.Format(_T("Test_Pdf_%s"), timeStr);
        pdfFilePath = folderPath + fileNameOriginal + _T(".pdf");
    }

    CStringA pdfFilePathA(pdfFilePath);
    strcpy_s(fname, pdfFilePathA);
    // =========================================== END PDF FOLDER CREATION ================================================

    PDF = HPDF_New(NULL, NULL);
    if (!PDF) return;

    if (setjmp(env)) {
        HPDF_Free(PDF);
        return;
    }
    font = HPDF_GetFont(PDF, "Helvetica", NULL);

    page = HPDF_AddPage(PDF);
    HPDF_Page_SetSize(page, pageSize, pageOrientation);

    const float pageHeight = HPDF_Page_GetHeight(page);
    const float pageWidth = HPDF_Page_GetWidth(page);

    // ========== CORRECTED Y POSITION CALCULATION ==========
    float topMargin = 160;
    currentY = pageHeight - topMargin;
    // First box (File Info) - pass TOP position
    float firstBoxHeight = 100;
    InsertFileInfo(PDF, page, font, currentY); // Pass TOP of box
    currentY -= 15; // Move down by box height + space
    // Measurement Info - pass TOP position  
    float measurementBoxHeight = 80;
    InsertMeasurementInfo(PDF, page, font, currentY); 
    currentY -= (15); 
    InsertTableData(PDF, page, font, currentY); // Pass TOP of table
    // ========== END CORRECTED Y POSITION CALCULATION ==========
    // LOAD_2D_3D_IMAGE(PDF, page);  
    
   
}
bool CExportPDF::SaveAndClosePDF()
{
    // Save the PDF document to file
    HPDF_SaveToFile(PDF, fname);
    // Clean up - free the PDF document
    HPDF_Free(PDF);
    m_pages.RemoveAll();
    PDF = NULL;
    ExtraPage = 0;
    pageNo = 1;
    ExtraPage = 0;
    Image3dCount = 0;
    return true;
}