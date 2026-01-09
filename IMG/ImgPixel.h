#pragma once

// Pixel format [7/3/2016 Yuen]
#define pfGREY 8
#define pfBGR 24
#define pfBGRA 32

#define BACKGRND 255
#define MARKED 2
#define FOREGRND 0
#define CLIP(v) ((v) >= 0 ? ((v) < 256 ? (v) : 255) : 0)

namespace IMGL
{
	struct sPIXCL
	{
		virtual BYTE GetIntensity() = 0;

		virtual BYTE Get(BYTE &r, BYTE &g, BYTE &b) = 0;
		virtual void Set(BYTE r, BYTE g, BYTE b) = 0;
		virtual void Set(int *p) = 0;
		virtual void SetBGrnd() = 0;
		virtual void SetFGrnd() = 0;
		virtual void SetBinr(BYTE v) = 0;
		virtual void SetGrey(BYTE v) = 0;
		virtual void MakeGrey() = 0;
		virtual BOOL IsFGrnd() = 0;
		virtual BOOL CanMark() = 0;
		virtual void Mark(short counter) = 0;
		virtual BOOL IsMarked() = 0;
		virtual void Threshold(BYTE v) = 0;
		virtual BYTE MaxRGB() = 0;
		virtual BYTE MinRGB() = 0;
		virtual void HisAdd(int hisR[], int hisG[], int hisB[]) = 0;
		virtual void HisEqualize(int hisR[], int hisG[], int hisB[]) = 0;
		virtual BYTE HsL() = 0;
		virtual void Add(BYTE amount) = 0;
		virtual void SubOff(short amount) = 0;
		virtual BYTE GetSetPixel(BYTE r, BYTE g, BYTE b) = 0;
	};

	struct sBINR /*: public sPIXCL*/
	{	// 8 bits binary image
		BYTE G;
	public:
		void SubOff(short amount)
		{
			short v = (G - amount) + 127;
			G = CLIP(v);
		}
		void Add(BYTE amount)
		{
			short v = G + amount;
			G = CLIP(v);
		}
		BYTE GetSetPixel(BYTE r, BYTE g, BYTE b)
		{
			BYTE v = G;
			G = g;
			return v;
		}
		BYTE HsL()
		{
			return G;
		}
		void Ave(sBINR *p, BYTE BG)
		{
			G = (p->G + BG) / 2;
		}
		void HisAdd(int hisR[], int hisG[], int hisB[])
		{
			hisG[G]++;
		}
		void HisEqualize(int hisR[], int hisG[], int hisB[])
		{
			int v = hisG[G];
			G = (v < 1) ? 0 : ((v > 255) ? 255 : v);
		}
		BYTE MinRGB()
		{
			return G;
		}
		BYTE MaxRGB()
		{
			return G;
		}
		BYTE Get(BYTE &r, BYTE &g, BYTE &b)
		{
			r = G; g = G; b = G;
			return G;
		}
		void Set(BYTE r, BYTE g, BYTE b)
		{
			short v = (r + 2 * g + b) / 4;
			G = (v < 1) ? 0 : 255;
		}
		void Set(int *pI)
		{
			G = (*pI == 0) ? 0 : 255;
		}
		void Set(sBINR *p)
		{
			G = p->G;
		}
		BYTE GetIntensity()
		{
			return G;
		}
		void SetBGrnd()
		{
			G = BACKGRND;
		}
		void SetFGrnd()
		{
			G = FOREGRND;
		}
		// for black object [7/7/2016 Yuen]
		void Threshold(BYTE lv)
		{
			if (G < lv) G = FOREGRND;
			else G = BACKGRND;
		}
		BOOL IsFGrnd()
		{	// Edge is in foreground color [7/3/2016 Yuen]
			return (G == FOREGRND);
		}
		void Mark(short counter)
		{
			G = MARKED;
		}
		BOOL CanMark()
		{
			if (G == FOREGRND)
			{
				return TRUE;
			}
			return FALSE;
		}
		BOOL IsMarked()
		{
			return (G == MARKED ? TRUE : FALSE);
		}
		void SetRGB(BYTE r, BYTE g, BYTE b)
		{
			G = g;
		}
		void SetRed()
		{
			G = 192;
		}
		void SetBlue()
		{
			G = 128;
		}
		void SetLightBlue()
		{
			G = 64;
		}
		void SetYellow()
		{
			G = 32;
		}
		void SetGrey(BYTE lv)
		{
			G = (lv == 0) ? FOREGRND : BACKGRND;
		}
		void SetBinr(BYTE lv)
		{
			G = (lv == 0) ? FOREGRND : BACKGRND;
		}
		sBINR operator=(sBINR &co)
		{
			G = co.G;
			return *this;
		}
	};

	struct sGREY /*: public sPIXCL*/
	{ // 8 bits grey image
		BYTE G;
	public:
		void SubOff(short amount)
		{
			short v = (G - amount) + 127;
			G = CLIP(v);
		}
		void Add(BYTE amount)
		{
			short v = G + amount;
			G = CLIP(v);
		}
		BYTE GetSetPixel(BYTE r, BYTE g, BYTE b)
		{
			BYTE v = G;
			G = g;
			return v;
		}
		BYTE HsL()
		{
			return G;
		}
		void Ave(sGREY *p, BYTE BG)
		{
			G = (p->G + BG) / 2;
		}
		void HisAdd(int hisR[], int hisG[], int hisB[])
		{
			hisG[G]++;
		}
		void HisEqualize(int hisR[], int hisG[], int hisB[])
		{
			int v = hisG[G];
			G = v < 0 ? 0 : (v > 255 ? 255 : v);
		}
		BYTE MinRGB()
		{
			return G;
		}
		BYTE MaxRGB()
		{
			return G;
		}
		BYTE Get(BYTE &r, BYTE &g, BYTE &b)
		{
			r = G; g = G; b = G;
			return G;
		}
		void Set(BYTE r, BYTE g, BYTE b)
		{
			int v = (r + 2 * g + b) / 4;
			G = (v < 1) ? 0 : 255;
		}
		void Set(int *pI)
		{
			G = (*pI == 0) ? 0 : 255;
		}
		void Set(sGREY *p)
		{
			G = p->G;
		}
		BYTE GetIntensity()
		{
			return G;
		}
		void SetBGrnd()
		{
			G = BACKGRND;
		}
		void SetFGrnd()
		{
			G = FOREGRND;
		}
		// for black object [7/7/2016 Yuen]
		void Threshold(BYTE lv)
		{
			if (G < lv) G = FOREGRND;
			else G = BACKGRND;
		}
		BOOL IsFGrnd()
		{	// Edge is in foreground color [7/3/2016 Yuen]
			return (G == FOREGRND);
		}
		void Mark(short counter)
		{
			G = MARKED;
		}
		BOOL CanMark()
		{
			if (G == FOREGRND)
			{
				return TRUE;
			}
			return FALSE;
		}
		BOOL IsMarked()
		{
			return (G == MARKED ? TRUE : FALSE);
		}
		void SetRGB(BYTE r, BYTE g, BYTE b)
		{
			G = g;
		}
		void SetRed()
		{
			G = 192;
		}
		void SetBlue()
		{
			G = 128;
		}
		void SetLightBlue()
		{
			G = 64;
		}
		void SetYellow()
		{
			G = 32;
		}
		void SetGrey(BYTE lv)
		{
			G = lv;
		}
		void SetBinr(BYTE lv)
		{
			G = (lv == 0) ? FOREGRND : BACKGRND;
		}
		sGREY operator=(sGREY &co)
		{
			G = co.G;
			return *this;
		}
	};

	struct sBGR /*: public sPIXCL*/
	{
		BYTE B, G, R;
	public:
		void SubOff(short amount)
		{
			short v;
			v = (R - amount) + 127;
			R = CLIP(v);
			v = (G - amount) + 127;
			G = CLIP(v);
			v = (B - amount) + 127;
			B = CLIP(v);
		}
		void Add(BYTE amount)
		{
			short v;
			v = R + amount;
			R = CLIP(v);
			v = G + amount;
			G = CLIP(v);
			v = B + amount;
			B = CLIP(v);
		}
		BYTE GetSetPixel(BYTE r, BYTE g, BYTE b)
		{
			BYTE vR = R;
			BYTE vG = G;
			BYTE vB = B;
			R = r;
			G = g;
			B = b;
			return (vR + 2 * vG + vB) / 4;
		}
		BYTE HsL()
		{
			float fR = R / 255.0f;
			float fG = G / 255.0f;
			float fB = B / 255.0f;
			float max, min;
			max = min = fR;
			if (fG > max) max = fG;
			if (fB > max) max = fB;
			if (fG < min) min = fG;
			if (fB < min) min = fB;
			return 255 * (max + min) / 2.0f;
		}
		void Ave(sBGR *p, BYTE BG)
		{
			B = (p->B + BG) / 2;
			G = (p->G + BG) / 2;
			R = (p->R + BG) / 2;
		}
		void HisAdd(int hisR[], int hisG[], int hisB[])
		{
			hisR[R]++;
			hisG[G]++;
			hisB[B]++;
		}
		void HisEqualize(int hisR[], int hisG[], int hisB[])
		{
			int v = hisR[R];
			R = v < 1 ? 0 : v>255 ? 255 : v;
			v = hisG[G];
			G = v < 1 ? 0 : v>255 ? 255 : v;
			v = hisB[B];
			B = v < 1 ? 0 : v>255 ? 255 : v;
		}
		BYTE MinRGB()
		{
			if (R <= B)
			{
				if (R <= G) return R;
			}
			else
			{
				if (B <= G) return B;
			}
			return G;
		}
		BYTE MaxRGB()
		{
			if (R >= B)
			{
				if (R >= G) return R;
			}
			else
			{
				if (B > G) return B;
			}
			return G;
		}
		BYTE Get(BYTE &r, BYTE &g, BYTE &b)
		{
			r = R; g = G; b = B;
			return (R + 2 * G + B) / 4;
		}
		void Set(BYTE r, BYTE g, BYTE b)
		{
			B = b; G = g; R = r;
		}
		void Set(int *pI)
		{
			int v = *pI;
			B = G = R = (v < 1) ? 0 : (v > 255 ? 255 : v);
		}
		void Set(sBGR *p)
		{
			R = p->R;
			G = p->G;
			B = p->B;
		}
		BYTE GetIntensity()
		{
			return (B + 2 * G + R) / 4;
		}
		void SetBGrnd()
		{
			R = G = B = BACKGRND;
		}
		void SetFGrnd()
		{
			R = G = B = FOREGRND;
		}
		// for black object [7/7/2016 Yuen]
		void Threshold(BYTE lv)
		{
			if (((R + 2 * G + B) / 4) > lv) R = G = B = BACKGRND;
			else R = G = B = FOREGRND;
		}
		BOOL IsFGrnd()
		{	// Edge is in foreground color [7/3/2016 Yuen]
			return ((G == FOREGRND) && (R == FOREGRND) && (B == FOREGRND));
		}
		void Mark(short counter)
		{
			G = MARKED;
			switch (counter % 8)
			{
			case 0:
				B = 127 - counter / 128;
				R = counter % 128;
			case 1:
				R = 127 - counter / 128;
				B = counter % 128;
				break;
			case 2:
				B = 127 + counter / 128;
				R = counter % 128;
				break;
			case 3:
				R = 127 + counter / 128;
				B = counter % 128;
			case 4:
				B = 127 - counter / 128;
				R = 127 + counter % 128;
				break;
			case 5:
				R = 255 - counter / 128;
				B = 127 + counter % 128;
				break;
			case 6:
				B = 127 - counter / 128;
				R = 127 + counter % 128;
				break;
			default:
				R = 255 - counter / 128;
				B = 127 + counter % 128;
				break;
			}
		}
		BOOL CanMark()
		{
			if (G == FOREGRND)
			{
				return TRUE;
			}
			return FALSE;
		}
		BOOL IsMarked()
		{
			return (G == MARKED ? TRUE : FALSE);
		}
		void SetRGB(BYTE r, BYTE g, BYTE b)
		{
			R = r; G = g; B = b;
		}
		void SetRed()
		{
			R = 255;
			B = 0;
		}
		void SetBlue()
		{
			B = 255;
			R = 0;
		}
		void SetLightBlue()
		{
			B = 192;
			R = 0;
		}
		void SetYellow()
		{
			B = 255; R = 255;
		}
		void SetGrey(BYTE lv)
		{
			B = G = R = lv;
		}
		void SetBinr(BYTE lv)
		{
			B = G = R = (lv == 0) ? FOREGRND : BACKGRND;
		}
		void MakeGrey()
		{
			short v = (R + 2 * G + B) / 4;
			R = G = B = v;
		}
		sBGR operator=(sBGR &co)
		{
			R = co.R;
			G = co.G;
			B = co.B;
			return *this;
		}
	};

	struct sBGRA /*: public sPIXCL*/
	{
		BYTE B, G, R, A;
	public:
		void SubOff(short amount)
		{
			short v;
			v = (R - amount) + 127;
			R = CLIP(v);
			v = (G - amount) + 127;
			G = CLIP(v);
			v = (B - amount) + 127;
			B = CLIP(v);
		}
		BYTE GetSetPixel(BYTE r, BYTE g, BYTE b)
		{
			BYTE vR = R;
			BYTE vG = G;
			BYTE vB = B;
			R = r;
			G = g;
			B = b;
			return (vR + 2 * vG + vB) / 4;
		}
		BYTE HsL()
		{
			float fR = R / 255.0f;
			float fG = G / 255.0f;
			float fB = B / 255.0f;
			float max, min;
			max = min = fR;
			if (fG > max) max = fG;
			if (fB > max) max = fB;
			if (fG < min) min = fG;
			if (fB < min) min = fB;
			return 255 * (max + min) / 2.0f;
		}
		void Ave(sBGRA *p, BYTE BG)
		{
			B = (p->B + BG) / 2;
			G = (p->G + BG) / 2;
			R = (p->R + BG) / 2;
		}
		void HisAdd(int hisR[], int hisG[], int hisB[])
		{
			hisR[R]++;
			hisG[G]++;
			hisB[B]++;
		}
		void HisEqualize(int hisR[], int hisG[], int hisB[])
		{
			int v = hisR[R];
			R = v < 0 ? 0 : v>255 ? 255 : v;
			v = hisG[G];
			G = v < 0 ? 0 : v>255 ? 255 : v;
			v = hisB[B];
			B = v < 0 ? 0 : v>255 ? 255 : v;
		}
		BYTE MinRGB()
		{
			if (R <= B)
			{
				if (R <= G) return R;
			}
			else
			{
				if (B <= G) return B;
			}
			return G;
		}
		BYTE MaxRGB()
		{
			if (R >= B)
			{
				if (R >= G) return R;
			}
			else
			{
				if (B > G) return B;
			}
			return G;
		}
		BYTE Get(BYTE &r, BYTE &g, BYTE &b)
		{
			r = R; g = G; b = B;
			return (R + 2 * G + B) / 4;
		}
		void Set(BYTE r, BYTE g, BYTE b)
		{
			B = b; G = g; R = r;
		}
		void Set(int *pI)
		{
			short v = *pI;
			B = G = R = (v < 1) ? 0 : (v > 255 ? 255 : v);
		}
		void Set(sBGRA *p)
		{
			R = p->R;
			G = p->G;
			B = p->B;
		}
		BYTE GetIntensity()
		{
			return (B + 2 * G + R) / 4;
		}
		void SetBGrnd()
		{
			R = G = B = BACKGRND;
		}
		void SetFGrnd()
		{
			R = G = B = FOREGRND;
		}
		BOOL IsFGrnd()
		{	// Edge is in foreground color [7/3/2016 Yuen]
			return ((G == FOREGRND) && (R == FOREGRND) && (B == FOREGRND));
		}
		void Mark(short counter)
		{
			G = MARKED;
			switch (counter % 8)
			{
			case 0:
				B = 127 - counter / 128;
				R = counter % 128;
			case 1:
				R = 127 - counter / 128;
				B = counter % 128;
				break;
			case 2:
				B = 127 + counter / 128;
				R = counter % 128;
				break;
			case 3:
				R = 127 + counter / 128;
				B = counter % 128;
			case 4:
				B = 127 - counter / 128;
				R = 127 + counter % 128;
				break;
			case 5:
				R = 255 - counter / 128;
				B = 127 + counter % 128;
				break;
			case 6:
				B = 127 - counter / 128;
				R = 127 + counter % 128;
				break;
			default:
				R = 255 - counter / 128;
				B = 127 + counter % 128;
				break;
			}
		}
		BOOL CanMark()
		{
			if (G == FOREGRND)
			{
				return TRUE;
			}
			return FALSE;
		}
		BOOL IsMarked()
		{
			return (G == MARKED ? TRUE : FALSE);
		}
		// for black object [7/7/2016 Yuen]
		void Threshold(BYTE lv)
		{
			if (((R + 2 * G + B) / 4) > lv) R = G = B = BACKGRND;
			else R = G = B = FOREGRND;
		}
		void SetRGB(BYTE r, BYTE g, BYTE b)
		{
			R = r; G = g; B = b;
		}
		void SetRed()
		{
			R = 255;
			B = 0;
		}
		void SetBlue()
		{
			R = 0;
			B = 255;
		}
		void SetLightBlue()
		{
			B = 192;
			R = 0;
		}
		void SetYellow()
		{
			B = 255; R = 255;
		}
		void SetGrey(BYTE lv)
		{
			B = G = R = lv;
		}
		void SetBinr(BYTE lv)
		{
			B = G = R = (lv == 0) ? FOREGRND : BACKGRND;
		}
		void MakeGrey()
		{
			short v = (R + 2 * G + B) / 4;
			R = G = B = v;
		}
		sBGRA operator=(sBGRA &co)
		{
			R = co.R;
			G = co.G;
			B = co.B;
			return *this;
		}
	};
}