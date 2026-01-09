// E95Doc.cpp : implementation of the CE95Doc class
//

#include "stdafx.h"
#include "E95.h"

#include "E95Doc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CE95Doc

IMPLEMENT_DYNCREATE(CE95Doc, CDocument)

BEGIN_MESSAGE_MAP(CE95Doc, CDocument)
END_MESSAGE_MAP()

// CE95Doc construction/destruction

CE95Doc::CE95Doc() {
	// TODO: add one-time construction code here
}

CE95Doc::~CE95Doc() {}

BOOL CE95Doc::OnNewDocument() {
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

// CE95Doc serialization

void CE95Doc::Serialize(CArchive& ar) {
	if (ar.IsStoring()) {
		// TODO: add storing code here
	}
	else {
		// TODO: add loading code here
	}
}

// CE95Doc diagnostics

#ifdef _DEBUG
void CE95Doc::AssertValid() const {
	CDocument::AssertValid();
}

void CE95Doc::Dump(CDumpContext& dc) const {
	CDocument::Dump(dc);
}
#endif //_DEBUG

// CE95Doc commands