#ifndef DIBMP_UNLEASHED
#define DIBMP_UNLEASHED
class CDIBitmap
{
    friend class CBmpPalette;
//constructors
public:
    CDIBitmap();
    virtual ~CDIBitmap();
private:
    CDIBitmap( const CDIBitmap& dbmp ){};

//operations
public:
    inline BITMAPINFO* GetHeaderPtr();
    inline BYTE* GetPixelPtr();
    virtual void DrawDIB( CDC* pDC, int x, int y );
    virtual BOOL Load( CFile* pFile );
    RGBQUAD* GetColorTablePtr();

protected:
    int GetPalEntries() const;
    int GetPalEntries( BITMAPINFOHEADER& infoHeader ) const;
protected:
    int GetWidth() const;
    int GetHeight() const;

//implementation
private:
    BITMAPINFO* m_pInfo;
    BYTE*       m_pPixels;
};
#endif