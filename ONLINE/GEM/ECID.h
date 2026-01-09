// ECID.h: interface for the CECID class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ECID_H__6130F772_B4DD_4D5C_986E_765F8BD0C960__INCLUDED_)
#define AFX_ECID_H__6130F772_B4DD_4D5C_986E_765F8BD0C960__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define ECIDSTRLEN	64
#define ECIDNAMELEN	32

class CECID  
{
public:
	void Set(short ID, char *Nam, char *Val);
	void Set(short ID, char *Nam, double val);
	enum ECIDTYPE { eVALUE, eSTRING };
	short No;
	ECIDTYPE Type;
	char strValue[ECIDSTRLEN+1];
	char Name[ECIDNAMELEN+1];
	char defValue[ECIDSTRLEN+1];
	char minValue[ECIDSTRLEN+1];
	char maxValue[ECIDSTRLEN+1];
	char unit[ECIDSTRLEN+1];

	CECID();
	virtual ~CECID();

};

#endif // !defined(AFX_ECID_H__6130F772_B4DD_4D5C_986E_765F8BD0C960__INCLUDED_)
