#include "stdafx.h"
#include "dib256.h"

CDIBitmap::CDIBitmap()
{
    m_pInfo = 0;
    m_pPixels = 0;
}

CDIBitmap::~CDIBitmap()
{
    delete [] (BYTE*)m_pInfo;
    delete [] m_pPixels;
}

BOOL CDIBitmap::Load( CFile* pFile )
{
    ASSERT( pFile );
    BOOL fReturn = TRUE;
    try
    {
        delete [] (BYTE*)m_pInfo;
        delete [] m_pPixels;
        m_pInfo = 0;
        m_pPixels = 0;
        DWORD       dwStart = pFile->GetPosition();
        //
        // Check to make sure we have a bitmap. The first two bytes must
        // be 'B' and 'M'.
        BITMAPFILEHEADER fileHeader;
        pFile->Read(&fileHeader, sizeof(fileHeader));
        if( fileHeader.bfType != 0x4D42 )
            throw TEXT("Error:Unexpected file type, not a DIB\n");
		
        BITMAPINFOHEADER infoHeader;
        pFile->Read( &infoHeader, sizeof(infoHeader) );
        if( infoHeader.biSize != sizeof(infoHeader) )
            throw TEXT("Error:OS2 PM BMP Format not supported\n");
		
        // Store the sizes of the DIB structures
        int cPaletteEntries = GetPalEntries( infoHeader );
        int cColorTable = 256 * sizeof(RGBQUAD);
        int cInfo = sizeof(BITMAPINFOHEADER) + cColorTable;
        int cPixels = fileHeader.bfSize - fileHeader.bfOffBits;
        //
        // Allocate space for a new bitmap info header, and copy
        // the info header that was loaded from the file. Read the
        // the file and store the results in the color table.
        m_pInfo = (BITMAPINFO*)new BYTE[cInfo];
        memcpy( m_pInfo, &infoHeader, sizeof(BITMAPINFOHEADER) );
        pFile->Read( ((BYTE*)m_pInfo) + sizeof(BITMAPINFOHEADER),
			cColorTable );
        //
        // Allocate space for the pixel area, and load the pixel
        // info from the file.
        m_pPixels = new BYTE[cPixels];
        pFile->Seek(dwStart + fileHeader.bfOffBits, CFile::begin);
        pFile->Read( m_pPixels, cPixels );
    }
	catch( TCHAR* psz )
    {
        TRACE( psz );
        fReturn = FALSE;
    }
	return fReturn;
}

//
// DrawDib uses StretchDIBits to display the bitmap.
void CDIBitmap::DrawDIB( CDC* pDC, int x, int y )
{
    ASSERT( pDC );
    HDC     hdc = pDC->GetSafeHdc();
    if( m_pInfo )
        StretchDIBits( hdc,
                       x,
                       y,
                       GetWidth(),
                       GetHeight(),
                       0,
                       0,
                       GetWidth(),
                       GetHeight(),
                       GetPixelPtr(),
                       GetHeaderPtr(),
                       DIB_RGB_COLORS,
                       SRCCOPY );
}

BITMAPINFO* CDIBitmap::GetHeaderPtr()
{
    ASSERT( m_pInfo );
    ASSERT( m_pPixels );
    return m_pInfo;
}

RGBQUAD* CDIBitmap::GetColorTablePtr()
{
    ASSERT( m_pInfo );
    ASSERT( m_pPixels );
    RGBQUAD* pColorTable = 0;
    if( m_pInfo != 0 )
    {
        int cOffset = sizeof(BITMAPINFOHEADER);
        pColorTable = (RGBQUAD*)(((BYTE*)(m_pInfo)) + cOffset);
    }
    return pColorTable;
}

BYTE* CDIBitmap::GetPixelPtr()
{
    ASSERT( m_pInfo );
    ASSERT( m_pPixels );
    return m_pPixels;
}

int CDIBitmap::GetWidth() const
{
    ASSERT( m_pInfo );
    return m_pInfo->bmiHeader.biWidth;
}

int CDIBitmap::GetHeight() const
{
    ASSERT( m_pInfo );
    return m_pInfo->bmiHeader.biHeight;
}

int CDIBitmap::GetPalEntries() const
{
    ASSERT( m_pInfo );
    return GetPalEntries( *(BITMAPINFOHEADER*)m_pInfo );
}

int CDIBitmap::GetPalEntries( BITMAPINFOHEADER& infoHeader ) const
{
    int nReturn;
    if( infoHeader.biClrUsed == 0 )
    {
        nReturn = ( 1 << infoHeader.biBitCount );
    }
    else
        nReturn = infoHeader.biClrUsed;

    return nReturn;
}
