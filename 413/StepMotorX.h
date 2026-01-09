// Motor.h: interface for the CStepMotor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STEPMOTOR_H__616E721A_5DF0_49E9_A8ED_DA24DDC3A1CA__INCLUDED_)
#define AFX_STEPMOTOR_H__616E721A_5DF0_49E9_A8ED_DA24DDC3A1CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CStepMotor
{
public:
	CStepMotor();
	virtual ~CStepMotor();

	virtual BOOL GetXYPosition(long *plX, long *plY) = NULL;
public:
	virtual int IsXYMoving() = NULL;
	virtual int SetXYSpeed(float fXSpeed, float fYSpeed) = NULL;
	virtual int MoveXY(long lXDistance, long lYDistance) = NULL;
	virtual int CruiseXY(float fXSpeed, float fYSpeed) = NULL;
	virtual int StopXY() = NULL;
	virtual int UpdateXYPosition() = NULL;
};

#endif // !defined(AFX_STEPMOTOR_H__616E721A_5DF0_49E9_A8ED_DA24DDC3A1CA__INCLUDED_)
