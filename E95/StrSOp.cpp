#include "stdafx.h"
#include "strSOp.h"

BOOL strscpy(char* dest, short szdest, const char* sorc) {
	strcpy(dest, sorc);
	return TRUE;

	//memset(dest,0,szdest);
	short len = short(strlen(sorc));
	if (len > 0) {
		if (len <= szdest) {
			strcpy(dest, sorc);
			*(dest + szdest - 1) = 0;
			return TRUE;
		}
		else {
			memcpy(dest, sorc, szdest - 2);
			*(dest + szdest - 1) = 0;
			return TRUE;
		}
	}
	*dest = 0;
	return FALSE;
}