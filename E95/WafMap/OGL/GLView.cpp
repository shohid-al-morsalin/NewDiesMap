#include "stdafx.h"

#include "Grapher.h"
#include "WafMap/DIB/DIBSurface.h"

#include "GLView.h"
#include "413App.h"

#pragma warning(disable: 4244)

inline void MinMax(float d, float& Min, float& Max) {
    if (d > Max) Max = d;
    else if (d < Min) Min = d;
}

IMPLEMENT_DYNCREATE(CGLView, CView)

CGLView::CGLView() {
    m_GridSize = 41;

    m_zScale = 0.7f;
    m_bShowLegend = TRUE;

    //====== Rendering context does not exist yet
    m_hRC = 0;
    m_hdc = NULL;

    //====== Initial image turn
    m_AngleX = 35.f;
    m_AngleY = 20.f;

    //====== Projection matrix view angle
    m_AngleView = 45.f;

    //====== Initial bkcolor
    m_BkClr = RGB(255, 255, 255);

    // Initial mode to fill the inner polygons (quads) points
    m_FillMode = GL_LINE;

    //====== Initial image shift
    m_zTrans = -3.5f;
    m_xTrans = m_yTrans = 0.f;

    //== Initial shift quantums (for rotation animation)
    m_dx = m_dy = 0.f;

    //====== Mouse is not captured
    m_bCaptured = false;
    //====== Right mouse button has not been pressed
    m_bRightButton = false;
    //====== We use quads to create the surface
    m_bQuad = FALSE;
    // Smooth colour
    m_bSmooth = TRUE;

    bInit = FALSE;
    m_nFontBase = 0;

    //====== Initial lighting params
    m_LightParam[0] = 50;    // X position
    m_LightParam[1] = 80;    // Y position
    m_LightParam[2] = 100;   // Z position
    m_LightParam[3] = 15;    // Ambient light
    m_LightParam[4] = 70;    // Diffuse light
    m_LightParam[5] = 100;   // Specular light
    m_LightParam[6] = 100;   // Ambient material
    m_LightParam[7] = 100;   // Diffuse material
    m_LightParam[8] = 40;    // Specular material
    m_LightParam[9] = 70;    // Shininess material
    m_LightParam[10] = 0;    // Emission material

    // Initialize color arrays
    cr[0] = 255;    cg[0] = 0;      cb[0] = 255;
    cr[1] = 204;    cg[1] = 0;      cb[1] = 255;
    cr[2] = 153;    cg[2] = 0;      cb[2] = 255;
    cr[3] = 102;    cg[3] = 0;      cb[3] = 255;
    cr[4] = 51;     cg[4] = 0;      cb[4] = 255;

    cr[5] = 0;      cg[5] = 0;      cb[5] = 255;
    cr[6] = 0;      cg[6] = 51;     cb[6] = 255;
    cr[7] = 0;      cg[7] = 102;    cb[7] = 255;
    cr[8] = 0;      cg[8] = 153;    cb[8] = 255;
    cr[9] = 0;      cg[9] = 204;    cb[9] = 255;

    cr[10] = 0;     cg[10] = 255;   cb[10] = 204;
    cr[11] = 0;     cg[11] = 255;   cb[11] = 153;
    cr[12] = 0;     cg[12] = 255;   cb[12] = 102;
    cr[13] = 0;     cg[13] = 255;   cb[13] = 51;
    cr[14] = 0;     cg[14] = 255;   cb[14] = 0;

    cr[15] = 0;     cg[15] = 255;   cb[15] = 0;
    cr[16] = 51;    cg[16] = 255;   cb[16] = 0;
    cr[17] = 102;   cg[17] = 255;   cb[17] = 0;
    cr[18] = 153;   cg[18] = 255;   cb[18] = 0;
    cr[19] = 204;   cg[19] = 255;   cb[19] = 0;

    cr[20] = 255;   cg[20] = 204;   cb[20] = 0;
    cr[21] = 255;   cg[21] = 153;   cb[21] = 0;
    cr[22] = 255;   cg[22] = 102;   cb[22] = 0;
    cr[23] = 255;   cg[23] = 51;    cb[23] = 0;
    cr[24] = 255;   cg[24] = 0;     cb[24] = 0;

    cr[25] = 255;   cg[25] = 0;     cb[25] = 0;

    pDIBSurf = NULL;
    pRcp = NULL;
}

CGLView::~CGLView() {
    // Cleanup OpenGL resources
    if (wglGetCurrentContext() == m_hRC) {
        wglMakeCurrent(NULL, NULL);
    }

    if (m_hRC) {
        wglDeleteContext(m_hRC);
        m_hRC = NULL;
    }

    // Release the device context
    if (m_hdc && !pDIBSurf) {
        ::ReleaseDC(GetSafeHwnd(), m_hdc);
        m_hdc = NULL;
    }

    if (pDIBSurf) {
        delete pDIBSurf;
        pDIBSurf = NULL;
    }

    // Delete font
    DeleteFont();
}

BEGIN_MESSAGE_MAP(CGLView, CView)
    //{{AFX_MSG_MAP(CGLView)
    ON_WM_ERASEBKGND()
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_TIMER()
    ON_WM_LBUTTONUP()
    ON_WM_LBUTTONDOWN()
    ON_WM_MOUSEMOVE()
    ON_WM_RBUTTONDOWN()
    ON_WM_RBUTTONUP()
    ON_WM_DESTROY()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGLView drawing

void CGLView::OnDraw(CDC* pDC) {
    // Check if OpenGL context is valid
    if (!m_hRC || !m_hdc) {
        return;
    }

    // Make sure our context is current
    if (!wglMakeCurrent(m_hdc, m_hRC)) {
        TRACE("Failed to make OpenGL context current in OnDraw\n");
        return;
    }

    //========== Clear the background and Z-buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //========== Clean the modeling matrix (make it equal the unity matrix)
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    //======= At first set the light (otherwise it will rotate with the image)
    // SetLight();

    glPushMatrix();
    //====== Change the modeling matrix coefficients in order
    glTranslatef(m_xTrans, m_yTrans, m_zTrans - .25);    // to shift
    glRotatef(m_AngleX, 1.0f, 0.0f, 0.0f);    // and to rotate
    glRotatef(m_AngleY, 0.0f, 1.0f, 0.0f);

    //====== the following vertices coordinates (they are being multiplied by matrix)
    glCallList(1);
    glCallList(2);

    glPopMatrix();

    if (m_bShowLegend && pRcp) {
        ShowLegend(pRcp);
    }

    //====== Switch back and front buffers (to show what happened)
    if (!SwapBuffers(m_hdc)) {
        TRACE("SwapBuffers failed: %d\n", GetLastError());
    }
    GdiFlush();
}

void CGLView::OnPrint(CDC* pDC, CPrintInfo* pInfo) {
    // 1. Determine the DIB size for either printing or print preview.
    HDC         m_hOldDC = NULL;
    HDC         m_hMemDC = NULL;
    HGLRC       m_hOldRC = NULL;
    HGLRC       m_hMemRC = NULL;
    LPVOID      m_pBitmapBits = NULL;
    HBITMAP     m_hDib = NULL;
    BITMAPINFO  m_bmi;
    CSize m_szPage;
    CRect rcClient;
    GetClientRect(&rcClient);
    float fClientRatio = float(rcClient.Height()) / rcClient.Width();

    // Get page size
    m_szPage.cx = pDC->GetDeviceCaps(HORZRES);
    m_szPage.cy = pDC->GetDeviceCaps(VERTRES);

    CSize szDIB;
    if (pInfo->m_bPreview) {
        // Use screen resolution for preview.
        szDIB.cx = rcClient.Width();
        szDIB.cy = rcClient.Height();
    }
    else {
        // Use higher resolution for printing.
        // Adjust size according screen's ratio.
        if (m_szPage.cy > fClientRatio * m_szPage.cx) {
            // View area is wider than Printer area
            szDIB.cx = m_szPage.cx;
            szDIB.cy = long(fClientRatio * m_szPage.cx);
        }
        else {
            // View area is narrower than Printer area
            szDIB.cx = long(float(m_szPage.cy) / fClientRatio);
            szDIB.cy = m_szPage.cy;
        }
        // Reduce the Resolution if the Bitmap size is too big.
        while (szDIB.cx * szDIB.cy > 20e6) {
            szDIB.cx = szDIB.cx / 2;
            szDIB.cy = szDIB.cy / 2;
        }
    }

    TRACE("Buffer size: %d x %d = %6.2f MB\n", szDIB.cx, szDIB.cy, szDIB.cx * szDIB.cy * 0.000001);

    // 2. Create DIB Section
    memset(&m_bmi, 0, sizeof(BITMAPINFO));
    m_bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    m_bmi.bmiHeader.biWidth = szDIB.cx;
    m_bmi.bmiHeader.biHeight = szDIB.cy;
    m_bmi.bmiHeader.biPlanes = 1;
    m_bmi.bmiHeader.biBitCount = 24;
    m_bmi.bmiHeader.biCompression = BI_RGB;
    m_bmi.bmiHeader.biSizeImage = szDIB.cx * szDIB.cy * 3;

    HDC hDC = ::GetDC(m_hWnd);
    if (!hDC) {
        TRACE("Failed to get DC for printing\n");
        return;
    }

    m_hDib = ::CreateDIBSection(hDC, &m_bmi, DIB_RGB_COLORS, &m_pBitmapBits, NULL, (DWORD)0);
    ::ReleaseDC(m_hWnd, hDC);

    if (!m_hDib) {
        TRACE("Failed to create DIB section\n");
        return;
    }

    // 3. Create memory DC, and associate it with the DIB.
    m_hMemDC = ::CreateCompatibleDC(NULL);
    if (!m_hMemDC) {
        TRACE("Failed to create compatible DC\n");
        DeleteObject(m_hDib);
        m_hDib = NULL;
        return;
    }
    SelectObject(m_hMemDC, m_hDib);

    // 4. Setup memory DC's pixel format.
    if (!SetDCPixelFormat(m_hMemDC, PFD_DRAW_TO_BITMAP | PFD_SUPPORT_OPENGL | PFD_STEREO_DONTCARE)) {
        TRACE("Failed to set DC pixel format for printing\n");
        DeleteObject(m_hDib);
        m_hDib = NULL;
        DeleteDC(m_hMemDC);
        m_hMemDC = NULL;
        return;
    }

    // 5. Create memory RC with error handling
    m_hMemRC = ::wglCreateContext(m_hMemDC);
    if (!m_hMemRC) {
        DWORD error = GetLastError();
        TRACE("wglCreateContext failed with error: %d\n", error);

        DeleteObject(m_hDib);
        m_hDib = NULL;
        DeleteDC(m_hMemDC);
        m_hMemDC = NULL;
        return;
    }

    // 6. Store old DC and RC
    m_hOldDC = ::wglGetCurrentDC();
    m_hOldRC = ::wglGetCurrentContext();

    // 7. Make the memory RC current
    if (!::wglMakeCurrent(m_hMemDC, m_hMemRC)) {
        TRACE("Failed to make memory RC current\n");
        ::wglDeleteContext(m_hMemRC);
        DeleteObject(m_hDib);
        DeleteDC(m_hMemDC);

        // Try to restore old context
        if (m_hOldDC && m_hOldRC) {
            ::wglMakeCurrent(m_hOldDC, m_hOldRC);
        }
        return;
    }

    // 8. Set OpenGL state for memory RC.
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);

    // Set background color
    GLclampf red = GetRValue(m_BkClr) / 255.f,
        green = GetGValue(m_BkClr) / 255.f,
        blue = GetBValue(m_BkClr) / 255.f;
    glClearColor(red, green, blue, 0.f);

    // Clear buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set projection
    double dAspect = szDIB.cx <= szDIB.cy ? double(szDIB.cy) / szDIB.cx : double(szDIB.cx) / szDIB.cy;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(m_AngleView, dAspect, 0.1, 10000.);
    glViewport(0, 0, szDIB.cx, szDIB.cy);

    // Set modelview
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Apply transformations
    glTranslatef(m_xTrans, m_yTrans, m_zTrans);
    glRotatef(m_AngleX, 1.0f, 0.0f, 0.0f);
    glRotatef(m_AngleY, 0.0f, 1.0f, 0.0f);

    // Draw the scene
    glCallList(1);

    // Flush OpenGL commands
    glFinish();

    // Now the image is in the DIB already. We don't need the memory RC anymore.
    // 1. Release memory RC, and restore the old DC and RC.
    ::wglMakeCurrent(NULL, NULL);
    ::wglDeleteContext(m_hMemRC);

    // Restore last DC and RC
    if (m_hOldDC && m_hOldRC) {
        ::wglMakeCurrent(m_hOldDC, m_hOldRC);
    }

    // 2. Calculate the target size according to the image size, and orientation of the paper.
    float fBmiRatio = float(m_bmi.bmiHeader.biHeight) / m_bmi.bmiHeader.biWidth;
    CSize szTarget;
    if (m_szPage.cx > m_szPage.cy) {  // Landscape page
        if (fBmiRatio < 1) {  // Landscape image
            szTarget.cx = m_szPage.cx;
            szTarget.cy = long(fBmiRatio * m_szPage.cx);
        }
        else {  // Portrait image
            szTarget.cx = long(m_szPage.cy / fBmiRatio);
            szTarget.cy = m_szPage.cy;
        }
    }
    else {  // Portrait page
        if (fBmiRatio < 1) {  // Landscape image
            szTarget.cx = m_szPage.cx;
            szTarget.cy = long(fBmiRatio * m_szPage.cx);
        }
        else {  // Portrait image
            szTarget.cx = long(m_szPage.cy / fBmiRatio);
            szTarget.cy = m_szPage.cy;
        }
    }

    CSize szOffset((m_szPage.cx - szTarget.cx) / 2, (m_szPage.cy - szTarget.cy) / 2);

    // 3. Stretch image to fit in the target size.
    int nRet = ::StretchDIBits(pDC->GetSafeHdc(),
        szOffset.cx, szOffset.cy,
        szTarget.cx, szTarget.cy,
        0, 0,
        m_bmi.bmiHeader.biWidth, m_bmi.bmiHeader.biHeight,
        (GLubyte*)m_pBitmapBits,
        &m_bmi, DIB_RGB_COLORS, SRCCOPY);

    if (nRet == GDI_ERROR) {
        TRACE("Failed in StretchDIBits()\n");
    }

    // 4. Release memory.
    DeleteObject(m_hDib);
    m_hDib = NULL;
    DeleteDC(m_hMemDC);
    m_hMemDC = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CGLView diagnostics

#ifdef _DEBUG
void CGLView::AssertValid() const {
    CView::AssertValid();
}

void CGLView::Dump(CDumpContext& dc) const {
    CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CGLView message handlers

void CGLView::DefaultGraphic() {
}

BOOL CGLView::PreCreateWindow(CREATESTRUCT& cs) {
    cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
    return CView::PreCreateWindow(cs);
}

void CGLView::SetLight() {
    //====== Both surface sides are considered when calculating
    //====== each pixel color with the lighting formula
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);

    //====== Light source position depends on the object sizes scaled to (0,100)
    float fPos[] = {
        (m_LightParam[0] - 50.0f),
        (m_LightParam[1] - 50.0f),
        (m_LightParam[2] - 50.0f),
        1.f
    };
    glLightfv(GL_LIGHT0, GL_POSITION, fPos);

    //====== Ambient light intensity
    float f = m_LightParam[3] / 100.f;
    float fAmbient[4] = { f, f, f, 0.f };
    glLightfv(GL_LIGHT0, GL_AMBIENT, fAmbient);

    //====== Diffuse light intensity
    f = m_LightParam[4] / 100.f;
    float fDiffuse[4] = { f, f, f, 0.f };
    glLightfv(GL_LIGHT0, GL_DIFFUSE, fDiffuse);

    //====== Specular light intensity
    f = m_LightParam[5] / 100.f;
    float fSpecular[4] = { f, f, f, 0.f };
    glLightfv(GL_LIGHT0, GL_SPECULAR, fSpecular);

    //====== Surface material reflection properties for each light component
    f = m_LightParam[6] / 100.f;
    float fAmbMat[4] = { f, f, f, 0.f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, fAmbMat);

    f = m_LightParam[7] / 100.f;
    float fDifMat[4] = { f, f, f, 1.f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, fDifMat);

    f = m_LightParam[8] / 100.f;
    float fSpecMat[4] = { f, f, f, 0.f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, fSpecMat);

    //====== Material shininess
    float fShine = 128 * m_LightParam[9] / 100.f;
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, fShine);

    //====== Material light emission property
    f = m_LightParam[10] / 100.f;
    float fEmission[4] = { f, f, f, 0.f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, fEmission);
}

BOOL CGLView::OnEraseBkgnd(CDC* pDC) {
    return TRUE;
}

int CGLView::OnCreate(LPCREATESTRUCT lpCreateStruct) {
    if (CView::OnCreate(lpCreateStruct) == -1) {
        return -1;
    }

    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA,
        24, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0,
        32, 0, 0,
        PFD_MAIN_PLANE,
        0, 0, 0, 0
    };

    // Check if we're using DIB surface or window DC
    if (pDIBSurf) {
        sizeBitmap.cx = 50;
        sizeBitmap.cy = 50;
        if (!pDIBSurf->Create(sizeBitmap.cx, sizeBitmap.cy, (HPALETTE)NULL)) {
            TRACE("Failed to create DIBSurface\n");
            return -1;
        }
        m_hdc = pDIBSurf->GetDC()->GetSafeHdc();
    }
    else {
        m_hdc = ::GetDC(GetSafeHwnd());
        if (!m_hdc) {
            TRACE("Failed to get window DC\n");
            return -1;
        }
    }

    // Check if we can get a valid pixel format
    int maxPixelFormats = DescribePixelFormat(m_hdc, 0, 0, NULL);
    if (maxPixelFormats == 0) {
        TRACE("No pixel formats available\n");
        if (!pDIBSurf) {
            ::ReleaseDC(GetSafeHwnd(), m_hdc);
        }
        return -1;
    }

    // Ask to find the nearest compatible pixel-format
    int iD = ChoosePixelFormat(m_hdc, &pfd);
    if (!iD) {
        TRACE("ChoosePixelFormat::Error\n");
        if (!pDIBSurf) {
            ::ReleaseDC(GetSafeHwnd(), m_hdc);
        }
        return -1;
    }

    // Try to set this format
    if (!SetPixelFormat(m_hdc, iD, &pfd)) {
        TRACE("SetPixelFormat::Error\n");
        if (!pDIBSurf) {
            ::ReleaseDC(GetSafeHwnd(), m_hdc);
        }
        return -1;
    }

    // Try to create the OpenGL rendering context
    if (!(m_hRC = wglCreateContext(m_hdc))) {
        TRACE("wglCreateContext::Error\n");
        if (!pDIBSurf) {
            ::ReleaseDC(GetSafeHwnd(), m_hdc);
        }
        return -1;
    }

    // Try to put it in action
    if (!wglMakeCurrent(m_hdc, m_hRC)) {
        TRACE("wglMakeCurrent::Error\n");
        wglDeleteContext(m_hRC);
        m_hRC = NULL;
        if (!pDIBSurf) {
            ::ReleaseDC(GetSafeHwnd(), m_hdc);
        }
        return -1;
    }

    //====== Now you can issue OpenGL commands (i.e. call gl*** functions)
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);

    //====== Our function to set the background
    SetBkColor();

    return 0;
}

void CGLView::SetBkColor() {
    if (!m_hRC) return;

    // Make sure context is current
    if (!wglMakeCurrent(m_hdc, m_hRC)) {
        return;
    }

    //====== Split the color to 3 components
    GLclampf red = GetRValue(m_BkClr) / 255.f,
        green = GetGValue(m_BkClr) / 255.f,
        blue = GetBValue(m_BkClr) / 255.f;
    //====== Set the clear (background) color
    glClearColor(red, green, blue, 0.f);

    //====== Actual background erasure
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void CGLView::DrawScene() {
    // This method should be implemented in derived classes
    // or overridden as needed
}

void CGLView::OnSize(UINT nType, int cx, int cy) {
    CView::OnSize(nType, cx, cy);

    if (!m_hRC || cx == 0 || cy == 0) {
        return;
    }

    // Make sure context is current
    if (!wglMakeCurrent(m_hdc, m_hRC)) {
        return;
    }

    double dAspect = (cx == 0 || cy == 0) ? 1.0 : (double)cx / (double)cy;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(m_AngleView, dAspect, 0.1, 10000.);
    glViewport(0, 0, cx, cy);

    // Restore modelview matrix
    glMatrixMode(GL_MODELVIEW);
}

void CGLView::OnTimer(UINT nIDEvent) {
    if (nIDEvent == 1) {
        KillTimer(1);
        LimitAngles();
        //====== Increase the angles
        m_AngleX += m_dy;
        m_AngleY += m_dx;
        Invalidate(FALSE);
        SetTimer(1, 25, 0);
    }
    CView::OnTimer(nIDEvent);
}

void CGLView::LimitAngles() {
    while (m_AngleX < -360.f)
        m_AngleX += 360.f;
    while (m_AngleX > 360.f)
        m_AngleX -= 360.f;
    while (m_AngleY < -360.f)
        m_AngleY += 360.f;
    while (m_AngleY > 360.f)
        m_AngleY -= 360.f;
}

void CGLView::OnLButtonUp(UINT nFlags, CPoint point) {
    //====== If we captured the mouse,
    if (m_bCaptured) {
        //=== query the desired quantum value
        //=== if it exceeds the sensitivity threshold
        if (fabs(m_dx) > 0.5f || fabs(m_dy) > 0.5f) {
            //=== Turn on the constant rotation
            SetTimer(1, 33, 0);
        }
        else {
            //=== Turn off the constant rotation
            KillTimer(1);
        }

        //====== Clear the capture flag
        m_bCaptured = false;
        ReleaseCapture();
    }

    CView::OnLButtonUp(nFlags, point);
}

void CGLView::OnLButtonDown(UINT nFlags, CPoint point) {
    //====== Stop rotation
    KillTimer(1);

    //====== Zero the quantums
    m_dx = 0.f;
    m_dy = 0.f;

    //====== Capture the mouse messages and direct them in our window
    SetCapture();
    //====== Remember the fact
    m_bCaptured = true;
    //====== and where it happened
    m_pt = point;

    CView::OnLButtonDown(nFlags, point);
}

void CGLView::OnMouseMove(UINT nFlags, CPoint point) {
    if (m_bCaptured) {
        // Desired rotation speed components
        m_dy = float(point.y - m_pt.y) / 40.f;
        m_dx = float(point.x - m_pt.x) / 40.f;

        //====== If Ctrl was pressed
        if (nFlags & MK_CONTROL) {
            //=== we shift (translate) the image
            m_xTrans += m_dx;
            m_yTrans -= m_dy;
        }
        else {
            //====== If the right mouse button is pressed
            if (m_bRightButton) {
                //====== we shift along the z-axis
                m_zTrans += 4 * (m_dx + m_dy);
            }
            else {
                //====== otherwise we rotate the image
                LimitAngles();
                double a = fabs(m_AngleX);
                if (90. < a && a < 270.) {
                    m_dx = -m_dx;
                }
                m_AngleX += 2.8 * m_dy;
                m_AngleY += 2.8 * m_dx;
            }
        }
        //=== In any case we should store the coordinates
        m_pt = point;
        Invalidate(FALSE);
    }

    CView::OnMouseMove(nFlags, point);
}

void CGLView::OnRButtonDown(UINT nFlags, CPoint point) {
    //====== Remember the fact
    m_bRightButton = true;

    //====== and reproduce the left button response
    OnLButtonDown(nFlags, point);

    CView::OnRButtonDown(nFlags, point);
}

void CGLView::OnRButtonUp(UINT nFlags, CPoint point) {
    m_bRightButton = false;
    if (m_bCaptured) {
        m_bCaptured = false;
        ReleaseCapture();
    }

    CView::OnRButtonUp(nFlags, point);
}

void CGLView::OnOptionFill() {
    m_FillMode = m_FillMode == GL_FILL ? GL_LINE : GL_FILL;

    DrawScene();
    Invalidate(FALSE);
    UpdateWindow();
}

void CGLView::OnUpdateOptionFill(CCmdUI* pCmdUI) {
    pCmdUI->SetCheck(m_FillMode == GL_FILL);
}

void CGLView::OnOptionQuad() {
    // m_bQuad = !m_bQuad;

    DrawScene();
    Invalidate(FALSE);
    UpdateWindow();
}

void CGLView::OnUpdateOptionQuad(CCmdUI* pCmdUI) {
    // pCmdUI->SetCheck(m_bQuad == TRUE);
}

void CGLView::OnOptionBackground() {
    CColorDialog dlg(m_BkClr);
    dlg.m_cc.Flags |= CC_FULLOPEN;
    if (dlg.DoModal() == IDOK) {
        m_BkClr = dlg.m_cc.rgbResult;
        SetBkColor();
        Invalidate(FALSE);
    }
}

void CGLView::OnOptionProperties() {
    // Implementation for properties dialog
}

void CGLView::SetLightParam(short lp, int nPos) {
    if (lp >= 0 && lp < 11) {
        m_LightParam[lp] = nPos;
        Invalidate(FALSE);
    }
}

void CGLView::GetLightParams(int* pPos) {
    if (pPos) {
        for (int i = 0; i < 11; i++) {
            pPos[i] = m_LightParam[i];
        }
    }
}

void CGLView::OnOptionSmooth() {
    m_bSmooth = !m_bSmooth;

    DrawScene();
    Invalidate(FALSE);
    UpdateWindow();
}

void CGLView::OnUpdateOptionSmooth(CCmdUI* pCmdUI) {
    pCmdUI->SetCheck(m_bSmooth);
}

BOOL CGLView::SetDCPixelFormat(HDC hDC, DWORD dwFlags) {
    PIXELFORMATDESCRIPTOR pixelDesc;

    pixelDesc.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pixelDesc.nVersion = 1;

    pixelDesc.dwFlags = dwFlags;
    pixelDesc.iPixelType = PFD_TYPE_RGBA;
    pixelDesc.cColorBits = 24;
    pixelDesc.cRedBits = 8;
    pixelDesc.cRedShift = 16;
    pixelDesc.cGreenBits = 8;
    pixelDesc.cGreenShift = 8;
    pixelDesc.cBlueBits = 8;
    pixelDesc.cBlueShift = 0;
    pixelDesc.cAlphaBits = 0;
    pixelDesc.cAlphaShift = 0;
    pixelDesc.cAccumBits = 64;
    pixelDesc.cAccumRedBits = 16;
    pixelDesc.cAccumGreenBits = 16;
    pixelDesc.cAccumBlueBits = 16;
    pixelDesc.cAccumAlphaBits = 0;
    pixelDesc.cDepthBits = 32;
    pixelDesc.cStencilBits = 8;
    pixelDesc.cAuxBuffers = 0;
    pixelDesc.iLayerType = PFD_MAIN_PLANE;
    pixelDesc.bReserved = 0;
    pixelDesc.dwLayerMask = 0;
    pixelDesc.dwVisibleMask = 0;
    pixelDesc.dwDamageMask = 0;

    int nPixelIndex = ::ChoosePixelFormat(hDC, &pixelDesc);
    if (nPixelIndex == 0) {
        nPixelIndex = 1;
        if (::DescribePixelFormat(hDC, nPixelIndex,
            sizeof(PIXELFORMATDESCRIPTOR), &pixelDesc) == 0) {
            return false;
        }
    }

    if (!::SetPixelFormat(hDC, nPixelIndex, &pixelDesc)) {
        return false;
    }

    return true;
}

UINT CGLView::GetPropDlgID() {
    return 0;
}

void CGLView::Cart2Polar(float& r, float& a) {
    float x = r;
    float z = a;
    if (!x && !z) {
        r = 0; a = 0;
        return;
    }
    r = float(sqrt(x * x + z * z));
    a = float(atan(z / x) * 180.0f / 3.141592654f);

    if (x < 0) {
        a = 180 + a;
    }
    else {
        if (z < 0) a = 360 + a;
    }
}

void CGLView::Polar2Cart(float& x, float& y) {
    float r = x;
    float a = y * 2.0f * 3.141592654f / 360.0f;

    x = float(r * cos(a));
    y = float(r * sin(a));

    if (fabs(x) < 1e-3) {
        x = 0;
    }
    if (fabs(y) < 1e-3) {
        y = 0;
    }
}

void CGLView::OnInitialUpdate() {
    CView::OnInitialUpdate();

    CreateFont();
    DefaultGraphic();
    DrawScene();

    if (p413App) {
        p413App->Global.is3Ddone = true;
    }
}

void CGLView::ShowLegend(CRecipe413* pRcp) {
    // Implementation for showing legend
    // This should be implemented in derived classes
}

void CGLView::CreateFont(string fontName, int height) {
    if (!m_hdc) return;

    HDC m_hDC = ::GetDC(m_hWnd);
    if (!m_hDC) return;

    // Make sure OpenGL context is current
    if (!wglMakeCurrent(m_hdc, m_hRC)) {
        ::ReleaseDC(m_hWnd, m_hDC);
        return;
    }

    CFont font;

    // Delete old font if exists
    if (m_nFontBase != 0) {
        ::glDeleteLists(m_nFontBase, 96);
        m_nFontBase = 0;
    }

    if ((m_nFontBase = ::glGenLists(96)) == 0) {
        TRACE("Can't create font display lists\n");
        ::ReleaseDC(m_hWnd, m_hDC);
        return;
    }

    if (fontName == _T("symbol")) {
        font.CreateFont(height, 0, 0, 0, FW_NORMAL, 0, FALSE, FALSE,
            SYMBOL_CHARSET, OUT_TT_PRECIS,
            CLIP_DEFAULT_PRECIS, DRAFT_QUALITY,
            DEFAULT_PITCH, fontName.c_str());
    }
    else {
        font.CreateFont(height, 0, 0, 0, FW_NORMAL, 0, FALSE, FALSE,
            ANSI_CHARSET, OUT_TT_PRECIS,
            CLIP_DEFAULT_PRECIS, DRAFT_QUALITY,
            DEFAULT_PITCH, fontName.c_str());
    }

    ::SelectObject(m_hDC, font);
    ::wglUseFontBitmaps(m_hDC, 32, 96, m_nFontBase);

    ::ReleaseDC(m_hWnd, m_hDC);
}

void CGLView::DeleteFont() {
    if (m_nFontBase == 0) return;

    if (wglGetCurrentContext() == m_hRC) {
        ::glDeleteLists(m_nFontBase, 96);
    }
    else if (m_hRC && m_hdc) {
        if (wglMakeCurrent(m_hdc, m_hRC)) {
            ::glDeleteLists(m_nFontBase, 96);
            wglMakeCurrent(NULL, NULL);
        }
    }
    m_nFontBase = 0;
}

void CGLView::OnDestroy() {
    DeleteFont();

    // Cleanup OpenGL resources
    if (wglGetCurrentContext() == m_hRC) {
        wglMakeCurrent(NULL, NULL);
    }

    if (m_hRC) {
        wglDeleteContext(m_hRC);
        m_hRC = NULL;
    }

    if (m_hdc && !pDIBSurf) {
        ::ReleaseDC(GetSafeHwnd(), m_hdc);
        m_hdc = NULL;
    }

    CView::OnDestroy();
}

// Additional helper method to recreate context if needed
BOOL CGLView::RecreateContext() {
    // Cleanup existing context
    if (m_hRC) {
        if (wglGetCurrentContext() == m_hRC) {
            wglMakeCurrent(NULL, NULL);
        }
        wglDeleteContext(m_hRC);
        m_hRC = NULL;
    }

    // Recreate the context
    if (!(m_hRC = wglCreateContext(m_hdc))) {
        TRACE("Failed to recreate OpenGL context\n");
        return FALSE;
    }

    if (!wglMakeCurrent(m_hdc, m_hRC)) {
        TRACE("Failed to make OpenGL context current\n");
        wglDeleteContext(m_hRC);
        m_hRC = NULL;
        return FALSE;
    }

    // Reinitialize OpenGL state
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    SetBkColor();

    return TRUE;
}