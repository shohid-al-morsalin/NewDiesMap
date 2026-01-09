// S2F24.cpp: implementation of the CS2F23 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "S2F23.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CS2F23::CS2F23() {}

CS2F23::~CS2F23() {}

BOOL CS2F23::Validate() {
	if (!Items.size()) {
		return FALSE;
	}
	SecsItem* Item = Items[0];
	if (!Item->IsList()) {
		return FALSE;
	}
	if (Item->NumContainedItems() != 5) {
		return FALSE;
	}
	if (!Item->ContainedItems(0)->IsAInt()) {
		return FALSE;
	}
	if (!Item->ContainedItems(1)->IsASCII()) {
		return FALSE;
	}
	if (!Item->ContainedItems(2)->IsAInt()) {
		return FALSE;
	}
	if (!Item->ContainedItems(3)->IsAInt()) {
		return FALSE;
	}
	if (!Item->ContainedItems(4)->IsList()) {
		return FALSE;
	}
	return TRUE;
}