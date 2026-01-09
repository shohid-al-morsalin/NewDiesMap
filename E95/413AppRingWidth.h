#pragma once
#include "413AppRough.h"
#include "CenterData.h"
#include "IM/IM.h"

struct SHtPoint {
public:
	int idx = 0;
	float x = 0, y = 0;
	float angle = 0;
	union {
		float ht = 0; // Top probe distance/height [1/3/2020 yuenl]
		float wd; // Taiko width [1/3/2020 yuenl]
	};
};

// 08132024 Mahedi Kamal // added class
class C413AppRingWidth : public C413AppRough
{
public:
	CIM Img, BgImg;
	BOOL bCenterWafer;
	BYTE Thres;
	float sfX, sfY;

	enum MotionAxis { X, Y };
	enum Motion
	{
		UP, RIGHT, DOWN, LEFT
	};
	std::map<Motion, std::vector<float>> SrchAct;
	std::vector<Motion> Line = { LEFT, LEFT, LEFT, LEFT, LEFT, LEFT, LEFT, LEFT, LEFT, LEFT,
								LEFT, LEFT, LEFT, LEFT, LEFT, LEFT, LEFT, LEFT, LEFT, LEFT,
								LEFT, LEFT, LEFT, LEFT, LEFT };
	std::vector<Motion> Snake = { UP, RIGHT, DOWN, DOWN, LEFT, LEFT, UP, UP, UP, RIGHT, RIGHT, RIGHT,
								DOWN, DOWN, DOWN, DOWN, LEFT ,LEFT ,LEFT ,LEFT ,UP ,UP ,UP ,UP };


	C413AppRingWidth();
	~C413AppRingWidth();
	void LoadParam();
	void SaveParam();
	CIM* GetImg() { return &Img; }
	CIM* GetBG() { return &BgImg; }
	BOOL CenterWafer(CRecipe413* pRcp, BYTE Thr, CSICam* pVideo, CImgWnd* pImgWnd, CListCtrl* pList);
	void AdjAngle(float& ang);
	BOOL FindCenter(float ang, float& dr, CSICam* pVideo, CImgWnd* pImgWnd);
	BOOL FindBevel(float WaferRadius, float offset, double& BevelWidth, CListCtrl* pList, CSICam* pVideo, CImgWnd* pImgWnd);
	std::vector<SHtPoint>HtLn;
	BOOL GenerateTaikoPoints(CRecipe413* pRcp);
	BOOL GetSubstrateRingWidth(CRecipe413* pRcp, CMPoint* pMP, BOOL bLast, short* code, CListCtrl* pList);
	void ShowImage(CIM Img, CImgWnd* pImgWnd);
};

