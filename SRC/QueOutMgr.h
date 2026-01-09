// QueOutMgr.h: interface for the CQueOutMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUEOUTMGR_H__7FE5C289_44F1_42AE_A484_F503807186BC__INCLUDED_)
#define AFX_QUEOUTMGR_H__7FE5C289_44F1_42AE_A484_F503807186BC__INCLUDED_


#include <vector>
using namespace std;

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CQueOutMgr  
{
	vector <CString> MSGList;

public:
	void Add(CString msg);
	BOOL Get(CString &msg);

	CQueOutMgr();
	virtual ~CQueOutMgr();

};

#endif // !defined(AFX_QUEOUTMGR_H__7FE5C289_44F1_42AE_A484_F503807186BC__INCLUDED_)
