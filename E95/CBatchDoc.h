#pragma once

class CRecipe;

class CBatchDoc {
public:
	void VerboseOut(FILE* fp);
	CString Note;
	CString WaferID;
	CString Project;
	CString LotID;
	CString CassetteID;
	CString OperatorName;

	void Conv(char** msg);
	long Binary(char** msg);
	long GetLength();

	void Clear();
	void Get(CString& PrjName, CString& CID, CString& LID, CString& WID, CString& Op, CString& Nt);

	void Serialize(CArchive& ar);
	CBatchDoc& operator=(CBatchDoc& co);

	CBatchDoc();
	~CBatchDoc();

private:
	short magic;
};
