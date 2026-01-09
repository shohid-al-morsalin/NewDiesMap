#include "stdafx.h"
#include "SRC/BMake.h"

#include "CBatchDoc.h"

// Magic needs to be copied @@Vijay#31Aug$01
CBatchDoc& CBatchDoc::operator =(CBatchDoc& co) {
	magic = co.magic;
	Note = co.Note;
	WaferID = co.WaferID;
	Project = co.Project;
	LotID = co.LotID;
	CassetteID = co.CassetteID;
	OperatorName = co.OperatorName;
	return *this;
}

void CBatchDoc::Serialize(CArchive& ar) {
	if (ar.IsStoring()) {
		magic = 1;
		ar << magic;
		ar << LotID;
		ar << Project;
		ar << WaferID;
		ar << CassetteID;
		ar << OperatorName;
		ar << Note;
	}
	else {
		ar >> magic;
		ar >> LotID;
		ar >> Project;
		ar >> WaferID;
		ar >> CassetteID;
		ar >> OperatorName;
		ar >> Note;
	}
}

CBatchDoc::CBatchDoc() {
	magic = 0;
	Clear();
}

CBatchDoc::~CBatchDoc() {}

void CBatchDoc::Get(CString& PrjName, CString& CID, CString& LID, CString& WID, CString& Op, CString& Nt) {
	PrjName = Project;
	CID = CassetteID;
	LID = LotID;
	WID = WaferID;
	Op = OperatorName;
	Nt = Note;
}

void CBatchDoc::Clear() {
	Note = "";
	LotID = "";
	Project = "";
	WaferID = "";
	CassetteID = "";
	OperatorName = "";
}

void CBatchDoc::Conv(char** msg) {
	CMake::Conv(Note, msg);
	CMake::Conv(LotID, msg);
	CMake::Conv(Project, msg);
	CMake::Conv(WaferID, msg);
	CMake::Conv(CassetteID, msg);
	CMake::Conv(OperatorName, msg);
}

long CBatchDoc::Binary(char** msg) {
	long sz = 0;

	sz += CMake::Binary(Note, msg);
	sz += CMake::Binary(LotID, msg);
	sz += CMake::Binary(Project, msg);
	sz += CMake::Binary(WaferID, msg);
	sz += CMake::Binary(CassetteID, msg);
	sz += CMake::Binary(OperatorName, msg);

	return sz;
}

long CBatchDoc::GetLength() {
	long sz = 0;

	sz += CMake::GetLength(Note);
	sz += CMake::GetLength(LotID);
	sz += CMake::GetLength(Project);
	sz += CMake::GetLength(WaferID);
	sz += CMake::GetLength(CassetteID);
	sz += CMake::GetLength(OperatorName);

	return sz;
}

void CBatchDoc::VerboseOut(FILE* fp) {
	if (!fp) {
		return;
	}
	fprintf(fp, "Batch information\n");
	fprintf(fp, "Version: %d\n", magic);
	fprintf(fp, "Lot ID: %s\n", LotID.GetBuffer(0));
	fprintf(fp, "Wafer ID: %s\n", WaferID.GetBuffer(0));
	fprintf(fp, "Cassette ID: %s\n", CassetteID.GetBuffer(0));
	fprintf(fp, "Operator: %s\n", OperatorName.GetBuffer(0));
	fprintf(fp, "Project\n");
	fprintf(fp, "%s\n", Project.GetBuffer(0));
	fprintf(fp, "Note\n");
	fprintf(fp, "%s\n", Note.GetBuffer(0));
}