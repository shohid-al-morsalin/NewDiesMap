#include "stdafx.h"
#include "E95.h"
#include "DiceView.h"
#include "WMap3DDlg.h"
#include "413App.h"


IMPLEMENT_DYNAMIC(CDiceView, CStatic)

CDiceView::CDiceView() {
	iX = 9999;
	iY = 9999;
	m_bgColor = RGB(125, 125, 125);
	rad = 75;
	
}

CDiceView::~CDiceView() {
	Clear();
}

BEGIN_MESSAGE_MAP(CDiceView, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()

// CDiceView message handlers

void CDiceView::OnPaint() {
	CPaintDC dc(this); // device context for painting
	//[10122024
	//if (p413App->Global.setDiceDisplay == true) {
		RedrawWithData();
	//}
	/*else {
		Redraw();
	}*/
	//]
	// Do not call CStatic::OnPaint() for painting messages
}

void CDiceView::Redraw(void) {
	CDC* pDC = GetDC();
	Draw(pDC);
	ReleaseDC(pDC);
}

#define DIMSON	350.0f

void CDiceView::Draw(CDC* pDC) {
	CString str;
	CRect rc;
	GetClientRect(&rc);
	CRgn Rg;
	Rg.CreateRectRgn(rc.left, rc.top, rc.right, rc.bottom);
	pDC->SelectClipRgn(&Rg);

	pDC->FillSolidRect(rc.left, rc.top, rc.Width(), rc.Height(), m_bgColor);

	float wd = DIMSON;
	float ht = DIMSON;
	float sfX = rc.Width() / wd;
	float sfY = rc.Height() / ht;
	int cx = rc.Width() / 2.0f;
	int cy = rc.Height() / 2.0f;

	pDC->Ellipse(cx - 150 * sfX, cy - 150 * sfY, cx + 150 * sfX, cy + 150 * sfY);
	CBrush* pB1 = new CBrush(RGB(0xC0, 0xC0, 0xC0));
	CBrush* pB2 = new CBrush(RGB(0xFF, 0xD0, 0xD0));
	CBrush* pOldBrush = pDC->SelectObject(pB1);

	pDC->SetBkMode(TRANSPARENT);

	POSITION pos = Sites.GetHeadPosition();
	while (pos) {
		CSiteItem* p = (CSiteItem*)Sites.GetNext(pos);
		if (p) {
			if (p->bSel) {
				pDC->SelectObject(pB1);
				pDC->Rectangle(cx + p->X * sfX, cy - p->Y * sfY, cx + (p->X + p->W) * sfX, cy - (p->Y + p->H) * sfY);
			}
			else {
				pDC->SelectObject(pB2);
				pDC->Rectangle(cx + p->X * sfX, cy - p->Y * sfY, cx + (p->X + p->W) * sfX, cy - (p->Y + p->H) * sfY);
			}
			str.Format("%d", p->n);
			//str.Format("%d", 15);
			pDC->TextOut(cx + p->X * sfX, cy - (p->Y + p->H) * sfY, str);
		}
	}

	pDC->SelectStockObject(NULL_BRUSH);
	pDC->Ellipse(cx - 150 * sfX, cy - 150 * sfY, cx + 150 * sfX, cy + 150 * sfY);
	pDC->SelectObject(pOldBrush);
	delete pB1;
	delete pB2;
}

void CDiceView::AddCell(float cx, float cy, float wd, float ht, short n) {
	CSiteItem* p = new CSiteItem;
	if (p) {
		p->Set(n, cx, cy, wd, ht);
		Sites.AddTail(p);
		Redraw();
	}
}

void CDiceView::Clear(void) {
	POSITION pos = Sites.GetHeadPosition();
	while (pos) {
		CSiteItem* p = (CSiteItem*)Sites.GetNext(pos);
		if (p) {
			delete p;
		}
	}
	Sites.RemoveAll();
}

void CDiceView::Locate(float fX, float fY) {
	POSITION pos = Sites.GetHeadPosition();
	while (pos) {
		CSiteItem* p = (CSiteItem*)Sites.GetNext(pos);
		if (p) {
			if ((fX >= p->X) && (fX <= (p->X + p->W))) {
				if ((fY >= p->Y) && (fY <= (p->Y + p->H))) {
					if (p->bSel) {
						p->bSel = FALSE;
					}
					else {
						p->bSel = TRUE;
					}
					break;
				}
			}
		}
	}
	Redraw();
}

void CDiceView::GetCoor(POINT& coord, float& fX, float& fY) {
	CRect rc;
	GetClientRect(&rc);
	float wd = DIMSON;
	float ht = DIMSON;
	float sfX = rc.Width() / wd;
	float sfY = rc.Height() / ht;

	ScreenToClient(&coord);
	int cx = coord.x - rc.Width() / 2.0f;
	int cy = coord.y - rc.Height() / 2.0f;

	fX = cx / sfX;
	fY = -cy / sfY;
}



void CDiceView::ClearSelectAll() {
	POSITION pos = Sites.GetHeadPosition();
	while (pos) {
		CSiteItem* p = (CSiteItem*)Sites.GetNext(pos);
		if (p) {
			p->bSel = FALSE;
		}
	}
	Redraw();
}

void CDiceView::SetSelectAll() {
	POSITION pos = Sites.GetHeadPosition();
	while (pos) {
		CSiteItem* p = (CSiteItem*)Sites.GetNext(pos);
		if (p) {
			p->bSel = TRUE;
		}
	}
	Redraw();
}

//[10122024


void CDiceView::ClearCells() {
	// Free the memory for all items in the list
	while (!Sites.IsEmpty()) {
		CSiteItem* p = (CSiteItem*)Sites.RemoveHead();
		delete p; // Clean up dynamically allocated memory
	}

	// Invalidate the view to clear the display
	//RedrawWithData();
    //Invalidate();
}


void CDiceView::RedrawWithData(void) {
	CDC* pDC = GetDC();
	DrawWithData(pDC);
	ReleaseDC(pDC);
}

//void CDiceView::DrawWithData(CDC* pDC) {
//	CString str;
//	CRect rc;
//	GetClientRect(&rc);
//	CRgn Rg;
//	Rg.CreateRectRgn(rc.left, rc.top, rc.right, rc.bottom);
//	pDC->SelectClipRgn(&Rg);
//
//	pDC->FillSolidRect(rc.left, rc.top, rc.Width(), rc.Height(), m_bgColor);
//
//	float wd = DIMSON;
//	float ht = DIMSON;
//	float sfX = rc.Width() / wd;
//	float sfY = rc.Height() / ht;
//	int cx = rc.Width() / 2.0f;
//	int cy = rc.Height() / 2.0f;
//
//	pDC->Ellipse(cx - 150 * sfX, cy - 150 * sfY, cx + 150 * sfX, cy + 150 * sfY);
//	CBrush* pB1 = new CBrush(RGB(0xC0, 0xC0, 0xC0));
//	CBrush* pB2 = new CBrush(RGB(0xFF, 0xD0, 0xD0));
//	CBrush* pOldBrush = pDC->SelectObject(pB1);
//
//	pDC->SetBkMode(TRANSPARENT);
//
//	POSITION pos = Sites.GetHeadPosition();
//	while (pos) {
//		CSiteItem* p = (CSiteItem*)Sites.GetNext(pos);
//		if (p) {
//			if (p->bSel) {
//				pDC->SelectObject(pB1);
//				pDC->Rectangle(cx + p->X * sfX, cy - p->Y * sfY, cx + (p->X + p->W) * sfX, cy - (p->Y + p->H) * sfY);
//			}
//			else {
//				pDC->SelectObject(pB2);
//				pDC->Rectangle(cx + p->X * sfX, cy - p->Y * sfY, cx + (p->X + p->W) * sfX, cy - (p->Y + p->H) * sfY);
//			}
//			str.Format("%d", p->n);
//			//str.Format("%d", 15);
//			pDC->TextOut(cx + p->X * sfX, cy - (p->Y + p->H) * sfY, str);
//		}
//	}
//
//	pDC->SelectStockObject(NULL_BRUSH);
//	pDC->Ellipse(cx - 150 * sfX, cy - 150 * sfY, cx + 150 * sfX, cy + 150 * sfY);
//	pDC->SelectObject(pOldBrush);
//	delete pB1;
//	delete pB2;
//}

BOOL CDiceView::IsPointInRectangle(float xCoords, float yCoords,float topLeftX, float topLeftY,float topRightX, float topRightY,float bottomRightX, float bottomRightY,float bottomLeftX, float bottomLeftY)
{

	/*if (xCoords > bottomLeftX && xCoords < bottomRightX && yCoords > bottomLeftY && yCoords < topLeftY)
	{
		return TRUE;
	}
	else return FALSE;*/
	// Vectors for rectangle edges
	float edge1X = topRightX - topLeftX;
	float edge1Y = topRightY - topLeftY;
	float edge2X = bottomLeftX - topLeftX;
	float edge2Y = bottomLeftY - topLeftY;

	// Vector from top-left to the point
	float pointVecX = xCoords - topLeftX;
	float pointVecY = yCoords - topLeftY;

	// Dot products
	float dot1 = (pointVecX * edge1X) + (pointVecY * edge1Y);
	float dot2 = (pointVecX * edge2X) + (pointVecY * edge2Y);

	// Lengths of the rectangle edges
	float edge1LengthSquared = (edge1X * edge1X) + (edge1Y * edge1Y);
	float edge2LengthSquared = (edge2X * edge2X) + (edge2Y * edge2Y);

	// Check if the point lies within the rectangle
	if (dot1 >= 0 && dot1 <= edge1LengthSquared && dot2 >= 0 && dot2 <= edge2LengthSquared)
	{
		return TRUE; // Point is inside
	}

	return FALSE; // Point is outside

}


//void CDiceView::DrawWithData(CDC* pDC) {
//	float dimson;
//	CString strOverCircle ;
//	if (rad == 150.00) {
//		dimson = 350.0f;
//	}
//	else if (rad == 100.00) {
//		dimson = 235.0f;
//	}
//	else if (rad == 75.00) {
//		dimson = 175.0f;
//	}
//	CString str;
//	CRect rc;
//	GetClientRect(&rc);
//	CRgn Rg;
//	Rg.CreateRectRgn(rc.left, rc.top, rc.right, rc.bottom);
//	pDC->SelectClipRgn(&Rg);
//
//	pDC->FillSolidRect(rc.left, rc.top, rc.Width(), rc.Height(), m_bgColor);
//
//	float wd = dimson;
//	float ht = dimson;
//	float sfX = rc.Width() / wd;
//	float sfY = rc.Height() / ht;
//	float minT = min(sfX, sfY);
//	sfX = minT;
//	sfY = minT;
//	//float sfY = sfX;
//	int cx = rc.Width() / 2.0f;
//	int cy = rc.Height() / 2.0f;
//
//	float sfX2 = sfX;
//	float sfY2 = sfY;
//	/*float sfX2 = rc.Width() / dimson;
//	float sfY2 = rc.Height() / dimson;*/
//
//	pDC->Ellipse(cx - rad * sfX, cy - rad * sfY, cx + rad * sfX, cy + rad * sfY);
//	CBrush* pB1 = new CBrush(RGB(0xC0, 0xC0, 0xC0));
//	CBrush* pB2 = new CBrush(RGB(0xFF, 0xD0, 0xD0));
//	CBrush* pOldBrush = pDC->SelectObject(pB1);
//
//	CBrush* pB3 = new CBrush(RGB(255, 128, 128)); // Red
//	CBrush* pB4 = new CBrush(RGB(0xFF, 0xFF, 0x00)); // Yellow
//	CBrush* pBGreen = new CBrush(RGB(0x9b, 0xff, 0xcd));//green
//
//	// New code to draw two additional strings on the top right side of the circle
//	CString firstString = _T("");
//	CString secondString = _T("");
//	bool isLtvFirst = false;
//	bool isDualMeasurement = false;
//
//	if (ltvIdx > stirIdx) {
//		firstString = _T("STIR");
//		secondString = _T("LTV");
//		isDualMeasurement = true;
//		isLtvFirst = false;
//	}
//	if (ltvIdx < stirIdx) {
//		firstString = _T("LTV");
//		secondString = _T("STIR");
//		isDualMeasurement = true;
//		isLtvFirst = true;
//	}
//	if (ltvIdx == -1 && stirIdx == -1) {
//		firstString = _T("");
//		secondString = _T("");
//		isDualMeasurement = false;
//	}
//
//	if (ltvIdx != -1 && stirIdx == -1) {
//		firstString = _T("");
//		secondString = _T("");
//		isDualMeasurement = false;
//		isLtvFirst = true;
//	}
//
//	if (ltvIdx == -1 && stirIdx != -1) {
//		firstString = _T("");
//		secondString = _T("");
//		isDualMeasurement = false;
//	}
//
//	// over circle title 
//
//	if (ltvIdx !=-1 && stirIdx != -1) {
//		strOverCircle = _T("LTV and STIR Measuremnt");
//
//	}
//	if (ltvIdx != -1 &&  stirIdx ==-1) {
//		strOverCircle = _T("LTV Measuremnt");
//		isLtvFirst = true;
//	}
//	if (ltvIdx == -1 && stirIdx != -1) {
//		strOverCircle = _T("STIR Measuremnt");
//	}
//	if (ltvIdx == -1 && stirIdx == -1) {
//		strOverCircle = _T("");
//	}
//
//	// Calculate text position over the circle
//	CSize circleTextSize = pDC->GetTextExtent(strOverCircle);
//	int circleTextX = cx - circleTextSize.cx / 2;
//	int circleTextY = cy - rad * sfY - circleTextSize.cy - 10;
//
//	// Draw string over the circle
//	pDC->TextOut(circleTextX, circleTextY, strOverCircle);
//
//
//	
//	
//
//	// Calculate text sizes
//	//Format("%%.%df", Me->MPa[j].D)
//	CSize firstStringSize = pDC->GetTextExtent(firstString);
//	CSize secondStringSize = pDC->GetTextExtent(secondString);
//
//	// Position the first string slightly to the right of the circle
//	int firstStringX = cx + rad * sfX + 5; // 5 pixels offset from the circle
//	int firstStringY = cy - rad * sfY - firstStringSize.cy + 150; 
//
//	// Position the second string below the first string
//	int secondStringX = firstStringX;
//	int secondStringY = firstStringY + firstStringSize.cy + 5; // 5 pixels below the first string
//
//	// Draw the additional strings
//	pDC->TextOut(firstStringX, firstStringY, firstString);
//	pDC->TextOut(secondStringX, secondStringY, secondString);
//
//	int diceID = 1;
//	pDC->SetBkMode(TRANSPARENT);
//
//	POSITION pos = Sites.GetHeadPosition();
//	while (pos) {
//		CSiteItem* p = (CSiteItem*)Sites.GetNext(pos);
//		if (p) {
//			if (p->n == -999) {
//
//				pDC->SelectObject(pB2);
//				//pDC->Rectangle(cx + p->X * sfX, cy - p->Y * sfY, cx + (p->X + p->W) * sfX, cy - (p->Y + p->H) * sfY);
//				pDC->Rectangle(cx + p->X * sfX2, cy- p->Y * sfY2, cx + (p->X + p->W) * sfX2, cy - (p->Y + p->H) * sfY2);
//			}
//			else {
//
//				
//				
//				p413App->Global.stt;
//				float xCoords =0.00, yCoords =0.00, fData1 =-9999.000, fData2 =-9999.000;
//				string firstData, secondData;
//				for (const auto& innerVec : p413App->Global.stt)
//				{
//					// Ensure the inner vector has 4 elements , both LTV and STIR
//					if (innerVec.size() == 4)
//					{
//						//xCoords = std::stof(innerVec[0]); // Extract the x-coordinate
//						//yCoords = std::stof(innerVec[1]); // Extract the y-coordinate
//						//data1 = std::stof(innerVec[2]);
//						//data2 = innerVec[3];
//
//						string s1, s2, s3, s4;
//						s1 = innerVec[0];
//						s2 = innerVec[1];
//						firstData = innerVec[2]; // LTV data string with header
//						secondData = innerVec[3]; // STIR data string with header
//
//						xCoords = std::stof(s1);// Extract the x-coordinate
//						yCoords = std::stof(s2);// Extract the y-coordinate
//						fData1 = std::stof(firstData);
//						fData2 = std::stof(secondData);
//						//string sub_str3 = "LTV";
//						//string sub_str4 = "STIR";
//
//						//size_t pos3 = s3.find(sub_str3);
//						//size_t pos4 = s4.find(sub_str4);
//						//if (pos3 != std::string::npos) {
//						//	//LTV
//						//	while (pos3 != string::npos) {
//						//		s3.erase(pos3, sub_str3.length());
//						//		pos3 = s3.find(sub_str3);
//						//	}
//						//	ltvData = std::stof(s3);
//						//}
//						//if (pos4 != std::string::npos) {
//						//	//STIR
//
//						//	while (pos4 != string::npos) {
//						//		s4.erase(pos4, sub_str4.length());
//						//		pos4 = s4.find(sub_str4);
//						//	}
//						//	stirData = std::stof(s4);
//						//}
//					}
//
//					float topLeftX = 0.00, topLeftY = 0.00, topRightX = 0.00, topRightY = 0.00;
//					float bottomLeftX = 0.00, bottomLeftY = 0.00, bottomRightX = 0.000, bottomRightY = 0.00;
//					
//
//					topLeftX = p->X; topLeftY = p->Y + p->H;
//					topRightX = p->X + p->W; topRightY = p->Y + p->H;
//					bottomLeftX = p->X; bottomLeftY = p->Y;
//					bottomRightX = p->X + p->W; bottomRightY = p->Y;
//
//					if (IsPointInRectangle(xCoords, yCoords, topLeftX, topLeftY, topRightX, topRightY, bottomRightX, bottomRightY, bottomLeftX, bottomLeftY))
//					{
//						float ltvHigh = p413App->Global.ltvHighLimit;
//						float ltvlow = p413App->Global.ltvLowLimit;
//						float stirHigh = p413App->Global.stirHighLimit;
//						float stirLow =  p413App->Global.stirLowLimit;
//
//						pDC->SelectObject(pBGreen); //Green pen
//
//						if (isDualMeasurement == true) {
//							if (ltvHigh == 0.00 && ltvlow == 0.00 && stirHigh == 0.00 && stirLow == 0.00) {
//								pDC->SelectObject(pBGreen); // choose green color if there is no high low value set from recipe
//							}
//							else {
//								// with custom limits
//								if (isLtvFirst == true) {
//									if (ltvHigh < fData1 && ltvlow > fData1) {
//										pDC->SelectObject(pB3); //Choose red color
//									}
//								}
//								if (isLtvFirst != true) {
//									if (stirHigh < fData2 && stirLow > fData2) {
//										pDC->SelectObject(pB3); //Choose red color
//									}
//								}
//							}
//						}
//						else {
//
//							if (ltvHigh == 0.00 && ltvlow == 0.00 && stirHigh == 0.00 && stirLow == 0.00) {
//								pDC->SelectObject(pBGreen); // choose green color if there is no high low value set from recipe
//							}
//							else {
//								// with custom limits
//								if (isLtvFirst == true) {
//									if (ltvHigh < fData1 && ltvlow > fData1) {
//										pDC->SelectObject(pB3); //Choose red color
//									}
//								}
//								if (isLtvFirst != true) {
//									if (stirHigh < fData1 && stirLow > fData1) {
//										pDC->SelectObject(pB3); //Choose red color
//									}
//								}
//							}
//
//						}
//
//						////13012025
//						// Calculate screen coordinates exactly 
//						vector<float> temp;
//						temp.emplace_back(p->X);              // Store original X
//						temp.emplace_back(p->Y);              // Store original Y
//						temp.emplace_back(p->X + p->W);       // Store right X
//						temp.emplace_back(p->Y + p->H);       // Store bottom Y
//						p413App->Global.rectangleMap[diceID++] = temp;
//
//
//						////
//						
//						pDC->Rectangle(cx + p->X * sfX2, cy - p->Y * sfY2, cx + (p->X + p->W) * sfX2, cy - (p->Y + p->H) * sfY2);
//						//str.Format("%.4f", fData1); // set precision 4 after . 
//						CString pre;
//						pre.Format("%%.%df", precision);
//						str.Format(pre, fData1);
//
//						// Calculate text size and position for centering
//						/*CSize textSize = pDC->GetTextExtent(str);
//						int deviationX = 0;
//						int deviationY = 7;
//						int textX = cx + p->X * sfX2 + (p->W * sfX2 - textSize.cx) / 2 - deviationX;
//						int textY = cy - (p->Y + p->H) * sfY2 + (p->H * sfY2 - textSize.cy) / 2 - deviationY;*/
//
//						// update: move data showing to top left of the dice
//						CSize textSize = pDC->GetTextExtent(str);
//						int margin = 3;
//						int textX = cx + static_cast<int>(p->X * sfX2) + margin;
//						int textY = cy - static_cast<int>((p->Y + p->H) * sfY2) + margin;
//						int textY2 = textY + textSize.cy + 2;
//
//						// Set font size
//						CFont font;
//						int sz = 15;
//						if (p->Y < 20 && p->H < 20) {
//							sz = 12;
//						}
//						
//						font.CreateFont(sz, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial"));
//						CFont* oldFont = pDC->SelectObject(&font);
//
//						// Do not show data of LTV or STIR if it is -99999.00, you can change it in ViewContentDlg413
//						if (fData1 != -99999.00) {
//							pDC->TextOut(textX, textY, str);
//						}
//
//						else if (fData2 == -99999.00) {
//							if (isDualMeasurement == true) {
//								str = "N/A";
//								pDC->TextOut(textX, textY, str);
//							}
//							
//						}
//						
//						int deviationY2 = 7;
//						//int textY2 = cy - (p->Y + p->H) * sfY2 + (p->H * sfY2 - textSize.cy) / 2 + deviationY2;
//						//str.Format("%.4f", fData2);// set precision 4 after . 
//
//						CString pre2;
//						pre2.Format("%%.%df", precision);
//						str.Format(pre2, fData2);
//
//						// Do not show data of LTV or STIR if it is -99999.00, you can change it in ViewContentDlg413
//						if (fData2 != -99999.00) {
//							pDC->TextOut(textX, textY2, str);
//						}
//						else if (fData2 == -99999.00) {
//							if (isDualMeasurement == true) {
//								str = "N/A";
//								pDC->TextOut(textX, textY2, str);
//							}
//							
//						}
//						
//					}
//					/*else {
//						pDC->SelectObject(pB3);
//						pDC->Rectangle(cx + p->X * sfX2, cy - p->Y * sfY2, cx + (p->X + p->W) * sfX2, cy - (p->Y + p->H) * sfY2);
//						str.Format("---");
//					}*/
//				}
//				
//			}
//			
//		}
//	}
//
//	pDC->SelectStockObject(NULL_BRUSH);
//	pDC->Ellipse(cx - rad * sfX, cy - rad * sfY, cx + rad * sfX, cy + rad * sfY);
//	pDC->SelectObject(pOldBrush);
//	delete pB1;
//	delete pB2;
//	delete pB3;
//	delete pB4;
//	delete pBGreen;
//}


// draw with 5 data 
//void CDiceView::DrawWithData(CDC* pDC) { // new function to support LTV min, max, avg
//	float dimson;
//	CString strOverCircle;
//	if (rad == 150.00) {
//		dimson = 350.0f;
//	}
//	else if (rad == 100.00) {
//		dimson = 235.0f;
//	}
//	else if (rad == 75.00) {
//		dimson = 175.0f;
//	}
//
//	CRect rc;
//	GetClientRect(&rc);
//	CRgn Rg;
//	Rg.CreateRectRgn(rc.left, rc.top, rc.right, rc.bottom);
//	pDC->SelectClipRgn(&Rg);
//
//	pDC->FillSolidRect(rc.left, rc.top, rc.Width(), rc.Height(), m_bgColor);
//
//	float wd = dimson;
//	float ht = dimson;
//	float sfX = rc.Width() / wd;
//	float sfY = rc.Height() / ht;
//	float minT = min(sfX, sfY);
//	sfX = minT;
//	sfY = minT;
//	int cx = rc.Width() / 2;
//	int cy = rc.Height() / 2;
//
//	float sfX2 = sfX;
//	float sfY2 = sfY;
//
//	// Draw main circle
//	pDC->Ellipse(cx - rad * sfX, cy - rad * sfY, cx + rad * sfX, cy + rad * sfY);
//
//	// Create brushes
//	CBrush* pB1 = new CBrush(RGB(0xC0, 0xC0, 0xC0)); // Gray
//	CBrush* pB2 = new CBrush(RGB(0xFF, 0xD0, 0xD0)); // Light red
//	CBrush* pB3 = new CBrush(RGB(255, 128, 128));    // Red
//	CBrush* pB4 = new CBrush(RGB(0xFF, 0xFF, 0x00)); // Yellow
//	CBrush* pBGreen = new CBrush(RGB(0x9b, 0xff, 0xcd)); // Green
//
//	// Determine measurement mode
//	bool hasLtv = (ltvIdx != -1);
//	bool hasStir = (stirIdx != -1);
//	bool isDualMeasurement = (hasLtv && hasStir);
//
//	// Set over-circle title
//	if (isDualMeasurement) {
//		strOverCircle = _T("LTV and STIR Measurement");
//	}
//	else if (hasLtv && !hasStir) {
//		strOverCircle = _T("LTV Measurement");
//	}
//	else if (!hasLtv && hasStir) {
//		strOverCircle = _T("STIR Measurement");
//	}
//	else {
//		strOverCircle = _T("");
//	}
//
//	// Draw title above circle
//	if (!strOverCircle.IsEmpty()) {
//		CSize circleTextSize = pDC->GetTextExtent(strOverCircle);
//		int circleTextX = cx - circleTextSize.cx / 2;
//		int circleTextY = cy - static_cast<int>(rad * sfY) - circleTextSize.cy - 10;
//		pDC->TextOut(circleTextX, circleTextY, strOverCircle);
//	}
//
//	// >>> UPDATED LEGEND <<<
//	int legendX = cx + static_cast<int>(rad * sfX) + 10;
//	int lineHeight = 16;
//	int currentY = cy - static_cast<int>(rad * sfY) + 50;
//
//	if (isDualMeasurement) {
//		pDC->TextOut(legendX, currentY, _T("LTV Min")); currentY += lineHeight;
//		pDC->TextOut(legendX, currentY, _T("LTV Max")); currentY += lineHeight;
//		pDC->TextOut(legendX, currentY, _T("LTV Avg")); currentY += lineHeight;
//		pDC->TextOut(legendX, currentY, _T("LTV")); currentY += lineHeight;
//		pDC->TextOut(legendX, currentY, _T("STIR"));
//	}
//	else if (hasLtv) {
//		pDC->TextOut(legendX, currentY, _T("LTV Min")); currentY += lineHeight;
//		pDC->TextOut(legendX, currentY, _T("LTV Max")); currentY += lineHeight;
//		pDC->TextOut(legendX, currentY, _T("LTV Avg")); currentY += lineHeight;
//		pDC->TextOut(legendX, currentY, _T("LTV"));
//	}
//	else if (hasStir) {
//		pDC->TextOut(legendX, currentY, _T("STIR"));
//	}
//	// 
//
//	// Draw sites
//	pDC->SetBkMode(TRANSPARENT);
//	POSITION pos = Sites.GetHeadPosition();
//	int diceID = 1;
//	while (pos) {
//		CSiteItem* p = (CSiteItem*)Sites.GetNext(pos);
//		if (p) {
//			if (p->n == -999) {
//				pDC->SelectObject(pB2);
//				pDC->Rectangle(cx + p->X * sfX2, cy - p->Y * sfY2,
//					cx + (p->X + p->W) * sfX2, cy - (p->Y + p->H) * sfY2);
//			}
//			else {
//				bool bHasData = false;
//				float xCoords = 0.0f, yCoords = 0.0f;
//				// Initialize all 5 values as missing
//				float values[5] = { -99999.0f, -99999.0f, -99999.0f, -99999.0f, -99999.0f };
//
//				// Search for matching measurement point
//				for (const auto& innerVec : p413App->Global.stt) {
//					if (innerVec.empty()) continue;
//
//					try {
//						xCoords = std::stof(innerVec[0]);
//						yCoords = std::stof(innerVec[1]);
//					}
//					catch (...) { continue; }
//
//					float topLeftX = p->X;
//					float topLeftY = p->Y + p->H;
//					float topRightX = p->X + p->W;
//					float topRightY = p->Y + p->H;
//					float bottomLeftX = p->X;
//					float bottomLeftY = p->Y;
//					float bottomRightX = p->X + p->W;
//					float bottomRightY = p->Y;
//
//					if (IsPointInRectangle(xCoords, yCoords,
//						topLeftX, topLeftY, topRightX, topRightY,
//						bottomRightX, bottomRightY, bottomLeftX, bottomLeftY)) {
//
//						bHasData = true;
//
//						// Parse based on vector size
//						size_t n = innerVec.size();
//						if (n >= 7 && isDualMeasurement) {
//							// [x, y, min, max, avg, ltv, stir]
//							try {
//								values[0] = std::stof(innerVec[2]); // min
//								values[1] = std::stof(innerVec[3]); // max
//								values[2] = std::stof(innerVec[4]); // avg
//								values[3] = std::stof(innerVec[5]); // ltv
//								values[4] = std::stof(innerVec[6]); // stir
//							}
//							catch (...) {}
//						}
//						else if (n >= 5 && hasLtv && !hasStir) {
//							// [x, y, min, max, avg]
//							try {
//								values[0] = std::stof(innerVec[2]); // min
//								values[1] = std::stof(innerVec[3]); // max
//								values[2] = std::stof(innerVec[4]); // avg
//								
//
//								// For now, leave values[3] as -99999 (N/A)
//							}
//							catch (...) {}
//						}
//						else if (n >= 3 && hasStir && !hasLtv) {
//							// [x, y, stir]
//							try {
//								values[4] = std::stof(innerVec[2]); // stir
//							}
//							catch (...) {}
//						}
//
//						// Store rectangle for interaction
//						vector<float> temp;
//						temp.emplace_back(p->X);
//						temp.emplace_back(p->Y);
//						temp.emplace_back(p->X + p->W);
//						temp.emplace_back(p->Y + p->H);
//						p413App->Global.rectangleMap[diceID++] = temp;
//
//						// Determine color: use LTV if present, else STIR
//						bool bOutOfSpec = false;
//						float ltvHigh = p413App->Global.ltvHighLimit;
//						float ltvLow = p413App->Global.ltvLowLimit;
//						float stirHigh = p413App->Global.stirHighLimit;
//						float stirLow = p413App->Global.stirLowLimit;
//
//						if (hasLtv) {
//							if (ltvHigh != 0.0f || ltvLow != 0.0f) {
//								if (values[3] > ltvHigh || values[3] < ltvLow) {
//									bOutOfSpec = true;
//								}
//							}
//						}
//						else if (hasStir) {
//							if (stirHigh != 0.0f || stirLow != 0.0f) {
//								if (values[4] > stirHigh || values[4] < stirLow) {
//									bOutOfSpec = true;
//								}
//							}
//						}
//
//						if (bOutOfSpec) {
//							pDC->SelectObject(pB3); // Red
//						}
//						else {
//							pDC->SelectObject(pBGreen); // Green
//						}
//
//						// Draw rectangle
//						pDC->Rectangle(cx + p->X * sfX2, cy - p->Y * sfY2,
//							cx + (p->X + p->W) * sfX2, cy - (p->Y + p->H) * sfY2);
//
//						// Draw 5-value display
//						int margin = 3;
//						int textX = cx + static_cast<int>(p->X * sfX2) + margin;
//						int baseY = cy - static_cast<int>((p->Y + p->H) * sfY2) + margin;
//
//						CFont font;
//						int sz = (p->Y < 20 && p->H < 20) ? 15 : 8;
//						font.CreateFont(sz, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
//							DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
//							CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
//							DEFAULT_PITCH | FF_SWISS, _T("Arial"));
//						CFont* oldFont = pDC->SelectObject(&font);
//
//						for (int i = 0; i < 5; ++i) {
//							CString strVal;
//							if (values[i] == -99999.0f) {
//								strVal = _T("N/A");
//							}
//							else {
//								CString fmt;
//								fmt.Format(_T("%%.%df"), precision);
//								strVal.Format(fmt, values[i]);
//							}
//							pDC->TextOut(textX, baseY + i * (sz + 1), strVal);
//						}
//
//						pDC->SelectObject(oldFont);
//						break; // one point per site
//					}
//				}
//
//				if (!bHasData) {
//					pDC->SelectObject(pB1); // Gray
//					pDC->Rectangle(cx + p->X * sfX2, cy - p->Y * sfY2,
//						cx + (p->X + p->W) * sfX2, cy - (p->Y + p->H) * sfY2);
//				}
//			}
//		}
//	}
//
//	// Redraw circle outline
//	pDC->SelectStockObject(NULL_BRUSH);
//	pDC->Ellipse(cx - rad * sfX, cy - rad * sfY, cx + rad * sfX, cy + rad * sfY);
//
//	// Cleanup
//	delete pB1;
//	delete pB2;
//	delete pB3;
//	delete pB4;
//	delete pBGreen;
//}

// Older version
//void CDiceView::DrawWithData(CDC* pDC) {
//	float dimson;
//	CString strOverCircle;
//	if (rad == 150.00) {
//		dimson = 350.0f;
//	}
//	else if (rad == 100.00) {
//		dimson = 235.0f;
//	}
//	else if (rad == 75.00) {
//		dimson = 175.0f;
//	}
//	CString str;
//	CRect rc;
//	GetClientRect(&rc);
//	CRgn Rg;
//	Rg.CreateRectRgn(rc.left, rc.top, rc.right, rc.bottom);
//	pDC->SelectClipRgn(&Rg);
//
//	pDC->FillSolidRect(rc.left, rc.top, rc.Width(), rc.Height(), m_bgColor);
//
//	float wd = dimson;
//	float ht = dimson;
//	float sfX = rc.Width() / wd;
//	float sfY = rc.Height() / ht;
//	float minT = min(sfX, sfY);
//	sfX = minT;
//	sfY = minT;
//	//float sfY = sfX;
//	int cx = rc.Width() / 2.0f;
//	int cy = rc.Height() / 2.0f;
//
//	float sfX2 = sfX;
//	float sfY2 = sfY;
//	/*float sfX2 = rc.Width() / dimson;
//	float sfY2 = rc.Height() / dimson;*/
//
//	pDC->Ellipse(cx - rad * sfX, cy - rad * sfY, cx + rad * sfX, cy + rad * sfY);
//	CBrush* pB1 = new CBrush(RGB(0xC0, 0xC0, 0xC0));
//	CBrush* pB2 = new CBrush(RGB(0xFF, 0xD0, 0xD0));
//	CBrush* pOldBrush = pDC->SelectObject(pB1);
//
//	CBrush* pB3 = new CBrush(RGB(255, 128, 128)); // Red
//	CBrush* pB4 = new CBrush(RGB(0xFF, 0xFF, 0x00)); // Yellow
//	CBrush* pBGreen = new CBrush(RGB(0x9b, 0xff, 0xcd));//green
//
//	// New code to draw two additional strings on the top right side of the circle
//	CString firstString = _T("");
//	CString secondString = _T("");
//	bool isLtvFirst = false;
//	bool isDualMeasurement = false;
//
//	if (ltvIdx > stirIdx) {
//		firstString = _T("STIR");
//		secondString = _T("LTV");
//		isDualMeasurement = true;
//		isLtvFirst = false;
//	}
//	if (ltvIdx < stirIdx) {
//		firstString = _T("LTV");
//		secondString = _T("STIR");
//		isDualMeasurement = true;
//		isLtvFirst = true;
//	}
//	if (ltvIdx == -1 && stirIdx == -1) {
//		firstString = _T("");
//		secondString = _T("");
//		isDualMeasurement = false;
//	}
//
//	if (ltvIdx != -1 && stirIdx == -1) {
//		firstString = _T("");
//		secondString = _T("");
//		isDualMeasurement = false;
//		isLtvFirst = true;
//	}
//
//	if (ltvIdx == -1 && stirIdx != -1) {
//		firstString = _T("");
//		secondString = _T("");
//		isDualMeasurement = false;
//	}
//
//	// over circle title 
//
//	if (ltvIdx != -1 && stirIdx != -1) {
//		strOverCircle = _T("LTV and STIR Measurement");
//
//	}
//	if (ltvIdx != -1 && stirIdx == -1) {
//		strOverCircle = _T("LTV Measurement");
//		isLtvFirst = true;
//	}
//	if (ltvIdx == -1 && stirIdx != -1) {
//		strOverCircle = _T("STIR Measurement");
//	}
//	if (ltvIdx == -1 && stirIdx == -1) {
//		strOverCircle = _T("");
//	}
//
//	// Calculate text position over the circle
//	CSize circleTextSize = pDC->GetTextExtent(strOverCircle);
//	int circleTextX = cx - circleTextSize.cx / 2;
//	int circleTextY = cy - rad * sfY - circleTextSize.cy - 10;
//
//	// Draw string over the circle
//	pDC->TextOut(circleTextX, circleTextY, strOverCircle);
//
//
//
//
//
//	// Calculate text sizes
//	//Format("%%.%df", Me->MPa[j].D)
//	CSize firstStringSize = pDC->GetTextExtent(firstString);
//	CSize secondStringSize = pDC->GetTextExtent(secondString);
//
//	// Position the first string slightly to the right of the circle
//	int firstStringX = cx + rad * sfX + 5; // 5 pixels offset from the circle
//	int firstStringY = cy - rad * sfY - firstStringSize.cy + 150;
//
//	// Position the second string below the first string
//	int secondStringX = firstStringX;
//	int secondStringY = firstStringY + firstStringSize.cy + 5; // 5 pixels below the first string
//
//	// Draw the additional strings
//	pDC->TextOut(firstStringX, firstStringY, firstString);
//	pDC->TextOut(secondStringX, secondStringY, secondString);
//
//	int diceID = 1;
//	pDC->SetBkMode(TRANSPARENT);
//
//	POSITION pos = Sites.GetHeadPosition();
//	while (pos) {
//		CSiteItem* p = (CSiteItem*)Sites.GetNext(pos);
//		if (p) {
//			if (p->n == -999) {
//
//				pDC->SelectObject(pB2);
//				//pDC->Rectangle(cx + p->X * sfX, cy - p->Y * sfY, cx + (p->X + p->W) * sfX, cy - (p->Y + p->H) * sfY);
//				pDC->Rectangle(cx + p->X * sfX2, cy - p->Y * sfY2, cx + (p->X + p->W) * sfX2, cy - (p->Y + p->H) * sfY2);
//			}
//			else {
//
//
//
//				p413App->Global.stt;
//				float xCoords = 0.00, yCoords = 0.00, fData1 = -9999.000, fData2 = -9999.000;
//				string firstData, secondData;
//				for (const auto& innerVec : p413App->Global.stt)
//				{
//					// Ensure the inner vector has 4 elements , both LTV and STIR
//					if (innerVec.size() == 4)
//					{
//						//xCoords = std::stof(innerVec[0]); // Extract the x-coordinate
//						//yCoords = std::stof(innerVec[1]); // Extract the y-coordinate
//						//data1 = std::stof(innerVec[2]);
//						//data2 = innerVec[3];
//
//						string s1, s2, s3, s4;
//						s1 = innerVec[0];
//						s2 = innerVec[1];
//						firstData = innerVec[2]; // LTV data string with header
//						secondData = innerVec[3]; // STIR data string with header
//
//						xCoords = std::stof(s1);// Extract the x-coordinate
//						yCoords = std::stof(s2);// Extract the y-coordinate
//						fData1 = std::stof(firstData);
//						fData2 = std::stof(secondData);
//						//string sub_str3 = "LTV";
//						//string sub_str4 = "STIR";
//
//						//size_t pos3 = s3.find(sub_str3);
//						//size_t pos4 = s4.find(sub_str4);
//						//if (pos3 != std::string::npos) {
//						//	//LTV
//						//	while (pos3 != string::npos) {
//						//		s3.erase(pos3, sub_str3.length());
//						//		pos3 = s3.find(sub_str3);
//						//	}
//						//	ltvData = std::stof(s3);
//						//}
//						//if (pos4 != std::string::npos) {
//						//	//STIR
//
//						//	while (pos4 != string::npos) {
//						//		s4.erase(pos4, sub_str4.length());
//						//		pos4 = s4.find(sub_str4);
//						//	}
//						//	stirData = std::stof(s4);
//						//}
//					}
//
//					float topLeftX = 0.00, topLeftY = 0.00, topRightX = 0.00, topRightY = 0.00;
//					float bottomLeftX = 0.00, bottomLeftY = 0.00, bottomRightX = 0.000, bottomRightY = 0.00;
//
//
//					topLeftX = p->X; topLeftY = p->Y + p->H;
//					topRightX = p->X + p->W; topRightY = p->Y + p->H;
//					bottomLeftX = p->X; bottomLeftY = p->Y;
//					bottomRightX = p->X + p->W; bottomRightY = p->Y;
//
//					if (IsPointInRectangle(xCoords, yCoords, topLeftX, topLeftY, topRightX, topRightY, bottomRightX, bottomRightY, bottomLeftX, bottomLeftY))
//					{
//						float ltvHigh = p413App->Global.ltvHighLimit;
//						float ltvlow = p413App->Global.ltvLowLimit;
//						float stirHigh = p413App->Global.stirHighLimit;
//						float stirLow = p413App->Global.stirLowLimit;
//
//						pDC->SelectObject(pBGreen); //Green pen
//
//						if (isDualMeasurement == true) {
//							if (ltvHigh == 0.00 && ltvlow == 0.00 && stirHigh == 0.00 && stirLow == 0.00) {
//								pDC->SelectObject(pBGreen); // choose green color if there is no high low value set from recipe
//							}
//							else {
//								// with custom limits
//								if (isLtvFirst == true) {
//									if (ltvHigh < fData1 && ltvlow > fData1) {
//										pDC->SelectObject(pB3); //Choose red color
//									}
//								}
//								if (isLtvFirst != true) {
//									if (stirHigh < fData2 && stirLow > fData2) {
//										pDC->SelectObject(pB3); //Choose red color
//									}
//								}
//							}
//						}
//						else {
//
//							if (ltvHigh == 0.00 && ltvlow == 0.00 && stirHigh == 0.00 && stirLow == 0.00) {
//								pDC->SelectObject(pBGreen); // choose green color if there is no high low value set from recipe
//							}
//							else {
//								// with custom limits
//								if (isLtvFirst == true) {
//									if (ltvHigh < fData1 && ltvlow > fData1) {
//										pDC->SelectObject(pB3); //Choose red color
//									}
//								}
//								if (isLtvFirst != true) {
//									if (stirHigh < fData1 && stirLow > fData1) {
//										pDC->SelectObject(pB3); //Choose red color
//									}
//								}
//							}
//
//						}
//
//						////13012025
//						// Calculate screen coordinates exactly 
//						vector<float> temp;
//						temp.emplace_back(p->X);              // Store original X
//						temp.emplace_back(p->Y);              // Store original Y
//						temp.emplace_back(p->X + p->W);       // Store right X
//						temp.emplace_back(p->Y + p->H);       // Store bottom Y
//						p413App->Global.rectangleMap[diceID++] = temp;
//
//
//						////
//
//						pDC->Rectangle(cx + p->X * sfX2, cy - p->Y * sfY2, cx + (p->X + p->W) * sfX2, cy - (p->Y + p->H) * sfY2);
//						//str.Format("%.4f", fData1); // set precision 4 after . 
//						CString pre;
//						pre.Format("%%.%df", precision);
//						str.Format(pre, fData1);
//
//						// Calculate text size and position for centering
//						CSize textSize = pDC->GetTextExtent(str);
//						int deviationX = 0;
//						int deviationY = 7;
//						int textX = cx + p->X * sfX2 + (p->W * sfX2 - textSize.cx) / 2 - deviationX;
//						int textY = cy - (p->Y + p->H) * sfY2 + (p->H * sfY2 - textSize.cy) / 2 - deviationY;
//
//						// Set font size
//						CFont font;
//						int sz = 15;
//						if (p->Y < 20 && p->H < 20) {
//							sz = 12;
//						}
//						if (p->Y < 20 && p->H < 10) {
//							sz = 8;
//						}
//						if (p->Y < 20 && p->H < 5) {
//							sz = 3;
//						}
//
//						font.CreateFont(sz, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial"));
//						CFont* oldFont = pDC->SelectObject(&font);
//
//						// Do not show data of LTV or STIR if it is -99999.00, you can change it in ViewContentDlg413
//						if (fData1 != -99999.00) {
//							pDC->TextOut(textX, textY, str);
//						}
//
//						else if (fData2 == -99999.00) {
//							if (isDualMeasurement == true) {
//								str = "N/A";
//								pDC->TextOut(textX, textY, str);
//							}
//
//						}
//
//						int deviationY2 = 7;
//						int textY2 = cy - (p->Y + p->H) * sfY2 + (p->H * sfY2 - textSize.cy) / 2 + deviationY2;
//						//str.Format("%.4f", fData2);// set precision 4 after . 
//
//						CString pre2;
//						pre2.Format("%%.%df", precision);
//						str.Format(pre2, fData2);
//
//						// Do not show data of LTV or STIR if it is -99999.00, you can change it in ViewContentDlg413
//						if (fData2 != -99999.00) {
//							pDC->TextOut(textX, textY2, str);
//						}
//						else if (fData2 == -99999.00) {
//							if (isDualMeasurement == true) {
//								str = "N/A";
//								pDC->TextOut(textX, textY2, str);
//							}
//
//						}
//
//					}
//					/*else {
//						pDC->SelectObject(pB3);
//						pDC->Rectangle(cx + p->X * sfX2, cy - p->Y * sfY2, cx + (p->X + p->W) * sfX2, cy - (p->Y + p->H) * sfY2);
//						str.Format("---");
//					}*/
//				}
//
//			}
//
//		}
//	}
//
//	pDC->SelectStockObject(NULL_BRUSH);
//	pDC->Ellipse(cx - rad * sfX, cy - rad * sfY, cx + rad * sfX, cy + rad * sfY);
//	pDC->SelectObject(pOldBrush);
//	delete pB1;
//	delete pB2;
//	delete pB3;
//	delete pB4;
//	delete pBGreen;
//}

void CDiceView::DrawWithData(CDC* pDC) {
    float dimson;
    CString strOverCircle;
    if (rad == 150.00) {
        dimson = 350.0f;
    }
    else if (rad == 100.00) {
        dimson = 235.0f;
    }
    else if (rad == 75.00) {
        dimson = 175.0f;
    }
    CString str;
    CRect rc;
    GetClientRect(&rc);
    CRgn Rg;
    Rg.CreateRectRgn(rc.left, rc.top, rc.right, rc.bottom);
    pDC->SelectClipRgn(&Rg);

    pDC->FillSolidRect(rc.left, rc.top, rc.Width(), rc.Height(), m_bgColor);

    float wd = dimson;
    float ht = dimson;
    float sfX = rc.Width() / wd;
    float sfY = rc.Height() / ht;
    float minT = min(sfX, sfY);
    sfX = minT;
    sfY = minT;
    int cx = rc.Width() / 2.0f;
    int cy = rc.Height() / 2.0f;

    float sfX2 = sfX;
    float sfY2 = sfY;

    pDC->Ellipse(cx - rad * sfX, cy - rad * sfY, cx + rad * sfX, cy + rad * sfY);
    CBrush* pB1 = new CBrush(RGB(0xC0, 0xC0, 0xC0));
    CBrush* pB2 = new CBrush(RGB(0xFF, 0xD0, 0xD0));
    CBrush* pOldBrush = pDC->SelectObject(pB1);

    CBrush* pB3 = new CBrush(RGB(255, 128, 128)); // Red
    CBrush* pB4 = new CBrush(RGB(0xFF, 0xFF, 0x00)); // Yellow
    CBrush* pBGreen = new CBrush(RGB(0x9b, 0xff, 0xcd));//green

    // String determination logic
    CString firstString = _T("");
    CString secondString = _T("");
    bool isLtvFirst = false;
    bool isDualMeasurement = false;

    if (ltvIdx > stirIdx) {
        firstString = _T("STIR");
        secondString = _T("LTV");
        isDualMeasurement = true;
        isLtvFirst = false;
    }
    if (ltvIdx < stirIdx) {
        firstString = _T("LTV");
        secondString = _T("STIR");
        isDualMeasurement = true;
        isLtvFirst = true;
    }
    if (ltvIdx == -1 && stirIdx == -1) {
        firstString = _T("");
        secondString = _T("");
        isDualMeasurement = false;
    }

    if (ltvIdx != -1 && stirIdx == -1) {
        firstString = _T("");
        secondString = _T("");
        isDualMeasurement = false;
        isLtvFirst = true;
    }

    if (ltvIdx == -1 && stirIdx != -1) {
        firstString = _T("");
        secondString = _T("");
        isDualMeasurement = false;
    }

    // Over circle title 
    if (ltvIdx != -1 && stirIdx != -1) {
        strOverCircle = _T("LTV and STIR Measurement");
    }
    if (ltvIdx != -1 && stirIdx == -1) {
        strOverCircle = _T("LTV Measurement");
        isLtvFirst = true;
    }
    if (ltvIdx == -1 && stirIdx != -1) {
        strOverCircle = _T("STIR Measurement");
    }
    if (ltvIdx == -1 && stirIdx == -1) {
        strOverCircle = _T("");
    }

    // Draw string over the circle
    CSize circleTextSize = pDC->GetTextExtent(strOverCircle);
    int circleTextX = cx - circleTextSize.cx / 2;
    int circleTextY = cy - rad * sfY - circleTextSize.cy - 10;
    pDC->TextOut(circleTextX, circleTextY, strOverCircle);

    // Draw side strings
    CSize firstStringSize = pDC->GetTextExtent(firstString);
    CSize secondStringSize = pDC->GetTextExtent(secondString);
    int firstStringX = cx + rad * sfX + 5;
    int firstStringY = cy - rad * sfY - firstStringSize.cy + 150;
    int secondStringX = firstStringX;
    int secondStringY = firstStringY + firstStringSize.cy + 5;
    pDC->TextOut(firstStringX, firstStringY, firstString);
    pDC->TextOut(secondStringX, secondStringY, secondString);

    int diceID = 1;
    pDC->SetBkMode(TRANSPARENT);

    int rectPos = 0; //12012025/HAQUE/SOLVED DIE DRAWING ISSUE
    vector<string>innerVec; //12012025/HAQUE/RECTANGLE CHECK ISSUE 
    POSITION pos = Sites.GetHeadPosition();
    while (pos) {
        CSiteItem* p = (CSiteItem*)Sites.GetNext(pos);
        if (p) {
            if (p->n == -999) {
                pDC->SelectObject(pB2);
                pDC->Rectangle(cx + p->X * sfX2, cy - p->Y * sfY2, cx + (p->X + p->W) * sfX2, cy - (p->Y + p->H) * sfY2);
            }
            else {
                float xCoords = 0.00, yCoords = 0.00, fData1 = -9999.000, fData2 = -9999.000;
                string firstData, secondData;
                //for (const auto& innerVec : p413App->Global.stt) {
                for( ; rectPos < p413App->Global.stt.size(); rectPos++) {   //12012025/HAQUE/SOLVED DIE LOADING ISSUE
                    innerVec = p413App->Global.stt[rectPos];
                    if (innerVec.size() == 4) {
                        string s1, s2, s3, s4;
                        s1 = innerVec[0];
                        s2 = innerVec[1];
                        firstData = innerVec[2];
                        secondData = innerVec[3];

                        xCoords = std::stof(s1);
                        yCoords = std::stof(s2);
                        fData1 = std::stof(firstData);
                        fData2 = std::stof(secondData);
                    }

                    float topLeftX = p->X, topLeftY = p->Y + p->H;
                    float topRightX = p->X + p->W, topRightY = p->Y + p->H;
                    float bottomLeftX = p->X, bottomLeftY = p->Y;
                    float bottomRightX = p->X + p->W, bottomRightY = p->Y;

                    if (IsPointInRectangle(xCoords, yCoords, topLeftX, topLeftY, topRightX, topRightY, bottomRightX, bottomRightY, bottomLeftX, bottomLeftY)) {
                        float ltvHigh = p413App->Global.ltvHighLimit;
                        float ltvlow = p413App->Global.ltvLowLimit;
                        float stirHigh = p413App->Global.stirHighLimit;
                        float stirLow = p413App->Global.stirLowLimit;

                        pDC->SelectObject(pBGreen);

                        if (isDualMeasurement == true) {
                            if (ltvHigh == 0.00 && ltvlow == 0.00 && stirHigh == 0.00 && stirLow == 0.00) {
                                pDC->SelectObject(pBGreen);
                            }
                            else {
                                if (isLtvFirst == true) {
                                    if (ltvHigh < fData1 && ltvlow > fData1) {
                                        pDC->SelectObject(pB3);
                                    }
                                }
                                if (isLtvFirst != true) {
                                    if (stirHigh < fData2 && stirLow > fData2) {
                                        pDC->SelectObject(pB3);
                                    }
                                }
                            }
                        }
                        else {
                            if (ltvHigh == 0.00 && ltvlow == 0.00 && stirHigh == 0.00 && stirLow == 0.00) {
                                pDC->SelectObject(pBGreen);
                            }
                            else {
                                if (isLtvFirst == true) {
                                    if (ltvHigh < fData1 && ltvlow > fData1) {
                                        pDC->SelectObject(pB3);
                                    }
                                }
                                if (isLtvFirst != true) {
                                    if (stirHigh < fData1 && stirLow > fData1) {
                                        pDC->SelectObject(pB3);
                                    }
                                }
                            }
                        }

                        // Store rectangle coordinates
                        vector<float> temp;
                        temp.emplace_back(p->X);
                        temp.emplace_back(p->Y);
                        temp.emplace_back(p->X + p->W);
                        temp.emplace_back(p->Y + p->H);
                        p413App->Global.rectangleMap[diceID++] = temp;

                        pDC->Rectangle(cx + p->X * sfX2, cy - p->Y * sfY2, cx + (p->X + p->W) * sfX2, cy - (p->Y + p->H) * sfY2);

                        // Calculate dynamic font size
                        float rectHeight = p->H * sfY2;
                        float rectWidth = p->W * sfX2;
                        int fontHeight = CalculateDynamicFontSize(pDC, rectWidth, rectHeight);

                        CFont font;
                        font.CreateFont(fontHeight, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                            DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial"));
                        CFont* oldFont = pDC->SelectObject(&font);

                        // Draw first data value
                        CString pre;
                        pre.Format("%%.%df", precision);
                        str.Format(pre, fData1);
                        CSize textSize = pDC->GetTextExtent(str);
                        CPoint textPos1 = CalculateTextPosition(p, textSize, sfX2, sfY2, cx, cy, true, rectHeight);

                        if (fData1 != -99999.00) {
                            pDC->TextOut(textPos1.x, textPos1.y, str);
                        }
                        else if (fData2 == -99999.00 && isDualMeasurement) {
                            CString naStr = "N/A";
                            CSize naSize = pDC->GetTextExtent(naStr);
                            CPoint naPos = CalculateTextPosition(p, naSize, sfX2, sfY2, cx, cy, true, rectHeight);
                            pDC->TextOut(naPos.x, naPos.y, naStr);
                        }

                        // Draw second data value
                        str.Format(pre, fData2);
                        textSize = pDC->GetTextExtent(str);
                        CPoint textPos2 = CalculateTextPosition(p, textSize, sfX2, sfY2, cx, cy, false, rectHeight);

                        if (fData2 != -99999.00) {
                            pDC->TextOut(textPos2.x, textPos2.y, str);
                        }
                        else if (fData2 == -99999.00 && isDualMeasurement) {
                            CString naStr = "N/A";
                            CSize naSize = pDC->GetTextExtent(naStr);
                            CPoint naPos = CalculateTextPosition(p, naSize, sfX2, sfY2, cx, cy, false, rectHeight);
                            pDC->TextOut(naPos.x, naPos.y, naStr);
                        }

                        pDC->SelectObject(oldFont);
                        font.DeleteObject();
                        break;
                    }
                }
            }
        }
    }

    pDC->SelectStockObject(NULL_BRUSH);
    pDC->Ellipse(cx - rad * sfX, cy - rad * sfY, cx + rad * sfX, cy + rad * sfY);
    pDC->SelectObject(pOldBrush);
    delete pB1;
    delete pB2;
    delete pB3;
    delete pB4;
    delete pBGreen;
}

int CDiceView::CalculateDynamicFontSize(CDC* pDC, float rectWidth, float rectHeight) {
    int fontHeight;

    if (rectHeight < 8) {
        fontHeight = max(4, (int)(rectHeight * 0.6));
    }
    else if (rectHeight < 15) {
        fontHeight = max(6, (int)(rectHeight * 0.5));
    }
    else if (rectHeight < 25) {
        fontHeight = max(8, (int)(rectHeight * 0.4));
    }
    else {
        fontHeight = 15;
    }

    // Ensure font fits width
    CString testStr = "000.00";
    int testFontHeight = fontHeight;

    while (testFontHeight > 4) {
        CFont testFont;
        testFont.CreateFont(testFontHeight, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial"));
        CFont* oldFont = pDC->SelectObject(&testFont);

        CSize testSize = pDC->GetTextExtent(testStr);
        pDC->SelectObject(oldFont);
        testFont.DeleteObject();

        if (testSize.cx <= rectWidth * 0.85) {
            fontHeight = testFontHeight;
            break;
        }
        testFontHeight--;
    }

    return fontHeight;
}

CPoint CDiceView::CalculateTextPosition(CSiteItem* p, CSize textSize, float sfX2, float sfY2, int cx, int cy, bool isFirstLine, float rectHeight) {
    CPoint pos;

    // Calculate horizontal center
    pos.x = cx + p->X * sfX2 + (p->W * sfX2 - textSize.cx) / 2;

    // Calculate bounds
    int topBound = cy - (p->Y + p->H) * sfY2 + 2;
    int bottomBound = cy - p->Y * sfY2 - textSize.cy - 2;

    // Calculate available space
    int availableSpace = bottomBound - topBound;
    int textHeight = textSize.cy;

    // Determine optimal line spacing
    int lineSpacing;
    if (availableSpace < textHeight * 2) {
        // Very tight space - minimal spacing
        lineSpacing = max(1, (availableSpace - textHeight * 2) / 2);
    }
    else if (availableSpace < textHeight * 3) {
        // Moderate space
        lineSpacing = textHeight / 2;
    }
    else {
        // Plenty of space
        lineSpacing = textHeight;
    }

    // Calculate positions
    int totalHeight = textHeight * 2 + lineSpacing;
    int startY = topBound + (availableSpace - totalHeight) / 2;

    if (isFirstLine) {
        pos.y = startY;
    }
    else {
        pos.y = startY + textHeight + lineSpacing;
    }

    // Ensure within bounds
    pos.y = max(topBound, min(pos.y, bottomBound));

    return pos;
}

void CDiceView::AddCellWithData(float cx, float cy, float wd, float ht, short n, float m_Radius, int ltv, int stir, CString name, int pre) {
	rad = m_Radius;
	ltvIdx = ltv;
	stirIdx = stir;
	rcpName = name;
	precision = pre;
	CSiteItem* p = new CSiteItem;
	if (p) {
		p->Set(n, cx, cy, wd, ht);
		Sites.AddTail(p);
		//RedrawWithData();
	}
}

void CDiceView::LocateAndSelect(float fX, float fY) {
	POSITION pos = Sites.GetHeadPosition();
	while (pos) {
		CSiteItem* p = (CSiteItem*)Sites.GetNext(pos);
		if (p) {
			
			if ((fX >= p->X) && (fX <= (p->X + p->W)) &&
				(fY >= p->Y) && (fY <= (p->Y + p->H))) {
				p->bSel = TRUE;  
				break;
			}
		}
	}
	RedrawWithData(); 
}

void CDiceView::Show3DMapView() {
	/*if (pDlg) { delete pDlg; pDlg = nullptr; }
	pDlg = new CWMap3DDlg;
	pDlg->SetParam(pRcp, DataCol);
	pDlg->Create(CWMap3DDlg::IDD);
	pDlg->ShowWindow(SW_SHOW);*/
	return;
}

BOOL CDiceView::SaveBitmap(CString& path)
{
	
   

	// Get the device context and client rectangle
	CDC* pDC = GetDC();
	CRect rect;
	GetClientRect(&rect);

	// Create a compatible memory DC
	CDC memDC;
	memDC.CreateCompatibleDC(pDC);

	// Create a compatible bitmap
	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());

	// Select the bitmap into the memory DC
	CBitmap* pOldBitmap = memDC.SelectObject(&bitmap);

	// Copy the contents of the original DC to the memory DC
	memDC.BitBlt(0, 0, rect.Width(), rect.Height(), pDC, 0, 0, SRCCOPY);

	// Create a bitmap info structure
	BITMAPINFO bmi;
	ZeroMemory(&bmi, sizeof(BITMAPINFO));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = rect.Width();
	bmi.bmiHeader.biHeight = -rect.Height(); // Top-down DIB
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32; // 32-bit for best compatibility
	bmi.bmiHeader.biCompression = BI_RGB;

	// Allocate memory for bitmap bits
	DWORD dwBitmapSize = rect.Width() * rect.Height() * 4;
	std::vector<BYTE> bitmapBuffer(dwBitmapSize);

	// Get the bitmap bits
	CDC screenDC;
	screenDC.CreateCompatibleDC(NULL);
	if (!GetDIBits(screenDC, (HBITMAP)bitmap.GetSafeHandle(), 0, rect.Height(),
		bitmapBuffer.data(), &bmi, DIB_RGB_COLORS))
	{
		TRACE(_T("GetDIBits failed. Error: %d\n"), GetLastError());
		return FALSE;
	}

	try
	{
		// Open the file for writing
		CFile file(path, CFile::modeCreate | CFile::modeWrite);

		// Write BITMAPFILEHEADER
		BITMAPFILEHEADER bfh;
		bfh.bfType = 0x4D42; // "BM"
		bfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwBitmapSize;
		bfh.bfReserved1 = 0;
		bfh.bfReserved2 = 0;
		bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

		// Write headers and bitmap data
		file.Write(&bfh, sizeof(BITMAPFILEHEADER));
		file.Write(&bmi.bmiHeader, sizeof(BITMAPINFOHEADER));
		file.Write(bitmapBuffer.data(), dwBitmapSize);

		// File is automatically closed when CFile goes out of scope
	}
	catch (CFileException* pEx)
	{
		// Handle file exceptions
		TCHAR szError[1024];
        pEx->GetErrorMessage(szError, 1024);
		/*CString x = (_T("File error: %s\n"), szError);
		AfxMessageBox(x);*/
		pEx->Delete();
		return FALSE;
	}

	return TRUE;
}


//BOOL CDiceView::SaveBitmap(CString path)
//{
//    CString filePath = path;
//    TRACE(_T("Attempting to save to: %s\n"), filePath);
//
//    // Enhanced retry logic for sharing violations
//    int maxRetries = 10;
//    int retryDelay = 100; // Start with 100ms
//
//    for (int retry = 0; retry < maxRetries; retry++) {
//        try
//        {
//            // [YOUR EXISTING BITMAP CREATION CODE HERE]
//            // Get the device context and client rectangle
//            CDC* pDC = GetDC();
//            CRect rect;
//            GetClientRect(&rect);
//
//            // Create a compatible memory DC
//            CDC memDC;
//            memDC.CreateCompatibleDC(pDC);
//
//            // Create a compatible bitmap
//            CBitmap bitmap;
//            bitmap.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
//
//            // Select the bitmap into the memory DC
//            CBitmap* pOldBitmap = memDC.SelectObject(&bitmap);
//
//            // Copy the contents of the original DC to the memory DC
//            memDC.BitBlt(0, 0, rect.Width(), rect.Height(), pDC, 0, 0, SRCCOPY);
//
//            // Create a bitmap info structure
//            BITMAPINFO bmi;
//            ZeroMemory(&bmi, sizeof(BITMAPINFO));
//            bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
//            bmi.bmiHeader.biWidth = rect.Width();
//            bmi.bmiHeader.biHeight = -rect.Height(); // Top-down DIB
//            bmi.bmiHeader.biPlanes = 1;
//            bmi.bmiHeader.biBitCount = 32; // 32-bit for best compatibility
//            bmi.bmiHeader.biCompression = BI_RGB;
//
//            // Allocate memory for bitmap bits
//            DWORD dwBitmapSize = rect.Width() * rect.Height() * 4;
//            std::vector<BYTE> bitmapBuffer(dwBitmapSize);
//
//            // Get the bitmap bits
//            CDC screenDC;
//            screenDC.CreateCompatibleDC(NULL);
//            if (!GetDIBits(screenDC, (HBITMAP)bitmap.GetSafeHandle(), 0, rect.Height(),
//                bitmapBuffer.data(), &bmi, DIB_RGB_COLORS))
//            {
//                TRACE(_T("GetDIBits failed. Error: %d\n"), GetLastError());
//                memDC.SelectObject(pOldBitmap);
//                ReleaseDC(pDC);
//                return FALSE;
//            }
//
//            // Clean up DCs
//            memDC.SelectObject(pOldBitmap);
//            ReleaseDC(pDC);
//
//            // Try to create the file
//            TRACE(_T("Creating file (attempt %d): %s\n"), retry + 1, filePath);
//            CFile file(filePath, CFile::modeCreate | CFile::modeWrite | CFile::shareExclusive);
//
//            // Write BITMAPFILEHEADER
//            BITMAPFILEHEADER bfh;
//            bfh.bfType = 0x4D42; // "BM"
//            bfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwBitmapSize;
//            bfh.bfReserved1 = 0;
//            bfh.bfReserved2 = 0;
//            bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
//
//            // Write headers and bitmap data
//            file.Write(&bfh, sizeof(BITMAPFILEHEADER));
//            file.Write(&bmi.bmiHeader, sizeof(BITMAPINFOHEADER));
//            file.Write(bitmapBuffer.data(), dwBitmapSize);
//
//            TRACE(_T("File saved successfully on attempt %d!\n"), retry + 1);
//            return TRUE;
//        }
//        catch (CFileException* pEx)
//        {
//            TCHAR szError[1024];
//            pEx->GetErrorMessage(szError, 1024);
//
//            if (pEx->m_cause == CFileException::sharingViolation && retry < maxRetries - 1) {
//                TRACE(_T("Sharing violation (attempt %d), retrying in %dms...\n"), retry + 1, retryDelay);
//                pEx->Delete();
//                Sleep(retryDelay);
//                retryDelay *= 2; // Exponential backoff
//                continue;
//            }
//
//            TRACE(_T("CFileException after %d attempts: %s\n"), retry + 1, szError);
//            pEx->Delete();
//            return FALSE;
//        }
//    }
//
//    return FALSE;
//}




//]

#include "128/CAS/CasMgr.h"
CString CDiceView::SetDiceMapFolderPath()
{
	
	p413App->Global.diceRcpName ;
	p413App->Global.diceSlotID ;
	p413App->Global.diceWaferID ;
	p413App->Global.diceLotID  ;
	//CString basePath = DosUtil.DirSys.ExtractFilePath(dummyC2C);
	////CProcJobInfo* pJobInfo = &pUDev->pProcJobInfo[ID];
	////if (!pJobInfo) return _T("");

	//// Use MakeCassetteName to get correct base path structure
	//CCasMgr CasMgr;
	//CasMgr.SetCassette(p413App->Global.pDiceJobInfo->Cas);
	//CString dummyC2C;
	//CString OpName = _T("FSM");
	//if (strlen(pGDev->username) != 0) OpName = pGDev->username;

	// Generate a dummy C2C filename to get the proper directory structure
	//CasMgr.MakeCassetteName(dummyC2C, p413App->Global.diceRcpName, _T("dummy"), p413App->Global.pDiceJobInfo->Cas.JobInfo.CarrierID, FALSE, FALSE, OpName);

	// Extract the directory path from the generated C2C filename
	CString basePath = p413App->Global.diceMapSavePath;
	return basePath;

	// Remove the "C2C" folder from the path and replace with "TXT\2D"
	int pos = basePath.ReverseFind(_T('\\'));
	if (pos != -1) {
		basePath = basePath.Left(pos); // Remove "C2C" folder

		// Create TXT\2D subdirectories
		CString txtPath = basePath + _T("\\TXT\\");
		CString folder2D = txtPath + _T("DiceMap\\");

		// Create directories using GotoDirec (which calls CreateDir)
		DosUtil.DirSys.GotoDirec(folder2D);

		return folder2D;
	}

	// Add fallback logic here if needed
	return _T("");
}

//[13012025
//for Dice with data ACME
void CDiceView::GetCoor2(POINT& coord, float& fX, float& fY) {
	CRect rc;
	GetClientRect(&rc);

	float dimson;
	if (rad == 150.00) {
		dimson = 350.0f;
	}
	else if (rad == 100.00) {
		dimson = 235.0f;
	}
	else if (rad == 75.00) {
		dimson = 175.0f;
	}

	float wd = dimson;
	float ht = dimson;
	float sfX = rc.Width() / wd;
	float sfY = rc.Height() / ht;
	float minT = min(sfX, sfY);
	sfX = minT;
	sfY = minT;

	ScreenToClient(&coord);

	// Convert to wafer coordinates relative to center
	float centerX = rc.Width() / 2.0f;
	float centerY = rc.Height() / 2.0f;

	// Adjust the conversion to match DrawWithData's coordinate system
	fX = (coord.x - centerX);        // Relative to center X
	fY = (centerY - coord.y);        // Relative to center Y, note the flip

	// Scale back to match the original coordinate space
	fX = fX / sfX;
	fY = fY / sfY;
}
//]

 //20251219_Mahmudul
BOOL CDiceView::CaptureToPDF()
{
	// 1. Check if PDF exporter is available
	if (!p413App || &(p413App->PdfExporter) == nullptr)
	{
		return FALSE;
	}
	// 2. Ensure we have a valid window
	if (!::IsWindow(m_hWnd))
		return FALSE;
	// 3. Update the window first to ensure proper rendering
	UpdateWindow();
	// 4. Capture current view to CImage
	CImage waferImage;
	if (!CaptureCurrentView(waferImage))
	{
		return FALSE;
	}
	p413App->PdfExporter.AddMultiPages(1);
	// 6. Get the current page (usually last page)
	HPDF_Page pdfPage = p413App->PdfExporter.GetPage(p413App->PdfExporter.ExtraPage);
	// 7. Calculate position (start near top of page)
	float pageHeight = HPDF_Page_GetHeight(pdfPage);
	float startY = pageHeight - 40.0f; // 100 points from top
	// 8. Insert image into PDF
	p413App->PdfExporter.InsertFullPageImage(
		pdfPage,                    // HPDF_Page
		waferImage                 // CImage
	);
	++p413App->PdfExporter.ExtraPage;
	return TRUE;
}


//20251219_Mahmudul
BOOL CDiceView::CaptureCurrentView(CImage& image)
{
	// Get current window rect
	CRect rect;
	GetClientRect(&rect);

	if (rect.Width() <= 0 || rect.Height() <= 0)
		return FALSE;

	// Get window DC
	CDC* pDC = GetDC();
	if (!pDC) return FALSE;

	// Create memory DC compatible with window
	CDC memDC;
	if (!memDC.CreateCompatibleDC(pDC))
	{
		ReleaseDC(pDC);
		return FALSE;
	}

	// Create CImage with 24-bit color
	if (!image.Create(rect.Width(), rect.Height(), 24))
	{
		ReleaseDC(pDC);
		return FALSE;
	}

	// Select bitmap into memory DC
	HBITMAP hOldBmp = (HBITMAP)memDC.SelectObject(image);

	// Draw wafer map directly to the image
	CRect rcTemp = rect;
	Draw(&memDC);  // Use your existing Draw function

	// Optional: Also do BitBlt to ensure everything is captured
	memDC.BitBlt(0, 0, rect.Width(), rect.Height(), pDC, 0, 0, SRCCOPY);

	// Restore old bitmap
	memDC.SelectObject(hOldBmp);

	// Cleanup
	ReleaseDC(pDC);

	return TRUE;
}