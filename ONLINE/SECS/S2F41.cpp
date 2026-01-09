// S2F41.cpp: implementation of the CS2F41 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "S2F41.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CS2F41::CS2F41() {}

CS2F41::~CS2F41() {}

BOOL CS2F41::Validate() {
	if (!Items.size()) {
		return FALSE;
	}
	SecsItem* Item = Items[0];
	if (!Item->IsList()) {
		return FALSE;
	}
	if (Item->NumContainedItems() != 2) {
		return FALSE;
	}
	if (!Item->ContainedItems(0)->IsASCII()) {
		return FALSE;
	}
	if (!Item->ContainedItems(1)->IsList()) {
		return FALSE;
	}
	return TRUE;
}