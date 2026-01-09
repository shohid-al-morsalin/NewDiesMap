// LoadPos.h: interface for the CLoadPos class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

// Properties of load position [9/12/2012 Administrator]

class CLoadPos {
public:
	void Save();
	void Load();
	void Clear();

	float PX, PY;		// Actual load position [9/12/2012 Administrator]
	float VX, VY, VZ;	// Verification location [9/12/2012 Administrator]
	int Exposure;		// Exposure value to verify load position [9/12/2012 Administrator]

	CLoadPos();
	virtual ~CLoadPos();
};
