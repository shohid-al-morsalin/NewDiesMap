#pragma once

class CBatchDoc;

class CWaferParam : CObject {
public:
	void VerboseOut(FILE* fp);
	void Clear();

	void Conv(char** msg);
	long Binary(char** msg);
	long GetLength();

	/**
	 * Wafer diameter
	 */
	short size;

	/**
	 * Daimeter of the wafer that is actually measure, specified in term of percentage.
	 */
	short mapsize;

	CWaferParam& operator=(CWaferParam& co);

	void Serialize(CArchive& ar);

	CWaferParam();

	~CWaferParam();

	// [01242024 ZHIMING
	short percentage;
	// ]

private:

	/**
	 * Version description: Should be used while serializing and deserilization
	 */
	short magic;
};
