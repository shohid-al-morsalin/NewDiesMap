// SelectVCamera.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "SelectVCamera.h"
#include "..\SRC\DOSUtil.h"
#include "CamBase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelectVCamera dialog

CSelectVCamera::CSelectVCamera(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectVCamera::IDD, pParent) {
	//{{AFX_DATA_INIT(CSelectVCamera)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CSelectVCamera::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectVCamera)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSelectVCamera, CDialog)
	//{{AFX_MSG_MAP(CSelectVCamera)
	ON_BN_CLICKED(IDC_SIMU1, OnSimu1)
	ON_BN_CLICKED(IDC_SIMU2, OnSimu2)
	ON_BN_CLICKED(IDC_SIIMAGING1, OnSiimaging1)
	ON_BN_CLICKED(IDC_SIIMAGING2, OnSiimaging2)
	ON_BN_CLICKED(IDC_LUMI1, OnLumi1)
	ON_BN_CLICKED(IDC_LUMI2, OnLumi2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectVCamera message handlers

void CSelectVCamera::OnSimu1() {
	DosUtil.SetCameraType(CCamBase::TYSIMUCAM, CCamBase::ECHOCAM);
}

void CSelectVCamera::OnSimu2() {
	DosUtil.SetCameraType(CCamBase::TYSIMUCAM, CCamBase::THINFCAM);
}

void CSelectVCamera::OnSiimaging1() {
	DosUtil.SetCameraType(CCamBase::TYSICAM, CCamBase::ECHOCAM);
}

void CSelectVCamera::OnSiimaging2() {
	DosUtil.SetCameraType(CCamBase::TYSICAM, CCamBase::THINFCAM);
}

void CSelectVCamera::OnLumi1() {
	DosUtil.SetCameraType(CCamBase::TYLUMICAM, CCamBase::ECHOCAM);
}

void CSelectVCamera::OnLumi2() {
	DosUtil.SetCameraType(CCamBase::TYLUMICAM, CCamBase::THINFCAM);
}

BOOL CSelectVCamera::OnInitDialog() {
	CDialog::OnInitDialog();

	switch (DosUtil.GetCameraType(CCamBase::ECHOCAM)) {
	case CCamBase::TYSIMUCAM:
		((CButton*)GetDlgItem(IDC_SIMU1))->SetCheck(TRUE);
		break;
	case CCamBase::TYSICAM:
		((CButton*)GetDlgItem(IDC_SIIMAGING1))->SetCheck(TRUE);
		break;
	case CCamBase::TYLUMICAM:
		((CButton*)GetDlgItem(IDC_LUMI1))->SetCheck(TRUE);
		break;
	}

	switch (DosUtil.GetCameraType(CCamBase::THINFCAM)) {
	case CCamBase::TYSIMUCAM:
		((CButton*)GetDlgItem(IDC_SIMU2))->SetCheck(TRUE);
		break;
	case CCamBase::TYSICAM:
		((CButton*)GetDlgItem(IDC_SIIMAGING2))->SetCheck(TRUE);
		break;
	case CCamBase::TYLUMICAM:
		((CButton*)GetDlgItem(IDC_LUMI2))->SetCheck(TRUE);
		break;
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}