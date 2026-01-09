#pragma once

#include "Line1D.h"
#include "ImageMeasure.h"

// Macros
#define NINT(f)   ((f >= 0) ? (int)(f + .5) : (int)(f - .5))
#define THRESH(d) ((d > 255) ? 255 : ((d < 0) ? 0 : d))
#define INRANGE() (m_image.minmax.min >= 0 && m_image.minmax.max <= 255)

// CImage is BGR so defs of red and blue are switched from GetXValue() defs
// #define RED(rgb)	(LOBYTE((rgb) >> 16))
// #define GRN(rgb)	(LOBYTE(((WORD)(rgb)) >> 8))
// #define BLU(rgb)	(LOBYTE(rgb))
// #define BGR(b,g,r)	RGB(b,g,r)
//#define BGR(r,g,b)	((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))
// #define YRGB(rgb)	(RED(rgb)*0.299 + GRN(rgb)*0.587 + BLU(rgb)*0.114)

#define LMOUSECLICK		WM_USER + 1001
#define LMOUSEDCLICK	WM_USER + 1002

namespace IMGL
{
	struct SRect_type
	{
		float left, right;
		float top, bottom;

		SRect_type()
		{
			left = right = -1;
			top = bottom = -1;
		}
		float Width()
		{
			return right - left;
		}
		float Height()
		{
			return bottom - top;
		}
	};

	struct SPoint_type
	{
		float px, py;		// Center of region [3/7/2015 Yuen]

		SPoint_type()
		{
			px = -1; py = -1;
		}
	};

	struct MinMax_type
	{						 // Image min..max (defined here for UpdateStatusbar()
		BOOL found;
		int min;
		int max;
	};

	struct Range_type
	{						 // Color min..max
		byte min;
		byte max;
	};

	struct Undo_type
	{						// Linked list of image buffers for undo
		int *p;				// Ptr to pixel buffer
		BOOL mod;			// Doc. modified status
		int ptype;			// Pixel type needed in case changed
		char hint[80];		// OnDo() caller's hint for OnUndo()
		Undo_type *next;	// Ptr to next node
	};

	class CImageL : public CImageMeasure
	{
		template<typename T> void Convert2Grey();
		template<typename T> BOOL Threshold(BYTE lvl);
		template<typename T> int AreaIntensity(float scale, int xst, int xed, int yst, int yed, BOOL bShow);
		template<typename T> int Histogram(CLine1D &Line, int xst, int xed, int yst, int yed);
		template<typename T> BOOL ReduceSize(CImageL &Src, short reduction);
		template<typename T> void Erosion(short K);
		template<typename T> void Dilation(short K);
		template<typename T> long AreaCount(BYTE thres);
		template<typename T> void SubOffset(short amount);

	public:
		template<class T> BOOL Scale(CImage16L<T> &Image);
		template<class T> BOOL Convert(CImage16L<T> &Image);

	public:
		//////////////////////////////////////////////////////////////////////////
		// Temporary variables [4/9/2016 Yuen]
		long t_sum;
		int t_cnt, t_int;
		int t_bright; // brightness
		CString t_message;
		//////////////////////////////////////////////////////////////////////////

		CCriticalSection CS1;
	public:
		MinMax_type minmax;		// Data range
		SPoint_type pos;		// Position of mouse click in  percentage form [3/6/2015 Yuen]
		enum ePT { ptROI, ptSRCH, ptIMG, ptTHM1, ptTHM2 };
		CRect rc[6];

		CLine1D Line;

		unsigned long histg[3][256];	// Histogram array
		unsigned long histg_neg;		// # negative values in histogram
		unsigned long histg_ovr;		// # > 255 in histogram

	public:
		//////////////////////////////////////////////////////////////////////////
		// IOs [3/11/2015 Yuen]
		void Dump();
		BOOL GetCG(float &x, float &y);
		//////////////////////////////////////////////////////////////////////////
		// Processing [3/11/2015 Yuen]
		// This function performs a 3 x 3 convolution on the active image.The
		// kernel array is passed externally.Edges are added(doubly weighted)
		// for the computation.
		void ConvlLowPass();
		void ConvlHighPass();
		void ConvlVSobel();
		void ConvlVSobelI();
		void ConvlHSobel();
		void ConvlHSobelI();
		void ConvlFreiChen();
		void ConvlVPrewitt();
		void ConvlHPrewitt();
		void Convl45Grad();
		void ConvlNGrad();
		void ConvlVShiftDiff();
		void ConvlLaplacian();
		void ConvlLaplacianSharp();
		void ConvlDiagLaplacian();
		void ConvlAvgDLaplacian();
		void ConvlCompLaplacian();
		void ConvlLineEnh();
		void ConvlEdgeDet();
		void ConvlEdgeEnh();
		void ConvlEdgeKirsh();
		void ConvlEmboss();
		void ConvlHEmboss();
		void ConvlVEmboss();
		//////////////////////////////////////////////////////////////////////////
		/*----------------------------------------------------------------------
		  This function performs a 3 x 3 convolution on the active image. The
		  kernel array is passed externally. Edges are added (doubly weighted)
		  for the computation. (Thanks to Frank Hoogterp and Steve Caito for the
		  original FORTRAN code).
		----------------------------------------------------------------------*/
		void Convl(
			float k1, float k2, float k3,
			float k4, float k5, float k6,
			float k7, float k8, float k9);
		// dilation: set to one if any 1 is present, zero otherwise
		void Dilate() { DilateErode(TRUE); }
		// erosion: set to zero if any 0 is present, one otherwise
		void Erode() { DilateErode(FALSE); }
		void Opening(short n, short nK);
		void Closing(short n, short nK);
		void DilateErode(BOOL bDilate);
		// Pixels operation [3/11/2015 Yuen]
		BOOL PixelDivision(CImageL &Image);
		BOOL fPixelDivision(CImageL &Image);
		BOOL AbsSubtract(CImageL &Image, BYTE Thresh, BOOL bRetain);// Absolute subtraction this -= Image [1/30/2015 FSM]
		BOOL AbsSubtract(CImageL &ImageDes, CImageL &ImageSrc, BYTE Thresh, BOOL bRetain);// Absolute subtraction this = ImageDes - ImageSc  [1/30/2015 FSM]
		BOOL Subtract(CImageL &Image1, CImageL &Image2, short Thresh, BOOL bRetain);// Simple subtraction [2/4/2015 Yuen]
		BOOL SubtractAdd(CImageL &Image1, BYTE val); // simple byte by byte image subtraction [1/30/2015 FSM]
		void Equalize() { /*Hist();*/ Eqliz(); }
		void Eqliz();
		BOOL Hist();
		void NrmlzRange(int ptype);
		BOOL Accumulate(CImageL &Image);
		void Sobel();
		//////////////////////////////////////////////////////////////////////////
		//  [7/13/2016 Yuen]
		//////////////////////////////////////////////////////////////////////////
		// Morphological operation [7/5/2016 Yuen]
		void Erosions(short K);
		void Dilations(short K);
		//////////////////////////////////////////////////////////////////////////
		// Conversions [3/11/2015 Yuen]
		float GetRelPosX(int pos);
		float GetRelPosY(int pos);
		SRect_type GetRelRect(CRect& rc);
		void GetBestRect(CRect& rc);
		//////////////////////////////////////////////////////////////////////////
		BOOL GetMinMax();
		BOOL ConvertToGrey();
		BOOL AutoBrightness();
		BOOL Convert24BitTopDown(BOOL bBGR = TRUE);
		BOOL Convert32BitTopDown(BOOL bBGR = TRUE);
		void Show(CDC *pDC, CRect &rc);
		BOOL GetArea(CImageL &Thumb, int sx, int sy, int wd, int ht);
		BOOL GetArea(CImageL &Thumb, CRect &rc); // Retrieve rc area as a CImageL [3/6/2015 Yuen]
		long GetAreaCount(CRect &rc, short thres);
		BOOL ReduceSize(CImageL &Image, short Reduction = 2);
		BOOL Overlay(CImageL &Mask);
		BOOL Overlay(CImageL &Image, CImageL &Mask);
		BOOL Threshold(BYTE level);
		BOOL ZeroMean();
		int AreaIntensity(float scale, float sx, float sy, BOOL bShow);
		int CalcHistogram(CLine1D &Line);
		int Histo(float His[], int nBin, float scale, float sx, float sy, BOOL bShow = FALSE);

	public:
		float GetAveIntensity();
		int GetPixelInt24(BYTE *p);
		void SetPixelIntGrey24(BYTE *p, BYTE n);
		void SetPixelInt24(BYTE *p, BYTE B, BYTE G, BYTE R);

	public:

		CImageL(void);
		virtual ~CImageL(void);

		CImageL &operator= (CImageL &Image);
		CImageL &operator-= (CImageL &Image); // simple byte by byte image subtraction [1/30/2015 FSM]
		CImageL &operator+= (CImageL &Image); // simple byte by byte image addition [1/30/2015 FSM]
		CImageL &operator- (CImageL &Image);
		CImageL &operator+ (CImageL &Image);
	};
}

/*
#define ChannelBlend_Normal(A,B)     ((uint8)(A))
#define ChannelBlend_Lighten(A,B)    ((uint8)((B > A) ? B:A))
#define ChannelBlend_Darken(A,B)     ((uint8)((B > A) ? A:B))
#define ChannelBlend_Multiply(A,B)   ((uint8)((A * B) / 255))
#define ChannelBlend_Average(A,B)    ((uint8)((A + B) / 2))
#define ChannelBlend_Add(A,B)        ((uint8)(min(255, (A + B))))
#define ChannelBlend_Subtract(A,B)   ((uint8)((A + B < 255) ? 0:(A + B - 255)))
#define ChannelBlend_Difference(A,B) ((uint8)(abs(A - B)))
#define ChannelBlend_Negation(A,B)   ((uint8)(255 - abs(255 - A - B)))
#define ChannelBlend_Screen(A,B)     ((uint8)(255 - (((255 - A) * (255 - B)) >> 8)))
#define ChannelBlend_Exclusion(A,B)  ((uint8)(A + B - 2 * A * B / 255))
#define ChannelBlend_Overlay(A,B)    ((uint8)((B < 128) ? (2 * A * B / 255):(255 - 2 * (255 - A) * (255 - B) / 255)))
#define ChannelBlend_SoftLight(A,B)  ((uint8)((B < 128)?(2*((A>>1)+64))*((float)B/255):(255-(2*(255-((A>>1)+64))*(float)(255-B)/255))))
#define ChannelBlend_HardLight(A,B)  (ChannelBlend_Overlay(B,A))
#define ChannelBlend_ColorDodge(A,B) ((uint8)((B == 255) ? B:min(255, ((A << 8 ) / (255 - B)))))
#define ChannelBlend_ColorBurn(A,B)  ((uint8)((B == 0) ? B:max(0, (255 - ((255 - A) << 8 ) / B))))
#define ChannelBlend_LinearDodge(A,B)(ChannelBlend_Add(A,B))
#define ChannelBlend_LinearBurn(A,B) (ChannelBlend_Subtract(A,B))
#define ChannelBlend_LinearLight(A,B)((uint8)(B < 128)?ChannelBlend_LinearBurn(A,(2 * B)):ChannelBlend_LinearDodge(A,(2 * (B - 128))))
#define ChannelBlend_VividLight(A,B) ((uint8)(B < 128)?ChannelBlend_ColorBurn(A,(2 * B)):ChannelBlend_ColorDodge(A,(2 * (B - 128))))
#define ChannelBlend_PinLight(A,B)   ((uint8)(B < 128)?ChannelBlend_Darken(A,(2 * B)):ChannelBlend_Lighten(A,(2 * (B - 128))))
#define ChannelBlend_HardMix(A,B)    ((uint8)((ChannelBlend_VividLight(A,B) < 128) ? 0:255))
#define ChannelBlend_Reflect(A,B)    ((uint8)((B == 255) ? B:min(255, (A * A / (255 - B)))))
#define ChannelBlend_Glow(A,B)       (ChannelBlend_Reflect(B,A))
#define ChannelBlend_Phoenix(A,B)    ((uint8)(min(A,B) - max(A,B) + 255))
#define ChannelBlend_Alpha(A,B,O)    ((uint8)(O * A + (1 - O) * B))
#define ChannelBlend_AlphaF(A,B,F,O) (ChannelBlend_Alpha(F(A,B),A,O))

ImageTColorR = ChannelBlend_Glow(ImageAColorR, ImageBColorR);
ImageTColorB = ChannelBlend_Glow(ImageAColorB, ImageBColorB);
ImageTColorG = ChannelBlend_Glow(ImageAColorG, ImageBColorG);

ImageTColor = RGB(ImageTColorR, ImageTColorB, ImageTColorG);

ImageTColorR = ChannelBlend_AlphaF(ImageAColorR, ImageBColorR, Blend_Subtract, 0.5F);

#define ColorBlend_Normal(T,A,B)        (ColorBlend_Buffer(T,A,B,Normal))
#define ColorBlend_Lighten(T,A,B)       (ColorBlend_Buffer(T,A,B,Lighten))
#define ColorBlend_Darken(T,A,B)        (ColorBlend_Buffer(T,A,B,Darken))
#define ColorBlend_Multiply(T,A,B)      (ColorBlend_Buffer(T,A,B,Multiply))
#define ColorBlend_Average(T,A,B)       (ColorBlend_Buffer(T,A,B,Average))
#define ColorBlend_Add(T,A,B)           (ColorBlend_Buffer(T,A,B,Add))
#define ColorBlend_Subtract(T,A,B)      (ColorBlend_Buffer(T,A,B,Subtract))
#define ColorBlend_Difference(T,A,B)    (ColorBlend_Buffer(T,A,B,Difference))
#define ColorBlend_Negation(T,A,B)      (ColorBlend_Buffer(T,A,B,Negation))
#define ColorBlend_Screen(T,A,B)        (ColorBlend_Buffer(T,A,B,Screen))
#define ColorBlend_Exclusion(T,A,B)     (ColorBlend_Buffer(T,A,B,Exclusion))
#define ColorBlend_Overlay(T,A,B)       (ColorBlend_Buffer(T,A,B,Overlay))
#define ColorBlend_SoftLight(T,A,B)     (ColorBlend_Buffer(T,A,B,SoftLight))
#define ColorBlend_HardLight(T,A,B)     (ColorBlend_Buffer(T,A,B,HardLight))
#define ColorBlend_ColorDodge(T,A,B)    (ColorBlend_Buffer(T,A,B,ColorDodge))
#define ColorBlend_ColorBurn(T,A,B)     (ColorBlend_Buffer(T,A,B,ColorBurn))
#define ColorBlend_LinearDodge(T,A,B)   (ColorBlend_Buffer(T,A,B,LinearDodge))
#define ColorBlend_LinearBurn(T,A,B)    (ColorBlend_Buffer(T,A,B,LinearBurn))
#define ColorBlend_LinearLight(T,A,B)   (ColorBlend_Buffer(T,A,B,LinearLight))
#define ColorBlend_VividLight(T,A,B)    (ColorBlend_Buffer(T,A,B,VividLight))
#define ColorBlend_PinLight(T,A,B)      (ColorBlend_Buffer(T,A,B,PinLight))
#define ColorBlend_HardMix(T,A,B)       (ColorBlend_Buffer(T,A,B,HardMix))
#define ColorBlend_Reflect(T,A,B)       (ColorBlend_Buffer(T,A,B,Reflect))
#define ColorBlend_Glow(T,A,B)          (ColorBlend_Buffer(T,A,B,Glow))
#define ColorBlend_Phoenix(T,A,B)       (ColorBlend_Buffer(T,A,B,Phoenix))

http://stackoverflow.com/questions/5919663/how-does-photoshop-blend-two-images-together

*/