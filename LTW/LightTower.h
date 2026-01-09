// LightTower.h: interface for the CLightTower class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LIGHTTOWER_H__D433E1DB_CD7B_4B66_8514_AFDFABBC66AB__INCLUDED_)
#define AFX_LIGHTTOWER_H__D433E1DB_CD7B_4B66_8514_AFDFABBC66AB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <vector>

class CLightTower  
{
public:
	enum LIGHT
	{
		RED,
		GREEN,
		AMBER,
	};
	enum LIGHTSTATE
	{
		OFF,
		ON,
		BLINK
	};
	struct  LightData
	{
		LIGHT Light;
		LIGHTSTATE State;
	};
	CLightTower();
	virtual ~CLightTower();
	// Operations
	virtual BOOL GreenLight(LIGHTSTATE NewState, BOOL bExclusive = TRUE)= 0;
	virtual BOOL RedLight(LIGHTSTATE NewState, BOOL bExclusive= TRUE)= 0;
	virtual BOOL AmberLight(LIGHTSTATE NewState, BOOL bExclusive= TRUE)= 0;
	virtual BOOL AllLightsOff()= 0 ;
	// Attributes
	virtual LIGHTSTATE GreenState()= 0;
	virtual LIGHTSTATE RedState()= 0;
	virtual LIGHTSTATE AmberState()= 0;
	virtual BOOL AllLights(std::vector<LightData> &)= 0;
	// 
protected:
	BOOL StateString(LIGHTSTATE State,CString &StateStr);
	BOOL LightString(LIGHT Light,CString &Color);
};

extern CLightTower &LightTower;
#endif // !defined(AFX_LIGHTTOWER_H__D433E1DB_CD7B_4B66_8514_AFDFABBC66AB__INCLUDED_)
