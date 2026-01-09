// TxR.h: interface for the CTxR class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
// Class to perform transformation, in this case Tx, Ty, Tr [10/24/2012 Yuen]

class CTxR {
public:
	float sinA, cosA;	// Intermediate [10/24/2012 Yuen]
	float Tx, Ty;		// Translation [10/24/2012 Yuen]
	float Tr;			// Rotation in degree [10/24/2012 Yuen]

	CTxR();
	virtual ~CTxR();

	void Set(float X, float Y, float A);
	void Tran(float& X, float& Y);	// Transform from logical space to device space [10/24/2012 Yuen]
	void RTran(float& X, float& Y);	// Transform from device space to logical space [10/24/2012 Yuen]
};
