// SelectMode.h: interface for the CSelectMode class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

class CSelectMode {
public:
	CSelectMode();
	virtual ~CSelectMode();

public:
	//      BYTE  m_STypeRec ;
	//      BYTE  m_STypeSnd ;
	BYTE m_RecvData[11];
	// 	 int  m_STypeArcnt;

	/************************************************/
	// Follow function and variable is a for Active
	/************************************************/
public:

	BOOL IsSelectReq(BYTE* nData);
	void SelectRep(BYTE* nData);

	/***********************************************/
	// Follow function and variable is a for Active
	/***********************************************/
public:
	CWnd* pWnd;
	BOOL IsSeperateReq(BYTE* nData);
	BOOL Pass_SelectCedure(DWORD T7);
};
