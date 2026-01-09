#pragma once

class CCoor : public CObject {
public:
	float x;
	float y;
	float z;
	short p;	// Position Marker (1 to 3 [7/23/2013 Yuen]
	short n;	// serial number of MPoints [11/1/2010 XPMUser]

private:
	char magic;

public:
	CString GetXYStr();
	CString GetNStr();
	void Conv(char** msg);
	long Binary(char** msg);
	long GetLength();

	float dia();
	float dia2();
	void SetXYZ(short nn, float xx, float yy, float zz);

	BOOL IsPoint(CCoor& point);

	CCoor& operator=(CCoor& co);

	void Serialize(CArchive& ar);

	CCoor();
	virtual ~CCoor();
	bool  operator==(const CCoor& Rhs);
};