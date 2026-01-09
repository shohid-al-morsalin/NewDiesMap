#include "stdafx.h"
#include "Glut2DMapView.h"

#include "stdafx.h"

#include "Data.h"
//#include "glut.h"
#include "MPoint.h"
#include "Recipe413.h"
#include "SRC/DOSUtil.h"
#include "WafMap/OGL/Grapher.h"
#include "WafMap/OGL/Interpolater.h"
#include "WafMap/OGL/InverseDist.h"
#include "E95/WafMap/WMap.h"
#include "E95/WafMap/WMDefines.h"
#include <algorithm>
#include <string>
#include <sstream>
#include "413App.h"
#include "E95/WafMap/glut.h"

#define PI2 6.283185307179586476925286766559f

#define IDT_POPUPMENU	(0x100)

IMPLEMENT_DYNCREATE(CGlut2DMapView, CGLViewStatic)

BEGIN_MESSAGE_MAP(CGlut2DMapView, CGLViewStatic)
	//ON_WM_RBUTTONUP()
	//ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(/*ID_FILE_PRINT_PREVIEW*/0xE109, CView::OnFilePrintPreview)
	ON_WM_SETFOCUS()
	ON_WM_MOUSEACTIVATE()
END_MESSAGE_MAP()

CGlut2DMapView::CGlut2DMapView() {
	m_nFontBase = 0;
	m_FillMode = GL_LINE;
	//m_GridSize = 61;
	m_GridSize = 101; //12082025// for testing

	//CString inifile(DosUtil.GetLocalCfgFile());
	//nLevelAdjustment = GetPrivateProfileInt("2DMap", "LevelAdjustment", 0, inifile);

	m_bDataChanged = true;
	m_bDisplayListValid = false;
	m_nLastDataIdx = -1;
}

CGlut2DMapView::~CGlut2DMapView() {
	if (m_nFontBase) glDeleteLists(m_nFontBase, 256);				// Free the display list
}

BOOL CGlut2DMapView::PreCreateWindow(CREATESTRUCT& cs) {
	return CView::PreCreateWindow(cs);
}

BOOL CGlut2DMapView::OnPreparePrinting(CPrintInfo* pInfo) {
	return DoPreparePrinting(pInfo);
}

#ifdef _DEBUG
void CGlut2DMapView::AssertValid() const {
	CView::AssertValid();
}

void CGlut2DMapView::Dump(CDumpContext& dc) const {
	CView::Dump(dc);
}
#endif

//void CGlut2DMapView::DrawScene() {
//
//
//	// Set up orthographic projection for top-down 2D view
//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();
//
//	// Set orthographic projection with proper aspect ratio
//	CRect rect;
//	GetClientRect(&rect);
//	float aspect = (float)rect.Width() / (float)rect.Height();
//
//	// Adjust these values based on your data range (-1 to 1 based on your code)
//	glOrtho(-aspect, aspect, -1.0, 1.0, -10.0, 10.0);
//
//	glMatrixMode(GL_MODELVIEW);
//	glLoadIdentity();
//
//	// Set camera to look straight down (top view)
//	// For top view, we want to look along negative Z axis
//	gluLookAt(0.0, 0.0, 1.0,    // eye position (above the scene)
//		0.0, 0.0, 0.0,    // look at center
//		0.0, 1.0, 0.0);   // up vector
//
//	Point3D pt;
//
//	glNewList(1, GL_COMPILE);
//
//	glEnable(GL_DEPTH_TEST);
//	glDisable(GL_CULL_FACE);
//	glPolygonMode(GL_FRONT_AND_BACK, m_FillMode);
//	glShadeModel(GL_SMOOTH);
//
//	if (m_bQuad) {
//		glBegin(GL_QUADS);
//	}
//	else {
//		glBegin(GL_TRIANGLES);
//	}
//
//	Point3D* p1, * p2, * p3, * p4;
//	for (UINT x = 0; x < MashPoints.size(); ) {
//		if (m_bQuad) {
//			p1 = &MashPoints[x++];
//			p2 = &MashPoints[x++];
//			p3 = &MashPoints[x++];
//			p4 = &MashPoints[x++];
//			if (p1->d && p2->d && p3->d && p4->d) {
//				Vertex(p1->x, p1->y, p1->z);
//				Vertex(p2->x, p2->y, p2->z);
//				Vertex(p3->x, p3->y, p3->z);
//				Vertex(p4->x, p4->y, p4->z);
//			}
//		}
//		else {
//			p1 = &MashPoints[x++];
//			p2 = &MashPoints[x++];
//			p3 = &MashPoints[x++];
//			if (p1->d && p2->d && p3->d) {
//				Vertex(p1->x, p1->y, p1->z);
//				Vertex(p2->x, p2->y, p2->z);
//				Vertex(p3->x, p3->y, p3->z);
//			}
//		}
//	}
//	glEnd();
//
//	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//	DrawColorBar();
//	glEndList();
//
//	BuildAxisList();
//	ShowLegend(pRcp);
//
//	HDC m_hDC = ::GetDC(m_hWnd);
//	glRotatef(0.0f, 0.0f, 0.0f, 0.0f);	// and to rotate
//	glRotatef(0.0f, 0.0f, 0.0f, 0.0f);
//
//	BEGIN_GL
//		//PrintString("X Axis", 0, 1.5, 0);
//		glColor3f(1.0, 0.0, 0.0);
//	::glRasterPos3f(-300, 0.2, -300.2);
//	printstring(GLUT_BITMAP_TIMES_ROMAN_10, "Help");
//
//	glRasterPos2i(20, 40);
//	printstring(GLUT_BITMAP_TIMES_ROMAN_10, "h - Togle Help");
//
//	END_GL
//}

void CGlut2DMapView::DrawScene() {
	Point3D pt;

	// Check if data has changed
	if (m_bDataChanged || m_nLastDataIdx != nDataIdx) {
		// Data has changed, regenerate geometry
		vector<Point3D> RawPoints;
		GenRealData(pRcp, RawPoints);
		SetGraphPoints(RawPoints);

		// Invalidate the display list so it will be regenerated
		if (m_bDisplayListValid) {
			glDeleteLists(1, 1);  // Delete old display list
			m_bDisplayListValid = false;
		}

		m_bDataChanged = false;
		m_nLastDataIdx = nDataIdx;
	}

	// If display list is not valid, create it
	if (!m_bDisplayListValid) {
		glNewList(1, GL_COMPILE);

		glEnable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, m_FillMode);
		glShadeModel(GL_SMOOTH);

		if (m_bQuad) {
			glBegin(GL_QUADS);
		}
		else {
			glBegin(GL_TRIANGLES);
		}

		Point3D* p1, * p2, * p3, * p4;
		for (UINT x = 0; x < MashPoints.size(); ) {
			if (m_bQuad) {
				p1 = &MashPoints[x++];
				p2 = &MashPoints[x++];
				p3 = &MashPoints[x++];
				p4 = &MashPoints[x++];
				if (p1->d && p2->d && p3->d && p4->d) {
					Vertex(p1->x, p1->y, p1->z);
					Vertex(p2->x, p2->y, p2->z);
					Vertex(p3->x, p3->y, p3->z);
					Vertex(p4->x, p4->y, p4->z);
				}
			}
			else {
				p1 = &MashPoints[x++];
				p2 = &MashPoints[x++];
				p3 = &MashPoints[x++];
				if (p1->d && p2->d && p3->d) {
					Vertex(p1->x, p1->y, p1->z);
					Vertex(p2->x, p2->y, p2->z);
					Vertex(p3->x, p3->y, p3->z);
				}
			}
		}
		glEnd();

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		// DrawColorBar(); // Consider if color bar should be regenerated too
		glEndList();

		m_bDisplayListValid = true;

		// Also rebuild axis list for new data
		BuildAxisList();
	}

	// Execute the display list
	glCallList(1);

	// Show legend with updated data
	ShowLegend(pRcp);

	HDC m_hDC = ::GetDC(m_hWnd);

	BEGIN_GL
		glColor3f(1.0, 0.0, 0.0);
	::glRasterPos3f(-300, 0.2, -300.2);
	printstring(GLUT_BITMAP_TIMES_ROMAN_10, "Help");

	glRasterPos2i(20, 40);
	printstring(GLUT_BITMAP_TIMES_ROMAN_10, "h - Togle Help");
	END_GL



}

void CGlut2DMapView::SetDataColumn(int nColumn) {
	//if (nDataIdx != nColumn) {
		nDataIdx = nColumn;
		m_bDataChanged = true;
		DefaultGraphic();
		// Force redraw
		//InvalidateRect(NULL, FALSE);
		//UpdateWindow();
	//}
}


void CGlut2DMapView::Vertex(float x, float y, float z) {
	int c = 26 * z;
	if (c > 25)  c = 25;
	if (c < 0) ASSERT(0);

	glColor3ub(cr[c], cg[c], cb[c]);
	//glVertex3f(x, m_zScale * z, y);
	glVertex2f(x, y);
}

void CGlut2DMapView::DefaultGraphic() {
	vector <Point3D> RawPoints;
	//GenFakeData(RawPoints);
	GenRealData(pRcp, RawPoints);
	SetGraphPoints(RawPoints);
	BuildAxisList();
	ShowLegend(pRcp);
}

void CGlut2DMapView::SetGraphPoints(vector <Point3D>& RawPoints) {
	//	m_xSize = m_zSize = m_GridSize / 2;
	MashPoints.resize(256 * m_GridSize);
	if (MashPoints.empty()) {
		return;
	}
	MashPoints.clear();

	//Circle(RawPoints);
	Square(pRcp, RawPoints);	//12082025/Tushar/Draw 3D Interpolation calulation

	float fMinZ, fMaxZ;
	vector<Point3D>::iterator iter;
	iter = max_element(MashPoints.begin(), MashPoints.end(), Point3DCompareZ);
	fMaxZ = (*iter).z;
	iter = min_element(MashPoints.begin(), MashPoints.end(), Point3DCompareZ);
	fMinZ = (*iter).z;

	float m_fRangeZ = fMaxZ - fMinZ;

	float val;
	int iMashPt = MashPoints.size();
	size_t i; for (i = 0; i < MashPoints.size(); i++) {
		if (m_fRangeZ > 0)
			val = (MashPoints[i].z - fMinZ) / m_fRangeZ;
		else
			val = 0;
		MashPoints[i].z = val;
	}
}

//void CGlut2DMapView::OnDraw(CDC* pDC) {
//	//========== Clear the background and Z-buffer
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	//========== Clean the modeling matrix (make it equal the unity matrix)
//	glMatrixMode(GL_MODELVIEW);
//	glLoadIdentity();
//
//	//======= At first set the light (otherwise it will rotate with the image)
////	SetLight();
//
//	glPushMatrix();
//	//====== Change the modeling matrix coefficients in order
//	glTranslatef(m_xTrans, m_yTrans, m_zTrans - .25);	// to shift
//	//glRotatef(m_AngleX, 1.0f, 0.0f, 0.0f);	// and to rotate
//	//glRotatef(m_AngleY, 0.0f, 1.0f, 0.0f);
//	glRotatef(m_AngleY, 0.0f, 1.0f, 0.0f);
//	//glRotatef(0.0f, 0.0f, 0.0f, 0.0f);	// and to rotate
//	//glRotatef(0.0f, 0.0f, 0.0f, 0.0f);
//
//	//====== the following vertices coordinates (they are being multiplied by matrix)
//	glCallList(1);
//	glCallList(2);
//
//	glPopMatrix();
//
//	/*if (m_bShowLegend) */ShowLegend(pRcp);
//	//====== Switch back and front buffers (to show what happened)
//	SwapBuffers(m_hdc);
//	GdiFlush();
//}
void CGlut2DMapView::OnDraw(CDC* pDC) {
	//========== Clear the background and Z-buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//========== Clean the modeling matrix (make it equal the unity matrix)
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//======= At first set the light (otherwise it will rotate with the image)
//??SetLight();

	glPushMatrix();
	//====== Change the modeling matrix coefficients in order

	// 1. Apply translation first
	glTranslatef(m_xTrans, m_yTrans, m_zTrans - .25);

	// 2. *** APPLY SCALING TO FLIP VERTICALLY (Y-AXIS) ***
	// The arguments are (scaleX, scaleY, scaleZ)
	glScalef(1.0f, -1.0f, 1.0f);

	// The rotation lines are no longer needed for a flip
	// glRotatef(0.0f, 0.0f, 0.0f, 0.0f);??
	// glRotatef(0.0f, 0.0f, 0.0f, 0.0f);

	//====== the following vertices coordinates (they are being multiplied by matrix)
	glCallList(1);
	glCallList(2);

	glPopMatrix();

	ShowLegend(pRcp);
		//====== Switch back and front buffers (to show what happened)
		SwapBuffers(m_hdc);
		GdiFlush();
}


void CGlut2DMapView::GenFakeData(vector <Point3D>& RawPoints) {
	Point3D P3D;

	RawPoints.clear();

	P3D.x = 0;
	P3D.y = 0;
	Polar2Cart(P3D.x, P3D.y);
	float z = 100 * sin(0.);
	P3D.z = z;
	RawPoints.push_back(P3D);
	for (float r = 0.1f; r <= 1.0f; r += 0.25f) {
		z = 100 * sin(r * PI / 2.0f);
		for (float a = 0; a < 360.0f; a += 90.0f) {
			P3D.x = r;
			P3D.y = a;
			Polar2Cart(P3D.x, P3D.y);
			P3D.z = z;
			RawPoints.push_back(P3D);
		}
	}
}

void CGlut2DMapView::GenRealData(CRecipe413* pRcp, std::vector <Point3D>& RawPoints) {
	//CRecipe* pRcp = AG.GetRecipe();
	if (!pRcp) return;
	CPattern* pPattern = &pRcp->Patt;
	short nCount = pPattern->MP.GetCount(); if (nCount <= 0) return;
	CWaferParam* Wp = pRcp->GetWp(); if (!Wp) return;

	RawPoints.clear();
	RawPoints.resize(nCount);
	float fRad;
	m_Rad = fRad = (float)Wp->size / 2.0;
	short n;
	for (n = 0; n < nCount; n++) {
		CMPoint* p = pPattern->MP.Get(n);
		if (!p) continue;
		float fX, fY;
		p->GetCoor(fX, fY);
		CData* d = p->GetData(0, FALSE); if (!d) continue;
		float fValue = d->Get(nDataIdx);
		// [06262024 ZHIMING
		if (pRcp->IsRoughnessMeasurement()) {
			fValue /= p413App->Global.umUnitInRoughness ? 10000.0f : 1.0f;
		}
		// ]
		if (0 < fValue) {
			RawPoints[n].x = fX;
			RawPoints[n].y = fY;
			RawPoints[n].z = fValue;
		}
	}

	float fXMin = RawPoints[0].x, fXMax = RawPoints[0].x, fYMin = RawPoints[0].y,
		fYMax = RawPoints[0].y, fZMin = RawPoints[0].z, fZMax = RawPoints[0].z;
	for (n = 1; n < nCount; n++) {
		if (fXMin > RawPoints[n].x) {
			fXMin = RawPoints[n].x;
		}
		if (fXMax < RawPoints[n].x) {
			fXMax = RawPoints[n].x;
		}
		if (fYMin > RawPoints[n].y) {
			fYMin = RawPoints[n].y;
		}
		if (fYMax < RawPoints[n].y) {
			fYMax = RawPoints[n].y;
		}
		if (fZMin > RawPoints[n].z) {
			fZMin = RawPoints[n].z;
		}
		if (fZMax < RawPoints[n].z) {
			fZMax = RawPoints[n].z;
		}
	}

	for (n = 0; n < nCount; n++) {
		float x = RawPoints[n].x;
		float y = RawPoints[n].y;
		float z = RawPoints[n].z;
#if 0
		if (x < 0) {
			RawPoints[n].x = -x / fXMin;
		}
		else {
			RawPoints[n].x = x / fXMax;
		}

		if (y < 0) {
			RawPoints[n].y = -y / fYMin;
		}
		else {
			RawPoints[n].y = y / fYMax;
		}
		if (z < 0) {
			RawPoints[n].z = -z * 10.0f / fZMin;
		}
		else {
			RawPoints[n].z = 10.0f * z / fZMax;
		}
#else
		float fXMax = max(fabs(fXMin), fabs(fXMin));
		float fYMax = max(fabs(fYMin), fabs(fYMin));
		fRad = max(fXMax, fYMax);
		m_fZMax = fZMax; m_fZMin = fZMin;
		if (fZMax > fZMin)
			RawPoints[n].z = (z - fZMin) / (fZMax - fZMin);
		if (fRad > 0) {
			RawPoints[n].x = x / fRad;
			RawPoints[n].y = y / fRad;
		}
#endif
	}
}

void CGlut2DMapView::Circle(vector <Point3D>& RawPoints) {
	int idx = 1;
	float r, d;
	inc = 1;
	d = 1 / float(m_GridSize);
	for (r = 0; r < 1.0f; r += d) {
		glBegin(GL_TRIANGLES);
		DrawCircle(RawPoints, r, d, idx++);
		glEnd();
	}
}
//[ 12112025_Tushar
void CGlut2DMapView::Square(CRecipe413* pRcp, std::vector<Point3D>& RawPoints)
{
	if (!pRcp) return;
	CPattern* pPattern = &pRcp->Patt;
	if (!pPattern) return;

	// Get the precomputed global grid
	std::vector<std::vector<double>>& globalGrid = p413App->Global.GeneralInterpolatedGrid;

	//if (globalGrid.empty() || globalGrid[0].empty()) {
	//	AfxMessageBox(_T("No interpolated data available!"));
	//	return;
	//}

	int iXDim = (int)globalGrid.size(); // grid dimension (e.g., 206)
	//if (globalGrid[0].size() != (size_t)iXDim) {
	//	AfxMessageBox(_T("Interpolated grid is not square!"));
	//	return;
	//}

	// Allocate pfZ to match grid size
	float* pfZ = new float[iXDim * iXDim];
	ZeroMemory(pfZ, iXDim * iXDim * sizeof(float));

	// Compute min/max from grid
	double m_fZMin = DBL_MAX;
	double m_fZMax = -DBL_MAX;

	for (int i = 0; i < iXDim; i++) {
		for (int j = 0; j < iXDim; j++) {
			double fZ = globalGrid[i][j];
			pfZ[i * iXDim + j] = static_cast<float>(fZ);
			if (fZ < m_fZMin) m_fZMin = fZ;
			if (fZ > m_fZMax) m_fZMax = fZ;
		}
	}

	// Handle degenerate case
	if (m_fZMin == DBL_MAX) {
		m_fZMin = m_fZMax = 0.0;
	}

	// Now render
	float fHstep = 0.5f / float((iXDim - 1) / 2); // because iXDim = 2*m_GridSize + 1
	double f3 = 1.0 - 4 * fHstep + 4 * fHstep * fHstep;

	glBegin(GL_QUADS);
	m_bQuad = TRUE;

	int i, j;
	float x, y;
	for (i = 0, x = -1.0f + fHstep; x <= 1.0f - fHstep; x += 2.0f * fHstep, i++) {
		for (j = 0, y = -1.0f + fHstep; y <= 1.0f - fHstep; y += 2.0f * fHstep, j++) {
			float r2 = x * x + y * y;
			if (r2 < f3 && i + 1 < iXDim && j + 1 < iXDim) {
				MakeMashPoint(x - fHstep, y - fHstep, pfZ[i * iXDim + j]);
				MakeMashPoint(x - fHstep, y + fHstep, pfZ[i * iXDim + j + 1]);
				MakeMashPoint(x + fHstep, y + fHstep, pfZ[(i + 1) * iXDim + j + 1]);
				MakeMashPoint(x + fHstep, y - fHstep, pfZ[(i + 1) * iXDim + j]);
			}
		}
	}

	glEnd();
	delete[] pfZ; // Don't forget cleanup!
}



//void CWMapView::Square(CRecipe413* pRcp, std::vector <Point3D>& RawPoints) {
//	//CRecipe* pRcp = AG.GetRecipe();
//	if (!pRcp) return;
//	CPattern* pPattern = &pRcp->Patt; if (!pPattern) return;
//
//	float x, y;
//	//	float x1, y1;
//	float fHstep = 0.5f / float(m_GridSize);
//	int i, j;
//	glBegin(GL_QUADS);
//	m_bQuad = TRUE;
//	int iXDim = 2 * m_GridSize + 1;
//
//#if 1
//	double f3 = 1.0 - 4 * fHstep + fHstep * fHstep * 4;
//	double f4 = 1.0 + 4 * fHstep + fHstep * fHstep * 4;
//	double fInvRange;
//	if (m_fZMax - m_fZMin) fInvRange = 1 / (m_fZMax - m_fZMin);
//	else fInvRange = 1;
//	float* pfZ = new float[iXDim * iXDim];
//	ZeroMemory(pfZ, iXDim * iXDim * sizeof(float));
//
//	//std::ifstream myfile("C:\\Users\\HP\\Desktop\\WaferInterpolatedMap.csv");
//	char c;
//	float fZ;
//	m_fZMax = -INT_MAX;
//	m_fZMin = INT_MAX;
//	std::string line;
//
//
//	/// 
//	CRect rc;
//	GetClientRect(&rc);
//#define FRAME_SIZE	(pRcp->Wp.size)
//	rc.InflateRect(-rc.Width() * 0.1, -rc.Height() * 0.1);
//	cpt = rc.CenterPoint();
//	m_nWaferSize = pRcp->Wp.size;
//	if (rc.Width() <= rc.Height()) {
//		m_fLog2Device = float(rc.Width()) / float((m_bFitScreen ? m_nWaferSize : FRAME_SIZE));
//	}
//	else {
//		m_fLog2Device = float(rc.Height()) / float((m_bFitScreen ? m_nWaferSize : FRAME_SIZE));
//	}
//
//	CWaferParam* Wp = pRcp->GetWp(); /*if (!Wp) return FALSE;*/
//	int nLogicalDiameter = (int)(Wp->size * m_fLog2Device);
//	nLogicalDiameter = (int)(0.9 * nLogicalDiameter);
//	int nStep = nLogicalDiameter / CELL_SIZE;
//
//	int nGridTableSize = (nStep + 1) * (nStep + 1);
//	double(*GridTable)[2] = new double[nGridTableSize + 2][2];
//	int nWeightTableSize = ((nStep + 1) * (nStep + 2)) / 2;
//	double* WeightTable = new double[nWeightTableSize + 1];
//
//	if (!GridTable || !WeightTable) {
//		//if (mp2) delete[] mp2;
//		if (GridTable)delete[] GridTable;
//		if (WeightTable)delete[]WeightTable;
//		//return FALSE;
//	}
//	memset(GridTable, 0, nGridTableSize * 2 * sizeof(double));
//
//	int nRadius = Wp->size / 2;
//	int nSquareRadius = nRadius * nRadius;
//	short nCount = pPattern->MP.GetCount();
//	double dfZone = 3.14 * nSquareRadius / (double)nCount;
//	double dfCellSize = CELL_SIZE / m_fLog2Device;
//	double dfRatio = dfCellSize * dfCellSize / dfZone;
//
//	int nIndex = 0;
//	for (int i = 0; i <= nStep; i++) {
//		int ii = i * i;
//		for (int j = 0; j < i; j++) {
//			double dfT = (ii + j * j);
//			WeightTable[nIndex++] = exp(-dfT * dfRatio);
//		}
//		if (nIndex < nWeightTableSize) {
//			WeightTable[nIndex++] = exp(-ii * 2 * dfRatio);
//		}
//		else {
//			ASSERT(0); // Overflow [6/5/2020 yuenl]
//			
//			if (GridTable)delete[] GridTable;
//			if (WeightTable)delete[]WeightTable;
//			//return FALSE;
//		}
//	}
//	const float nStep2 = (nStep + 1) / 2.0f;
//	const float nStepWaferSize = nStep / float(Wp->size);
//	for (short n = 0; n < nCount; n++) {
//		CMPoint* p = pPattern->MP.Get(n); if (!p) continue;
//		float fX, fY;
//		p->GetCoor(fX, fY);
//		//mp2[n].fX = fX; mp2[n].fY = fY;
//		int nXCell = (int)floor(fX * nStepWaferSize + nStep2);
//		int nYCell = (int)floor(fY * nStepWaferSize + nStep2);
//		CData* pData = p->GetData(0, FALSE); if (!pData) continue;
//		float fValue = pData->Get(nDataIdx);
//		if (fValue != -9999990.00f) {
//			int nIndex = 0;
//			for (int i = 0; i <= nStep; i++) {
//				for (int j = 0; j <= nStep; j++) {
//					int nX = abs(i - nXCell);
//					int nY = abs(j - nYCell);
//					int k = ((nY > nX) ? (nY * (nY + 1) / 2 + nX) : (nX * (nX + 1) / 2 + nY));
//					bool b = (0 <= k) && (k < nWeightTableSize);
//					if (b) {
//						GridTable[nIndex][0] += WeightTable[k];
//						GridTable[nIndex][1] += WeightTable[k] * fValue;
//					}
//					nIndex++;
//				}
//			}
//		}
//		//mp2[n].fT = fValue;
//	}
//	double dfMin, dfMax;
//
//	double df = GridTable[nIndex][1] / GridTable[nIndex][0];
//	dfMin = dfMax = df;
//	nIndex = 0;
//	for (int i = 0; i <= nStep; i++) {
//		for (int j = 0; j <= nStep; j++) {
//			if (0 < GridTable[nIndex][0]) {
//				df = GridTable[nIndex][1] / GridTable[nIndex][0];
//				GridTable[nIndex][1] = df;
//				//if (0 < df) { // 20250926_Shamim commented because now we are accepting negative values also. Otherwise 2d map will not show for columns that have all negative values. 
//				if (/*(dfMin == 0) ||*/ (dfMin > df)) dfMin = df;
//				if (/*(dfMax == 0) ||*/ (dfMax < df)) dfMax = df;
//				//} // 20250926_Shamim commented
//			}
//			nIndex++;
//		}
//	}
//	m_fZMax = dfMax;
//	m_fZMin = dfMin;
//	/// 
//	nStep = nLogicalDiameter / CELL_SIZE;
//	std::vector<std::vector<double>> m_InterpolatedGrid;
//	int dim = nStep + 1;
//
//	// Resize the 2D vector
//	m_InterpolatedGrid.clear();
//	m_InterpolatedGrid.resize(dim, std::vector<double>(dim, 0.0));
//
//	int idx = 0;
//	for (int i = 0; i < dim; i++) {
//		for (int j = 0; j < dim; j++, idx++) {
//			if (GridTable[idx][0] > 0) {
//				m_InterpolatedGrid[i][j] = GridTable[idx][1]; // interpolated value
//			}
//		}
//	}
//
//	///+++++++++++++++++++++++++++
//	//int dimmm = (int)m_InterpolatedGrid.size(); // assumes square grid
//
//	//CString strFileName = _T("WaferInterpolatedMap_Vector.csv");
//	//CFileDialog fileDlg(FALSE, _T("csv"), strFileName,
//	//	OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
//	//	_T("CSV Files (*.csv)|*.csv|All Files (*.*)|*.*||"));
//
//	//// Optional: set default directory
//	//// fileDlg.m_ofn.lpstrInitialDir = _T("C:\\DATA");
//
//	//if (fileDlg.DoModal() != IDOK)
//	//	return;
//
//	//CString strPath = fileDlg.GetPathName();
//	//CStdioFile file;
//	//if (!file.Open(strPath, CFile::modeCreate | CFile::modeWrite | CFile::typeText))
//	//{
//	//	AfxMessageBox(_T("Failed to open file for writing!"), MB_ICONERROR);
//	//	return;
//	//}
//
//	//// Write grid exactly like the original GridTable version
//	//for (int i = 0; i < dimmm; i++)
//	//{
//	//	CString strLine;
//	//	for (int j = 0; j < dimmm; j++)
//	//	{
//	//		if (j > 0)
//	//			strLine += _T(",");
//
//	//		// Format value to 6 decimal places (same as original)
//	//		CString strVal;
//	//		strVal.Format(_T("%.6f"), m_InterpolatedGrid[i][j]);
//	//		strLine += strVal;
//	//	}
//	//	file.WriteString(strLine + _T("\n"));
//	//}
//
//	//file.Close();
//	//AfxMessageBox(_T("Vector-based interpolated grid saved to:\n") + strPath);
//	
//	///
//	
//
//	for (int i = 0; i < iXDim; i++) {
//		for (int j = 0; j < iXDim; j++) {
//			double fZ = m_InterpolatedGrid[i][j]; 
//
//			pfZ[i * iXDim + j] = static_cast<float>(fZ);
//
//			if (fZ < m_fZMin) m_fZMin = fZ;
//			if (fZ > m_fZMax) m_fZMax = fZ;
//		}
//	}
//	
//	//for (int i = 0; i < iXDim; i++) {
//	//	std::getline(myfile, line);
//	//	std::stringstream ss(line);
//	//	std::string cell;
//	//	for (int j = 0; j < iXDim; j++) {
//	//		std::getline(ss, cell, ',');
//	//		//fZ = (fZ - m_fZMin) * fInvRange;
//	//		fZ = std::stof(cell);
//	//		pfZ[i * iXDim + j] =fZ;
//	//		if (m_fZMax < (double)fZ) {
//	//			m_fZMax = fZ;
//	//		}
//	//		if(m_fZMin > (double)fZ) {
//	//			m_fZMin = fZ;
//	//		}
//	//	}
//	//}
//	//myfile.close();
//	/*nIndex = 0;
//	for (int i = 0; i <= nStep; i++) {
//		for (int j = 0; j <= nStep; j++) {
//			if (0 < GridTable[nIndex][0]) {
//				pfZ[i * iXDim + j] = GridTable[nIndex][1] / GridTable[nIndex][0];
//			}
//			nIndex++;
//		}
//	}*/
//
//
//	/*fInvRange = 1 / (m_fZMax - m_fZMin);
//
//	for (int i = 0; i < iXDim; i++) {
//		for (int j = 0; j < iXDim; j++) {
//			pfZ[i * iXDim + j] = (pfZ[i * iXDim + j] - m_fZMin) * fInvRange;
//		}
//	}*/
//
//	// Get the global interpolated grid
//	std::vector<std::vector<double>>& globalGrid = p413App->Global.GeneralInterpolatedGrid;
//
//	// Calculate iXDim from the grid dimensions
//	//int iXDim = 0;
//	if (!globalGrid.empty()) {
//		iXDim = (int)globalGrid.size(); // Assuming square grid
//	}
//
//	// Validate grid is valid and square
//	if (iXDim == 0 || globalGrid[0].size() != (size_t)iXDim) {
//		// Handle error - grid is empty or not square
//		AfxMessageBox(_T("Interpolated grid is empty or invalid!"));
//		return; // or handle appropriately
//	}
//
//	// Allocate pfZ (make sure to delete[] later if dynamically allocated)
//	//float* pfZ = new float[iXDim * iXDim];
//
//	// Initialize min/max values
//	//double m_fZMin = DBL_MAX;
//	//double m_fZMax = -DBL_MAX;
//
//	// Fill pfZ and compute min/max
//	for (int i = 0; i < iXDim; i++) {
//		for (int j = 0; j < iXDim; j++) {
//			double fZ = globalGrid[i][j];
//			pfZ[i * iXDim + j] = static_cast<float>(fZ);
//
//			if (fZ < m_fZMin) m_fZMin = fZ;
//			if (fZ > m_fZMax) m_fZMax = fZ;
//		}
//	}
//
//	// Handle case where all values are the same (avoid division by zero later)
//	if (m_fZMin == DBL_MAX) {
//		m_fZMin = m_fZMax = 0.0;
//	}
//
//
//	/*for (i = 0, x = -1.0f + fHstep; x <= 1.0f - fHstep; x += fHstep * 2.0f, i++) {
//		for (j = 0, y = -1.0f + fHstep; y <= 1.0f - fHstep; y += fHstep * 2.0f, j++) {
//			float r2 = x * x + y * y;
//			if (r2 <= f4) {
//				double fZ;
//				pPattern->Interpolate(pRcp, (x - fHstep) * m_Rad, (y - fHstep) * m_Rad, nDataIdx, &fZ);
//				fZ = (fZ - m_fZMin) * fInvRange;
//				pfZ[i * iXDim + j] = (float)fZ;
//			}
//		}
//	}
//
//	*/
//
//
//
//
//#endif
//
//	for (i = 0, x = -1.0f + fHstep; x <= 1.0f - fHstep; x += fHstep * 2.0f, i++) {
//		for (j = 0, y = -1.0f + fHstep; y <= 1.0f - fHstep; y += fHstep * 2.0f, j++) {
//			float r2 = x * x + y * y;
//			if (r2 < f3) {
//				MakeMashPoint(x - fHstep, y - fHstep, pfZ[i * iXDim + j]);
//				MakeMashPoint(x - fHstep, y + fHstep, pfZ[i * iXDim + j + 1]);
//				MakeMashPoint(x + fHstep, y + fHstep, pfZ[(i + 1) * iXDim + j + 1]);
//				MakeMashPoint(x + fHstep, y - fHstep, pfZ[(i + 1) * iXDim + j]);
//			}
//		}
//	}
//	//delete[] pfZ;
//	glEnd();
//}

//]

void CGlut2DMapView::DrawCircle(vector <Point3D>& RawPoints, float r, float dr, int idx) {
	float a;

	float da;
	float x1, y1;
	float x2, y2;
	float x3, y3;
	if (r != 0) {
		if ((idx < 7)) {
			da = 360.0f / pow(2., idx);
			for (a = 0; a < 360.0f; a += da) {
				x1 = r; y1 = a;
				Polar2Cart(x1, y1);
				x2 = r + dr; y2 = a;
				Polar2Cart(x2, y2);
				x3 = r + dr; y3 = a + da / 2.0f;
				Polar2Cart(x3, y3);
				MakeMashPoint(RawPoints, x1, y1);
				MakeMashPoint(RawPoints, x2, y2);
				MakeMashPoint(RawPoints, x3, y3);

				x2 = r; y2 = a + da;
				Polar2Cart(x2, y2);
				MakeMashPoint(RawPoints, x3, y3);
				MakeMashPoint(RawPoints, x2, y2);
				MakeMashPoint(RawPoints, x1, y1);

				x1 = r + dr; y1 = a + da;
				Polar2Cart(x1, y1);
				MakeMashPoint(RawPoints, x1, y1);
				MakeMashPoint(RawPoints, x2, y2);
				MakeMashPoint(RawPoints, x3, y3);
			}
		}
		else if (idx % 2) {
			da = 360.0f / pow(2., idx - inc);
			inc++;
			for (a = 0; a < 360.0f; a += da) {
				x1 = r; y1 = a;
				Polar2Cart(x1, y1);
				x2 = r + dr; y2 = a;
				Polar2Cart(x2, y2);
				x3 = r; y3 = a + da / 2;
				Polar2Cart(x3, y3);
				MakeMashPoint(RawPoints, x1, y1);
				MakeMashPoint(RawPoints, x2, y2);
				MakeMashPoint(RawPoints, x3, y3);

				x1 = r + dr; y1 = a + da;
				Polar2Cart(x1, y1);
				MakeMashPoint(RawPoints, x2, y2);
				MakeMashPoint(RawPoints, x1, y1);
				MakeMashPoint(RawPoints, x3, y3);

				x2 = r; y2 = a + da;
				Polar2Cart(x2, y2);
				MakeMashPoint(RawPoints, x1, y1);
				MakeMashPoint(RawPoints, x2, y2);
				MakeMashPoint(RawPoints, x3, y3);
			}
		}
		else {
			da = 360.0f / pow(2., idx - inc);
			inc++;
			for (a = 0; a < 360.0f; a += da) {
				x1 = r; y1 = a;
				Polar2Cart(x1, y1);
				x2 = r + dr; y2 = a;
				Polar2Cart(x2, y2);
				x3 = r + dr; y3 = a + da / 2.0f;
				Polar2Cart(x3, y3);
				MakeMashPoint(RawPoints, x1, y1);
				MakeMashPoint(RawPoints, x2, y2);
				MakeMashPoint(RawPoints, x3, y3);

				x2 = r; y2 = a + da;
				Polar2Cart(x2, y2);
				MakeMashPoint(RawPoints, x1, y1);
				MakeMashPoint(RawPoints, x3, y3);
				MakeMashPoint(RawPoints, x2, y2);

				x1 = r + dr; y1 = a + da;
				Polar2Cart(x1, y1);
				MakeMashPoint(RawPoints, x3, y3);
				MakeMashPoint(RawPoints, x1, y1);
				MakeMashPoint(RawPoints, x2, y2);
			}
		}
	}
	else {
		x1 = y1 = 0;
		x1 = x1;
		y1 = y1;
		da = 360.0f / 4.0f;
		for (float a = 0; a < 360.0f; a += da) {
			x2 = dr; y2 = a;
			Polar2Cart(x2, y2);
			x3 = dr; y3 = a + da;
			Polar2Cart(x3, y3);
			MakeMashPoint(RawPoints, x1, y1);
			MakeMashPoint(RawPoints, x2, y2);
			MakeMashPoint(RawPoints, x3, y3);
		}
	}
}

void CGlut2DMapView::MakeMashPoint(vector <Point3D>& RawPoints, float x, float y) {
	Interpolater<InverseDist> itp;
	Point3D pt = Point3D(x, y, itp.GetInterpolatedZ(x, y, RawPoints.begin(), RawPoints.end()));
	if (sqrt(x * x + y * y) > 1.01f) {
		pt.d = 0;
	}
	MashPoints.push_back(pt);
}

void CGlut2DMapView::ShowLegend(CRecipe413* pRcp) {
	if (!pRcp) return;
	int off = 500;

	CPattern* pPat = &pRcp->Patt; if (!pPat) return;

	::glPushMatrix();
	::glTranslated(m_xTrans, m_yTrans, m_zTrans);

	// Draw color bar at the bottom 
	int i; for (i = 0; i < 26; i++) {
		glColor3ub(cr[i], cg[i], cb[i]);
		glRectf((i - 13) / 26.0 + 0.4, -1.08, 0.4 + (i - 12) / 26.0, -1.0);
	}

	// Draw title at the top
	CString title = pRcp->MeParam[nDataIdx / MAXMEPASET].MPa[nDataIdx - MaXPA * (nDataIdx / MAXMEPASET)].Name;
	char szStr[50];
	float fSpace = 0.08;

	glColor4f(0.0, 0.0, 0.0, 0.4);

	CStat pStat(pPat->MP.Stats[nDataIdx]);
	// [06262024 ZHIMING
	if (pRcp->IsRoughnessMeasurement()) {
		pStat.Divide(p413App->Global.umUnitInRoughness ? 10000.0f : 1.0f);
	}
	// ]

	// Draw title at top center
	void* pColor = GLUT_BITMAP_TIMES_ROMAN_24;
	::glRasterPos2d(-0.0, 0.95);  // Top center for title
	sprintf(szStr, "%s", title);
	printstring(pColor, szStr);

	float fCoeff = 1.0;

	// Draw color bar labels at bottom 
	pColor = GLUT_BITMAP_HELVETICA_12;
	::glRasterPos2d(-0.15, -.95);
	sprintf(szStr, "%.2f\n", pStat.m_fMin);
	printstring(pColor, szStr);
	::glRasterPos2d(0.1, -.95);
	sprintf(szStr, "%.2f\n", pStat.m_fMin + 0.25 * (pStat.m_fMax - pStat.m_fMin));
	printstring(pColor, szStr);
	::glRasterPos2d(0.35, -.95);
	sprintf(szStr, "%.2f\n", pStat.m_fMin + 0.5 * (pStat.m_fMax - pStat.m_fMin));
	printstring(pColor, szStr);
	::glRasterPos2d(0.6, -.95);
	sprintf(szStr, "%.2f\n", pStat.m_fMin + 0.75 * (pStat.m_fMax - pStat.m_fMin));
	printstring(pColor, szStr);
	::glRasterPos2d(0.85, -.95);
	sprintf(szStr, "%.2f\n", pStat.m_fMax);
	printstring(pColor, szStr);

	// *** MOVED TO TOP-LEFT CORNER ***
	pColor = GLUT_BITMAP_HELVETICA_10;
	if (pPat) {
		if (pRcp->IsEchoBowWarpMeasurement() || pRcp->IsConfocalWarpMeasurement() || pRcp->IsConfocalDualWarpMeasurement()) {
			// For Bow/Warp measurements - TOP-LEFT corner
			::glRasterPos2d(-0.95, 0.85);  // Changed from (-0.75, -0.95 + fSpace * 4)
			sprintf(szStr, "Min: \t%.2f\n", pStat.m_fMin);
			printstring(pColor, szStr);

			::glRasterPos2d(-0.95, 0.85 - fSpace);  // Changed from (-0.75, -0.95 + fSpace * 3)
			sprintf(szStr, "Max: \t%.2f\n", pStat.m_fMax);
			printstring(pColor, szStr);

			::glRasterPos2d(-0.95, 0.85 - fSpace * 2);  // Changed from (-0.75, -0.95 + fSpace * 2)
			sprintf(szStr, "Warp: \t%.2f\n", pStat.m_fWarp);
			printstring(pColor, szStr);

			::glRasterPos2d(-0.95, 0.85 - fSpace * 3);  // Changed from (-0.75, -0.95 + fSpace)
			sprintf(szStr, "Bow: \t%.2f\n", pStat.m_fBow);
			printstring(pColor, szStr);
		}
		else {
			// For other measurements (Min/Ave/Max/Std) - TOP-LEFT corner
			::glRasterPos2d(-0.95, 0.85);  // Changed from (-0.75, -0.95 + fSpace * 4)
			sprintf(szStr, "Min: \t%.2f\n", pStat.m_fMin);
			printstring(pColor, szStr);

			::glRasterPos2d(-0.95, 0.85 - fSpace);  // Changed from (-0.75, -0.95 + fSpace * 3)
			sprintf(szStr, "Ave: \t%.2f\n", pStat.m_fAverage);
			printstring(pColor, szStr);

			::glRasterPos2d(-0.95, 0.85 - fSpace * 2);  // Changed from (-0.75, -0.95 + fSpace * 2)
			sprintf(szStr, "Max: \t%.2f\n", pStat.m_fMax);
			printstring(pColor, szStr);

			::glRasterPos2d(-0.95, 0.85 - fSpace * 3);  // Changed from (-0.75, -0.95 + fSpace)
			sprintf(szStr, "Std: \t%.2f\n", pStat.m_fSTD);
			printstring(pColor, szStr);
		}
	}

	::glPopMatrix();

	glFlush();
	GdiFlush();
}

//void CGlut2DMapView::ShowLegend(CRecipe413* pRcp) {
//	if (!pRcp) return;
//	int off = 500;
//
//	CPattern* pPat = &pRcp->Patt; if (!pPat) return;
//
//	::glPushMatrix();
//	::glTranslated(m_xTrans, m_yTrans, m_zTrans);
//
//	int i; for (i = 0; i < 26; i++) {
//		glColor3ub(cr[i], cg[i], cb[i]);
//		glRectf((i - 13) / 26.0 + 0.4, -1.08, 0.4 + (i - 12) / 26.0, -1.0);
//	}
//	CString title = pRcp->MeParam[nDataIdx / MAXMEPASET].MPa[nDataIdx - MaXPA * (nDataIdx / MAXMEPASET)].Name; //p413App->getSelectedTitle(); //30122024_1
//	char szStr[50];
//	float fSpace = 0.08;
//
//	glColor4f(0.0, 0.0, 0.0, 0.4);
//
//	CStat pStat(pPat->MP.Stats[nDataIdx]);
//	// [06262024 ZHIMING
//	if (pRcp->IsRoughnessMeasurement()) {
//		pStat.Divide(p413App->Global.umUnitInRoughness ? 10000.0f : 1.0f);
//	}
//	// ]
//	//[30122024_1
//	void* pColor = GLUT_BITMAP_TIMES_ROMAN_24;
//	::glRasterPos2d(-0.0, -0.95 + fSpace * 27);
//	sprintf(szStr, "%s", title);
//	printstring(pColor, szStr);
//	//]
//	float fCoeff = 1.0;
//	pColor = GLUT_BITMAP_HELVETICA_12;
//	::glRasterPos2d(-0.15, -.95);
//	sprintf(szStr, "%.2f\n", pStat.m_fMin);
//	printstring(pColor, szStr);
//	::glRasterPos2d(0.1, -.95);
//	sprintf(szStr, "%.2f\n", pStat.m_fMin + 0.25 * (pStat.m_fMax - pStat.m_fMin));
//	printstring(pColor, szStr);
//	::glRasterPos2d(0.35, -.95);
//	sprintf(szStr, "%.2f\n", pStat.m_fMin + 0.5 * (pStat.m_fMax - pStat.m_fMin));
//	printstring(pColor, szStr);
//	::glRasterPos2d(0.6, -.95);
//	sprintf(szStr, "%.2f\n", pStat.m_fMin + 0.75 * (pStat.m_fMax - pStat.m_fMin));
//	printstring(pColor, szStr);
//	::glRasterPos2d(0.85, -.95);
//	sprintf(szStr, "%.2f\n", pStat.m_fMax);
//	printstring(pColor, szStr);
//
//	pColor = GLUT_BITMAP_HELVETICA_10;
//	if (pPat) {
//		if (pRcp->IsEchoBowWarpMeasurement() || pRcp->IsConfocalWarpMeasurement() || pRcp->IsConfocalDualWarpMeasurement()) {	//11252022 ZHIMING // 23102024 added IsConfocalWarpMeasurement()
//			::glRasterPos2d(-0.75, -0.95 + fSpace * 4);
//			sprintf(szStr, "Min: \t%.2f\n", pStat.m_fMin);
//			printstring(pColor, szStr);
//			::glRasterPos2d(-0.75, -0.95 + fSpace * 3);
//			sprintf(szStr, "Max: \t%.2f\n", pStat.m_fMax);
//			printstring(pColor, szStr);
//			sprintf(szStr, "Warp: \t%.2f\n", pStat.m_fWarp);
//			::glRasterPos2d(-0.75, -0.95 + fSpace * 2);
//			printstring(pColor, szStr);
//			::glRasterPos2d(-0.75, -0.95 + fSpace);
//			sprintf(szStr, "Bow: \t%.2f\n", pStat.m_fBow);
//			printstring(pColor, szStr);
//			//::glRasterPos2d(-0.75, -0.95);
//			//sprintf(szStr, "Stress: \t%.2f\n", pStat->m_fStress);
//			//printstring(pColor, szStr);
//		}
//		else {
//			::glRasterPos2d(-0.75, -0.95 + fSpace * 4);
//			sprintf(szStr, "Min: \t%.2f\n", pStat.m_fMin);
//			printstring(pColor, szStr);
//			::glRasterPos2d(-0.75, -0.95 + fSpace * 3);
//			sprintf(szStr, "Ave: \t%.2f\n", pStat.m_fAverage);
//			printstring(pColor, szStr);
//			sprintf(szStr, "Max: \t%.2f\n", pStat.m_fMax);
//			::glRasterPos2d(-0.75, -0.95 + fSpace * 2);
//			printstring(pColor, szStr);
//			::glRasterPos2d(-0.75, -0.95 + fSpace);
//			sprintf(szStr, "Std: \t%.2f\n", pStat.m_fSTD);
//			printstring(pColor, szStr);
//			//::glRasterPos2d(-0.75, -0.95);
//			//printstring(pColor, szStr);
//		}
//	}
//
//	::glPopMatrix();
//
//	glFlush();
//	GdiFlush();
//}

//void CGlut2DMapView::OnRButtonUp(UINT nFlags, CPoint point) {
//	//CMenu menu;
//	//VERIFY(menu.LoadMenu(IDR_SUBMENUS));
//	//CMenu* pPopup = menu.GetSubMenu(28); if (!pPopup) return;
//	//CPoint ScrnPt = point;
//	//ClientToScreen(&ScrnPt);
//	//int res = pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, ScrnPt.x, ScrnPt.y, AfxGetMainWnd());
//
//	OnWaferviewToolExport();
//
//	CGLView::OnRButtonUp(nFlags, point);
//}

BOOL WriteWindowToDIB(LPCTSTR szFile, CWnd* pWnd);

void CGlut2DMapView::OnWaferviewToolExport() {
	CString strFilter(_T("Bitmap Files (*.bmp)|*.bmp|All Files (*.*)|*.*||"));
	CFileDialog dlg(FALSE, _T("bmp"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, strFilter, this);
	if (dlg.DoModal() == IDOK) {
		UpdateWindow();
		if (!::WriteWindowToDIB(dlg.GetPathName(), this)) {
			AfxMessageBox(_T("Failed to save image"));
		}
	}
}

//void CGlut2DMapView::OnRButtonDown(UINT nFlags, CPoint point) {
//	//	CGLView::OnRButtonDown(nFlags, point);
//}

void CGlut2DMapView::BuildAxisList() {
	double x = 1.1;
	double y = 1.1 * m_zScale;
	double z = 1.1;

	string m_strXtitle = "RIGHT";
	string m_strYtitle = "Backside";
	string m_strZtitle = "Major";

	//	BEGIN_GL

	::glNewList(2, GL_COMPILE);
	// y axis(thickness)
	/*::glBegin(GL_LINES);
	::glColor3ub(128, 128, 255);
	::glVertex3d(0, -y + .6, 0);
	::glVertex3d(0, y + 0.6, 0);
	::glEnd();*/
	////PrintString("X Axis", 0, y + 0.5, 0);

	//  x axis(x-coord)
	/*::glBegin(GL_LINES);
	::glColor3ub(200, 0, 255);
	::glVertex3d(-x, 0.5, 0);
	::glVertex3d(x, 0.5, 0);
	::glEnd();
	::glBegin(GL_TRIANGLES);
	::glVertex3d(x, 0.5, 0);
	::glVertex3d(x - 0.02, 0.5, 0.02);
	::glVertex3d(x - 0.02, 0.5, -0.02);
	::glEnd();*/
	// draw "X"
	/*::glBegin(GL_LINES);
	::glVertex3d(x - 0.04, 0.5, 0.05);
	::glVertex3d(x, 0.5, 0.09);
	::glEnd();
	::glBegin(GL_LINES);
	::glVertex3d(x - 0.04, 0.5, 0.09);
	::glVertex3d(x, 0.5, 0.05);
	::glEnd();*/

	// PrintString("XAxis", 1.1, 0.5, 0);
	// y axis(thickness)
	/*::glBegin(GL_LINES);
	::glColor3ub(128, 128, 255);
	::glVertex3d(0, -y + .5, 0);
	::glVertex3d(0, y + 0.5, 0);
	::glEnd();
	::glBegin(GL_TRIANGLES);
	::glColor3ub(128, 128, 255);
	::glVertex3d(-0.02, y + .48, 0);
	::glVertex3d(0, y + 0.5, 0);
	::glVertex3d(0.02, y + 0.48, 0);
	::glEnd();*/

	// draw "Z"
	/*::glBegin(GL_LINES);
	::glVertex3d(0.05, y + 0.5, 0);
	::glVertex3d(0.09, y + 0.5, 0);
	::glEnd();
	::glBegin(GL_LINES);
	::glVertex3d(0.09, y + 0.5, 0);
	::glVertex3d(0.05, y + 0.46, 0);
	::glEnd();
	::glBegin(GL_LINES);
	::glVertex3d(0.05, y + 0.46, 0);
	::glVertex3d(0.09, y + 0.46, 0);
	::glEnd();*/

	//PrintString("XY", 0, y + 0.5, 0);

	// z axis(y-coord)
	/*::glBegin(GL_LINES);
	::glColor3ub(255, 0, 0);
	::glVertex3d(0, 0.5, -z);
	::glVertex3d(0, 0.5, z);
	::glEnd();
	::glBegin(GL_TRIANGLES);
	::glVertex3d(0, 0.5, -z);
	::glVertex3d(0.02, 0.5, -z + 0.02);
	::glVertex3d(-0.02, 0.5, -z + 0.02);
	::glEnd();*/

	// draw "y"
	/*::glBegin(GL_LINES);
	::glVertex3d(0.09, 0.5, -z);
	::glVertex3d(0.05, 0.5, -z + 0.04);
	::glEnd();
	::glBegin(GL_LINES);
	::glVertex3d(0.05, 0.5, -z);
	::glVertex3d(0.07, 0.5, -z + 0.02);
	::glEnd();*/
	//draw notch
	/*::glBegin(GL_TRIANGLES);
	::glColor3ub(0, 0, 0);
	::glVertex3d(-0.03, 0.5, -z + 2.05);
	::glVertex3d(0.0, 0.5, -z + 2);
	::glVertex3d(0.03, 0.5, -z + 2.05);
	::glEnd();*/
	//draw notch
	::glBegin(GL_TRIANGLES);
	::glColor3ub(0, 0, 0);
	::glVertex3d(-0.03, 0.95, 0);
	::glVertex3d(0.0, 0.90, 0);
	::glVertex3d(0.03, 0.95, 0);
	::glEnd();
	//PrintString(m_strZtitle, 0, 0.5, z);
	//PrintString(m_strZtitle, 0, 0.5, -z);
	::glEndList();
	//	END_GL
}

void CGlut2DMapView::PrintString(string str, double x, double y, double z) {
	//	HDC m_hDC = ::GetDC(m_hWnd);
	//	BEGIN_GL
	::glRasterPos3f(x, y, z);
	::glPushAttrib(GL_LIST_BIT);
	::glListBase(m_nFontBase);
	::glCallLists(str.length(), GL_UNSIGNED_BYTE, str.c_str());
	::glPopAttrib();
	//	END_GL
}

void CGlut2DMapView::MakeMashPoint(float x, float y, float z) {
	Point3D pt = Point3D(0.9 * x, -0.9 * y, z);
	MashPoints.push_back(pt);
}

void CGlut2DMapView::DrawColorBar() {
	glBegin(GL_QUADS);
	int i; for (i = 0; i < 26; i++) {
		double fZ1 = (double)i / 26.0;
		double fZ2 = (double)(i + 1) / 26.0;
		glColor3ub(cr[i], cg[i], cb[i]);
		glVertex3f(0.95, 0.5, (13 - i) / 26.0);
		glColor3ub(cr[i], cg[i], cb[i]);
		glVertex3f(1.0, 0.5, (13 - i) / 26.0);
		glColor3ub(cr[i], cg[i], cb[i]);
		glVertex3f(1.0, 0.5, (12 - i) / 26.0);
		glColor3ub(cr[i], cg[i], cb[i]);
		glVertex3f(0.95, 0.5, (12 - i) / 26.0);
	}
}

void CGlut2DMapView::CreateFont(string fontName, int height) {
	HDC m_hDC = ::GetDC(m_hWnd);

	BEGIN_GL
		CFont font;

	if ((m_nFontBase = ::glGenLists(256)) == 0)
		throw CGrapherException(_T("Can't create font"));;

	if (fontName == _T("symbol"))
		font.CreateFont(height, 0, 0, 0, FW_NORMAL, 0, FALSE, FALSE, SYMBOL_CHARSET, OUT_TT_PRECIS,
			CLIP_DEFAULT_PRECIS, DRAFT_QUALITY, DEFAULT_PITCH, fontName.c_str());
	else
		font.CreateFont(height, 0, 0, 0, FW_NORMAL, 0, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS,
			CLIP_DEFAULT_PRECIS, DRAFT_QUALITY, DEFAULT_PITCH, fontName.c_str());

	::SelectObject(m_hDC, font);
	::wglUseFontOutlines(m_hDC,							// This is the global HDC with the desired font selected.
		0,								// This is the starting ASCII value.
		255,							// This is the ending ASCII value to use (255 is the last).
		m_nFontBase,					// This is the base pointer of our display list we wish to use.
		0,								// This is the deviation from a true outline (floating point)
		0.4f,							// This is the extrude value, or in other words, the thickness in Z.
		WGL_FONT_POLYGONS,				// We specify here that we want it rendered in polygons rather than lines
		m_GlyphInfo);					// The address to the buffer that will hold the 3D font info for each character.
	END_GL
}

void CGlut2DMapView::OnInitialUpdate() {
	CGLViewStatic::OnInitialUpdate();
	//SetupTopView();


}

void CGlut2DMapView::printstring(void* font, char* string) {
	int len = (int)strlen(string);
	for (int i = 0; i < len; i++) {
		glutBitmapCharacter(font, string[i]);
	}
}

int CGlut2DMapView::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	// Do nothing — prevent activation and eat the click
	return MA_NOACTIVATEANDEAT;
}

void CGlut2DMapView::ExportTo(CString exportPath) {
	if (exportPath.IsEmpty()) {
		AfxMessageBox(_T("No export path specified."));
		return;
	}

	// Determine target path ¡ª if file exists, append numeric specifier before extension.
	CString target = exportPath;
	CFileStatus status;
	if (CFile::GetStatus(target, status)) { // file exists
		int dot = target.ReverseFind('.');
		CString base = (dot != -1) ? target.Left(dot) : target;
		CString ext = (dot != -1) ? target.Mid(dot) : _T("");

		int idx = 1;
		CString candidate;
		// find first non-existing filename with suffix _1, _2, ...
		do {
			candidate.Format(_T("%s_%d%s"), base, idx, ext);
			idx++;
		} while (CFile::GetStatus(candidate, status));
		target = candidate;
	}

	UpdateWindow();
	if (!::WriteWindowToDIB(target, this)) {
		AfxMessageBox(_T("Failed to save image"));
	}
}


////// utils.cpp or similar
//#include <wingdi.h>
//
//BOOL WriteWindowToDIB(LPCTSTR szFile, CWnd* pWnd)
//{
//	CDC* pDC = pWnd->GetDC();
//	CDC memDC;
//	memDC.CreateCompatibleDC(pDC);
//
//	CRect rect;
//	pWnd->GetClientRect(&rect);
//	CBitmap bitmap;
//	bitmap.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
//	CBitmap* pOldBitmap = memDC.SelectObject(&bitmap);
//
//	// Render into memory DC
//	pWnd->Print(&memDC, PRF_CLIENT | PRF_ERASEBKGND);
//
//	// Save to file
//	BOOL bSuccess = FALSE;
//	CImage image;
//	image.Attach((HBITMAP)bitmap.Detach());
//	bSuccess = SUCCEEDED(image.Save(szFile));
//	image.Detach();
//
//	memDC.SelectObject(pOldBitmap);
//	pWnd->ReleaseDC(pDC);
//	return bSuccess;
//}

void CGlut2DMapView::OnSetFocus(CWnd* pOldWnd)
{
	// Optionally prevent focus entirely
	return; // don't call base class
}