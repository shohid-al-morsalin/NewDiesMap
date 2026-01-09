// Event.h: interface for the CEvt class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "..\..\IPC\smol.h"

#define MAXMSGLEN	900

class SecsItem;
class CMPoint;
class CMeParam;
class CRecipe413;

class CEvt/* : public CObject*/
{
	// replaced
	//char  msg[MAXMSGLEN];		// Used to return message header to host in case of T3 timeout
	INT nmsg;
	BYTE* pmsg;

public:
	int eventid;
	int make;
	short no;
	short phase;
	enum OPTION { NORMAL, FORMAT, SEND };
	OPTION option;
	mutable SecsItem* pReport;
	UINT SysByte;	// CEquipEvtThread, Usage unknown ???
	short wafercnt;
	short slotno;	// Use in STS

	UINT alid;
	BYTE alcd;
	short almno;
	short PRJobState;

	//	short cnt;
	short PortNo;
	HWND ReplyWnd;

	CMPoint* pMPoint;  // pointer MPoint structure [8/18/2010 XPMUser]
	CMeParam* pMeParam; // pointer to MeParam [8/18/2010 XPMUser]
	CRecipe413* pRcp;	// Pointer to CRecipe413 object [9/8/2012 Administrator]
	sScanData scndta;

	short slotnolist[26];
	char ObjID[OBJIDLEN + 1];
	char SubstID[OBJIDLEN + 1];
	char LocID[LOCIDLEN + 1];
	char PRJobID[PRJIDLEN + 1];
	char CJobID[CRJIDLEN + 1];

public:
	INT GetNMsg();
	BYTE* GetMsg(long len = -1);
	CEvt::OPTION GetOption();
	void Dealloc();
	BOOL SetMsg(BYTE* bytrs, long nbytes);
	BOOL Alloc(long nbytes);
	CEvt(int id, int mk);
	CEvt(const CEvt& Rhs);
	CEvt& operator=(const CEvt& Rhs);

	void Set(int id, int mk);
	void Set(int id, int mk, UINT SysByte);

	void Clear();
	void Set(int id, int mk, short cnt, short* slt);
	void Set(int id, int mk, short n);

	void Set(int id, int mk, short n, char* message);
	void Set(int id, int mk, char* bytes, short cnt);
	void SetObjID(int id, int mk, char* txt, short cnt);
	void SetPrjID(int id, int mk, char* txt, short cnt);
	void Set(BOOL onoff, int id, int mk, short aid, short acd, short amno);
	void Set(int id, int mk, short acd, short aid, char* altxt);

	CEvt();
	virtual ~CEvt();
};