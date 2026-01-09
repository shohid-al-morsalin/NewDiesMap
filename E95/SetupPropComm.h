// SetupPropComm.h: interface for the CSetupPropComm class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "Global413.h"
#include "413AppPatt.h"

class CCoor;
class CSICam;
class CImgWnd;
class CMPoint;
class CWaferMap;
class CInfoPanelSetup;

class CSetupPropComm {
protected:
	C413Global* g;
	CRecipe413* pRcp;

public:
	HWND hWndPC;
	CSICam* m_pVideo;
	HANDLE m_hLiveThread1;
	HANDLE m_hLiveThread2;
	CInfoPanelSetup* m_Parent;

	CString name;
	short nPatSSet;

public:
	void ExeRecipeSel();
	void Renumber(CListCtrl& cList, CWaferMap* pWaferMap);
	void DoSelectMeasurment(CMPoint* pMP, short nSel, CListCtrl& cList, CWaferMap* pWaferMap);
	//void StopCamera(CSICam *pVideo);
	//void StartCamera(CCamBase::CAMERAID No,CSICam *pVideo);
	void StopAllCamera(CSICam* pVideo);
	void StartCamera2(CCamBase::CAMERAID No, CSICam* pVideo);
	void StopCamera2(CSICam* pVideo);
	BOOL UpdatePointB(CCoor* p);
	void GotoPointB(CSICam* pVideo, CCoor* p, BOOL bZ = TRUE);
	CMPoint* FindMPoint(short nSel);
	void ShowMotionControl(CWnd* pWnd);

public:
	void RecipeIO();
	void DoMeasurmentSetOne(CMPoint* pMP, short nSel, CListCtrl& cList, CWaferMap* pWaferMap);
	void DoMeasurmentSetOneAllOnce(CMPoint* pMP, short nSel, CListCtrl& cList, CWaferMap* pWaferMap);
	BOOL DoPatternMatching(SDoPattrnMatching& Param, BOOL bMeasureTF);
	void ReleaseUI();
	void BlockUI();
	virtual void LocalToRecipe() = 0;
	virtual void RecipeToLocal() = 0;
	void SetPointer(BOOL bArt = TRUE);
	//void UpdateTargets(CSICam *pVideo);
	BOOL LoadRcpSetup();
	long ButClick(WPARAM wP, LPARAM lP, CRect& rc);
	CString MoMove(WPARAM wP, LPARAM lP);
	CString MoMove2(WPARAM wP, LPARAM lP, int width, int height);
	BOOL Rotate(float& x, float& y, float angle);
	BOOL OffsetZ(float cz);
	BOOL ZeroZ(CRecipe413* pRcp);
	BOOL OffsetXY(float cx, float cy);
	BOOL ReverseZ();
	BOOL DeletePoint(CCoor* co);
	BOOL EditPoint(CCoor* co);
	BOOL AddPoint(CCoor* co);
	BOOL AddAlgnPoint(CCoor* co);
	BOOL DeletePoint(short nSel);
	void MPointApplyAll(CMPoint* pMP);
	BOOL RelocateToTopB(CCoor* p);
	BOOL RelocateToBackB(CCoor* p);
	BOOL MarkThisPoint();
	BOOL AddZPositionB(CMPoint* pMP);

	CSetupPropComm(CWnd* pParent = NULL);
	virtual ~CSetupPropComm();
};