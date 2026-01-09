#pragma once

#include <vector>
#include "../E95/Stat.h"
#include "define.h"
#include "PtLine.h"
#include <atlimage.h>

class CPackedDIB;

struct SLoc {
	int x = -1, y = -1; // location of edge on image [1/7/2020 yuenl]
	CString str();
	SLoc() {}
	SLoc(int xx, int yy) { x = xx; y = yy; }
};

struct SEdge {
	std::vector<float> width; // Width calculate from Inner and Outer [6/15/2020 yuenl]
	float px = 0, py = 0; // location on stage [1/7/2020 yuenl]
	std::vector<SLoc> Inner, Outer;

	void Clear();
	BOOL IsValid(int i);
	void DumpConst(FILE* fp);
	void DumpHeader(FILE* fp);
	void Dump(FILE* fp);
	float AveWidth();
	SEdge& operator=(SEdge& co) {
		width = co.width;
		px = co.px; py = co.py;
		Inner = co.Inner;
		Outer = co.Outer;
		return *this;
	}
};

class CIM : public CImage {
	//////////////////////////////////////////////////////////////////////////
	// Least square line info [2/1/2020 yuenl]
	BOOL bVertEdge = FALSE;
	//////////////////////////////////////////////////////////////////////////
	//int _wd = 0, _ht = 0, _bpp = 0;

public:
	float ang = 0, pos = 0;
	float distIn = 0, distOut = 0;
	CString filename = "";
	int Histo[MAXBINS] = { 0 };
	enum EDIR { LEFT, RIGHT, LAST };
	SEdge edge;
	//std::vector<SLoc> ledge, redge;
	SStat StatD; // statistic of distances [1/7/2020 yuenl]

public:
	float ScaleFactorX = 0.002731545f;
	float ScaleFactorY = 0.002774015f;

private:
	//////////////////////////////////////////////////////////////////////////
	// buffer access [11/3/2019 FSM]
	int GetBYPP();
	int GetBYPR();
	ULONG ImageSize();
	ULONG BufferSize();
	PVOID BufferStart();
	COLORREF GetPixel(int x, int y);
	bool SetPixel(int x, int y, COLORREF& cr);
	//////////////////////////////////////////////////////////////////////////
	// Buffer modification [11/3/2019 FSM]
	void AddDiv(CIM& Img);
	void BThres(short lvl);
	void Sub(CIM& Img);
	//////////////////////////////////////////////////////////////////////////
	// Helpers [11/5/2019 FSM]
	BOOL IsSameSize(CIM& DiB);
	ULONG GetBufferSize();
	//////////////////////////////////////////////////////////////////////////
public:
	BYTE* GetBufferStart();
	void DrawPolyFit();
	void DrawLSqFit(BOOL bOuter);
	BOOL DrawLSqFit2(std::vector<SLoc>& ledge, float Angle, float& a, float& b, BOOL& bXind);
	void RangeLEdge(std::vector<SLoc>& Inner, int& dx, int& dy, int* pX, int* pY);
	void LSQLEdge(std::vector<SLoc>& Inner, float& a, float& b);
	void StraightLine(int x0, int y0, int x, int y, BYTE r, BYTE g, BYTE b);
	//////////////////////////////////////////////////////////////////////////
public:
	CString GetEquationStr(float a, float b, BOOL bXind);
	bool ReduceSize(CIM& Image, short factor);
	int NEdge();
	CString Report();
	//void GetDimension();
	void GetDim(int& wd, int& ht, int& bpp);
	//////////////////////////////////////////////////////////////////////////
	bool SetBuffer(PBYTE pBuffer, int wd, int ht, int bpp);
	//////////////////////////////////////////////////////////////////////////
	// helpers [11/6/2019 FSM]
	void Cart2Polar(float fPx, float fPy, float& fPr, float& fPa);
	void DumpEdge(BYTE* pSrc, BYTE* pDes, short wd, short bpp, char* filename);
	void DumpEdge(char* filename, int* pX, int* pY, short n);
	void DumpPoints(std::vector<SLoc>& ledge, std::vector<SLoc>& redge, char* filename);
	void Polar2Cart(float fPr, float fPa, float& fPx, float& fPy);
	//////////////////////////////////////////////////////////////////////////
	// Buffer modification [11/3/2019 FSM]
	void SetPixel(int x, int y, BYTE r, BYTE g, BYTE b);
	//////////////////////////////////////////////////////////////////////////
	// Measure [11/3/2019 FSM]
	bool GetInnerEdge(std::vector<SLoc>& Outer, float& x, float& y);
	bool GetOuterEdge(std::vector<SLoc>& Inner, float& x, float& y);
	//////////////////////////////////////////////////////////////////////////
	void CalcStat();
	void DoStat(std::vector<float>& width);
	void AverStdev(UINT sz, int* pD, float& Ave, float& Stdev);
	void EliminateFlyers(std::vector<float>& width);
	void RemoveFlyer(UINT& sz, int* pX, int* pY, BOOL bXind, float& a, float& b);
	void Dump(UINT sz, int* pX, int* pY, char* Filename);
	//////////////////////////////////////////////////////////////////////////
	bool CalcHisto();
	int HistoMedian();
	int MinValue();
	bool IsInRect(int x, int y, int wd, int ht);
	int GetPx(float angle, int dy);
	int GetPy(float angle, int dx);
	void SweepXTop(std::vector<SLoc>& ledge, float angle, BYTE pix, BOOL bRight);
	void SweepXBottom(std::vector<SLoc>& ledge, float angle, BYTE pix, BOOL bRight);
	void SweepX(std::vector<SLoc>& ledge, float angle, BYTE pix, BOOL bRight);
	void SweepYLeft(std::vector<SLoc>& ledge, float angle, BYTE pix, BOOL bRight);
	void SweepYRight(std::vector<SLoc>& ledge, float angle, BYTE pix, BOOL bRight);
	void SweepY(std::vector<SLoc>& ledge, float angle, BYTE pix, BOOL bRight);
	CPtLine::Vec2 FindBand(float angle);
	BOOL FindEdge(std::vector<SLoc>& Inner, float angle, BYTE pix, BOOL bRight);
	bool FindEdgesDark(float angle, int cx, int cy);
	bool FindEdgesBright(float angle, int cx, int cy);
	bool FindEdgesOuter(SLoc& Outer, int cx, int cy, float ang, BYTE bg, BOOL bInv, BOOL bVert);
	bool FindEdgesInner(SLoc& Inner, int cx, int cy, float ang, BYTE bg, BOOL bInv, BOOL bVert);
	bool SweepToInnerY(SLoc& Inner, float ang, BYTE bg, int cx, int cy, BOOL bInv, BOOL bVert);
	bool SweepToInnerX(SLoc& Inner, float ang, BYTE bg, int cx, int cy, BOOL bInv, BOOL bVert);
	bool SweepToOuterX(SLoc& Outer, float ang, BYTE bg, int cx, int cy, BOOL bInv, BOOL bVert);
	bool SweepToOuterY(SLoc& Outer, float ang, BYTE bg, int cx, int cy, BOOL bInv, BOOL bVert);
	short Average(BYTE* src, short wdw, short bpp);
	short CountBlack(BYTE* src, short wdw, short bpp, short& lcnt, short& rcnt);
	short CountWhite(BYTE* src, short wdw, short bpp, short& lcnt, short& rcnt);
	//////////////////////////////////////////////////////////////////////////
	// Maths [2/1/2020 yuenl]
	bool Distance(float BevelWidth, float sfX, float sfY);
	bool Distance1(std::vector<SLoc>& Inner, float BevelWidth, float sfX, float sfY, CPtLine::Vec2& v, CPtLine::Vec2& w);
	CPtLine::Vec2 CTransform(CPtLine::Vec2 d, float ang);
	float GetDisplacement(std::vector<SLoc>& Inner, float ang, CPtLine::Vec2& p, CPtLine::Vec2& q);
	int NearestDistance0(std::vector<SLoc>& Inner, std::vector<SLoc>& Outer, int wdw, int nn);
	//////////////////////////////////////////////////////////////////////////
	// Convolution operation [1/5/2020 yuenl]
	void ConvlHSobel();
	void ConvlVSobel();
	bool Sobel();
	bool Magnitude(CIM& I1, CIM& I2);
	void Convl(float k1, float k2, float k3, float k4, float k5, float k6, float k7, float k8, float k9);
	//////////////////////////////////////////////////////////////////////////
	void ConvlLowPass();
	bool Smooth();
	//////////////////////////////////////////////////////////////////////////
	short HistoCG();
	void HistoMaxMin(int& Max, int& Min);
	void HistoMaxMin(int& Max, int& Min, float lmtpcl, float lmtpch);
	void SmoothLeft(std::vector<SLoc>& Inner, short wdw, short ntime, short bpp);
	//////////////////////////////////////////////////////////////////////////
	// Process [11/3/2019 FSM]
	bool BThreshold(BYTE Thres);
	bool Compare(CIM& Img);
	void ContractStretch();
	void ContractStretch(int Max, int Min);
	bool FillEdge();
	bool RemoveSpeckle(short wdw);
	bool RemoveSpeckle2(short wdw);
	//////////////////////////////////////////////////////////////////////////
	// Input output [11/2/2019 FSM]
	bool ExportHisto(char* filename);
	bool LoadPic(CString name, float& Ang);
	bool LoadPic(float& Ang);
	bool SaveEdges();
	bool SaveEdges(CString name);
	bool SaveLEdges();
	bool SaveLEdges(CString name);
	bool SavePic();
	bool SavePicture(float r, float a);
	CString GetProgramDir();
	//////////////////////////////////////////////////////////////////////////
	// Display [11/3/2019 FSM]
	void CopyColorMap(CIM& Image);
	void BestRect(CRect& rc);
	//////////////////////////////////////////////////////////////////////////
	// For CPackedDIB [08202024 ZHIMING]
	void Attach(CPackedDIB* pDIB);

public:
	CIM();
	~CIM();
	CIM& operator= (CIM& Image);
	CIM& operator/ (CIM& Image);
};
