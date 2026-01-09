// MeshView.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "MeshView.h"
#include "MeshDoc.h"
#include "EditColor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMeshView

IMPLEMENT_DYNCREATE(CMeshView, CView)

CMeshView::CMeshView()
{
	pDoc = new CMeshDoc;
	if (!pDoc)
	{
		ASSERT(0);
	}

	nSPL = 12;
	m_Title = "";

	// OpenGL
	m_hGLContext = NULL;
	m_GLPixelIndex = 0;
	
	// Mouse
	m_LeftButtonDown = FALSE;
	m_RightButtonDown = FALSE;
	
	// Colors
	m_ClearColorRed   = 0.4f;
	m_ClearColorGreen = 0.3f;
	m_ClearColorBlue  = 0.2f;
	
	// Animation
	m_StepRotationX = 0.0f;
	m_StepRotationY = 5.0f;
	m_StepRotationZ = 0.0f;
	
	InitGeometry();

	LoadGeometry();
}

CMeshView::~CMeshView()
{
	if (pDoc)
	{
		delete pDoc;
	}
}

void CMeshView::PostNcDestroy() 
{
	if (pDoc)
	{
		delete pDoc;
		pDoc = NULL;
	}
	delete this;
}



//********************************************
// InitGeometry
//********************************************
void CMeshView::InitGeometry(void)
{
	m_xRotation = 0.0f;
	m_yRotation = 0.0f;
	m_zRotation = 0.0f;
	
	m_xTranslation = 0.0f;
	m_yTranslation = 0.0f;
	m_zTranslation = -5.0f;
	
	m_xScaling = 1.0f;
	m_yScaling = 1.0f;
	m_zScaling = 1.0f;
	
	m_SpeedRotation = 1.0f / 3.0f;
	m_SpeedTranslation = 1.0f / 50.0f;
	
	m_xyRotation = 1;

	
}


BEGIN_MESSAGE_MAP(CMeshView, CView)
	//{{AFX_MSG_MAP(CMeshView)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_FILE_PROPERTY, OnProperty)
	ON_COMMAND(ID_MAP3D_ADDWIREFRAME, OnMap3dAddwireframe)
	ON_UPDATE_COMMAND_UI(ID_MAP3D_ADDWIREFRAME, OnUpdateMap3dAddwireframe)
	ON_COMMAND(ID_MAP3D_COLORFROMHEIGHT, OnMap3dColorfromheight)
	ON_COMMAND(ID_MAP3D_FACE, OnMap3dFace)
	ON_UPDATE_COMMAND_UI(ID_MAP3D_FACE, OnUpdateMap3dFace)
	ON_COMMAND(ID_MAP3D_LIGHT, OnMap3dLight)
	ON_UPDATE_COMMAND_UI(ID_MAP3D_LIGHT, OnUpdateMap3dLight)
	ON_COMMAND(ID_MAP3D_LINE, OnMap3dLine)
	ON_UPDATE_COMMAND_UI(ID_MAP3D_LINE, OnUpdateMap3dLine)
	ON_COMMAND(ID_MAP3D_SMOOTH, OnMap3dSmooth)
	ON_UPDATE_COMMAND_UI(ID_MAP3D_SMOOTH, OnUpdateMap3dSmooth)
	ON_COMMAND(ID_MAP3D_SUBDIVISION, OnMap3dSubdivision)
	ON_COMMAND(ID_MAP3D_VERTEX, OnMap3dVertex)
	ON_UPDATE_COMMAND_UI(ID_MAP3D_VERTEX, OnUpdateMap3dVertex)
	ON_WM_PAINT()
	ON_COMMAND(ID_MAP3D_BACKGROUNDCOLOR, OnMap3dBackgroundcolor)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMeshView drawing

void CMeshView::OnDraw(CDC* pDC)
{
	wglMakeCurrent(pDC->GetSafeHdc(),m_hGLContext);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glPushMatrix();
	
	SXFrm XF;
	XF.Set(m_xTranslation,m_yTranslation,m_zTranslation,
		   m_xRotation,m_yRotation,m_zRotation,
		   m_xScaling,m_yScaling,m_zScaling
		   );

	// Start rendering...
	pDoc->RenderScene(XF);
	
	glPopMatrix();
	glFlush();
	
	// Double buffer
	SwapBuffers(pDC->GetSafeHdc());
}

/////////////////////////////////////////////////////////////////////////////
// CMeshView diagnostics

#ifdef _DEBUG
void CMeshView::AssertValid() const
{
	CView::AssertValid();
}

void CMeshView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMeshView printing

void CMeshView::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
{
	if (!pDoc) {
		return ;
	}
	pDoc->GenerateScene();

	OnPrint1(pDC, pInfo, this);
	SetOpenGLState(m_hMemDC);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	SXFrm XF;
	XF.Set(m_xTranslation,m_yTranslation,m_zTranslation,
		m_xRotation,m_yRotation,m_zRotation,
		m_xScaling,m_yScaling,m_zScaling
		);
	pDoc->RenderScene(XF);
	glPopMatrix();
	glFlush();
	OnPrint2(pDC);
}

void CMeshView::OnPrint1(CDC* pDC, CPrintInfo* pInfo, CView* pView) 
{
	CRect rcClient;
	pView->GetClientRect(&rcClient); 
	float fClientRatio = float(rcClient.Height())/rcClient.Width();

	// Get page size
	m_szPage.cx  = pDC->GetDeviceCaps(HORZRES);
	m_szPage.cy = pDC->GetDeviceCaps(VERTRES);

	if (pInfo->m_bPreview)
	{
		// Use screen resolution for preview.
		szDIB.cx = rcClient.Width();
		szDIB.cy = rcClient.Height();
	}
	else  // Printing
	{
		// Use higher resolution for printing.
		// Adjust size according screen's ratio.
		if (m_szPage.cy > fClientRatio*m_szPage.cx)
		{
			// View area is wider than Printer area
			szDIB.cx = m_szPage.cx;
			szDIB.cy = long(fClientRatio*m_szPage.cx);
		}
		else
		{
			// View area is narrower than Printer area
			szDIB.cx = long(float(m_szPage.cy)/fClientRatio);
			szDIB.cy = m_szPage.cy;
		}
		// Reduce the Resolution if the Bitmap size is too big.
		// Ajdust the maximum value, which is depends on printer's memory.
		// I use 20 MB. 
		while (szDIB.cx*szDIB.cy > 20e6)   
		{
			szDIB.cx = szDIB.cx/2;
			szDIB.cy = szDIB.cy/2;
		}
	}

//	TRACE("Buffer size: %d x %d = %6.2f MB\n", szDIB.cx, szDIB.cy, szDIB.cx*szDIB.cy*0.000001);
	
	// Initialize the bitmap header info.
	memset(&m_bmi, 0, sizeof(BITMAPINFO));
	m_bmi.bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
	m_bmi.bmiHeader.biWidth			= szDIB.cx;
	m_bmi.bmiHeader.biHeight		= szDIB.cy;
	m_bmi.bmiHeader.biPlanes		= 1;
	m_bmi.bmiHeader.biBitCount		= 24;
	m_bmi.bmiHeader.biCompression	= BI_RGB;
	m_bmi.bmiHeader.biSizeImage		= szDIB.cx * szDIB.cy * 3;

	// Create DIB
	HDC	hDC = ::GetDC(pView->m_hWnd);
	m_hDib = ::CreateDIBSection(hDC, &m_bmi, DIB_RGB_COLORS, &m_pBitmapBits, NULL, (DWORD)0);
	::ReleaseDC(pView->m_hWnd, hDC);

	// Create memory DC
	m_hMemDC = ::CreateCompatibleDC(NULL);
	if (!m_hMemDC)
	{
		DeleteObject(m_hDib);
		m_hDib = NULL;
		return;
	}

	m_hOldDib = SelectObject(m_hMemDC, m_hDib);

	// Setup memory DC's pixel format.
	if (!SetWindowPixelFormat(m_hMemDC, PFD_DRAW_TO_BITMAP | PFD_SUPPORT_OPENGL | PFD_STEREO_DONTCARE))
	{
		SelectObject(m_hMemDC, m_hOldDib);
		DeleteObject(m_hDib);
		m_hDib = NULL;
		DeleteDC(m_hMemDC);
		m_hMemDC = NULL;
		return;
	}

	// Create memory RC
	m_hMemRC = ::wglCreateContext(m_hMemDC);
	if (!m_hMemRC)
	{
		SelectObject(m_hMemDC, m_hOldDib);
		DeleteObject(m_hDib);
		m_hDib = NULL;
		DeleteDC(m_hMemDC);
		m_hMemDC = NULL;
		return;
	}

	// Store old DC and RC
	m_hOldDC = ::wglGetCurrentDC();
	m_hOldRC = ::wglGetCurrentContext(); 

	// Make m_hMemRC the current RC.
	::wglMakeCurrent(m_hMemDC, m_hMemRC);

	//	SetFrustum();
	//	CreateDisplayList(1);
}

void CMeshView::OnPrint2(CDC* pDC) 
{
	::wglMakeCurrent(NULL, NULL);	
	::wglDeleteContext(m_hMemRC);

	// Restore last DC and RC
	::wglMakeCurrent(m_hOldDC, m_hOldRC);	

	// Size of printing image
	float fBmiRatio = float(m_bmi.bmiHeader.biHeight) / m_bmi.bmiHeader.biWidth;
	CSize szImageOnPage;  
	if (m_szPage.cx > m_szPage.cy)	 // Landscape page
	{
		if(fBmiRatio<1)	  // Landscape image
		{
			szImageOnPage.cx = m_szPage.cx;
			szImageOnPage.cy = long(fBmiRatio * m_szPage.cy);
		}
		else			  // Portrait image
		{
			szImageOnPage.cx = long(m_szPage.cy/fBmiRatio);
			szImageOnPage.cy = m_szPage.cy;
		}
	}
	else		    // Portrait page
	{
		if(fBmiRatio<1)	  // Landscape image
		{
			szImageOnPage.cx = m_szPage.cx;
			szImageOnPage.cy = long(fBmiRatio * m_szPage.cx);
		}
		else			  // Portrait image
		{
			szImageOnPage.cx = long(m_szPage.cx/fBmiRatio);
			szImageOnPage.cy = m_szPage.cy;
		}
	}

	szImageOnPage.cx *= 0.70;
	szImageOnPage.cy *= 0.90;
	CSize szOffset((m_szPage.cx - szImageOnPage.cx) / 2, (m_szPage.cy - szImageOnPage.cy) / 2);

	// Stretch the Dib to fit the image size.
	int nRet = ::StretchDIBits(pDC->GetSafeHdc(),
							  szOffset.cx, szOffset.cy,
							  szImageOnPage.cx, szImageOnPage.cy,
							  0, 0,
							  m_bmi.bmiHeader.biWidth, m_bmi.bmiHeader.biHeight,
							  (GLubyte*) m_pBitmapBits,
							  &m_bmi, DIB_RGB_COLORS, SRCCOPY);

	if(nRet == GDI_ERROR) {
//		TRACE0("Failed in StretchDIBits()");
	}

	if (m_hOldDib) {
		SelectObject(m_hMemDC, m_hOldDib);
	}

	DeleteObject(m_hDib);
	m_hDib = NULL;
	DeleteDC(m_hMemDC);	
	m_hMemDC = NULL;
	m_hOldDC = NULL;
}

//BOOL CMeshView::SetDCPixelFormat(HDC hDC, DWORD dwFlags)
//{
//	PIXELFORMATDESCRIPTOR pixelDesc;
//	
//	pixelDesc.nSize = sizeof(PIXELFORMATDESCRIPTOR);
//	pixelDesc.nVersion = 1;
//	
//	pixelDesc.dwFlags = dwFlags;
//	pixelDesc.iPixelType = PFD_TYPE_RGBA;
//	pixelDesc.cColorBits = 24;
//	pixelDesc.cRedBits = 8;
//	pixelDesc.cRedShift = 16;
//	pixelDesc.cGreenBits = 8;
//	pixelDesc.cGreenShift = 8;
//	pixelDesc.cBlueBits = 8;
//	pixelDesc.cBlueShift = 0;
//	pixelDesc.cAlphaBits = 0;
//	pixelDesc.cAlphaShift = 0;
//	pixelDesc.cAccumBits = 64;
//	pixelDesc.cAccumRedBits = 16;
//	pixelDesc.cAccumGreenBits = 16;
//	pixelDesc.cAccumBlueBits = 16;
//	pixelDesc.cAccumAlphaBits = 0;
//	pixelDesc.cDepthBits = 32;
//	pixelDesc.cStencilBits = 8;
//	pixelDesc.cAuxBuffers = 0;
//	pixelDesc.iLayerType = PFD_MAIN_PLANE;
//	pixelDesc.bReserved = 0;
//	pixelDesc.dwLayerMask = 0;
//	pixelDesc.dwVisibleMask = 0;
//	pixelDesc.dwDamageMask = 0;
//	
//	int nPixelIndex = ::ChoosePixelFormat(hDC, &pixelDesc);
//	if (nPixelIndex == 0) // Choose default
//	{
//		nPixelIndex = 1;
//		if (::DescribePixelFormat(hDC, nPixelIndex, 
//			sizeof(PIXELFORMATDESCRIPTOR), &pixelDesc) == 0)
//			return false;
//	}
//	
//	if (!::SetPixelFormat(hDC, nPixelIndex, &pixelDesc))
//		return false;
//	
//	return true;
//}

BOOL CMeshView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CMeshView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

void CMeshView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

/////////////////////////////////////////////////////////////////////////////
// CMeshView message handlers
void CMeshView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	HWND hWnd = GetSafeHwnd();
	HDC hDC = ::GetDC(hWnd);
	wglMakeCurrent(hDC,m_hGLContext);
	::ReleaseDC(hWnd,hDC);
	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}



//********************************************
// CreateViewGLContext
// Create an OpenGL rendering context
//********************************************
BOOL CMeshView::CreateViewGLContext(HDC hDC)
{
	m_hGLContext = wglCreateContext(hDC);
	
	if(m_hGLContext==NULL)
		return FALSE;
	
	if(wglMakeCurrent(hDC,m_hGLContext)==FALSE)
		return FALSE;
	
	return TRUE;
}

//********************************************
// SetWindowPixelFormat
//********************************************
BOOL CMeshView::SetWindowPixelFormat(HDC hDC, DWORD dwFlags)
{
	PIXELFORMATDESCRIPTOR pixelDesc;
	
	pixelDesc.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pixelDesc.nVersion = 1;
	
//	pixelDesc.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_STEREO_DONTCARE;
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
	
	m_GLPixelIndex = ChoosePixelFormat(hDC,&pixelDesc);
	if(m_GLPixelIndex == 0) // Choose default
	{
		m_GLPixelIndex = 1;
		if(DescribePixelFormat(hDC,m_GLPixelIndex,
			sizeof(PIXELFORMATDESCRIPTOR),&pixelDesc)==0)
			return FALSE;
	}
	
	if(!SetPixelFormat(hDC,m_GLPixelIndex,&pixelDesc))
		return FALSE;
	
	return TRUE;
}

void CMeshView::OnDestroy() 
{
	if(wglGetCurrentContext() != NULL) {
		wglMakeCurrent(NULL,NULL);
	}
	
	if(m_hGLContext != NULL)
	{
		wglDeleteContext(m_hGLContext);
		m_hGLContext = NULL;
	}
	CView::OnDestroy();
}

void CMeshView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	HWND hWnd = GetSafeHwnd();
	HDC hDC = ::GetDC(hWnd);
	//TRACE("Activate view, set active OpenGL rendering context...\n");
	wglMakeCurrent(hDC,m_hGLContext);
	
	// Set OpenGL perspective, viewport and mode
	double aspect = (cy == 0) ? cx : (double)cx/(double)cy;
	
	glViewport(0,0,cx,cy);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45,aspect,0.001,1000.0);
	glMatrixMode(GL_MODELVIEW);
//	glLoadIdentity();
	glDrawBuffer(GL_BACK);
	
	// Release
	::ReleaseDC(hWnd,hDC);
}

void CMeshView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_LeftButtonDown = TRUE;
	m_LeftDownPos = point;
	SetCapture();
	
	CView::OnLButtonDown(nFlags, point);
}

void CMeshView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	m_RightButtonDown = FALSE;
	m_LeftButtonDown = FALSE;
	ReleaseCapture();
	
	CView::OnLButtonUp(nFlags, point);
}

void CMeshView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	m_RightButtonDown = TRUE;
	m_RightDownPos = point;
	SetCapture();
	
	CView::OnRButtonDown(nFlags, point);
}

void CMeshView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	m_RightButtonDown = FALSE;
	m_LeftButtonDown = FALSE;
	ReleaseCapture();
	
	CView::OnRButtonUp(nFlags, point);
}

BOOL CMeshView::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
	
//	return CView::OnEraseBkgnd(pDC);
}

int CMeshView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	HWND hWnd = GetSafeHwnd();
	HDC hDC = ::GetDC(hWnd);
	
	if(SetWindowPixelFormat(hDC,PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_STEREO_DONTCARE)==FALSE) {
		return FALSE;
	}
	
	if(CreateViewGLContext(hDC)==FALSE) {
		return FALSE;
	}

	SetOpenGLState(hDC);
	
	::ReleaseDC(hWnd,hDC);
	
	return 1;
}

void CMeshView::OnMouseMove(UINT nFlags, CPoint point) 
{
	// Both : rotation
	if(m_LeftButtonDown && m_RightButtonDown)
	{
		m_xTranslation -= (float)(m_LeftDownPos.x - point.x) * m_SpeedTranslation;
		m_yTranslation += (float)(m_LeftDownPos.y - point.y) * m_SpeedTranslation;
		m_LeftDownPos = point;
		InvalidateRect(NULL,FALSE);
	}
	
	else {
		
		// Left : x / y translation
		if(m_LeftButtonDown)
		{
			if(m_xyRotation)
			{
				m_yRotation -= (float)(m_LeftDownPos.x - point.x) * m_SpeedRotation;
				m_xRotation -= (float)(m_LeftDownPos.y - point.y) * m_SpeedRotation;
			}
			else
			{
				m_zRotation -= (float)(m_LeftDownPos.x - point.x) * m_SpeedRotation;
				m_xRotation -= (float)(m_LeftDownPos.y - point.y) * m_SpeedRotation;
			}
			m_LeftDownPos = point;
			m_RightDownPos = point;
			InvalidateRect(NULL,FALSE);
		}
		
		else {
			
			// Right : z translation
			if(m_RightButtonDown)
			{
				m_zTranslation += (float)(m_RightDownPos.y - point.y) * m_SpeedTranslation;
				m_RightDownPos = point;
				InvalidateRect(NULL,FALSE);
			}
		}
			
			/*
			TRACE("\nPosition\n");
			TRACE("Translation : %g %g %g\n",m_xTranslation,m_yTranslation,m_zTranslation);
			TRACE("Rotation    : %g %g %g\n",m_xRotation,m_yRotation,m_zRotation);
			*/
	}	
	CView::OnMouseMove(nFlags, point);
}

std::vector<CPoint3D> * CMeshView::GetDataPtr()
{
	if (pDoc)
	{
		return pDoc->GetDataPtr();
	}
	return NULL;
}

void CMeshView::LoadGeometry()
{
	if (pDoc)
	{
		CString filename;
		DosUtil.GetProgramDir(filename);
		filename += "waferHRes.wrl";
		pDoc->OpenVRMLFile(filename);
	}

}

void CMeshView::GenerateScene()
{
	if (pDoc)
	{
		pDoc->GenerateScene();
//		Invalidate();
	}
}

BOOL CMeshView::SetOpenGLState(HDC hDC)
{
	
	// Default mode
	glPolygonMode(GL_FRONT,GL_FILL);
	glPolygonMode(GL_BACK,GL_FILL);
	
	glShadeModel(GL_SMOOTH);
	glEnable(GL_NORMALIZE);
	
	// Lights properties
	float	ambientProperties[]  = {0.7f, 0.7f, 0.7f, 1.0f};
	float	diffuseProperties[]  = {0.8f, 0.8f, 0.8f, 1.0f};
	float	specularProperties[] = {1.0f, 1.0f, 1.0f, 1.0f};
	
	glLightfv( GL_LIGHT0, GL_AMBIENT, ambientProperties);
	glLightfv( GL_LIGHT0, GL_DIFFUSE, diffuseProperties);
	glLightfv( GL_LIGHT0, GL_SPECULAR, specularProperties);
	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 1.0);
	
	glClearColor(m_ClearColorRed,m_ClearColorGreen,m_ClearColorBlue,1.0f);
	glClearDepth(1.0f);
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	
	// Perspective
	CRect rect;
	GetClientRect(&rect);
	double aspect = (rect.Height() == 0) ? rect.Width() : (double)rect.Width()/(double)rect.Height();
	gluPerspective(0,aspect,0.001,1000.0);
	
	//glPolygonMode(GL_FRONT,GL_FILL);
	//glPolygonMode(GL_BACK,GL_POINT);
	
	// Default : lighting
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	
	// Default : blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	
	// Default : material
	//float	MatAmbient[]  = {0.0f, 0.33f, 0.50f, 1.0f};
	//float	MatDiffuse[]  = {0.5f, 0.5f, 0.5f, 1.0f};
	//float	MatSpecular[]  = {0.1f, 0.1f, 0.1f, 1.0f};
	//float	MatShininess[]  = { 84 };
	//float	MatEmission[]  = {0.0f, 0.0f, 0.0f, 1.0f};
	
	// Back : green
	//float MatAmbientBack[]  = {0.0f, 0.5f, 0.0f, 1.0f};
	
	glEnable(GL_DEPTH_TEST);
	//glDepthFunc(Gl_LESS);
	
	// Modulate : texture lighting
	glEnable(GL_TEXTURE_2D);
//	TRACE("Texture parameters...\n");
	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	return TRUE;
}

void CMeshView::OnProperty() 
{
	CPoint point;
	
	GetCursorPos(&point);	
	ScreenToClient(&point);
	
	CMenu menu;
	VERIFY(menu.LoadMenu(IDR_MENU1));
	CMenu* pPopup = menu.GetSubMenu(19);
	ASSERT(pPopup);
	
	CPoint ScrnPt = point;
	ClientToScreen(&ScrnPt);
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, ScrnPt.x, ScrnPt.y, this);
}

void CMeshView::OnMap3dAddwireframe() 
{
	if (pDoc) {
		pDoc->m_AddWireframe = !pDoc->m_AddWireframe;
		Invalidate();
	}	
}

void CMeshView::OnUpdateMap3dAddwireframe(CCmdUI* pCmdUI) 
{
	if (pDoc) {
		pCmdUI->SetCheck(pDoc->m_AddWireframe);
	}
}

void CMeshView::OnMap3dColorfromheight() 
{
	if (!pDoc) {
		return ;
	}
	CColorRamp ramp;
	ramp.BuildRainbow();
	((CMesh3d *)pDoc->m_SceneGraph.GetAt(0))->ColorHeight(&ramp);
	Invalidate();
}

void CMeshView::OnMap3dFace() 
{
	if (pDoc) {
		pDoc->m_Mode = GL_FILL;
		Invalidate();
	}
}

void CMeshView::OnUpdateMap3dFace(CCmdUI* pCmdUI) 
{
	if (pDoc) {
		pCmdUI->SetCheck(pDoc->m_Mode == GL_FILL);
	}	
}

void CMeshView::OnMap3dLight() 
{
	if (pDoc) {
		pDoc->m_Light = !pDoc->m_Light;
		Invalidate();
	}
}

void CMeshView::OnUpdateMap3dLight(CCmdUI* pCmdUI) 
{
	if (pDoc) {
		pCmdUI->SetCheck(pDoc->m_Light);
	}
}

void CMeshView::OnMap3dLine() 
{
	if (pDoc) {
		pDoc->m_Mode = GL_LINE;
		Invalidate();
	}
}

void CMeshView::OnUpdateMap3dLine(CCmdUI* pCmdUI) 
{
	if (pDoc) {
		pCmdUI->SetCheck(pDoc->m_Mode == GL_LINE);
	}	
}

void CMeshView::OnMap3dSmooth() 
{
	if (!pDoc) {
		return;
	}
	pDoc->m_Smooth = !pDoc->m_Smooth;
	if(pDoc->m_Smooth)
	{
		pDoc->m_SceneGraph.SetNormalBinding(NORMAL_PER_VERTEX);
		::glShadeModel(GL_SMOOTH);
		pDoc->m_SceneGraph.BuildAdjacency();
	}
	else
	{
		pDoc->m_SceneGraph.SetNormalBinding(NORMAL_PER_FACE);
		::glShadeModel(GL_FLAT);
		pDoc->m_SceneGraph.BuildAdjacency();
	}
	Invalidate();
}

void CMeshView::OnUpdateMap3dSmooth(CCmdUI* pCmdUI) 
{
	if (pDoc) {
		pCmdUI->SetCheck(pDoc->m_Smooth);
	}
}

void CMeshView::OnMap3dSubdivision() 
{
	if (!pDoc) {
		return ;
	}
	BeginWaitCursor();
	int NbObject = pDoc->m_SceneGraph.NbObject();
	for(int i=0;i<NbObject;i++)
	{
		CObject3d *pObject3d = pDoc->m_SceneGraph[i];
		if(pObject3d->GetType() == TYPE_MESH3D)
		{
			CMesh3d *pMesh  = (CMesh3d *)pObject3d;
			pMesh->SubdivisionLoop();
		}
	}
	if ((pDoc->m_Mode == GL_LINE) && !pDoc->m_Light) {
		OnMap3dLight();
	}
	else {
		Invalidate();
	}
	EndWaitCursor();
}

void CMeshView::OnMap3dVertex() 
{
	if (pDoc) {
		pDoc->m_Mode = GL_POINT;
		Invalidate();
	}
}

void CMeshView::OnUpdateMap3dVertex(CCmdUI* pCmdUI) 
{
	if (pDoc) {
		pCmdUI->SetCheck(pDoc->m_Mode == GL_POINT);
	}	
}

void CMeshView::OnMap3dBackgroundcolor() 
{
	CEditColor dlg;

	dlg.m_Blue = m_ClearColorBlue;
	dlg.m_Green = m_ClearColorGreen;
	dlg.m_Red = m_ClearColorRed;
	if (dlg.DoModal() == IDOK) {
		m_ClearColorBlue = dlg.m_Blue;
		m_ClearColorGreen = dlg.m_Green;
		m_ClearColorRed = dlg.m_Red;
		glClearColor(m_ClearColorRed,m_ClearColorGreen,m_ClearColorBlue,1.0f);
		Invalidate();
	}
}
