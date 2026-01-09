#pragma once

// #include "413AppThinF.h"
#include "FFUThread.h"
#include "CWL.h"
#include "413AppRoughWithCWL.h" // Mahedi Kamal 12120224
#include "DiceView.h"
#include "ExportPDF.h" // 20251512_Mohir
#include "hpdf.h" //20251210_Mohir
class CImgWnd;
class CMPoint;
class CPeakParam;
class CRecipe413;
class CSICam;
class CWaferMap;
class CDiceView;


class SFINDCENTER {
public:
	float* pCXOffset = NULL, * pCYOffset = NULL;
	float* pCRadius = NULL;
	float* pAng = NULL;
	short Method = 0;
	CString* pName1 = NULL;
	CString* pName2 = NULL;
	CRecipe413* pRcp = NULL;
	CSICam* pVideo = NULL;
	CImgWnd* pImgWnd = NULL;
	CListCtrl* pList = NULL;
	BOOL bTest = TRUE;
	
	

	// For Thread use [8/7/2013 Yuen]
	BOOL bRetCode = FALSE;
	LPVOID pObj = NULL;

	SFINDCENTER() {
		//bTest = TRUE;
		//pVideo = NULL;
		//pImgWnd = NULL;
		//pList = NULL;
		//pRcp = NULL;
	}
};

class C413App : public C413AppRoughWithCWL/* Mahedi Kamal - 12120224*/ {
public:
	BOOL bUIEnabled;
	BOOL bUIBlocked;
	CFFUThread FFUThread;
	//20220520/yukchiu
	short markerCount = 0;
	//[[20241213/Sihab
	BOOL bMask = FALSE;
	//]]

public:
	void RecordEvent(CString strEvent);
	
	BOOL ConfirmLoadPosPattMatch(SDoPattrnMatching& Param);
	BOOL ConfirmLoadPosEcho();
	BOOL FindWaferCenterA(SFINDCENTER& SFC);
	static DWORD WINAPI FindWaferCenterThread(LPVOID Param);
	BOOL LoadRecipeFromDataFile(CRecipe413& Rcp);
	BOOL CanTransfer2Cas(short Lp, short slot);
	BOOL CanTransfer2Stage(short Lp, short slot);
	BOOL DoSPCalib(CRecipe413* pRcpl);
	void QuickEdit(CRecipe413* pRcp);
	void RestoreView(CCamBase::CAMERAID SIView, CRecipe413* pRcp, CSICam* pVideo, CListCtrl* pList);
	BOOL FindWaferCenterUsingEdgeDetection(SFINDCENTER& S/*float &CXOffset, float &CYOffset, float &CRadius, CRecipe413 *pRcp, CSICam *pVideo, CImgWnd *pImgWnd, CListCtrl *pList*/);
	BOOL FindWaferCenterUsingEdgeDetectionFast(SFINDCENTER& S/*float &CXOffset, float &CYOffset, float &CRadius, CRecipe413 *pRcp, CSICam *pVideo, CImgWnd *pImgWnd, CListCtrl *pList*/);
	BOOL FindWaferCenterUsingEdgeDetectionA(SFINDCENTER& S/*float &CXOffset, float &CYOffset, float &CRadius, float *pAng, CRecipe413 *pRcp, CSICam *pVideo, CImgWnd *pImgWnd, CListCtrl *pList*/);
	void SetPeakParamDualCalib(CPeakParam& PParam);
	BOOL IsAborting();
	BOOL LoadRecipe(CRecipe413* pRcp, short Option = 0);
	BOOL ConfirmLoadPos(SDoPattrnMatching& Param);
	short ConfirmLoadPosOne(CRecipe413* pRcpl);
	void StartDialogBoxes();
	long DeltaMove(CPoint* pT, CCamBase::CAMERAID ActiveCamera, CRect& rc);

	void Dump2(CString name, short* pBuf, short len);

	BOOL MeasureMultiPoint(CRecipe413* pRcp, CMPoint* p, CCamBase::CAMERAID ActiveCamera, CSICam* pVideo, CImgWnd* pImgWnd, CChartWnd* pProfile);
	BOOL IsBothTypeData(CRecipe413* pRcp);
	BOOL MeasureThinFilm(CRecipe413* pRcp, CMPoint* p, short nTrial, CCamBase::CAMERAID ActiveCamera, CSICam* pVideo, CImgWnd* pImgWnd, CListCtrl* pList/*, short ID*/);
	BOOL GetThinFilmMarker(short Marker, CRecipe413* pRcp, CMPoint* p, short nTrial, CCamBase::CAMERAID ActiveCamera, CSICam* pVideo, CImgWnd* pImgWnd, CListCtrl* pList, short ID);

	void Enlarge(CRecipe413* pRcp, CWaferMap* pWaferMap, CDiceView* m_cDiceMap);

	BOOL MeasureOnePointEcho(CRecipe413* pRcp, CMPoint* p, short* code, short nTrial, CCamBase::CAMERAID ActCamera, CSICam* pVideo, CImgWnd* pImgWnd, CListCtrl* pList, BOOL bDoNotMoveZ);
	BOOL GetEchoMarker(short Marker, CRecipe413* pRcp, CMPoint* p, short nTrial, CCamBase::CAMERAID ActiveCamera, CSICam* pVideo, CImgWnd* pImgWnd, CListCtrl* pList, short ID);

	void Enlarge(CRecipe413* pRcp, CWaferMap* pWaferMap);

	BOOL GetRadialMultiPoint(CRecipe413* pRcp, CMPoint* pMP, short nTrial, CSICam* pVideo, CImgWnd* pImgWnd, CChartWnd* pProfile);

	void LocationProp(CRecipe413& Rcp, HWND hWnd);
	BOOL PatternProp(CRecipe413& Rcp);
	BOOL EchoProp(CRecipe413& Rcp, short& MeSet);

	void Deinitialize();
	void Initialize(CListBox* pMessage);
	void StopSystem();
	BOOL StartSystem(CListBox* pMessage);

	C413App();
	virtual ~C413App();

	BOOL StartMotionSystem(CListBox* pMessage);
	BOOL StartProbeSwitchingSystem(CListBox* pMessage); //02082024/MORSALIN
	void StopMotionSystem(void);
	void StopProbeSwitchingSystem(void); //02082024/MORSALIN

	// [5/27/2020 Zhiming]
	BOOL IsDoorInterlockTriggered();
	// [04/13/2022 ZHIMING]
	BOOL MeasureThinFilm_v2(CRecipe413* pRcp, CMPoint* p, short nTrial, CCamBase::CAMERAID ActiveCamera, CSICam* pVideo, CImgWnd* pImgWnd, CListCtrl* pList);
	// [11082022 Sabbir
	BOOL MeasureOnePointConfocal(CRecipe413* pRcp, CMPoint* p, short* code, short nTrial, CListCtrl* pList);
	// [20231227 ZHIMING Updated
	BOOL GetConfocalValue(CRecipe413* pRcp, CMPoint* pMP, BOOL bLast, short* code, CListCtrl* pList);
	BOOL GetConfocalValue2(CRecipe413* pRcp, CMPoint* pMP, BOOL bLast, short* code, CListCtrl* pList);
	// Mahedi Kamal - 03202024 
	BOOL MeasureOnePointRough(CRecipe413* pRcp, CMPoint* p, short* code, short nTrial, CSICam* pVideo, short NAver, CImgWnd* pImgWnd, CListCtrl* pList, BOOL bDoNotMoveZ /*BOOL bSettling*/);
	// ]

	//08142024 - START
	BOOL PreMeasureRingWidth(CRecipe413* pRcp, CCamBase::CAMERAID ActiveCamera, CSICam* pVideo, CImgWnd* pImgWnd, CListCtrl* pList);
	BOOL MeasureOnePointRingWidth(CRecipe413* pRcp, CMPoint* p, CCamBase::CAMERAID ActiveCamera, CSICam* pVideo, CImgWnd* pImgWnd, CListCtrl* pList, short* code);
	BOOL DoMeasureOnePointRingWidth(CRecipe413* pRcp, CMPoint* p, CSICam* pVideo, CImgWnd* pImgWnd, CListCtrl* pList);
	BOOL PostMeasureRingWidth(CListCtrl* pList); 
	BOOL DoTaikoSearchAround(CRecipe413* pRcp, CMPoint* p, CSICam* pVideo, CImgWnd* pImgWnd, CListCtrl* pList);
	BOOL LinearEdgeSearch(CRecipe413* pRcp, CMPoint* p, CSICam* pVideo, CImgWnd* pImgWnd, CListCtrl* pList);
	BOOL CircularEdgeSearch(CRecipe413* pRcp, CMPoint* p, CSICam* pVideo, CImgWnd* pImgWnd, CListCtrl* pList);
	BOOL FindOuterEdge(float& radius, float angle, float& fDist, CSICam* pVideo, CImgWnd* pImgWnd, CListCtrl* pList);
	BOOL MoveToPosition(float radius, float angle, CListCtrl* pList);
	void Init_motion(float fStepSize);
	void Sleep2(DWORD nTime);
	BOOL MoveEdgeToCenter(float fDist, float radius, float angle, CListCtrl* pList);
	BOOL FindStripWhite(float& radius, float angle, float& inner, float& outer, CSICam* pVideo, CImgWnd* pImgWnd, CListCtrl* pList);
	BOOL IsRingWidthWithinRange(double RingWidth, CRecipe413* pRcp);
	BOOL FindStripBlack(float& radius, float angle, float& inner, float& outer, CSICam* pVideo, CImgWnd* pImgWnd, CListCtrl* pList);
	//08142024 - END
	
	//30122024_1
	CString curTitle;
	
	void setSelectedTitle(CWaferMap* pWaferMap);
	CString getSelectedTitle();
	//

	void ResetAtBegin(); //03062025
	
	float ThicknessScanLength; //20250926_Mohir
	BOOL m_bIsDiceType;
	void SetDiceType(BOOL b) { m_bIsDiceType = b; }

	//[ // MAHEDI 10292025_MH
	int pointIdx;
	vector<float>standardresult;
	BOOL IsDataInrange(int pointIdx, vector<float>&standardresult, float accuracy, float measuredVal); // MAHEDI 10292025_MH
	double GetRandomNumber(float min, float max);
	//]
	CExportPDF PdfExporter; //20251215_Mohir
	 
};

extern C413App* p413App;
