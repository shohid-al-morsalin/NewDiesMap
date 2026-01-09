#include "stdafx.h"

#include "413/MotorS2.h"
#include "GlobalDeviceContainer.h"
#include "MPoint.h"

#include "413AppPatt.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
SDoMatchPattern::SDoMatchPattern() {
	pSourceDIB = pMatchDIB = pMatchDibA = pMatchDibB = NULL;
	startX = startY = endX = endY = 0;
	dI = -1; dT = -1;
	GoF = 0;
	sourceWd = sourceHt = wdA = htA = wdB = htB = -1;
	bFast = TRUE;
}
BOOL SDoMatchPattern::SetSourceDIB(CPackedDIB* pDIB) {
	if (!pDIB || !pDIB->pDIB || !pDIB->pDIB->pDIBBits) {
		//ASSERT(0);
		return FALSE;
	}
	pSourceDIB = pDIB;
	pSourceDIB->GetDimension(sourceWd, sourceHt);
	return TRUE;
}
void SDoMatchPattern::SetMatchDIBA(CPackedDIB* pDIB, float X, float Y) {
	if (!pDIB || !pDIB->pDIB || !pDIB->pDIB->pDIBBits) {
		//ASSERT(0);
		return;
	}
	pMatchDibA = pDIB;
	pMatchDibA->GetDimension(wdA, htA);
	pMatchDibA->fTgtX = X;
	pMatchDibA->fTgtY = Y;
}
void SDoMatchPattern::SetMatchDIBB(CPackedDIB* pDIB, float X, float Y) {
	if (!pDIB || !pDIB->pDIB || !pDIB->pDIB->pDIBBits) {
		//ASSERT(0);
		return;
	}
	pMatchDibB = pDIB;
	pMatchDibB->GetDimension(wdB, htB);
	pMatchDibB->fTgtX = X;
	pMatchDibB->fTgtY = Y;
}
BOOL SDoMatchPattern::GetWidthHeight() {
	if (pSourceDIB) {
		if (!pSourceDIB->GetDimension(sourceWd, sourceHt)) return FALSE;
	}
	else return FALSE;
	if (pMatchDibA) {
		if (!pMatchDibA->GetDimension(wdA, htA)) return FALSE;
	}
	else return FALSE;
	if (pMatchDibB) {
		pMatchDibB->GetDimension(wdB, htB);
	}
	return TRUE;
}
BOOL SDoMatchPattern::IsValid() {
	if (pSourceDIB && pSourceDIB->pDIB && pSourceDIB->pDIB->pDIBBits) {
		if (pMatchDIB && pMatchDIB->pDIB && pMatchDIB->pDIB->pDIBBits) {
			return TRUE;
		}
	}
	//ASSERT(0);
	return FALSE;
}
BOOL SDoMatchPattern::ValidCheckdTdI() {
	if ((dI == -1) || (dT == -1)) {
		ASSERT(0);
		return FALSE;
	}
	if (dI < 1) {
		dI = 1;
	}
	if (dT < 1) {
		dT = 1;
	}
	return TRUE;
}
BOOL SDoMatchPattern::SetMatchRegionA(BOOL bReduceArea) {
	if ((sourceWd == -1) || (sourceHt == -1) || (wdA == -1) || (htA == -1)) {
		ASSERT(0);
		return FALSE;
	}
	// Specify full image to search [11/27/2012 Yuen]
	startX = startY = 6;
	endX = sourceWd - wdA - 6;
	endY = sourceHt - htA - 6;
	return TRUE;
}
BOOL SDoMatchPattern::SetMatchRegionB() {
	if ((sourceWd == -1) || (sourceHt == -1) || (wdB == -1) || (htB == -1)) {
		ASSERT(0);
		return FALSE;
	}
	// Specify full image to search [11/27/2012 Yuen]
	startX = startY = 6;
	endX = sourceWd - wdB - 6;
	endY = sourceHt - htB - 6;
	return TRUE;
}

C413AppPatt::C413AppPatt() {}

C413AppPatt::~C413AppPatt() {}

// Matching with reduced area [5/25/2014 Yuen]
// PaSet start from 1 [11/3/2012 Yuen]
// SDiB image to search for a match [11/27/2012 Yuen]
// ppDIB pattern to search for [11/27/2012 Yuen]
// Returned value is in image coordinate [11/27/2012 Yuen]
// Used by: DoMatchingA [5/1/2014 Administrator]
BOOL C413AppPatt::Match(SDoPattrnMatching& Param, SLocMatch& Loc, BOOL bReduceArea) {
	Param.Par.ValidCheckdTdI();
	Param.Par.SetMatchRegionA(bReduceArea); // May move to caller function [6/9/2014 Yuen]
	return DoMatchPattern(Loc, Param);
}

// Matching without reduced area [5/25/2014 Yuen]
// PaSet starts from 1 [11/3/2012 Yuen]
// Assume SDiB is properly loaded [5/16/2013 user]
// Use to match confirmation image [5/2/2014 Administrator]
BOOL C413AppPatt::Match2(SDoPattrnMatching& Param, SLocMatch& Loc) {
	Param.Par.ValidCheckdTdI();
	Param.Par.GetWidthHeight();
	Param.Par.SetMatchRegionB();	// May move to caller function [6/9/2014 Yuen]
	return DoMatchPattern(Loc, Param);
}

// PaSet starts from 1 [11/3/2012 Yuen]
// Limit the search around expected match location[5/2/2014 Administrator]
BOOL C413AppPatt::Match3(SDoPattrnMatching& Param, SLocMatch& Loc, SLocMatch& SizeC) {
	if (!Param.Par.IsValid()) {
		return FALSE;
	}

	Param.Par.ValidCheckdTdI();
	Param.Par.GetWidthHeight();

	// Search area [5/25/2014 Yuen]
	int swd = Param.Par.sourceWd * 0.5f;	// Match area [6/25/2013 Yuen]
	int sht = Param.Par.sourceHt * 0.5f;	// Match area [6/25/2013 Yuen]

	// if search area is smaller tha template size, increase the search area by 32 pixels [5/25/2014 Yuen]
	if (swd < (Param.Par.wdA + 32)) {
		swd = Param.Par.wdA + 32;
		if ((swd % 2) == 0) {
			// need odd amount [7/23/2013 Yuen]
			swd++;
		}
	}
	if (sht < (Param.Par.htA + 32)) {
		sht = Param.Par.htA + 32;
		if ((sht % 2) == 0) {
			// need odd amount [7/23/2013 Yuen]
			sht++;
		}
	}

	Param.Par.startX = SizeC.nX - swd / 2 - Param.Par.wdA / 2;
	Param.Par.startY = SizeC.nY - sht / 2 - Param.Par.htA / 2;
	Param.Par.endX = SizeC.nX + swd / 2 - Param.Par.wdA / 2;
	Param.Par.endY = SizeC.nY + sht / 2 - Param.Par.htA / 2;

	if (Param.Par.endY >= (Param.Par.sourceHt - Param.Par.htA - 3)) {
		Param.Par.endY = Param.Par.sourceHt - Param.Par.htA - 3;
		Param.Par.startY = Param.Par.endY - sht;
		if (Param.Par.startY < 0) {
			Param.Par.startY = 0;
		}
	}
	if (Param.Par.startY < 0) {
		Param.Par.startY = 0;
		Param.Par.endY = sht;
	}
	if (Param.Par.endX >= (Param.Par.sourceWd - Param.Par.wdA - 3)) {
		Param.Par.endX = Param.Par.sourceWd - Param.Par.wdA - 3;
		Param.Par.startX = Param.Par.endX - swd;
		if (Param.Par.startX < 0) {
			Param.Par.startX = 0;
		}
	}
	if (Param.Par.startX < 0) {
		Param.Par.startX = 0;
		Param.Par.endX = swd;
	}

	return DoMatchPattern(Loc, Param);
}

// PaSet starts from 1 [11/14/2012 Yuen]
// Return status of match result [2/24/2013 Administrator]
C413AppPatt::PATMATCHRTN C413AppPatt::DoPattrnMatching(SDoPattrnMatching& Param) {
	PATMATCHRTN iret = pmFAILED;
	if ((Param.PaSet == -1) || (Param.MatchAlgorithm == -1) || (Param.SearchDepth == -1) || (Param.PatMatOpt == -1)) {
		ASSERT(0);
		return iret;
	}

	if (Param.MatchAlgorithm == 0) {
		// Method 1 [7/27/2013 Yuen]
		// This function return with the image at matched position [11/27/2012 Yuen]
		// Motor is moved [3/19/2013 FSMT]
		iret = DoMatchingA(Param);
		switch (iret) {
		case  pmFAILED:
			MsgStr = "MATCH FAILED";
			Message(Param.pList, MsgStr);
			break;
		case pmOK:
			MoveToOffsetMatchA(0, Param);
			MsgStr = "MATCH SUCCESS";
			Message(Param.pList, MsgStr);
			return pmOK;
			break;
		}
		return pmFAILED;
	}
	else if (Param.MatchAlgorithm == 4) {
		// Method 4 [4/30/2014 Administrator]
		// This function uses up to 4 primary template to locate the ROI in turn [4/30/2014 Administrator]
		for (int k = 0; k < 4; k++) {
			Param.PaSSet = k;
			Param.MoveDist.nX = Param.MoveDist.nY = 0;
			Param.OffDist.nX = Param.OffDist.nY = 0;
			if (Global.bAborting) {
				break;
			}
			// This function return with the image NOT at matched position [11/27/2012 Yuen]
			// SizeE contains the offsets in pixels of the match image from green cross hair [2/21/2013 Administrator]
			// No stage movement [7/17/2013 Yuen]
			ULONGLONG tick = GetTickCount64();
			iret = DoMatchingB(Param);	// No stage movement [8/5/2013 Yuen]
			MsgStr.Format("PatMat: MatchB %.2f secs", (GetTickCount64() - tick) / 1000.0f);
			Message(Param.pList, MsgStr);
			switch (iret) {
			case pmOK:
				// Move MoveToOffsetMatchB [4/20/2013 Administrator]
				if (Param.bApplyOffset) {
					ULONGLONG tick = GetTickCount64();
					MoveToOffsetMatchB(0, Param);
					if (Param.PatMatOpt & CRecipe413::MATCHATOFFSET) {
						// Perform final matching to accurately place the wafer [7/27/2013 Yuen]
						iret = DoMatchingC(Param);
					}
					MsgStr.Format("PatMat: OffsetB %.2f secs", (GetTickCount64() - tick) / 1000.0f);
					Message(Param.pList, MsgStr);
					MsgStr = "MATCH SUCCESS";
					Message(Param.pList, MsgStr);
				}
				else if (Param.bMoveCenter) {
					ULONGLONG tick = GetTickCount64();
					MoveMatchPatCenter(Param);
					MsgStr.Format("PatMat: CenterB %.2f secs", (GetTickCount64() - tick) / 1000.0f);
					Message(Param.pList, MsgStr);
				}
				return iret;
				break;
			case pmNOMORE:
				// Fall through [7/28/2013 Yuen]
			case pmFAILED:
				//return iret;
				break;
			case pmABORT:
				return iret;
				break;
			case pmMOVES:
				// Fall through intended [2/24/2013 Administrator]
			case pmMOVE:
				//if (Param.bMoveCenter)
				//{
					//ULONGLONG tick = GetTickCount64();
					//MoveMatchPatCenter(Param);
					//MsgStr.Format("PatMat: CenterB %.2f secs",(GetTickCount64()-tick)/1000.0f);
					//Message(Param.pList, MsgStr);
				//}
				break;
			}
		}
		return pmFAILED;
	}
	else if ((Param.MatchAlgorithm == 1) || (Param.MatchAlgorithm == 3)) {
		// Method 2 [7/27/2013 Yuen]
		// Perform max 6 times (for the sake of simulation) in response to pmMove and pmMoves, other will break out of the loop [3/20/2013 FSMT]
		Param.MoveDist.nX = Param.MoveDist.nY = 0;
		Param.OffDist.nX = Param.OffDist.nY = 0;
		for (int i = 0; i < Param.SearchDepth; i++) {
			if (Global.bAborting) {
				break;
			}
			// This function return with the image NOT at matched position [11/27/2012 Yuen]
			// SizeE contains the offsets in pixels of the match image from green cross hair [2/21/2013 Administrator]
			// No stage movement [7/17/2013 Yuen]
			if (Param.MatchAlgorithm == 3) {
				short bakPaSet = Param.PaSet;
				int j = Param.PaSet;
				for (; j < MAXMATCHPATT; j++) {
					Param.PaSet = j;
					iret = DoMatchingB(Param);
					if (iret == pmOK) {
						// PaSet is modified [7/28/2013 Yuen]
						//Param.pRcp->ChangePaSet(j);
						break;
					}
					if (iret == pmNOMORE) {
						Param.PaSet = bakPaSet;
						break;
					}
					if ((iret == pmMOVE) || (iret == pmMOVES)) {
						break;
					}
				}
				if (j >= MAXMATCHPATT) {
					Param.PaSet = bakPaSet;
				}
			}
			else {
				iret = DoMatchingB(Param);	// No stage movement [8/5/2013 Yuen]
			}
			switch (iret) {
			case pmOK:
				// Move MoveToOffsetMatchB [4/20/2013 Administrator]
				if (Param.bApplyOffset) {
					ULONGLONG tick = GetTickCount64();
					MoveToOffsetMatchB(0, Param);
					if (Param.PatMatOpt & CRecipe413::MATCHATOFFSET) {
						// Perform final matching to accurately place the wafer [7/27/2013 Yuen]
						iret = DoMatchingC(Param);
					}
					MsgStr.Format("PatMat: OffsetB %.2f secs", (GetTickCount64() - tick) / 1000.0f);
					Message(Param.pList, MsgStr);
					MsgStr = "MATCH SUCCESS";
					Message(Param.pList, MsgStr);
				}
				return iret;
				break;
			case pmNOMORE:
				// Fall through [7/28/2013 Yuen]
			case pmFAILED:
				return iret;
				break;
			case pmABORT:
				return iret;
				break;
			case pmMOVES:
				// Fall through intended [2/24/2013 Administrator]
			case pmMOVE:
				//if (Param.bMoveCenter)
				//{
				ULONGLONG tick = GetTickCount64();
				MoveMatchPatCenter(Param);
				MsgStr.Format("PatMat: CenterB %.2f secs", (GetTickCount64() - tick) / 1000.0f);
				Message(Param.pList, MsgStr);
				//}
				//else return pmOK;
				break;
			}
		}
	}
	return iret;
}

// Depend on PaSSet, Param.Par.pMatchDIB will be loaded appropriately by caller [6/8/2014 Yuen]
C413AppPatt::PATMATCHRTN C413AppPatt::DoMatchingA(SDoPattrnMatching& Param) {
	if ((!Param.pVideo)) {
		ASSERT(0);  // Cannot do pattern matching if no video [9/12/2011 Administrator]
		return pmFAILED;
	}
	if (Param.SearchDistance == -1) {
		ASSERT(0);
		return pmFAILED;
	}
	MsgStr = "Method 1";
	Message(Param.pList, MsgStr);

	if (Param.pImgWnd) {
		Param.pImgWnd->ClearOverlay();
	}

	CRect rc;
	Param.pVideo->GetClientRect(&rc);

	float dist;
	CString str;
	int rcw = rc.Width();
	int rch = rc.Height();
	SLocMatch SizeC;  // Coordinate in image space [9/13/2011 Administrator]
	SLocMatch SizeD;  // Coordinate in image space [9/13/2011 Administrator]
	SLocMatch SizeE;  // Coordinate in image space [9/13/2011 Administrator]

	if (pMtr) {
		pMtr->GetXYPos(&Param.initX, &Param.initY);		// Record starting location [11/8/2011 C2C]
	}

	//LoadSimuDIB(Param.pVideo);

	for (int i = 0; i < Param.SearchDepth; i++) {
		if (Global.bAborting || Global.bStop) {
			return pmABORT;	// Outer loop will take care of exiting [11/8/2012 Yuen]
		}

		float X2 = 0, Y2 = 0;
		if (pMtr) {
			pMtr->GetXYPos(&X2, &Y2);
			MsgStr.Format("Loc: %.3f, %.3f   Trial %d", X2, Y2, i + 1);
			Message(Param.pList, MsgStr);
		}
		if (Param.SearchDistance > 0) {
			// Terminate search if move too far away from expectation [5/15/2013 user]
			dist = sqrt(Param.initX * Param.initX + Param.initY * Param.initY) - sqrt(X2 * X2 + Y2 * Y2);
			if (fabs(dist) > Param.SearchDistance) {
				MsgStr.Format("Wondering too far, dR: %.3f", fabs(dist));
				Message(Param.pList, MsgStr);
				return pmTOOFAR;
			}
		}
		if (Param.pVideo->DoubleSnapOneFrame()) {
			Param.Par.SetSourceDIB(&Global.StoredDIB);
		}
		else {
			MsgStr = "Video error";
			Message(Param.pList, MsgStr);
			return pmFAILED;
		}

		// SizeC contains the coordinate of the center of Match DIB with respect to image space [9/10/2011 XPMUser]
		Param.Par.pMatchDIB = Param.Par.pMatchDibA;
		if (GetMatchPosition(SizeC, Param, FALSE)) {
			if (0) {
				short wd = Param.Par.pMatchDIB->IWidth / 2;
				short ht = Param.Par.pMatchDIB->IHeight / 2;
				for (int y = -2; y < 2; y++) {
					for (int x = -2; x < 2; x++) {
						Param.Par.pSourceDIB->SetPixelRGB(SizeC.nX + x, SizeC.nY + y, 255, 0, 0);
						Param.Par.pSourceDIB->SetPixelRGB(SizeC.nX + x - wd, SizeC.nY + y - ht, 255, 255, 0);
						Param.Par.pSourceDIB->SetPixelRGB(SizeC.nX + x + wd, SizeC.nY + y + ht, 255, 255, 0);
						Param.Par.pSourceDIB->SetPixelRGB(SizeC.nX + x - wd, SizeC.nY + y + ht, 255, 255, 0);
						Param.Par.pSourceDIB->SetPixelRGB(SizeC.nX + x + wd, SizeC.nY + y - ht, 255, 255, 0);
					}
				}
				Param.Par.pSourceDIB->Save("C:\\TEMP\\A.BMP");
			}
			SizeE = SizeC;
		}
		else {
			return pmFAILED;
		}

		if (Param.Par.GoF > 0) {
			if (SizeC.GOF < Param.Par.GoF) {
				MsgStr.Format("FAILED GOF Constrain: GOF %.3f > Set GOF %.3f", SizeC.GOF, Param.Par.GoF);
				Message(Param.pList, MsgStr);
				return pmFAILLIMIT;
			}
		}

		// Confirming matching accuracy if secondary image is available [4/16/2013 Administrator]
		if (Param.Par.pMatchDibB && Param.Par.pMatchDibB->pDIB && Param.Par.pMatchDibB->pDIB->pDIBBits) {
			if (Param.MatchRes == -1) {
				ASSERT(0);
				return pmFAILED;
			}
			// Check whether secondary image is visible base on position of supposed primary position [4/16/2013 Administrator]
			short dIx = Param.Par.pSourceDIB->IWidth * (Param.Par.pMatchDibB->fTgtX - Param.Par.pMatchDibA->fTgtX); // Relative distance between Green and Blue cross hair [4/16/2013 Administrator]
			short dIy = Param.Par.pSourceDIB->IHeight * (Param.Par.pMatchDibB->fTgtY - Param.Par.pMatchDibA->fTgtY); // Relative distance between Green and Blue cross hair, fTgtY is reverse [4/16/2013 Administrator]
			short Wdib2 = Param.Par.pMatchDibB->IWidth / 2;
			short Hdib2 = Param.Par.pMatchDibB->IHeight / 2;

			short dx, dy;
			dx = SizeC.nX + dIx;
			dy = SizeC.nY + dIy;
			if ((dx <= Wdib2) || (dx >= (Param.Par.pSourceDIB->IWidth - Wdib2)) ||
				(dy <= Hdib2) || (dy >= (Param.Par.pSourceDIB->IHeight - Hdib2))) {
				// Need moving. Move primary image to green cross hair [4/16/2013 Administrator]
				SizeC.nX = SizeC.nX - Param.Par.pSourceDIB->IWidth * Param.Par.pMatchDibA->fTgtX;
				SizeC.nY = -(SizeC.nY - Param.Par.pSourceDIB->IHeight * Param.Par.pMatchDibA->fTgtY);	// Value reversed to motor space [6/8/2014 Yuen]
				MsgStr = "PAT2 is out of bound, move PAT1 to green cross hair";
				Message(Param.pList, MsgStr);
				MsgStr.Format("Delta(%.3f,%.3f)pix", SizeC.nX, SizeC.nY);
				Message(Param.pList, MsgStr);
				Param.MoveDist = SizeC;
				MoveMatchPatCenter(Param);
				continue;
			}

			// SizeD contains the coordinate of the center of Match DIB with respect to main image [9/10/2011 XPMUser]
			Param.Par.pMatchDIB = Param.Par.pMatchDibB;
			if (Match2(Param, SizeD)) {
				if (0) {
					short wd = Param.Par.pMatchDIB->IWidth / 2;
					short ht = Param.Par.pMatchDIB->IHeight / 2;
					for (int y = -2; y < 2; y++) {
						for (int x = -2; x < 2; x++) {
							Param.Par.pSourceDIB->SetPixelRGB(SizeD.nX + x, SizeD.nY + y, 255, 0, 0);
							Param.Par.pSourceDIB->SetPixelRGB(SizeD.nX + x - wd, SizeD.nY + y - ht, 255, 255, 0);
							Param.Par.pSourceDIB->SetPixelRGB(SizeD.nX + x + wd, SizeD.nY + y + ht, 255, 255, 0);
							Param.Par.pSourceDIB->SetPixelRGB(SizeD.nX + x - wd, SizeD.nY + y + ht, 255, 255, 0);
							Param.Par.pSourceDIB->SetPixelRGB(SizeD.nX + x + wd, SizeD.nY + y - ht, 255, 255, 0);
						}
					}
					Param.Par.pSourceDIB->Save("C:\\TEMP\\B.BMP");
				}
			}
			else {
				return pmFAILED;
			}

			if (Param.pImgWnd) {
				ShrinkMatchDIB(2, Param.Par.pMatchDibB->pDIB, SizeD, Param.pImgWnd);
			}

			str.Format("PAT2: Loc (%d, %d)   GOF:%.4f", SizeD.nX, SizeD.nY, SizeD.GOF);
			Message(Param.pList, str);

			// Check relativity [4/16/2013 Administrator]
			// Image  is consider found if relative distance of green and blue cross hair is close to relative distance between the two match images [4/16/2013 Administrator]
			// Relative distance between PAT1 and PAT2 should be similar to relative distance between green and blue cross hair [4/20/2013 Administrator]
			short x = Param.Par.pSourceDIB->IWidth * (Param.Par.pMatchDibA->fTgtX); // Relative distance between Green and Blue crosshair [4/16/2013 Administrator]
			short y = Param.Par.pSourceDIB->IHeight * (Param.Par.pMatchDibA->fTgtY); // Relative distance between Green and Blue crosshair [4/16/2013 Administrator]
			dx = (SizeE.nX) - x;
			dy = (SizeE.nY) - y;
			if ((abs(dx) < Param.MatchRes) && (abs(dy) < Param.MatchRes)) {
				// Compare relative distance of both template [6/10/2014 FSMT]
				x = Param.Par.pSourceDIB->IWidth * (Param.Par.pMatchDibA->fTgtX - Param.Par.pMatchDibB->fTgtX);
				y = Param.Par.pSourceDIB->IHeight * (Param.Par.pMatchDibA->fTgtY - Param.Par.pMatchDibB->fTgtY);
				dx = (SizeE.nX - SizeD.nX) - x;
				dy = (SizeE.nY - SizeD.nY) - y;
				if ((abs(dx) < Param.MatchRes) && (abs(dy) < Param.MatchRes)) {
					MsgStr = "MATCH CONFIRMED";
					Message(Param.pList, MsgStr);
					return pmOK;
				}
				return pmFAILED;
			}

			MsgStr.Format("MATCH not within MatchRes %d: %d %d", Param.MatchRes, dx, dy);
			Message(Param.pList, MsgStr);

			SizeC.nX = SizeC.nX - Param.Par.pSourceDIB->IWidth * Param.Par.pMatchDibA->fTgtX;
			SizeC.nY = -(SizeC.nY - abs(Param.Par.pSourceDIB->IHeight) * Param.Par.pMatchDibA->fTgtY);
			Param.MoveDist = SizeC;
			MoveMatchPatCenter(Param);
		}
		else {
			//////////////////////////////////////////////////////////////////////////
			// Processing when secondary image is not available [4/16/2013 Administrator]
			//////////////////////////////////////////////////////////////////////////
			SizeC.nX = SizeC.nX - (Param.Par.pSourceDIB->IWidth * Param.Par.pMatchDibA->fTgtX);
			SizeC.nY = -(SizeC.nY - abs(Param.Par.pSourceDIB->IHeight) * Param.Par.pMatchDibA->fTgtY);

			// NOTE: SizeC now contains the amount to move to reach green crosshair [4/16/2013 Administrator]

			BOOL bDone = TRUE;
			// Check if it is centered [4/16/2013 Administrator]
			if ((abs(SizeC.nX) <= Param.MatchRes) && (abs(SizeC.nY) <= Param.MatchRes)) {
				if (Param.Par.GoF > 0) {
					if (SizeC.GOF > Param.Par.GoF) {
					}
					else {
						bDone = FALSE;
						MsgStr.Format("FAILED GOF Constrain: GOF %.3f > Set GOF %.3f", SizeC.GOF, Param.Par.GoF);
						Message(Param.pList, MsgStr);
					}
				}
				if (Param.pList && bDone) {
					dist = sqrt(Param.initX * Param.initX + Param.initY * Param.initY) - sqrt(X2 * X2 + Y2 * Y2);
					str.Format("dx:%.3f dy:%.3f GOF:%.4f dR:%.3f", SizeC.nX, SizeC.nY, SizeC.GOF, fabs(dist));
					Message(Param.pList, str);
					return pmOK;
				}
			}
			Param.MoveDist = SizeC;
			MoveMatchPatCenter(Param);
		}
	}
	return pmFAILED;
}

// PaSet starts from 1 [11/14/2012 Yuen]
// SizeE return the offset of pattern from center of image [2/6/2013 Administrator]
// This function will not move the stage, instead it provide the offset of match patter to either the green or blue cross hair [11/27/2012 Yuen]
// Must snap a live image before entering function [6/8/2014 Yuen]
C413AppPatt::PATMATCHRTN C413AppPatt::DoMatchingB(SDoPattrnMatching& Param) {
	CRect rc;
	CString str;

	if (Param.SearchDistance == -1) {
		ASSERT(0);
		return pmFAILED;
	}
	if (Param.MatchAlgorithm == 3) {
		if (!Param.Par.pMatchDibB || !Param.Par.pMatchDibB->pDIB || !Param.Par.pMatchDibB->pDIB->pDIBBits) {
			MsgStr = "Method 3 requires pattern B";
			Message(Param.pList, MsgStr);
			return pmNOMORE;
		}
		MsgStr = "Method 3";
		Message(Param.pList, MsgStr);
	}
	else {
		if (!Param.Par.pMatchDibB || !Param.Par.pMatchDibB->pDIB || !Param.Par.pMatchDibB->pDIB->pDIBBits) {
			MsgStr = "Method 2 requires pattern B";
			Message(Param.pList, MsgStr);
			return pmNOMORE;
		}
		MsgStr = "Method 2";
		Message(Param.pList, MsgStr);
	}

	Param.pVideo->GetClientRect(&rc);

	float dist;
	int rcw = rc.Width();
	int rch = rc.Height();

	SLocMatch SizeC;  // Intermediate. Coordinate in image space [9/13/2011 Administrator]

	if (Param.pImgWnd) {
		Param.pImgWnd->ClearOverlay();
	}
	if (Global.bAborting) {
		return pmABORT;	// Outer loop will take care of exiting [11/8/2012 Yuen]
	}
	if (Param.SearchDistance > 0) {
		float X2 = Param.finalX - Param.initX;
		float Y2 = Param.finalY - Param.initY;
		dist = sqrt(X2 * X2 + Y2 * Y2);
		MsgStr.Format("Distance check, dR: %.3f mm", fabs(dist));
		Message(Param.pList, MsgStr);
		if (fabs(dist) > Param.SearchDistance) {
			MsgStr.Format("Wondering too far, dR: %.3f mm", fabs(dist));
			Message(Param.pList, MsgStr);
			return pmFAILED;
		}
	}

	if (Param.pVideo->DoubleSnapOneFrame()) {
		Param.Par.SetSourceDIB(&Global.StoredDIB);
	}
	else {
		MsgStr = "Video error";
		Message(Param.pList, MsgStr);
		return pmFAILED;
	}

	Param.Par.pMatchDIB = Param.Par.pMatchDibA;	// User template A [6/9/2014 Yuen]
	BOOL bOK = GetMatchPosition(SizeC, Param, FALSE);

	int cX = Param.Par.sourceWd * 0.5f;  // center of image  [6/26/2013 Yuen]
	int cY = Param.Par.sourceHt * 0.5f;	// unit in pix [6/26/2013 Yuen]

	// Locate match pattern in image, no stage movement [2/6/2013 Administrator]
	// This is the green cursor location [6/9/2014 Yuen]
	int pXg = Param.Par.pMatchDIB->fTgtX * Param.Par.sourceWd;
	int pYg = Param.Par.pMatchDIB->fTgtY * Param.Par.sourceHt;

	str.Format("PAT1: C(%d, %d) pix  G(%d, %d) pix  G-C(%d,%d) pix",
		SizeC.nX - cX, cY - SizeC.nY, SizeC.nX - pXg, pYg - SizeC.nY, pXg - cX, (cY - pYg));
	Message(Param.pList, str);
	if (!bOK) {
		// Error, could not continue [3/2/2012 Administrator]
		return pmFAILED;
	}

	Param.MatchPos = SizeC;

	float fXg, fYg, fXb, fYb;

	fXg = Param.Par.pMatchDibA->fTgtX;
	fYg = Param.Par.pMatchDibA->fTgtY;
	fXb = Param.Par.pMatchDibB->fTgtX;
	fYb = Param.Par.pMatchDibB->fTgtY;

	BOOL bConfPatUse = FALSE;
	if (Param.Par.pMatchDibB && Param.Par.pMatchDibB->pDIB && Param.Par.pMatchDibB->pDIB->pDIBBits) {
		bConfPatUse = TRUE;
		// dimension of secondary image [5/25/2014 Yuen]
		int swd = Param.Par.pMatchDibB->pDIB->BmpInfo.biWidth;
		int sht = abs(Param.Par.pMatchDibB->pDIB->BmpInfo.biHeight);
		// Perform second pattern match if available [2/6/2013 Administrator]
		// Get offset distance of pattern 2 from pattern 1 [11/27/2012 Yuen]
		SizeC.nX += (fXb - fXg) * Param.Par.sourceWd;
		SizeC.nY += (fYb - fYg) * Param.Par.sourceHt;
		// SizeC is now suppose to be at blue cross hair [4/16/2013 Administrator]
		// If SizeC is outside of image, cannot match and need to move image [11/27/2012 Yuen]
		if ((SizeC.nX) <= 0 || (SizeC.nX >= (Param.Par.sourceWd - swd - 1)) || ((SizeC.nY) <= 0) || (SizeC.nY >= (Param.Par.sourceHt - sht - 1))) {
			// Second pattern is off screen [2/6/2013 Administrator]
			// Move stage, repeat matching process [11/27/2012 Yuen]
			// MoveDist is reference to green cross hair [6/24/2013 FSM413]
			Param.MoveDist = Param.MatchPos;
			Param.MoveDist.nX = Param.MoveDist.nX - fXg * Param.Par.sourceWd;
			Param.MoveDist.nY = -(Param.MoveDist.nY - fYg * Param.Par.sourceHt);	// Y in image space is flipped [4/20/2013 Administrator]
			MsgStr.Format("Move required, (%d,%d) pix", Param.MoveDist.nX, Param.MoveDist.nY);
			Message(Param.pList, MsgStr);
			return pmMOVE;
		}
		// At return, Param.ConfPos contains the coordinates of second pattern on screen [2/6/2013 Administrator]
		Param.Par.pMatchDIB = Param.Par.pMatchDibB;
		bOK = GetConfirmPosition(Param, Param.ConfPos, SizeC);
		MsgStr.Format("PAT2: C(%d, %d) pix  B(%d, %d) pix", SizeC.nX - cX, cY - SizeC.nY, int(SizeC.nX - fXb * Param.Par.sourceWd), int(fYb * Param.Par.sourceHt - SizeC.nY));
		Message(Param.pList, MsgStr);
		if (!bOK) {
			// Error, could not continue [3/2/2012 Administrator]
			MsgStr = "Error pattern matching";
			Message(Param.pList, MsgStr);
			return pmFAILED;
		}
		int dvX = SizeC.nX - Param.ConfPos.nX;
		int dvY = SizeC.nY - Param.ConfPos.nY;
		if ((abs(dvX) > Param.MatchRes) || (abs(dvY) > Param.MatchRes)) {
			// Confirm DIB position incorrect [6/25/2013 Yuen]
			MsgStr.Format("MATCH FAILED: ConfPos: dXY(%d,%d) MatchRes(%d)", dvX, dvY, Param.MatchRes);
			Message(Param.pList, MsgStr);
			return pmFAILED;
		}
	}
	else {
		// Confirmation pattern not specified [11/27/2012 Yuen]
		// Move pattern to center of screen and redo matching if too far from center [11/27/2012 Yuen]
		// SizeC now contains the displacement from center of image [4/20/2013 Administrator]
		// Displacement of PAT from green cursor [4/20/2013 Administrator]
		Param.MoveDist.nX = Param.MatchPos.nX - pXg;
		Param.MoveDist.nY = -(Param.MatchPos.nY - pYg);	// Y in image space is flipped [4/20/2013 Administrator]

		// Check if it is centered [4/16/2013 Administrator]
		if (!((abs(Param.MoveDist.nX) <= Param.MatchRes) && (abs(Param.MoveDist.nY) <= Param.MatchRes))) {
			MsgStr.Format("Move required, (%d,%d) pix", Param.MoveDist.nX, Param.MoveDist.nY);
			Message(Param.pList, MsgStr);
			return pmMOVES;
		}
	}

	// Calculate offsets from center of view, unit in pixels [6/25/2013 FSM413]
	Param.OffDist.nX = Param.MatchPos.nX - cX;
	Param.OffDist.nY = -(Param.MatchPos.nY - cY);	// Y in image space is flipped [4/20/2013 Administrator]

	// Calculate offsets from green cursor, unit in pixels [6/25/2013 FSM413]
	Param.MoveDist.nX = Param.MatchPos.nX - pXg;
	Param.MoveDist.nY = -(Param.MatchPos.nY - pYg);	// Y in image space is flipped [4/20/2013 Administrator]

	MsgStr.Format("MoveDist(%d, %d) pix", Param.MoveDist.nX, Param.MoveDist.nY);
	Message(Param.pList, MsgStr);

	MsgStr.Format("OffsetDist(%d,%d) pix", Param.OffDist.nX, Param.OffDist.nY);
	Message(Param.pList, MsgStr);

	Param.finalX = Global.LocXY.X;
	Param.finalY = Global.LocXY.Y;
	//pMtr->GetXYPos(&Param.finalX,&Param.finalY);
	MsgStr.Format("End Loc (%.3f,  %.3f) mm", Param.finalX, Param.finalY);

	Message(Param.pList, MsgStr);

	MsgStr.Format("MATCH SUCCESS");
	Message(Param.pList, MsgStr);

	return pmOK;
}

// Note: All values in Param must be valid
// Loc contain the match point in main image space [4/20/2013 Administrator]
// Param startX,startY,endX,endY specify the area to search without taking template size into consideration [7/23/2013 Yuen]
BOOL C413AppPatt::DoMatchPattern(SLocMatch& Loc, SDoPattrnMatching& Param) {
	if (!Param.Par.IsValid()) {
		return FALSE;
	}
	Param.Par.ValidCheckdTdI();

	Loc.Clear();
	Loc.Set(0, 0, 0);

	short BPPi = Param.Par.pSourceDIB->pDIB->BmpInfo.biBitCount / 8;
	int bpri = Param.Par.sourceWd * BPPi;
	bpri += (4 - bpri % 4) % 4;

	short BPPd = Param.Par.pMatchDIB->pDIB->BmpInfo.biBitCount / 8;
	int bpr = Param.Par.wdA * BPPd;
	bpr += (4 - bpr % 4) % 4;

	short dI1 = (Param.Par.dT - 1) * BPPi;	// Number of bytes to skip [12/3/2011 XPMUser]
	if (dI1 < 0) dI1 = 0;
	if (BPPi > 3) {
		// skip alpha channel for pixel in calculation [9/18/2011 FSMT]
		dI1++;	// assume BPPi = 4 only [5/4/2014 Yuen]
	}

	short dT1 = (Param.Par.dT - 1) * BPPd;	// Number of bytes to skip [12/3/2011 XPMUser]
	if (dT1 < 0) {
		dT1 = 0;
	}
	if (BPPd > 3) {
		// skip alpha channel for pixel in calculation [9/18/2011 FSMT]
		dT1++;	// assume BPPi = 4 only [5/4/2014 Yuen]
	}

	PBYTE pS = Param.Par.pSourceDIB->pDIB->pDIBBits;
	PBYTE pM = Param.Par.pMatchDIB->pDIB->pDIBBits;

	short wd = Param.Par.pMatchDIB->IWidth;
	short ht = Param.Par.pMatchDIB->IHeight;

	CSingleLock Lock(&Global.StoredDIB.CS);
	Lock.Lock();
	for (int y = Param.Par.startY; y < Param.Par.endY; y += Param.Par.dI) {
		for (int x = Param.Par.startX; x < Param.Par.endX; x += Param.Par.dI) {
			double sum = 0;
			double sum1 = 0;
			double sum2 = 0;
			double sum3 = 0;
			for (int ay = 0; ay < ht; ay += Param.Par.dT) {
				BYTE* pI = pS + ((y + ay) * bpri) + BPPi * x;
				BYTE* pT = pM + (ay * bpr);
				if (!Param.Par.bFast) {
					for (int ax = 0; ax < wd; ax += Param.Par.dT) {
						sum1 += ((*pI) * (*pT));
						sum2 += ((*pI) * (*pI));
						sum3 += ((*pT) * (*pT));

						pT++; pI++;
						sum1 += ((*pI) * (*pT));
						sum2 += ((*pI) * (*pI));
						sum3 += ((*pT) * (*pT));		// Store Sum3 in a buffer because it is invariant [11/10/2011 C2C]
						pT++; pI++;

						sum1 += ((*pI) * (*pT));
						sum2 += ((*pI) * (*pI));
						sum3 += ((*pT) * (*pT));
						pT++; pI++;

						if (dI1) {
							pI += (dI1); pT += (dT1);  // skip the skipped bytes [10/19/2011 Administrator]
						}
					}
				}
				else {
					for (int ax = 0; ax < wd; ax += Param.Par.dT) {
						pT++; pI++;	// Needed, do not delete [7/23/2013 Yuen]
						sum1 += ((*pI) * (*pT));
						sum2 += ((*pI) * (*pI));
						sum3 += ((*pT) * (*pT));		// Store Sum3 in a buffer because it is invariant [11/10/2011 C2C]
						pT++; pI++;
						pT++; pI++;	// Needed, do not delete [7/23/2013 Yuen]

						if (dI1) {
							pI += (dI1); pT += (dT1);  // skip the skipped bytes [10/19/2011 Administrator]
						}
					}
				}
			}
			sum = fabs(sum1 / sqrt((sum2) * (sum3)));
			if (sum > Loc.GOF) {
				Loc.Set(x, y, sum);
			}
		}
	}
	Lock.Unlock();

	// Add offset to center of match pattern [9/18/2011 Administrator]
	Loc.nX += (Param.Par.pMatchDIB->IWidth / 2);
	Loc.nY += (Param.Par.pMatchDIB->IHeight / 2);

	return TRUE;
}

// Shrink and store match dib as overlay in pImgWnd [4/16/2013 Administrator]
void C413AppPatt::ShrinkMatchDIB(int n, SIPackedDIB* pDIB, SLocMatch& SizeC, CImgWnd* pImgWnd) {
	if (!pImgWnd) {
		return;
	}

	int w2 = pDIB->BmpInfo.biWidth / 2;
	int h2 = abs(pDIB->BmpInfo.biHeight) / 2;

	short BPP = pDIB->BmpInfo.biBitCount / 8;
	long BytesPerRow = pDIB->BmpInfo.biWidth * BPP;
	BytesPerRow += (4 - BytesPerRow % 4) % 4;	// int alignment

	long BytesPerRow2 = w2 * BPP;
	BytesPerRow2 += (4 - BytesPerRow2 % 4) % 4;	// int alignment

	long SizeImage = BytesPerRow2 * h2;
	BYTE* p = new BYTE[SizeImage + 2 * BytesPerRow2];  // Half size image buffer [9/13/2011 Administrator]
	CreateHalfSizeDIB(pDIB, p, SizeImage);

	// cosmetics: Draw border
	BYTE* pp = p;
	BYTE* p2 = p + BytesPerRow2 * (h2 - 1);
	memset(pp, 255, BytesPerRow2);  // image has been reduced by half [8/11/2010 XPMUser]
	memset(p2, 255, BytesPerRow2);  // image has been reduced by half [8/11/2010 XPMUser]
	for (int y = 1; y < h2; y++) {
		memset(p + y * BytesPerRow2 - BPP, 255, 2 * BPP);
	}

	SIPackedDIB* pDIB1 = new SIPackedDIB;
	if (!pDIB1) {
		return;
	}
	pDIB1->BmpInfo = pDIB->BmpInfo;
	pDIB1->BmpInfo.biSize = sizeof(BITMAPINFOHEADER);
	pDIB1->BmpInfo.biWidth = w2;
	pDIB1->BmpInfo.biHeight = h2 * sign(pDIB->BmpInfo.biHeight);
	pDIB1->BmpInfo.biSizeImage = SizeImage;
	pDIB1->pDIBBits = p;

	switch (n) {
	case 1:
		pImgWnd->pOverlayDIB1->ClearDIB();
		pImgWnd->pOverlayDIB1->pDIB = pDIB1;  // divide by 2 because image has been shrunk by half [6/3/2010 Valued Customer]
		pImgWnd->pOverlayDIB1->CalcBytePerRow();
		pImgWnd->Oly1CX = SizeC.nX;
		pImgWnd->Oly1CY = SizeC.nY;
		{
			CPackedDIB* pO = pImgWnd->pOverlayDIB1;
			int cx = pO->GetWidth() / 2;
			int cy = pO->GetHeight() / 2;
			pO->SetPixelRGB(cx, cy, 255, 0, 0);
			pO->SetPixelRGB(cx + 1, cy, 255, 0, 0);
			pO->SetPixelRGB(cx, cy + 1, 255, 0, 0);
			pO->SetPixelRGB(cx - 1, cy, 255, 0, 0);
			pO->SetPixelRGB(cx, cy - 1, 255, 0, 0);
		}
		pImgWnd->Refresh();
		break;
	default:
		pImgWnd->pOverlayDIB2->ClearDIB();
		pImgWnd->pOverlayDIB2->pDIB = pDIB1;  // divide by 2 because image has been shrunk by half [6/3/2010 Valued Customer]
		pImgWnd->pOverlayDIB2->CalcBytePerRow();
		pImgWnd->Oly2CX = SizeC.nX;
		pImgWnd->Oly2CY = SizeC.nY;
		{
			CPackedDIB* pO = pImgWnd->pOverlayDIB2;
			int cx = pO->GetWidth() / 2;
			int cy = pO->GetHeight() / 2;
			pO->SetPixelRGB(cx, cy, 255, 0, 0);
			pO->SetPixelRGB(cx + 1, cy, 255, 0, 0);
			pO->SetPixelRGB(cx, cy + 1, 255, 0, 0);
			pO->SetPixelRGB(cx - 1, cy, 255, 0, 0);
			pO->SetPixelRGB(cx, cy - 1, 255, 0, 0);
		}
		pImgWnd->Refresh();
		break;
	}
}

// Shrink and store pDIB to pIngWnd [11/27/2012 Yuen]
BOOL C413AppPatt::ShrinkStoredDIB(SIPackedDIB* pDIBSrc, CImgWnd* pImgWnd) {
	if (!pDIBSrc || !pDIBSrc->pDIBBits || !pImgWnd) {
		return FALSE;
	}

	// pDIB is original full size image [3/2/2012 Administrator]
	int wd = pDIBSrc->BmpInfo.biWidth;
	int ht = abs(pDIBSrc->BmpInfo.biHeight);
	short BPP = pDIBSrc->BmpInfo.biBitCount / 8;
	short BPP2 = 2 * BPP;

	// Full size image [3/2/2012 Administrator]
	long BPR1 = wd * BPP;
	BPR1 += (4 - BPR1 % 4) % 4;	// int alignment

	// Half size image [3/2/2012 Administrator]
	long BPR2 = (wd / 2) * BPP;
	BPR2 += (4 - BPR2 % 4) % 4;	// int alignment

	BYTE* p = new BYTE[BPR2 * (ht) / 2 + 2 * BPR2];  // destination image is half size, add one and extra 1 for divide by 2 problem [8/11/2010 XPMUser]
	if (!p) {
		// Memory allocation problem [3/2/2012 Administrator]
		return FALSE;
	}
	BYTE* q = (BYTE*)pDIBSrc->pDIBBits;

	int n = 0;
	for (int y = 0; y < ht; y += 2) {
		BYTE* qq = q + y * BPR1;		// Full size image [3/2/2012 Administrator]
		BYTE* pp = p + n * BPR2; n++;	// Half size image [3/2/2012 Administrator]
		for (int x = 0; x < wd; x += 2) {
			memcpy(pp, qq, BPP);
			pp += BPP; qq += BPP2;
		}
	}

	SIPackedDIB* pDIB1 = new SIPackedDIB;
	if (pDIB1) {
		pDIB1->BmpInfo = pDIBSrc->BmpInfo;
		pDIB1->BmpInfo.biSize = sizeof(BITMAPINFOHEADER);
		pDIB1->BmpInfo.biWidth = pDIBSrc->BmpInfo.biWidth / 2;
		pDIB1->BmpInfo.biHeight = pDIBSrc->BmpInfo.biHeight / 2;
		pDIB1->BmpInfo.biSizeImage = BPR2 * abs(pDIB1->BmpInfo.biHeight);
		pDIB1->pDIBBits = p;

		if (pImgWnd->pPackedDIB1->pDIB) {
			pImgWnd->pPackedDIB1->ClearDIB();
		}
		pImgWnd->pPackedDIB1->pDIB = pDIB1;  // divide by 2 because image has been shrunk by half [6/3/2010 Valued Customer]

		pImgWnd->Refresh();
		return TRUE;
	}
	delete[] p;
	return FALSE;
}

double C413AppPatt::GetScore(SIPackedDIB* pDIBi, SIPackedDIB* pDIB) {
	if (!pDIB || !pDIBi) {
		return -1;
	}
	short BPP = pDIBi->BmpInfo.biBitCount / 8;
	int bpri = pDIBi->BmpInfo.biWidth * BPP;
	bpri += (4 - bpri % 4) % 4;

	int bpr = pDIB->BmpInfo.biWidth * BPP;
	bpr += (4 - bpr % 4) % 4;

	int x = (pDIBi->BmpInfo.biWidth - pDIB->BmpInfo.biWidth) / 2;
	int y = (abs(pDIBi->BmpInfo.biHeight) - abs(pDIB->BmpInfo.biHeight)) / 2;

	double sum = 0;
	double sum1 = 0;
	double sum2 = 0;
	double sum3 = 0;
	for (int ay = 0; ay < (abs(pDIB->BmpInfo.biHeight)); ay++) {
		BYTE* pI = pDIBi->pDIBBits + ((y + ay) * bpri) + BPP * x;
		BYTE* pT = pDIB->pDIBBits + (ay * bpr);
		for (int ax = x; ax < (x + pDIB->BmpInfo.biWidth); ax++) {
			// 			sum1 += ((*pI) * (*pT));
			// 			sum2 += ((*pI) * (*pI));
			// 			sum3 += ((*pT) * (*pT));
			sum += ((*pI) - (*pT));
			pT++; pI++;
			// 			sum1 += ((*pI) * (*pT));
			// 			sum2 += ((*pI) * (*pI));
			// 			sum3 += ((*pT) * (*pT));
			sum += ((*pI) - (*pT));
			pT++; pI++;
			// 			sum1 += ((*pI) * (*pT));
			// 			sum2 += ((*pI) * (*pI));
			// 			sum3 += ((*pT) * (*pT));
			sum += ((*pI) - (*pT));
			pT++; pI++;
			if (BPP > 3) {
				// Skip alpha channel [9/18/2011 FSMT]
				pT++; pI++;
			}
		}
	}
	// 	return sum = fabs(sum1 / (sqrt(sum2) * sqrt(sum3)));
	return sum;
}
void C413AppPatt::CreateHalfSizeDIB(SIPackedDIB* pDIB, BYTE* pBuf, long SizeImage) {
	if (!pBuf) {
		ASSERT(0);
		return;
	}
	int w = pDIB->BmpInfo.biWidth;
	int h = abs(pDIB->BmpInfo.biHeight);
	short BPP = pDIB->BmpInfo.biBitCount / 8;
	short BPP2 = 2 * BPP;

	long BytesPerRow = w * BPP;
	BytesPerRow += (4 - BytesPerRow % 4) % 4;	// int alignment

	long BytesPerRow2 = w / 2 * BPP;
	BytesPerRow2 += (4 - BytesPerRow2 % 4) % 4;	// int alignment

	BYTE* p = pBuf;						// Half size image buffer [9/13/2011 Administrator]
	BYTE* q = (BYTE*)pDIB->pDIBBits;	// Original image buffer [9/13/2011 Administrator]

	BYTE* pp;
	BYTE* qq;

	for (int y = 0; y < 2 * (h / 2); y += 2) {
		qq = q + y * BytesPerRow;		// original buffer [9/18/2011 FSMT]
		pp = p + y / 2 * BytesPerRow2;	// new buffer [9/18/2011 FSMT]
		for (int x = 0; x < 2 * (w / 2); x += 2) {
			memcpy(pp, qq, BPP);
			pp += BPP; qq += BPP2;
		}
	}
}

// Find location of Match Pattern [11/27/2012 Yuen]
// Return SizeC as the center of match position [5/25/2014 Yuen]
BOOL C413AppPatt::GetMatchPosition(SLocMatch& SizeC, SDoPattrnMatching& Param, BOOL bReduceArea) {
	Param.Par.IsValid();
	Param.Par.pSourceDIB->PreProcess();	// Pre-process captured image [11/4/2012 Yuen]

	if (!ShrinkStoredDIB(Param.Par.pSourceDIB->pDIB, Param.pImgWnd))	// Shrink and Store capture image in pImgWnd [11/27/2012 Yuen]
	{
		return FALSE;
	}
	// SizeC contains the coordinate of the center of Match DIB with respect to main image [9/10/2011 XPMUser]

	if (!Match(Param, SizeC, bReduceArea)) {
		MsgStr = "FAILED MATCH";
		Message(Param.pList, MsgStr);
		return FALSE;
	}
	if (Param.pImgWnd) {
		ShrinkMatchDIB(1, Param.Par.pMatchDIB->pDIB, SizeC, Param.pImgWnd);
	}

	MsgStr.Format("PAT1: Loc (%d, %d)pix   GOF(%.3f)", SizeC.nX, SizeC.nY, SizeC.GOF);
	Message(Param.pList, MsgStr);

	return TRUE;
}

// Find location of ConfirmPattern [11/27/2012 Yuen]
// PaSet starts from 1 [11/27/2012 Yuen]
// SizeC is the general area where matching should be done [11/27/2012 Yuen]
BOOL C413AppPatt::GetConfirmPosition(SDoPattrnMatching& Param, SLocMatch& SizeD, SLocMatch& SizeC) {
	if (Match3(Param, SizeD, SizeC)) {
		if (Param.pImgWnd) {
			ShrinkMatchDIB(2, Param.Par.pMatchDibB->pDIB, SizeD, Param.pImgWnd);
			if (Param.pVideo->IsSimu()) {
				Param.pImgWnd->pPackedDIB1->SetPixelRGB(SizeD.nX, SizeD.nY, 255, 0, 0);
				Param.pImgWnd->pPackedDIB1->SetPixelRGB(SizeD.nX + 1, SizeD.nY, 255, 0, 0);
				Param.pImgWnd->pPackedDIB1->SetPixelRGB(SizeD.nX, SizeD.nY + 1, 255, 0, 0);
				Param.pImgWnd->pPackedDIB1->SetPixelRGB(SizeD.nX - 1, SizeD.nY, 255, 0, 0);
				Param.pImgWnd->pPackedDIB1->SetPixelRGB(SizeD.nX, SizeD.nY - 1, 255, 0, 0);
			}
			Param.pImgWnd->Refresh();
		}
		MsgStr.Format("PAT2: Loc (%d, %d) pix  GOF(%.3f)", SizeD.nX, SizeD.nY, SizeD.GOF);
		Message(Param.pList, MsgStr);
		return TRUE;
	}
	return FALSE;
}

// SizeE should contains displacement from center of image [4/20/2013 Administrator]
BOOL C413AppPatt::MoveMatchPatCenter(SDoPattrnMatching& Param) {
	if (!Param.pVideo) {
		ASSERT(0);
		return FALSE;
	}
	float dx = 0, dy = 0;
	if (Param.pVideo && Param.pVideo->IsSimu()) {
		Param.pVideo->LoadStockDIB("BSI\\CENTER1.BMP");
	}
	// Displacement [9/13/2011 Administrator]
	// Convert to motor unit [3/21/2012 Yuen]
	switch (Param.pVideo->iCID) {
	default:
		dx = Global.ScrnCalibECH.dX * (Param.MoveDist.nX) * Global.ScrnCalibECH.ScrnX / float(Param.pVideo->ImgWd);
		dy = Global.ScrnCalibECH.dY * (Param.MoveDist.nY) * Global.ScrnCalibECH.ScrnY / float(Param.pVideo->ImgHt);
		break;
	case CCamBase::THINFCAM:
		dx = Global.ScrnCalibTF.dX * Param.MoveDist.nX * Global.ScrnCalibTF.ScrnX / float(Param.pVideo->ImgWd);
		dy = Global.ScrnCalibTF.dY * Param.MoveDist.nY * Global.ScrnCalibTF.ScrnY / float(Param.pVideo->ImgHt);
		break;
	}

	if ((fabs(dx) < 2.0f) && (fabs(dy) < 2.0f)) {
		if (pMtr) {
			pMtr->MoveRXY(dx, dy);	// Y is reverse [6/10/2014 FSMT]
			MsgStr.Format("Pat2Center: Relative move (%.3f %.3f) mm", dx, dy);
			Message(Param.pList, MsgStr);
			return TRUE;
		}
	}
	MsgStr = "Move Skipped because move distance too far";
	Message(Param.pList, MsgStr);
	return FALSE;
}

// Assumption: match point is at center of image [4/20/2013 Administrator]
void C413AppPatt::MoveToOffsetMatchA(short n, SDoPattrnMatching& Param) {
	// Save an image before moving [7/5/2013 Yuen]
	if (Param.bSaveVideo && Param.pMP) {
		// Take a landing picture here [7/4/2013 Yuen]
		// No refresh in bSimuVideo [7/4/2013 Yuen]
		Param.pVideo->FileDirc.Format("C:\\TEMP\\%s\\%s-%s-%d\\", CTime::GetCurrentTime().Format("%y%m%d"), Param.CarrierID, Param.RcpeName, Param.SlotID);
		Param.pVideo->FileBase.Format("%03d-02.BMP", Param.pMP->Co.n);
		Param.pVideo->bStoreComposite = TRUE;
		while (Param.pVideo->bStoreComposite) {
			Sleep(40);
		}
	}

	if (Param.pVideo && Param.pVideo->IsSimu()) {
		Param.pVideo->LoadStockDIB("BSI\\MEASURE.BMP");
	}
	if (pMtr) {
		if ((Param.POffsetX == -9999) && (Param.POffsetY == -9999)) {
			// No further action needed [6/8/2014 Yuen]
			return;
		}
		MsgStr = "";
		float X, Y;
		pMtr->GetXYPos(&Param.finalX, &Param.finalY);
		if (pGDev->SystemModel == CGlobalDeviceContainer::FSM413C2C) {
			X = Param.POffsetX + Param.PatOffX;
			Y = Param.POffsetY + Param.PatOffY;
			pMtr->MoveRXY(X, Y);
			MsgStr.Format("MOVE OFFSET (mm): dX=%.3f  dY=%.3f", X, Y);
		}
		else if (pGDev->SystemModel == CGlobalDeviceContainer::FSM413SA) {
			X = -(Param.POffsetX + Param.PatOffX);
			Y = -(Param.POffsetY + Param.PatOffY);
			pMtr->MoveRXY(X, Y);
			MsgStr.Format("MOVE OFFSET (mm): dX=%.3f  dY=%.3f", X, Y);
		}
		else if (pGDev->SystemModel == CGlobalDeviceContainer::FSM413MOT) {
			X = Param.POffsetX + Param.PatOffX;
			Y = -(Param.POffsetY + Param.PatOffY);
			pMtr->MoveRXY(X, Y);
			MsgStr.Format("MOVE OFFSET (mm): dX=%.3f  dY=%.3f", X, Y);
		}
		Message(Param.pList, MsgStr);
	}
}

void C413AppPatt::MoveToOffsetMatchB(short n, SDoPattrnMatching& Param) {
	if (!Param.pVideo) {
		ASSERT(0);
		return;
	}

	Param.finalX = Global.LocXY.X;
	Param.finalY = Global.LocXY.Y;
	//if (pMtr) pMtr->GetXYPos(&Param.finalX,&Param.finalY);

	float dX, dY;
	switch (Param.pVideo->iCID) {
	default:
		dX = Global.ScrnCalibECH.dX * Global.ScrnCalibECH.ScrnX / float(Param.pVideo->ImgWd);
		dY = Global.ScrnCalibECH.dY * Global.ScrnCalibECH.ScrnY / float(Param.pVideo->ImgHt);
		break;
	case CCamBase::THINFCAM:
		dX = Global.ScrnCalibTF.dX * Global.ScrnCalibTF.ScrnX / float(Param.pVideo->ImgWd);
		dY = Global.ScrnCalibTF.dY * Global.ScrnCalibTF.ScrnY / float(Param.pVideo->ImgHt);
		break;
	}
	// Current match position at current position
	Param.pVideo->PaOffX = Param.PatOffX = Param.MoveDist.nX * dX;
	Param.pVideo->PaOffY = Param.PatOffY = Param.MoveDist.nY * dY;
	// Get ready for the next point [7/17/2013 Yuen]
	// Offset from landing to to green cross hair [7/23/2013 Yuen]

	// Save an image before moving [7/5/2013 Yuen]
	if (Param.bSaveVideo && Param.pMP) {
		Param.pVideo->FileBase.Format("%03d-04.BMP", Param.pMP->Co.n);
		Param.pVideo->bStoreComposite = TRUE;
		while (Param.pVideo->bStoreComposite) {
			Sleep(40);
		}
	}

	if (Param.pVideo && Param.pVideo->IsSimu()) {
		if (Param.PatMatOpt == -1) {
			ASSERT(0);
			return;
		}
		if (Param.PatMatOpt & CRecipe413::MATCHATOFFSET) {
			LoadSimuDIB2(Param.pVideo);
		}
		else {
			Param.pVideo->LoadStockDIB("BSI\\MEASURE.BMP");
		}
	}

	// Performing motor movement here [3/19/2013 FSMT]
	if (pMtr) {
		CString str;
		str.Format("PatOff: %.3f %.3f", Param.PatOffX, Param.PatOffY);
		Message(Param.pList, str, 2);
		str.Format("Final Center: X=%.3f  Y=%.3f", Param.finalX, Param.finalY);
		Message(Param.pList, str, 2);

		if ((Param.POffsetX == -9999) || (Param.POffsetY == -9999)) {
			ASSERT(0);
			return;
		}
		float X = Param.POffsetX + Param.PatOffX;
		float Y = Param.POffsetY + Param.PatOffY;
		pMtr->MoveRXY(X, -Y);

		// Save an image after moving [7/5/2013 Yuen]
		if (Param.bSaveVideo && Param.pMP) {
			Param.pVideo->FileBase.Format("%03d-05.BMP", Param.pMP->Co.n);
			Param.pVideo->bStoreComposite = TRUE;
			while (Param.pVideo->bStoreComposite) {
				Sleep(40);
			}
		}
	}
}

void C413AppPatt::PreprocessBeforeMatching(CRecipe413* pRcp, short CurPaSet) {
	// Auto Exposure [4/20/2013 Administrator]
	return;
	if (pRcp->PatMatOpt[CurPaSet - 1] & CRecipe413::EXPB4FOCUS) {
	}
	// Auto Focus [4/20/2013 Administrator]
	if (pRcp->PatMatOpt[CurPaSet - 1] & CRecipe413::FOCB4MATCH) {
	}
	// Auto Exposure [4/20/2013 Administrator]
	if (pRcp->PatMatOpt[CurPaSet - 1] & CRecipe413::EXPAFOCUS) {
	}
}

void C413AppPatt::PreprocessAfterMatching(CRecipe413* pRcp, short CurPaSet) {
	// Auto Focus [4/20/2013 Administrator]
	return;
	if (pRcp->PatMatOpt[CurPaSet - 1] & CRecipe413::FOCAMATCH) {
	}
	// Auto Exposure [4/20/2013 Administrator]
	if (pRcp->PatMatOpt[CurPaSet - 1] & CRecipe413::EXPAMATCH) {
	}
}

short C413AppPatt::DoPatternMatchingWithRetrialTF(SDoPattrnMatching& Param, CRecipe413* pRcp) {
	if (!pMtr || !Param.pMP) {
		ASSERT(0);
		return pmFAILED;
	}
	if (Param.pMP->PatSet == 0) {
		return pmOK;
	}

	int j;
	C413Global* g = &Global;

	if (Param.bSaveVideo && Param.pMP) {
		Param.pVideo->PaOffX = Param.pMP->Co.x;
		Param.pVideo->PaOffY = Param.pMP->Co.y;
		CSingleLock Lock(&Param.pVideo->RsltSpec.CS);
		Lock.Lock();
		// Take a landing picture here [7/4/2013 Yuen]
		Param.pVideo->RsltSpec.Clear();
		Lock.Unlock();
		Sleep(100); // wait refresh (must wait) [7/10/2013 Yuen]
		Param.pVideo->FileDirc.Format("C:\\TEMP\\%s\\%s-%s-%d\\", CTime::GetCurrentTime().Format("%y%m%d"), Param.CarrierID, Param.RcpeName, Param.SlotID);
		Param.pVideo->FileBase.Format("%03d-01.BMP", Param.pMP->Co.n);
		Param.pVideo->bStoreComposite = TRUE;
		while (Param.pVideo->bStoreComposite) {
			Sleep(40);
		}
	}

	PreMatchingTreatment(CCamBase::THINFCAM, pRcp, Param.pMP, Param.pVideo, Param.pList);

	if (Param.bSaveVideo && Param.pMP) {
		// Take a post video treated picture here [7/4/2013 Yuen]
		Param.pVideo->FileBase.Format("%03d-02-0.BMP", Param.pMP->Co.n);
		Param.pVideo->bStoreComposite = TRUE;
		while (Param.pVideo->bStoreComposite) {
			Sleep(40);
		}
	}
	for (j = 1; j <= Param.nTrialPat; j++) {
		if (g->bAborting) {
			return C413AppPatt::pmABORT;
		}
		MsgStr.Format("Trial #%d", j);
		Message(Param.pList, MsgStr);

		ULONGLONG tick = GetTickCount64();
		if (DoPattrnMatching(Param) == pmOK) {
			MsgStr.Format("PatMat Total: %.2f secs", (GetTickCount64() - tick) / 1000.0);
			Message(Param.pList, MsgStr);

			// Image at measurement location [7/17/2013 Yuen]
			PostMatchingTreatment(CCamBase::THINFCAM, pRcp, Param.pMP, Param.pVideo, Param.pList);
			if (Param.bSaveVideo && Param.pMP) {
				Param.pVideo->PaOffX = Param.PatOffX;
				Param.pVideo->PaOffY = Param.PatOffY;
				if (Param.bSaveVideo) {
					Sleep(100);     // Video refresh [7/17/2013 Yuen]
					Param.pVideo->FileBase.Format("%03d-06.BMP", Param.pMP->Co.n);
					Param.pVideo->bStoreComposite = TRUE;
					while (Param.pVideo->bStoreComposite) {
						Sleep(80);
					}
				}
			}
			return pmOK;
		}

		// Image could be anywhere because of MoveToCenter function inside DoPattrnMatching [7/17/2013 Yuen]
		LoadSimuDIB(Param.pVideo);

		// Not found, move to new location [6/28/2013 Yuen]
		float lX = Param.pMP->Co.x + gPX[j] * Param.fStepSizePat;
		float lY = Param.pMP->Co.y + gPY[j] * Param.fStepSizePat;

		//20131017-D
		//char tch[255];
		//sprintf(tch,"<< New Loc1 X = %.3f Y = %.3f", Param.pMP->Co.x+gPX[j]*Param.fStepSizePat, Param.pMP->Co.y+gPY[j]*Param.fStepSizePat);
		//OutputDebugString(tch);
		//sprintf(tch,"<< New Loc2 X = %.3f Y = %.3f", lX, lY);
		//OutputDebugString(tch);

		if (!pMtr->GotoXY(lX, lY)) {
			return FALSE;
		}
		MsgStr.Format("NewPos(%d): X=%.3f Y=%.3f", j, lX, lY);
		Message(Param.pList, MsgStr);

		if (Param.bSaveVideo && Param.pMP) {
			Param.pVideo->PaOffX = Param.pMP->Co.x;
			Param.pVideo->PaOffY = Param.pMP->Co.y;
			Param.pVideo->FileBase.Format("%03d-02-%d.BMP", Param.pMP->Co.n, j);
			Param.pVideo->bStoreComposite = TRUE;
			while (Param.pVideo->bStoreComposite) {
				Sleep(80);
			}
			Param.pVideo->PaOffX = Param.pVideo->PaOffY = 0;
		}
	}
	return pmFAILED;
}

// Effective only when bApplyOffset is set [7/28/2013 Yuen]
C413AppPatt::PATMATCHRTN  C413AppPatt::DoMatchingC(SDoPattrnMatching& Param) {
	if (!Param.pVideo) {
		ASSERT(0);  // Cannot do pattern matching if no video [9/12/2011 Administrator]
		return pmFAILED;
	}
	// 	if ((Param.PaSet<1)||(Param.PaSet>MAXMATCHPATT))
	// 	{
	// 		ASSERT(0);	// Input error [11/4/2012 Yuen]
	// 		return pmFAILED;
	// 	}
	// 	if (!Param.pRcp->MatchDIBA2[Param.PaSet-1].pDIB)
	// 	{
	// 		// No match pattern [11/4/2012 Yuen]
	// 		return pmFAILED;
	// 	}
	// 	if (!Param.pRcp->MatchDIBA2[Param.PaSet-1].pDIB->pDIBBits)
	// 	{
	// 		// No match pattern [11/4/2012 Yuen]
	// 		return pmFAILED;
	// 	}

	MsgStr = "Method 3";
	Message(Param.pList, MsgStr);

	if (Param.pImgWnd) {
		Param.pImgWnd->ClearOverlay();
	}

	CRect rc;
	Param.pVideo->GetClientRect(&rc);

	float dist;
	CString str;
	int rcw = rc.Width();
	int rch = rc.Height();
	SLocMatch SizeC;  // Coordinate in image space [9/13/2011 Administrator]
	SLocMatch SizeD;  // Coordinate in image space [9/13/2011 Administrator]

	if (pMtr) {
		pMtr->GetXYPos(&Param.initX, &Param.initY);		// Record starting location [11/8/2011 C2C]
	}

	// Do not clear overlay inside SearchDepth look so user can see them longer [4/20/2013 Administrator]
// 	if (Param.pImgWnd)
// 	{
// 		Param.pImgWnd->pOverlayDIB1->ClearDIB();
// 		Param.pImgWnd->pOverlayDIB2->ClearDIB();
// 		Param.pImgWnd->Refresh();
// 	}

	for (int i = 0; i < Param.SearchDepth; i++) {
		if (Global.bAborting || Global.bStop) {
			return pmABORT;	// Outer loop will take care of exiting [11/8/2012 Yuen]
		}

		float X2 = 0, Y2 = 0;
		if (pMtr) {
			pMtr->GetXYPos(&X2, &Y2);
		}
		if (Param.SearchDistance > 0) {
			// Terminate search if move too far away from expectation [5/15/2013 user]
			dist = sqrt(Param.initX * Param.initX + Param.initY * Param.initY) - sqrt(X2 * X2 + Y2 * Y2);
			if (fabs(dist) > Param.SearchDistance) {
				MsgStr.Format("Wondering too far, dR: %.3f", fabs(dist));
				Message(Param.pList, MsgStr);
				return pmTOOFAR;
			}
		}
		if (!Param.pVideo->DoubleSnapOneFrame()) {
			MsgStr = "Video error";
			Message(Param.pList, MsgStr);
			return pmFAILED;
		}
		if (!Global.StoredDIB.pDIB) {
			return pmFAILED;
		}

		// SizeC contains the coordinate of the center of Match DIB with respect to image space [9/10/2011 XPMUser]
		if (!GetMatchPosition(SizeC, Param, FALSE)) return pmFAILED;
		if (Param.Par.GoF > 0) {
			if (SizeC.GOF < Param.Par.GoF) {
				MsgStr.Format("FAILED GOF Constrain: GOF %.3f > Set GOF %.3f", SizeC.GOF, Param.Par.GoF);
				Message(Param.pList, MsgStr);
				return pmFAILLIMIT;
			}
		}
		//////////////////////////////////////////////////////////////////////////
		// Processing when secondary image is not available [4/16/2013 Administrator]
		//////////////////////////////////////////////////////////////////////////
		CFPoint TgtRed;

		switch (Global.ActiveCamera) {
		case CCamBase::ECHOCAM:
			TgtRed = Global.fTgtRedECHO;
			break;
		default:
			TgtRed = Global.fTgtRedTF;
			break;
		}

		// SizeC now show difference from red cross hair [7/28/2013 Yuen]
		SizeC.nX = SizeC.nX - (Global.StoredDIB.GetWidth() * TgtRed.X);
		SizeC.nY = -(SizeC.nY - abs(Global.StoredDIB.GetHeight()) * TgtRed.Y);

		// NOTE: SizeC now contains the amount to move to reach green cross hair [4/16/2013 Administrator]
		// Check if it is centered [4/16/2013 Administrator]
		if ((abs(SizeC.nX) <= Param.MatchRes) && (abs(SizeC.nY) <= Param.MatchRes)) {
			if (Param.pList) {
				dist = sqrt(Param.initX * Param.initX + Param.initY * Param.initY) - sqrt(X2 * X2 + Y2 * Y2);
				str.Format("dx:%.3f dy:%.3f GOF:%.4f dR:%.3f", SizeC.nX, SizeC.nY, SizeC.GOF, fabs(dist));
				Message(Param.pList, str);
				return pmOK;
			}
		}
		Param.MoveDist = SizeC;
		MoveMatchPatToRedCross(Param);
	}
	return pmFAILED;
}

void C413AppPatt::MoveMatchPatToRedCross(SDoPattrnMatching& Param) {
	float dX, dY;
	switch (Param.pVideo->iCID) {
	default:
		dX = Global.ScrnCalibECH.dX * Global.ScrnCalibECH.ScrnX / float(Param.pVideo->ImgWd);
		dY = Global.ScrnCalibECH.dY * Global.ScrnCalibECH.ScrnY / float(Param.pVideo->ImgHt);
		break;
	case CCamBase::THINFCAM:
		dX = Global.ScrnCalibTF.dX * Global.ScrnCalibTF.ScrnX / float(Param.pVideo->ImgWd);
		dY = Global.ScrnCalibTF.dY * Global.ScrnCalibTF.ScrnY / float(Param.pVideo->ImgHt);
		break;
	}

	// Current match position at current position
	Param.pVideo->PaOffX = Param.PatOffX = Param.MoveDist.nX * dX;
	Param.pVideo->PaOffY = Param.PatOffY = Param.MoveDist.nY * dY;
	// Get ready for the next point [7/17/2013 Yuen]
	// Offset from landing to to green cross hair [7/23/2013 Yuen]
	//Param.pVideo->CXOff = Param.XOff = Param.PatOffX + (Param.finalX - Param.initX);
	//Param.pVideo->CYOff = Param.YOff = Param.PatOffY + (Param.finalY - Param.initY);

	// Save an image before moving [7/5/2013 Yuen]
	if (Param.bSaveVideo && Param.pMP) {
		Param.pVideo->FileBase.Format("%03d-07.BMP", Param.pMP->Co.n);
		Param.pVideo->bStoreComposite = TRUE;
		while (Param.pVideo->bStoreComposite) {
			Sleep(80);
		}
	}

	if (Param.pVideo && Param.pVideo->IsSimu()) {
		Param.pVideo->LoadSimuDIB("C:\\TEMP\\BSI\\NEARTARGET.BMP");
	}

	// Performing motor movement here [3/19/2013 FSMT]
	if (pMtr) {
		pMtr->MoveRXY(Param.PatOffX, Param.PatOffY);
		// Save an image after moving [7/5/2013 Yuen]
		if (Param.bSaveVideo && Param.pMP) {
			Param.pVideo->FileBase.Format("%03d-05.BMP", Param.pMP->Co.n);
			Param.pVideo->bStoreComposite = TRUE;
			while (Param.pVideo->bStoreComposite) {
				Sleep(80);
			}
		}
	}
	if (Param.pImgWnd) {
		Param.pImgWnd->pOverlayDIB1->ClearDIB();
		Param.pImgWnd->pOverlayDIB2->ClearDIB();
		Param.pImgWnd->Refresh();
	}
}

BOOL C413AppPatt::SetMatchDIBA(CRecipe413* pRcp, SDoPattrnMatching& Param) {
	short PaSet = Param.PaSet - 1;
	if (PaSet < 0) return FALSE;	// 04132022 ZHIMING
	short PaSSet = Param.PaSSet;

	CPackedDIB* pDIB = NULL;
	switch (PaSSet) {
	case 0:
		pDIB = &pRcp->MatchDIBA1[PaSet];
		break;
	case 1:
		pDIB = &pRcp->MatchDIBA2[PaSet];
		break;
	case 2:
		pDIB = &pRcp->MatchDIBA3[PaSet];
		break;
	case 3:
		pDIB = &pRcp->MatchDIBA4[PaSet];
		break;
	}

	if (pDIB) {
		Param.Par.SetMatchDIBA(pDIB, pRcp->fTgtGRnX[PaSet][PaSSet], pRcp->fTgtGRnY[PaSet][PaSSet]);
		return TRUE;
	}
	return FALSE;
}

BOOL C413AppPatt::SetMatchDIBB(CRecipe413* pRcp, SDoPattrnMatching& Param) {
	short PaSet = Param.PaSet - 1;
	if (PaSet < 0) return FALSE;	// 04132022 ZHIMING
	short PaSSet = Param.PaSSet;
	
	CPackedDIB* pDIB = NULL;
	switch (PaSSet) {
	case 0:
		pDIB = &pRcp->MatchDIBB1[PaSet];
		break;
	case 1:
		pDIB = &pRcp->MatchDIBB2[PaSet];
		break;
	case 2:
		pDIB = &pRcp->MatchDIBB3[PaSet];
		break;
	case 3:
		pDIB = &pRcp->MatchDIBB4[PaSet];
		break;
	}

	if (pDIB) {
		Param.Par.SetMatchDIBB(pDIB, pRcp->fTgtBLuX[PaSet][PaSSet], pRcp->fTgtBLuY[PaSet][PaSSet]);
		return TRUE;
	}
	return FALSE;
}

void C413AppPatt::SetupParam(CRecipe413* pRcp, SDoPattrnMatching& Param, short PaSSet) {
	C413Global* g = &Global;
	short PaSet = g->CurPaSet - 1;
	if (PaSet < 0) PaSet = 0;	// 04132022 ZHIMING

	Param.MeSet = g->CurMeSet;
	Param.PaSet = g->CurPaSet;
	Param.PaSSet = PaSSet;
	Param.MatchAlgorithm = pRcp->MatchAlgorithm[PaSet];
	Param.SearchDepth = pRcp->SearchDept[PaSet];
	Param.SearchDistance = pRcp->SearchDist[PaSet];
	Param.PatMatOpt = pRcp->PatMatOpt[PaSet];
	Param.Par.dI = pRcp->SubSamI[PaSet];
	Param.Par.dT = pRcp->SubSamS[PaSet];
	Param.MatchRes = pRcp->MatchRes[PaSet];
	if (Param.pVideo) {
		Param.POffsetX = pRcp->POffsetX[PaSet][0];
		Param.POffsetY = pRcp->POffsetY[PaSet][0];
	}

	SetMatchDIBA(pRcp, Param);
	SetMatchDIBB(pRcp, Param);
}