#include "stdafx.h"
#include "strSOp.h"
#include "AlrmItem.h"

// CAlrmItem::CAlrmItem()
// {
// 	alCD = RESET;
// 	alID = 0;
// 	alTX[0] = 0;
//
// 	alED = FALSE;
// 	SetAlrmCEID = 0;
// 	ClearAlrmCEID = 0;
// 	EnableSetAlrmCEID = 0;
// 	EnableClearAlrmCEID = 0;
// }
//
// CAlrmItem::~CAlrmItem()
// {
//
// }

void CAlrmItem::Preset() {
	alCD = RESET;
	alID = 0;
	alTX[0] = 0;

	alED = FALSE;
	SetAlrmCEID = 0;
	ClearAlrmCEID = 0;
	EnableSetAlrmCEID = 0;
	EnableClearAlrmCEID = 0;
}

void CAlrmItem::Set(int id, char* idTx) {
	alID = id;
	strscpy(alTX, ALMTXTLEN, idTx);
}

void CAlrmItem::SetCEID(int SetAlrm, int ClrAlrm, int EnSetAlrm, int EnClrAlrm) {
	SetAlrmCEID = SetAlrm;
	ClearAlrmCEID = ClrAlrm;
	EnableSetAlrmCEID = EnSetAlrm;
	EnableClearAlrmCEID = EnClrAlrm;
}