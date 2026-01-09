// LightTowerMC.h: interface for the CLightTowerMC class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LIGHTTOWERMC_H__456DC2AA_999B_4E12_96ED_39485FC442DA__INCLUDED_)
#define AFX_LIGHTTOWERMC_H__456DC2AA_999B_4E12_96ED_39485FC442DA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "LightTower.h"
/*
	Never create an object of this class
	1) Only one object is required to be created that will be done automatically
	2) Use the pointer in CLightTower. It will have correct implementation

*/
class CLightTowerMC : public CLightTower  
{
	LightData Red,Amber,Green;
	void Log(const CString &Msg);
	BOOL CanDoIt();
public:
	CLightTowerMC();
	virtual ~CLightTowerMC();
	virtual BOOL GreenLight(LIGHTSTATE NewState, BOOL bExclusive= TRUE);
	virtual BOOL RedLight(LIGHTSTATE NewState, BOOL bExclusive= TRUE);
	virtual BOOL AmberLight(LIGHTSTATE NewState, BOOL bExclusive= TRUE);
	virtual BOOL AllLightsOff() ;
	virtual LIGHTSTATE GreenState();
	virtual LIGHTSTATE RedState();
	virtual LIGHTSTATE AmberState();
	virtual BOOL AllLights(std::vector<LightData> &);
};

#endif // !defined(AFX_LIGHTTOWERMC_H__456DC2AA_999B_4E12_96ED_39485FC442DA__INCLUDED_)
