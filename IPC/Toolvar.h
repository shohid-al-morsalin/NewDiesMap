// Toolvar.h: interface for the CToolVar class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
class CToolVar {
public:
	long Binary(char** msg);
	long GetLength();
	void Conv(char** msg);
	short OpMode;
	CToolVar();
	virtual ~CToolVar();
};
