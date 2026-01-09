// QueOutMgr.h: interface for the CQueOutMgr class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
#include <vector>
using namespace std;

class CQueOutMgr {
	vector <CString> MSGList;

public:
	void Add(CString msg);
	BOOL Get(CString& msg);

	CQueOutMgr();
	virtual ~CQueOutMgr();
};