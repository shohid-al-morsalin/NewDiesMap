#include "stdafx.h"
#include "ImageIO.h"
#include "ImgPixel.h"

using namespace IMGL;

//////////////////////////////////////////////////////////////////////////
// pInt value ranges from 0 to 255 [7/3/2016 Yuen]

template<typename T>
void CImageIO::SetBuffer(int * pInt)
{
	int wd = GetWidth();
	int ht = GetHeight();

#pragma omp parallel for
	for (int y = 0; y < ht; y++)
	{
		T *p = (T *)GetPixelAddress(0, y);
		int *pI = pInt + y * wd;
		for (int x = 0; x < wd; x++)
		{
			if (*pI) p->Set(pI);
			p++; pI++;
		}
	}
}

template<typename T>
void CImageIO::SetBuffer(float * pflt)
{
	int wd = GetWidth();
	int ht = GetHeight();

#pragma omp parallel for
	for (int y = 0; y < ht; y++)
	{
		T *p = (T *)GetPixelAddress(0, y);
		float *pF = pflt + y * wd;
		for (int x = 0; x < wd; x++)
		{
			if (*pF)
			{
				int v = *pF;
				p->Set(&v);
			}
			p++; pF++;
		}
	}
}

template<typename T>
void IMGL::CImageIO::Copy(CImageIO & DiB, short scale)
{
	int wd = DiB.GetWidth();
	int ht = DiB.GetHeight();
	int iwd = DiB.GetWidth();
	int iht = DiB.GetHeight();

#pragma omp parallel for
	for (int y = 0; y < ht; y++)
	{
		T *des = (T *)GetPixelAddress(0, y);
		T *src = (T *)DiB.GetPixelAddress(0, y*scale);
		for (int x = 0; x < wd; x++)
		{
			*des = *src; des++; src += scale;
		}
	}
}

void IMGL::CImageIO::SetBuffer(int * pInt)
{
	switch (GetBYPP())
	{
	case 1:
		SetBuffer<sBINR>(pInt);
		break;
	case 3:
		SetBuffer<sBGR>(pInt);
		break;
	case 4:
		SetBuffer<sBGRA>(pInt);
		break;
	default:
		ASSERT(0);
		break;
	}
}

void IMGL::CImageIO::SetBuffer(float * pflt)
{
	switch (GetBYPP())
	{
	case 1:
		SetBuffer<sBINR>(pflt);
		break;
	case 3:
		SetBuffer<sBGR>(pflt);
		break;
	case 4:
		SetBuffer<sBGRA>(pflt);
		break;
	default:
		ASSERT(0);
		break;
	}
}

CImageIO::CImageIO()
{
	ptype = ptNOTSET;
}

CImageIO::~CImageIO()
{
}

void CImageIO::Serialize(CArchive &ar)
{
	int v;
	BYTE *p;
	int wd = 0, ht = 0, bpp = 0, pitch = 0;
	if (ar.IsStoring())
	{
		if (!IsNull())
		{
			wd = GetWidth();
			ar << wd;
			ht = GetHeight();
			ar << ht;
			bpp = GetBPP();
			ar << bpp;
			pitch = GetPitch();
			ar << pitch;
			if (pitch < 0)
			{
				p = (BYTE *)GetBits() + pitch * (ht - 1);
			}
			else
			{
				p = (BYTE *)GetBits();
			}
			ar << ptype;
			ar.Write(p, abs(pitch)*ht);
		}
		else
		{
			ar << wd;
			ar << ht;
			ar << bpp;
			ar << pitch;
			v = ptype;
			ar << v;
		}
	}
	else
	{
		if (!IsNull()) Destroy();

		ar >> wd;
		ar >> ht;
		ar >> bpp;
		ar >> pitch;
		ar >> v; ptype = (PIXELTYPE)v;
		if (bpp > 0)
		{
			Create(wd, ht, bpp); /*GetDim();*/
			if (IsNull())
			{
				ASSERT(0);  // Don't know what to do [1/27/2015 FSM]
				return;
			}
			if (pitch != GetPitch())
			{
				ASSERT(0);  // Don't know what to do [1/27/2015 FSM]
				return;
			}
			if (pitch < 0)
			{
				p = (BYTE *)GetBits() + pitch * (ht - 1);
			}
			else
			{
				p = (BYTE *)GetBits();
			}
			ar.Read(p, abs(pitch)*ht);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Image properties [4/4/2015 VIT#1]

int IMGL::CImageIO::GetBYPR()
{
	return abs(GetPitch());
}

int IMGL::CImageIO::GetBYPP()
{
	return GetBPP() / 8;
}

//////////////////////////////////////////////////////////////////////////
// Creation and destruction [4/4/2015 VIT#1]

BOOL IMGL::CImageIO::Copy(CImageIO & DiB)
{
	if (DiB.IsNull()) return FALSE;
	if (IsNull())
	{
		if (!Create(DiB.GetWidth(), DiB.GetHeight(), DiB.GetBPP())) return FALSE;
		ptype = DiB.ptype;
	}
	else if (!IsSameSize(DiB))
	{
		Destroy();
		if (!Create(DiB.GetWidth(), DiB.GetHeight(), DiB.GetBPP())) return FALSE;
		ptype = DiB.ptype;
	}
	if (IsNull()) return FALSE;

	memcpy((BYTE *)GetBufferStart(), (BYTE *)DiB.GetBufferStart(), DiB.GetBufferSize());
	return TRUE;
}

BOOL IMGL::CImageIO::Copy(CImageIO & DiB, short scale)
{
	if (DiB.IsNull()) return FALSE;
	short bypp = DiB.GetBYPP();
	int wd = DiB.GetWidth() / scale;
	int ht = DiB.GetHeight() / scale;
	if (IsNull())
	{
		if (!Create(wd, ht, GetBPP())) return FALSE;
		ptype = DiB.ptype;
	}
	else if (!IsSize(wd, ht))
	{
		Destroy();
		if (!Create(wd, ht, GetBPP())) return FALSE;
		ptype = DiB.ptype;
	}
	if (IsNull()) return FALSE;

	switch (bypp)
	{
	case 1:
		Copy<sBINR>(DiB, scale);
		break;
	case 3:
		Copy<sBGR>(DiB, scale);
		break;
	case 4:
		Copy<sBGRA>(DiB, scale);
		break;
	default:
		ASSERT(0);
		break;
	}
	return TRUE;
}

BOOL CImageIO::LoadL(CString fname)
{
	CImage::Destroy();
	if (SUCCEEDED(CImage::Load(fname)))
	{
		switch (GetBPP())
		{
		case 8:
			ptype = ptGREY;
			break;
		case 24:
			ptype = ptBGR;
			break;
		case 32:
			ptype = ptRGBA;
			break;
		}
		return TRUE;
	}
	return FALSE;
}

BOOL IMGL::CImageIO::CreateIf(int wd, int ht, PIXELTYPE type)
{
	return FALSE;
}

BOOL CImageIO::CreateIf(int wd, int ht, int bpp)
{
	if (IsNull())
	{
		if (Create(wd, ht, bpp))
		{
			return TRUE;
		}
		return FALSE;
	}
	if ((wd != GetWidth()) || (ht != GetHeight()))
	{
		Destroy();
		if (Create(wd, ht, bpp))
		{
			return TRUE;
		}
		return FALSE;
	}
	switch (bpp)
	{
	case 8: ptype = ptGREY; break;
	case 16: ptype = ptINT16; break;
	case 24: ptype = ptBGR; break;
	case 32: ptype = ptBGRA; break;
	}
	return TRUE;
}

BOOL CImageIO::IsSameSize(CImageIO &DiB)
{
	if (IsNull() || DiB.IsNull())
	{
		return FALSE;
	}
	if ((DiB.GetWidth() != GetWidth()) || (DiB.GetHeight() != GetHeight()) ||
		(DiB.GetBPP() != GetBPP()))
	{
		return FALSE;
	}
	return TRUE;
}

BOOL IMGL::CImageIO::IsSize(int wd, int ht)
{
	if ((GetWidth() != wd) || (GetHeight() != ht))
	{
		return FALSE;
	}
	return TRUE;
}

void IMGL::CImageIO::GetDimension(int & wd, int & ht)
{
	wd = GetWidth(); ht = GetHeight();
}

// Get beginning of image buffer [11/21/2015 Yuen]
// Caller must lock [1/24/2016 Yuen]
BYTE *CImageIO::GetBufferStart()
{
	if (IsNull()) return NULL;

	if (GetPitch() < 0)
	{
		return (PBYTE)GetPixelAddress(0, GetHeight() - 1);
	}
	return (PBYTE)GetPixelAddress(0, 0);
}

unsigned long CImageIO::GetImageSize()
{
	return(GetWidth() * GetHeight());
}

unsigned long CImageIO::GetBufferSize()
{
	return GetHeight() * abs(GetPitch());
}

void CImageIO::GetPixel(int x, int y, BYTE &r, BYTE &g, BYTE &b)
{
	switch (GetBPP())
	{
	case 8:
	{
		sBINR *p = (sBINR *)GetPixelAddress(x, y);
		p->Set(r, g, b);
		break;
	}
	case 24:
	{
		sBGR *p = (sBGR *)GetPixelAddress(x, y);
		p->Set(r, g, b);
		break;
	}
	case 32:
	{
		sBGRA *p = (sBGRA *)GetPixelAddress(x, y);
		p->Set(r, g, b);
		break;
	}
	}
}

void CImageIO::SetPixel(int x, int y, BYTE r, BYTE g, BYTE b)
{
	switch (GetBPP())
	{
	case 8:
	{
		sBINR *p = (sBINR *)GetPixelAddress(x, y);
		p->Set(r, g, b);
		break;
	}
	case 24:
	{
		sBGR *p = (sBGR *)GetPixelAddress(x, y);
		p->Set(r, g, b);
		break;
	}
	case 32:
	{
		sBGRA *p = (sBGRA *)GetPixelAddress(x, y);
		p->Set(r, g, b);
		break;
	}
	}
}

void CImageIO::SetPixel(int x, int y, BYTE v)
{
	switch (GetBPP())
	{
	case 8:
	{
		sBINR *p = (sBINR *)GetPixelAddress(x, y);
		p->Set(v, v, v);
		break;
	}
	case 24:
	{
		sBGR *p = (sBGR *)GetPixelAddress(x, y);
		p->Set(v, v, v);
		break;
	}
	case 32:
	{
		sBGRA *p = (sBGRA *)GetPixelAddress(x, y);
		p->Set(v, v, v);
		break;
	}
	}
}

// Operation on RGB monochrome image [4/4/2015 VIT#1]
void CImageIO::SetPixelRed(int x, int y, BYTE v)
{
	switch (GetBYPP())
	{
	case 1:
	{
		sBINR *p = (sBINR *)GetPixelAddress(x, y);
		p->Set(0, 192, 0);
		break;
	}
	case 3:
	{
		sBGR *p = (sBGR *)GetPixelAddress(x, y);
		p->Set(255, 0, 0);
		break;
	}
	case 4:
	{
		sBGRA *p = (sBGRA *)GetPixelAddress(x, y);
		p->Set(255, 0, 0);
		break;
	}
	}
}

// Caller must lock [1/24/2016 Yuen]
void CImageIO::_SetAllPixel(BYTE r, BYTE g, BYTE b)
{
	int wd = GetWidth();
	int ht = GetHeight();

	switch (GetBYPP())
	{
	case 1:
#pragma omp parallel for
		for (int y = 0; y < ht; y++)
		{
			sBINR *p = (sBINR *)GetPixelAddress(0, y);
			for (int x = 0; x < wd; x++)
			{
				p->Set(r, g, b); p++;
			}
		}
		break;
	case 3:
#pragma omp parallel for
		for (int y = 0; y < ht; y++)
		{
			sBGR *p = (sBGR *)GetPixelAddress(0, y);
			for (int x = 0; x < wd; x++)
			{
				p->Set(r, g, b); p++;
			}
		}
		break;
	case 4:
#pragma omp parallel for
		for (int y = 0; y < ht; y++)
		{
			sBGRA *p = (sBGRA *)GetPixelAddress(0, y);
			for (int x = 0; x < wd; x++)
			{
				p->Set(r, g, b); p++;
			}
		}
		break;
	}
}

short IMGL::CImageIO::GetPixelInt(int x, int y)
{
	switch (GetBPP())
	{
	case 8:
	{
		sGREY *p = (sGREY *)GetPixelAddress(x, y);
		return p->G;
		break;
	}
	case 24:
	{
		sBGR *p = (sBGR *)GetPixelAddress(x, y);
		return (p->B + p->G * 2 + p->R) / 4;
		break;
	}
	case 32:
	{
		sBGRA *p = (sBGRA *)GetPixelAddress(x, y);
		return (p->B + p->G * 2 + p->R) / 4;
		break;
	}
	default:
		ASSERT(0);
		break;
	}
	return 0;
}