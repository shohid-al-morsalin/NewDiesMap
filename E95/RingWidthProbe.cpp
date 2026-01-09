#include "stdafx.h"
#include "RingWidthProbe.h"

BOOL CRingWidthProbe::Deinitialize() {
	return TRUE;
}

BOOL CRingWidthProbe::Initialize(CListBox* pMessage) {
	pMessage->InsertString(pMessage->GetCount(), "Ring Width system started OK");
	return TRUE;
}