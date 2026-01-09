// EchoPeak.h: interface for the CEchoPeak class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
class CEchoPeak {
public:
	void Reset();
	CEchoPeak();
	virtual ~CEchoPeak();

	void SetPosition(int nData);
	void SetIndex(int nData);
	void SetHeight(int nData);
	void SetCM(int index, float fData);
	void SetMid(double fData);

	float GetPosition();
	int GetIndex();
	int GetHeight();
	float fGetCM();
	float GetMid();

public:
	void Dump(FILE* fp);
	float GetShapeFactor();
	float fXMaxima;	// Maximum Peak position (micron)
	int iXMaxima;	// Index of XMaxima
	int YMaxima;	// Maximum Peak height [11/30/2010 XPMUser]
	int iLL;		// Index of left limit
	int iRL;		// Index of right limit
	float fCMs;		// Center of mass (micron)
	int iCMs;		// Index of CM [9/25/2011 Administrator]
	float Mid;		// Middle of peak [1/6/2011 C2C]
};
