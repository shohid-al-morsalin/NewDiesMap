#include "stdafx.h"

#include "DataList.h"
#include "Data.h"

#include "SRC/BMake.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;

#define new DEBUG_NEW
#endif

CDataList& CDataList::operator =(CDataList& co) {
	magic = co.magic;
	ClearList();
	POSITION pos = co.DatLst.GetHeadPosition();
	while (pos) {
		CData* p = (CData*)co.DatLst.GetNext(pos);
		if (p) {
			CData* pp = new CData(); // 20 for TM413 [6/24/2010 Yuen]
			if (pp) {
				*pp = *p;
				DatLst.AddTail(pp);
			}
		}
	}
	return *this;
}

void CDataList::Serialize(CArchive& ar) {
	if (ar.IsStoring()) {
		ar << magic;
		short n = (short)DatLst.GetCount();
		ar << n;
		if (n > 0) {
			POSITION pos = DatLst.GetHeadPosition();
			while (pos) {
				CData* p = (CData*)DatLst.GetNext(pos);
				if (p) p->Serialize(ar);
			}
		}
	}
	else {
		ClearList();
		ar >> magic;
		short n;
		ar >> n;
		for (int i = 0; i < n; i++) {
			CData* p = new CData(); // 20 for TM413 [6/24/2010 Yuen]
			if (p) {
				p->Serialize(ar);
				DatLst.AddTail(p);
			}
		}
	}
}

CDataList::CDataList() {
	magic = 0;
}

CDataList::~CDataList() {
	ClearList();
}

void CDataList::ClearList() {
	POSITION pos = DatLst.GetHeadPosition();
	while (pos) {
		CData* pDat = (CData*)DatLst.GetNext(pos);
		if (pDat) {
			delete pDat;
		}
	}
	if (DatLst.GetCount()) {
		DatLst.RemoveAll();
	}
}

void CDataList::AddTail(CData* pDat) {
	DatLst.AddTail(pDat);
}

void CDataList::AddHead(CData* pDat) {
	DatLst.AddHead(pDat);
}

// void CDataList::FakeData()
// {
// 	ClearList();
// 	CData *pDat = GetData(0);
// 	if (pDat) {
// 		// Object present, add fake data
// 		pDat->FakeData();
// 	}
// 	else {
// 		// Object not present, create, add fake data
// 		pDat = new CData();  // 20 for TM413 [6/24/2010 Yuen]
// 		if (pDat) {
// 			pDat->FakeData();
// 			DatLst.AddTail(pDat);
// 		}
// 	}
// }

CData* CDataList::GetData(short index) {
	POSITION pos = DatLst.FindIndex(index);
	if (pos) {
		return (CData*)DatLst.GetAt(pos);
	}
	return NULL;
}

void CDataList::SetData(CData* pDat) {
	ASSERT(pDat);
	DatLst.AddHead(pDat);
}

void CDataList::AddData(CData* pDat) {
	ASSERT(pDat);
	DatLst.AddTail(pDat);
}

void CDataList::Conv(char** msg) {
	int cnt = 0;
	CMake::Conv(cnt, msg);
	ClearList();
	for (int i = 0; i < cnt; i++) {
		CData* p = new CData;
		if (p) {
			p->Conv(msg);
			DatLst.AddTail(p);
		}
	}
}

long CDataList::Binary(char** msg) {
	long sz = 0;
	int cnt = DatLst.GetCount();

	sz += CMake::Binary(cnt, msg);

	POSITION pos = DatLst.GetHeadPosition();
	while (pos) {
		CData* p = (CData*)DatLst.GetNext(pos);
		if (p) {
			sz += p->Binary(msg);
		}
	}
	return sz;
}

long CDataList::GetLength() {
	long sz = 0;

	sz += sizeof(int);

	POSITION pos = DatLst.GetHeadPosition();
	while (pos) {
		CData* p = (CData*)DatLst.GetNext(pos);
		if (p) {
			sz += p->GetLength();
		}
	}
	return sz;
}