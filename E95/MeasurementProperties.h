// MeasurementProperties.h: interface for the CMeasurementProperties class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
class CMeasurementProperties {
private:
	short magic;

public:
	void VerboseOut(FILE* fp);
	void Conv(char** msg);
	long Binary(char** msg);
	long GetLength();

	void Clear();
	void Serialize(CArchive& ar);

	short matrixsize;
	short matrixspacing;
	/**
	 * Calibration gauge position
	 */
	short calpos;

	/**
	 * Sample average
	 */
	short ave;
	float xdicesize;
	float ydicesize;
	int cfactorindex; //fw added
	/**
	 * Bit info MPrj
	 * Each bit indicate a measurement type to perform
	 * Bit info TM
	 * Value indicate a measurement type to perform
	 */
	long measurement;

	CMeasurementProperties& operator=(CMeasurementProperties& co);
	CMeasurementProperties();
	virtual ~CMeasurementProperties();
};
