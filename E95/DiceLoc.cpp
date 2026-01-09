#include "stdafx.h"
#include "e95.h"
#include "DiceLoc.h"
#include "413App.h" //20130827 SCOTT ADD

CDiceLoc::CDiceLoc() {
	ClearAll();
}

CDiceLoc::~CDiceLoc() {}

void CDiceLoc::Calc() {
	// X direction [7/23/2013 Yuen]
	if (Pt[1].IsSet()) {
		dX = Pt[1].GetX();
		dY = Pt[1].GetY();
		dXy = dYx = 0;
	}
	if (Pt[1].IsSet() && Pt[3].IsSet()) {
		dX = Pt[3].GetX() - Pt[1].GetX();
		dXy = Pt[3].GetY() - Pt[1].GetY();
	}
	// Y direction [7/23/2013 Yuen]
	if (Pt[1].IsSet() && Pt[2].IsSet()) {
		dY = Pt[2].GetY() - Pt[1].GetY();
		dYx = Pt[2].GetX() - Pt[1].GetX();
	}
}

void CDiceLoc::ClearAll() {
	Pt[1].Clear();
	Pt[2].Clear();
	Pt[3].Clear();

	dX = dXy = 0;
	dY = dYx = 0;

	ClearOffsets();
}

void CDiceLoc::ClearOffsets() {
	bNeedOffset = TRUE;
	XOff = YOff = Z1Off = Z2Off = 0;	// Normal displacement [7/23/2013 Yuen]
}

// Pass in intended X,Y, return actual X,Y. [7/23/2013 Yuen]
BOOL CDiceLoc::GetCoor(float& X, float& Y) {
	//char tch[255]; // 20131017-D
	BOOL bX, bY, bRet = FALSE;
	bX = bY = FALSE;
	if (!Pt[1].IsSet() && !Pt[2].IsSet() && !Pt[3].IsSet()) {
		x1r = X;
		y1r = Y;
	}
	else if (Pt[1].IsSet() && !Pt[2].IsSet() && !Pt[3].IsSet()) {
		x2r = X;
		y2r = Y;
		X += (Pt[1].GetX() - x1r);
		Y += (Pt[1].GetY() - y1r);
		bX = TRUE;
		bY = TRUE;
		bRet = TRUE;
	}
	else if (Pt[1].IsSet() && Pt[2].IsSet() && !Pt[3].IsSet()) {
		X += ((Pt[2].GetX() - x2r) + (Pt[1].GetX() - x1r)) / 2.0;
		Y += ((Pt[2].GetY() - y2r) + (Pt[1].GetY() - y1r)) / 2.0;
		bX = TRUE;
		bY = TRUE;
		bRet = TRUE;
	}
	if (Pt[1].IsSet() && Pt[2].IsSet() && Pt[3].IsSet()) {
		float fX = (X - Pt[1].GetX()) / dX;
		short nX = (fX >= 0) ? int(fX + 0.5f) : int(fX - 0.5f);
		float fY = (Y - Pt[1].GetY()) / dY;
		short nY = (fY >= 0) ? int(fY + 0.5f) : int(fY - 0.5f);

		X = Pt[1].GetX() + nX * dX + nY * dYx;
		Y = Pt[1].GetY() + nY * dY + nX * dXy;
		bX = bY = TRUE;
		bRet = TRUE;
		//sprintf(tch,"<< GetCoor Step1 Pt1.X = %.3f Pt1.Y = %.3f", Pt[1].GetX(), Pt[1].GetY());
		//OutputDebugString(tch);
		//sprintf(tch,"<< GetCoor Step1 New.X = %.3f New.Y = %.3f", X, Y);
		//OutputDebugString(tch);
	}
#if 0 // XW 11-13
	if (Pt[1].IsSet() && Pt[2].IsSet() && !bY) {
		float fX = (X - Pt[1].GetX()) / dX;
		short nX = (fX >= 0) ? int(fX + 0.5f) : int(fX - 0.5f);
		float fY = (Y - Pt[1].GetY()) / dY;
		short nY = (fY >= 0) ? int(fY + 0.5f) : int(fY - 0.5f);

		Y = Pt[1].GetY() + nY * dY + nX * dXy;
		bY = TRUE;
		bRet = TRUE;
		//sprintf(tch,"<< GetCoor Step2 Pt1.X = %.3f Pt1.Y = %.3f", Pt[1].GetX(), Pt[1].GetY());
		//OutputDebugString(tch);
		//sprintf(tch,"<< GetCoor Step2 New.X = %.3f New.Y = %.3f", X, Y);
		//OutputDebugString(tch);
	}
	//20131017-1 SCOTT EDIT if (Pt[1].IsSet() && Pt[3].IsSet() && !bX)
	if (Pt[1].IsSet() && !bX)  // It's for calculation of delta X with Pt[2] position. Scott.
	{
		float fX = (X - Pt[1].GetX()) / dX;
		short nX = (fX >= 0) ? int(fX + 0.5f) : int(fX - 0.5f);
		float fY = (Y - Pt[1].GetY()) / dY;
		short nY = (fY >= 0) ? int(fY + 0.5f) : int(fY - 0.5f);

		X = Pt[1].GetX() + nX * dX + nY * dYx;
		bX = TRUE;
		bRet = TRUE;
		//sprintf(tch,"<< GetCoor Step3 Pt1.X = %.3f Pt1.Y = %.3f", Pt[1].GetX(), Pt[1].GetY());
		//OutputDebugString(tch);
		//sprintf(tch,"<< GetCoor Step3 New.X = %.3f New.Y = %.3f", X, Y);
		//OutputDebugString(tch);
	}
#endif
	if (!bNeedOffset) {
		if (!bX) {
			X += XOff;
		}
		if (!bY) {
			Y += YOff;
		}
		bRet = TRUE;
	}
	return bRet;
}
/*
BOOL CDiceLoc::GetCoor(float &X, float &Y)
{
	//130827 char tch[255];
	BOOL bX,bY,bRet=FALSE;
	bX=bY=FALSE;
	if (Pt[1].IsSet() && Pt[2].IsSet() && Pt[3].IsSet())
	{
		float fX = (X - Pt[1].GetX()) / dX;
		short nX = (fX >= 0) ? int(fX+0.5f) : int(fX-0.5f);
		float fY = (Y - Pt[1].GetY()) / dY;
		short nY = (fY >= 0) ? int(fY+0.5f) : int(fY-0.5f);

		//20130827-1 SCOTT EDIT ----------------------------------------------
		// Pt[1].GetX() return the position of pattern matched.
		// It makes wrong offset. The point of nexe position would be wrong.
		// X = Pt[1].GetX() + nX * dX + nY * dYx;
		// Y = Pt[1].GetY() + nY * dY + nX * dXy;
		X = p413App->Global.fGPatOrgX + nX * dX + nY * dYx;
		Y = p413App->Global.fGPatOrgY + nY * dY + nX * dXy;
		//------------------------------------------------------------------

		bX=bY=TRUE;
		bRet = TRUE;
		//130827 sprintf(tch,"&&&&& CDiceLoc::GetCoor Step 1 X = %.2f nY = %d dY = %.2f nX = %d dXy = %.2f" ,X, nY , dY , nX , dXy);
		//130827 OutputDebugString(tch);
		//130827 sprintf(tch,"&&&&& CDiceLoc::GetCoor Step 1 Y = %.2f nX = %d dX = %.2f nY = %d dYx = %.2f" ,Y, nX , dX , nY , dYx);
		//130827 OutputDebugString(tch);
	}

	if (Pt[1].IsSet() && Pt[2].IsSet() && !bY)   // IT IS FOR POINT 3;
	{
		float fX = (X - Pt[1].GetX()) / dX;
		short nX = (fX >= 0) ? int(fX+0.5f) : int(fX-0.5f);
		float fY = (Y - Pt[1].GetY()) / dY;
		short nY = (fY >= 0) ? int(fY+0.5f) : int(fY-0.5f);

		//20130827-1 SCOTT EDIT ---------------------------------------
		// Pt[1] is a point of echo probe measurement position.
		// Because of this code, Point 3 has same Y with Point 1.
		// In the position, there was no recognition mark on the FOV.
		// Y = Pt[1].GetY() + nY * dY + nX * dXy;
		Y = Y + nY * dY + nX * dXy;
		//-----------------------------------------------------------

		bY=TRUE;
		bRet = TRUE;
		//130827 sprintf(tch,"&&&&& CDiceLoc::GetCoor Step 2 nY = %d dY = %.2f nX = %d dXy = %.2f" ,nY , dY , nX , dXy);
		//130827 OutputDebugString(tch);
	}

	if (Pt[1].IsSet() && Pt[3].IsSet() && !bX)
	{
		float fX = (X - Pt[1].GetX()) / dX;
		short nX = (fX >= 0) ? int(fX+0.5f) : int(fX-0.5f);
		float fY = (Y - Pt[1].GetY()) / dY;
		short nY = (fY >= 0) ? int(fY+0.5f) : int(fY-0.5f);

		X = Pt[1].GetX() + nX * dX + nY * dYx;
		bX=TRUE;
		bRet = TRUE;
	}
	if (!bNeedOffset)
	{
		if (!bX)
		{
			X += XOff;
		}
		if (!bY)
		{
			Y += YOff;
		}
		bRet = TRUE;
	}
	return bRet;
}
*/
void CDiceLoc::SetOffset(float X, float Y) {
	XOff = X; YOff = Y; bNeedOffset = FALSE;
}

BOOL CDiceLoc::NeedOffset() {
	return bNeedOffset;
}