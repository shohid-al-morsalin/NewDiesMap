// DataDesc.h: interface for the CDataDesc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DATADESC_H__44008770_72F2_4450_B9B8_262CE5C94696__INCLUDED_)
#define AFX_DATADESC_H__44008770_72F2_4450_B9B8_262CE5C94696__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// 16 x 9 entries [7/3/2012 user]
#define DESCENTRY	144

class CDataDesc  
{
public:
	short GetColumnIndex(short col);
	short GetColumnCount();
	CString desc[DESCENTRY];
	BOOL dsp[DESCENTRY];
	CDataDesc();
	virtual ~CDataDesc();

};

#endif // !defined(AFX_DATADESC_H__44008770_72F2_4450_B9B8_262CE5C94696__INCLUDED_)
