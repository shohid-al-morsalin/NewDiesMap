#include "stdafx.h"
#include "Image16L.h"

using namespace IMGL;

template<class T>
short CImage16L<T>::GetBYPP()
{
	return sizeof(T);
}
template<class T>
BOOL CImage16L<T>::IsNull()
{
	return (ptBits == NULL) ? TRUE : FALSE;
}
template<class T>
void IMGL::CImage16L<T>::Preset()
{
	tMax = tMin = 0;
}
template<class T>
BOOL CImage16L<T>::IsSameSize(CRect rc)
{
	if (rc.Width() != Width) return FALSE;
	if (rc.Height() != Height) return FALSE;
	return TRUE;
}
template<class T>
BOOL CImage16L<T>::IsSameSize(int wd, int ht)
{
	if (wd != Width) return FALSE;
	if (ht != Height) return FALSE;
	return TRUE;
}

template<class T>
BOOL CImage16L<T>::IsSameSize(CImage16L<T>& DiB)
{
	if (IsNull() || DiB.IsNull())
	{
		return FALSE;
	}
	if ((DiB.GetWidth() != GetWidth()) || (DiB.GetHeight() != GetHeight()))
	{
		return FALSE;
	}
	if (ptype != DiB.ptype)
	{
		return FALSE;
	}
	return TRUE;
}

template<class T>
int CImage16L<T>::GetWidth()
{
	return Width;
}

template<class T>
int CImage16L<T>::GetHeight()
{
	return Height;
}

template<class T>
BOOL CImage16L<T>::Create(int wd, int ht)
{
	if (ptBits)
	{
		if ((wd != Width) || (ht != Height))
		{
			delete[] ptBits; ptBits = NULL;
		}
	}

	if (!ptBits) ptBits = new T[wd*ht];

	if (ptBits)
	{
		memset(ptBits, 0, wd*ht * sizeof(T));
		Width = wd; Height = ht;
		ptype = ptFLOAT;
		return TRUE;
	}
	return FALSE;
}

template<class T>
void CImage16L<T>::Destroy()
{
	if (ptBits)
	{
		delete[] ptBits; ptBits = NULL;
		Width = 0; Height = 0; ptype = ptNOTSET;
	}
}

template<class T>
void CImage16L<T>::GetMaxMin(T & min, T & max)
{
	min = max = *ptBits;

#pragma omp parallel for
	for (int y = 0; y < Height; y++)
	{
		T *p = ptBits + y * Width;
		for (int x = 0; x < Width; x++)
		{
			if (*p < min)
			{
				min = *p;
			}
			if (*p > max)
			{
				max = *p;
			}
			p++;
		}
	}
	tMax = max;
	tMin = min;
}

template<class T>
T * CImage16L<T>::GetPixel(int x, int y)
{
	if (!ptBits) return FALSE;
	if ((x < 0) || (y < 0)) return FALSE;
	if ((x >= Width) || (y >= Height)) return FALSE;
	return ptBits + y * Width + x;
}

template<class T>
T * CImage16L<T>::GetBitsStart()
{
	return ptBits;
}

template<class T>
T * CImage16L<T>::GetPixelAddress(int x, int y)
{
	if ((x < Width) && (y < Height))
	{
		return (ptBits + y * Width + x);
	}
	return NULL;
}

// For speed consideration, ptBits assumed to be valid and x and y within image range [4/2/2015 VIT#1]
template<class T>
BOOL CImage16L<T>::SetPixel(int x, int y, T v)
{
	if (v > tMax) tMax = v;
	if (v < tMin) tMin = v;
	*(ptBits + y * Width + x) = v;
	return TRUE;
}

// Dump one horizontal line [11/9/2014 Yuen]
template<class T>
void CImage16L<T>::Dump(int y)
{
	CString str;
	str.Format("C:\\TEMP\\IMAGE16-%d.CSV", y);

	FILE *fp;
	fp = fopen(str.GetBuffer(0), "wb");
	if (fp)
	{
		T *p = ptBits + y * Width;
		for (int i = 0; i < Width; i++)
		{
			fprintf(fp, "%d,%.4f\n", i, (double)*(p++));
		}
		fprintf(fp, "\n");
		fclose(fp);
	}
}

// Save simple float pixel bitmap [3/13/2015 VIT]
template<class T>
BOOL CImage16L<T>::Save(CString fname)
{
	CFile theFile;
	if (theFile.Open(fname, CFile::modeWrite | CFile::modeCreate))
	{
		CArchive archive(&theFile, CArchive::store);
		Serialize(archive);
		archive.Close();
		theFile.Close();
		return TRUE;
	}
	return FALSE;
}

// Load simple float pixel bitmap [3/13/2015 VIT]
template<class T>
BOOL CImage16L<T>::Load(CString fname)
{
	fname += ".IMF";
	CFile theFile;
	if (theFile.Open(fname, CFile::modeRead))
	{
		CArchive archive(&theFile, CArchive::load);
		Serialize(archive);
		archive.Close();
		theFile.Close();
		return TRUE;
	}
	return FALSE;
}

template<class T>
BOOL CImage16L<T>::GetScore(T max, int & c1, int & c2)
{
	if (!ptBits) return FALSE;
	T *p = ptBits;
	int i;
	T max2 = max * 0.98;
	c1 = 0; c2 = 0;
	int size = Width * Height;
	for (i = 0; i < size; i++)
	{
		if (*p >= max) c1++;
		if (*p >= max2) c2++;
		p++;
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
template<class T>
CImage16L<T>::CImage16L()
{
	ptBits = NULL;
	//tMax = -1;
	//tMin = FLT_MAX;
	Width = 0; Height = 0;
	ptype = ptNOTSET;
}
template<class T>
CImage16L<T>::CImage16L(int wd, int ht)
{
	ptBits = NULL;
	//tMax = -1;
	//tMin = FLT_MAX;
	Width = 0; Height = 0;
	ptype = ptNOTSET;
	Create(wd, ht);
}
template<class T>
CImage16L<T>::~CImage16L()
{
	Destroy();
}
template<class T>
CImage16L<T> &CImage16L<T>::operator=(CImage16L<T> &Img)
{
	if (Img.ptBits)
	{
		tMax = Img.tMax;
		tMin = Img.tMin;
		ptype = Img.ptype;
		if (ptBits)
		{
			if ((Width != Img.Width) && (Height != Img.Height)) delete[] ptBits;
			Width = Img.Width;
			Height = Img.Height;
			ptBits = new T[Width*Height];
		}
		else
		{
			Width = Img.Width;
			Height = Img.Height;
			ptBits = new T[Width*Height];
		}
		memcpy(ptBits, Img.ptBits, Width*Height * sizeof(T));
	}
	return *this;
}
template<class T>
void CImage16L<T>::Serialize(CArchive & ar)
{
	short magic;
	long size = Width * Height;
	if (ar.IsStoring())
	{
		magic = 2;
		ar << magic;
		ar << Width;
		ar << Height;
		ar << tMax;
		ar << tMin;
		ar << size;
		ar << (int)ptype;
		ar.Write(ptBits, size * sizeof(T));
	}
	else
	{
		if (ptBits)
		{
			delete[] ptBits; ptBits = NULL;
		}
		ar >> magic;
		ar >> Width;
		ar >> Height;
		ar >> tMax;
		ar >> tMin;
		ar >> size;
		if (magic > 1)
		{
			int v;
			ar >> v;
			ptype = (PIXELTYPE)v;
		}
		ptBits = new T[size];
		if (ptBits)
		{
			ar.Read(ptBits, size * sizeof(T));
		}
	}
}

// Below is a must to explicitly instantiate the codes for the template [7/16/2016 Yuen]
template class CImage16L<int>;
template class CImage16L<float>;