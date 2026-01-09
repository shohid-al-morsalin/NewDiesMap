// ImageStatic.cpp : implementation file
//

#include "stdafx.h"
#include "ImageStatic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CImageStatic

CImageStatic::CImageStatic() {}

CImageStatic::~CImageStatic() {}

BEGIN_MESSAGE_MAP(CImageStatic, CStatic)
	//{{AFX_MSG_MAP(CImageStatic)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImageStatic message handlers

void CImageStatic::LoadImage_Icon(int nImageID) {
	if (nImageID == -1) {
		return;
	}
	if (nImageID != -1) {
		m_nImageID = nImageID;
	}

	hIcon = (HICON)::LoadImage(AfxGetInstanceHandle(),
		MAKEINTRESOURCE(m_nImageID), IMAGE_ICON, 0, 0,
		LR_LOADMAP3DCOLORS);

	if (hIcon == NULL) {
		return;
	}
	else {
		hIcon = SetIcon(hIcon);
	}

	if (hIcon == NULL) {
		DeleteObject(hIcon);
	}
}

void CImageStatic::LoadImage_Bmp(int nImageID) {
	if (nImageID == -1) {
		return;
	}
	if (nImageID != -1) {
		m_nImageID = nImageID;
	}

	hBmp = (HBITMAP)::LoadImage(AfxGetInstanceHandle(),
		MAKEINTRESOURCE(m_nImageID),
		IMAGE_BITMAP, 0, 0,
		LR_LOADMAP3DCOLORS);

	if (hBmp == NULL) {
		return;
	}
	else {
		hBmp = SetBitmap(hBmp);
	}

	if (hBmp == NULL) {
		DeleteObject(hBmp);
	}
}

void CImageStatic::UnLoadImage_Icon() {
	DeleteObject(hIcon);
}

void CImageStatic::UnLoadImage_Bmp() {
	DeleteObject(hBmp);
}