// WaferInfoMgr.cpp: implementation of the CWaferInfoMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WaferInfoMgr.h"

#include "..\..\SRC\BMake.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWaferInfoMgr& CWaferInfoMgr::operator =(CWaferInfoMgr& co) {
	Clear();

	enPort = co.enPort;
	InPort = co.InPort;
	magic = co.magic;
	OutPort = co.OutPort;

	POSITION pos = co.winfo.GetHeadPosition();
	while (pos) {
		CWaferInfo* p = (CWaferInfo*)co.winfo.GetNext(pos);
		if (p) {
			CWaferInfo* pW = new CWaferInfo;
			if (pW) {
				*pW = *p;
				winfo.AddTail(pW);
			}
		}
	}
	return *this;
}

void CWaferInfoMgr::Serialize(CArchive& ar) {
	int cnt;

	if (ar.IsStoring()) {
		magic = 2;
		ar << magic;
		ar << InPort;
		ar << OutPort;
		cnt = winfo.GetCount();
		ar << cnt;
		if (cnt) {
			POSITION pos = winfo.GetHeadPosition();
			while (pos) {
				CWaferInfo* p = (CWaferInfo*)winfo.GetNext(pos);
				if (p) {
					p->Serialize(ar);
				}
			}
		}
	}
	else {
		Clear();	// Must be called before InPort & OutPort is re-initialized
		InPort = 1;
		OutPort = 1;

		ar >> magic;
		if (magic > 1) {
			ar >> InPort;
			ar >> OutPort;
		}
		ar >> cnt;
		for (int i = 0; i < cnt; i++) {
			CWaferInfo* p = new CWaferInfo;
			if (p) {
				p->Serialize(ar);
				winfo.AddTail(p);
			}
		}
	}
}

CWaferInfoMgr::CWaferInfoMgr() {
	enPort = 0;
	Clear();
}

CWaferInfoMgr::~CWaferInfoMgr() {
	Clear();
}

void CWaferInfoMgr::Clear() {
	InPort = 0;
	OutPort = 0;

	POSITION pos = winfo.GetHeadPosition();
	while (pos) {
		CWaferInfo* p = (CWaferInfo*)winfo.GetNext(pos);
		if (p) {
			delete p;
		}
	}
	winfo.RemoveAll();
}

void CWaferInfoMgr::Add(CWaferInfo* p, BOOL bCreate) {
	POSITION pos = winfo.GetHeadPosition();
	while (pos) {
		CWaferInfo* q = (CWaferInfo*)winfo.GetNext(pos);
		if (q) {
			if (q->wafernum == p->wafernum) {
				// Update
				*q = *p;	// Copy
				if (!bCreate) {
					delete p;
				}
				return;
			}
		}
	}
	if (bCreate) {
		// Add new
		CWaferInfo* pWI = new CWaferInfo;
		if (pWI) {
			*pWI = *p;  // Copy
			winfo.AddTail(pWI);
		}
	}
	else {
		winfo.AddTail(p);
	}
}

// slot is 1 base
CWaferInfo* CWaferInfoMgr::FindWafer(short wafernum) {
	POSITION pos = winfo.GetHeadPosition();
	while (pos) {
		CWaferInfo* p = (CWaferInfo*)winfo.GetNext(pos);
		if (p) {
			if (p->wafernum == wafernum) return p;
		}
	}
	return NULL;
}

BOOL CWaferInfoMgr::Remove(short wafernum) {
	POSITION pos2;
	POSITION pos = winfo.GetHeadPosition();
	while (pos) {
		pos2 = pos;
		CWaferInfo* p = (CWaferInfo*)winfo.GetNext(pos);
		if (p) {
			if (p->wafernum == wafernum) {
				delete p;
				winfo.RemoveAt(pos2);
				return TRUE;
			}
		}
	}
	return FALSE;
}

CWaferInfo* CWaferInfoMgr::AddWafer(short wafernum) {
	CWaferInfo* p = FindWafer(wafernum);
	if (p) return p;
	// Add new entry if not found
	p = new CWaferInfo;
	if (p) {
		p->wafernum = wafernum;
		winfo.AddTail(p);
		return p;
	}
	return NULL;
}

long CWaferInfoMgr::GetLength() {
	long sz = 0;

	sz += CMake::GetLength(enPort);
	sz += CMake::GetLength(InPort);
	sz += CMake::GetLength(OutPort);

	sz += CMake::GetLength(sz); // list count
	POSITION pos = winfo.GetHeadPosition();
	while (pos) {
		CWaferInfo* p = (CWaferInfo*)winfo.GetNext(pos);
		if (p) {
			sz += p->GetLength();
		}
	}
	return sz;
}

long CWaferInfoMgr::Binary(char** msg) {
	long count, sz = 0;

	sz += CMake::Binary(enPort, msg);
	sz += CMake::Binary(InPort, msg);
	sz += CMake::Binary(OutPort, msg);

	count = winfo.GetCount();
	sz += CMake::Binary(count, msg);
	POSITION pos = winfo.GetHeadPosition();
	while (pos) {
		CWaferInfo* p = (CWaferInfo*)winfo.GetNext(pos);
		if (p) {
			sz += p->Binary(msg);
		}
	}
	return sz;
}

void CWaferInfoMgr::Conv(char** msg) {
	long count;

	Clear();

	CMake::Conv(enPort, msg);
	CMake::Conv(InPort, msg);
	CMake::Conv(OutPort, msg);

	CMake::Conv(count, msg);
	for (int i = 0; i < count; i++) {
		CWaferInfo* p = new CWaferInfo;
		if (p) {
			p->Conv(msg);
			winfo.AddTail(p);
		}
	}
}

void CWaferInfoMgr::SetLPStation(short InP, short OutP) {
	enPort = 1 << (InP - 1);
	InPort = InP;
	OutPort = OutP;
}

void CWaferInfoMgr::SetContentMap(short wafernum, CContentMap& Ct) {
	CWaferInfo* p = FindWafer(wafernum);
	if (!p) {
		p = new CWaferInfo;
		if (p) {
			p->wafernum = wafernum;// Required so that it can be located later
			winfo.AddTail(p);
		}
	}
	if (p) {
		p->WaferID = Ct.SubstID;
		p->LotID = Ct.LotID;
	}
}

CString CWaferInfoMgr::GetWaferID(short wafernum) {
	CWaferInfo* p = FindWafer(wafernum);
	if (p) return p->WaferID;
	return "";
}

void CWaferInfoMgr::SetWaferID(short wafernum, CString ID) {
	CWaferInfo* p = FindWafer(wafernum);
	if (p) {
		p->WaferID = ID;
	}
	else {
		CWaferInfo* wI = AddWafer(wafernum);
		if (wI) {
			wI->WaferID = ID;
		}
	}
}

CString CWaferInfoMgr::GetLotID(short wafernum) {
	CWaferInfo* p = FindWafer(wafernum);
	if (p) return p->LotID;
	return "";
}

void CWaferInfoMgr::SetLotID(short wafernum, CString ID) {
	CWaferInfo* p = FindWafer(wafernum);
	if (p) {
		p->LotID = ID;
	}
	else {
		CWaferInfo* wI = AddWafer(wafernum);
		if (wI) {
			wI->LotID = ID;
		}
	}
}