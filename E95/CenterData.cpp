#include "stdafx.h"

#include "CenterData.h"

SCenterData WC;

void SCenterData::ClearOffset() {
	XOff = 0; YOff = 0;
}

void SCenterData::SetOffset(float X, float Y) {
	XOff = X; YOff = Y;
}

void SCenterData::FindOffset() {
	//////////////////////////////////////////////////////////////////////////
	bOffsetInUse = TRUE;
	XOff = (X0 + X180) / 2.0f;
	YOff = (Y90 + Y270) / 2.0f;
	//////////////////////////////////////////////////////////////////////////
}

void SCenterData::Serialize(CArchive& ar) {
	short magic = 1;
	if (ar.IsStoring()) {
		ar << magic;
		ar << X0;
		ar << X90;
		ar << X180;
		ar << X270;
		ar << XOff;
		ar << YOff;
	}
	else {
		ar >> magic;
		ar >> X0;
		ar >> X90;
		ar >> X180;
		ar >> X270;
		ar >> XOff;
		ar >> YOff;
	}
}

void SCenterData::Load(CString filename) {
	CFile theFile;
	if (theFile.Open(filename, CFile::modeRead)) {
		CArchive archive(&theFile, CArchive::load);
		Serialize(archive);
		archive.Close();
		theFile.Close();
		return;
	}
}

void SCenterData::Save(CString filename) {
	CFile theFile;
	if (theFile.Open(filename, CFile::modeCreate | CFile::modeWrite)) {
		CArchive archive(&theFile, CArchive::store);
		Serialize(archive);
		archive.Close();
		theFile.Close();
		return;
	}
}