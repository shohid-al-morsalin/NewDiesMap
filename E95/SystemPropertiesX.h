// SystemProperties.h: interface for the CSystemProperties class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SYSTEMPROPERTIES_H__FC422B21_B6FA_47FF_829A_C7ACE72E6C33__INCLUDED_)
#define AFX_SYSTEMPROPERTIES_H__FC422B21_B6FA_47FF_829A_C7ACE72E6C33__INCLUDED_

#include "PProfile.h"// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/**
 * System wide properties
 * aka global properties 
 */

#define SYSTEM_TM413	20

class CSystemProperties : public CObject  
{
private:
    short magic;
public:
//    enum { MPrj = 10, MPrjSim, TM413 = 20, TM413Sim, RTC2C = 30, RTC2CSim, TC900 = 40, TC900Sim };
//     /**
//          * Model of the hardware that the software support
//          *  10 : M Project
//          *  20 : 413
//          *  30 : 128L C2C
//          *  40 : TC900 Series
//          */
	short model;

    void Serialize(CArchive &ar);

	CSystemProperties();
	virtual ~CSystemProperties();

};

#endif // !defined(AFX_SYSTEMPROPERTIES_H__FC422B21_B6FA_47FF_829A_C7ACE72E6C33__INCLUDED_)
