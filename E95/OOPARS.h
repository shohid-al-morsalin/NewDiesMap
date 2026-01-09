// OOPARS.h: interface for the OOPARS class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OOPARS_H__9733F750_7F2A_43E6_A9C1_14957E34D319__INCLUDED_)
#define AFX_OOPARS_H__9733F750_7F2A_43E6_A9C1_14957E34D319__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class OOPARS  
{
public:
	void SaveParam();
	void LoadParam();
	float fThkFactor;
	float fThkMin;
	float fThkMax;
	float fPeakMin;
	int nMethod;
	float fCalCoeff;  // Scaling factor for reflectivity correction [7/20/2011 XPMUser]
	float fRefrIndex;
	int boxcar;
	int nMeastime;
	int nPort;
	int nRepeat;

	OOPARS();
	virtual ~OOPARS();

};

#endif // !defined(AFX_OOPARS_H__9733F750_7F2A_43E6_A9C1_14957E34D319__INCLUDED_)
