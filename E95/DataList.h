#pragma once

class CData;
/**
 * Contains a list of results returned from Measurement Class.
 * Each CMPoint object contains a CDataList object that contains results returned from multiple measurements.
 * Drawback : there is only one copy of global CDataDesc object that describe the content of each CData object
 */
class CDataList/* : public CObject */
{
private:
	short magic;
public:

	/**
	 * List of CData objects
	 */
	CPtrList DatLst;
public:
	void Conv(char** msg);
	long Binary(char** msg);
	long GetLength();

	void AddTail(CData* pDat);
	void AddHead(CData* pDat);

	void ClearList();

	void AddData(CData* d);
	void SetData(CData* d);
	CData* GetData(short index);
	// 	void FakeData();

	CDataList& operator=(CDataList& co);

	void Serialize(CArchive& ar);

	CDataList();
	virtual ~CDataList();
};
