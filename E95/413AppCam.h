#pragma once

#include "413AppBase.h"
#include "ImgWnd.h"

struct SFINDEDGES {
	// Outputs [7/4/2013 Yuen]
	float dist1;
	float dist2;
	int score;
	BOOL bTest;

	// Inputs [7/4/2013 Yuen]
	float angle;	// Can be modify by inner functions [7/4/2013 Yuen]
	float radius;
	float Thres;
	float ZPosition;
	CImgWnd::FNDMTHD method;
	int Exposure, In, Out;
	short AutoExpose;
	BOOL bFindBestAngle, bMoveStage;
	float Cmin, Cmax;  // Controls for ContrastStretching [7/4/2013 Yuen]
	BOOL bContrastStrecth;
	CString name;
	CSICam* pVideo;
	CRecipe413* pRcp;
	CImgWnd* pImgWnd;
	CListCtrl* pList;

	SFINDEDGES() {
		dist1 = dist2 = 0;
		score = 0;
		ZPosition = 0;
		radius = angle = 0;
		method = CImgWnd::DARKFWD;
		In = Out = 0;
		Exposure = 400; AutoExpose = 128;
		bMoveStage = TRUE;
		bFindBestAngle = TRUE;
		Cmax = Cmin = 0;
		bContrastStrecth = FALSE;
		name = "";
		bTest = TRUE;

		pRcp = NULL;
		pVideo = NULL;
		pImgWnd = NULL;
		pList = NULL;
	}
};

class C413AppCam : public C413AppBase {
	CSICam* pVideo1;
	static HANDLE hCameraThread1;
	CSICam* pVideo2;
	static HANDLE hCameraThread2;

public:
	void StopCamera(CSICam* pVideo, CListCtrl* pList);
	BOOL StartCamera(CSICam* pVideo, CCamBase::CAMERAID No, CListCtrl* pList, CRecipe413* pRcp);
	void SwitchCameraSetExposure(CCamBase::CAMERAID view, int Exposure, CSICam* pVideo, CListCtrl* pList, CRecipe413* pRcp);
	float FindBestAngle(float angle, CImgWnd* pImgWnd);
	BOOL FindEdges(SFINDEDGES& Param);
	void PostMatchingTreatment(CCamBase::CAMERAID ID, CRecipe413* pRcp, CMPoint* p, CSICam* pVideo, CListCtrl* pList);
	void PreMatchingTreatment(CCamBase::CAMERAID ID, CRecipe413* pRcp, CMPoint* p, CSICam* pVideo, CListCtrl* pList);
	void DoAutoExposure(CPackedDIB& StoreDIB, CPackedDIB& SobDIB, short dir, float target, short Wdw, float XOff, float YOff, CSICam* pVideo, CListBox* pList = NULL);
	void DoAutoFocus(CCamBase::CAMERAID ActiveCamera, float dir, float XOff, float YOff, CSICam* pVideo, short ID, CImgWnd* pImgWnd = NULL, CListBox* pList = NULL);
	void AutoFocusVideo(CCamBase::CAMERAID ActiveCamera, float XOff, float YOff, CSICam* pVideo, CImgWnd* pImgWnd = NULL, CListBox* pList = NULL);
	BOOL StopCameraThread(CSICam& m_cVideo);
	BOOL StopCameraThread2(CSICam& m_cVideo);
	BOOL StartCameraThread(CCamBase::CAMERAID No, CRecipe413* pRcp, CSICam& m_cVideo);
	BOOL StartCameraThread2(CCamBase::CAMERAID No, CRecipe413* pRcp, CSICam& m_cVideo);
	void CameraSetting(CSICam& m_cVideo);
	void StoreDIB2(CPackedDIB& StoreDIB, CPackedDIB& MatchDIB, float fTargetX, float fTargetY, int ww, int hh);
	void StoreDib1(CPackedDIB& StoreDIB, CPackedDIB& MatchDIB, int Width, int Height, float fTargetX, float fTargetY, short XOff = 0, short YOff = 0);
	void LoadPicture(CString name, tagSIPackedDIB* pDIB);
	void TakePicture(CSICam* pVideo, CString name);

	//20130828-1 SCOTT ADD ------------------------
	BOOL isEdge(SFINDEDGES& Par);
	BOOL refindEdges(SFINDEDGES& Par, float& dist);
	//-------------------------------------------

	C413AppCam();
	virtual ~C413AppCam();
};
