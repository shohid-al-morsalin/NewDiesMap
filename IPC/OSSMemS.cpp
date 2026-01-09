#include "stdafx.h"
#include "strSOp.h"
#include "OSSMemS.h"

void OSSMemS::Preset() {
	memset(ObjID, 0, OBJIDLEN);
	memset(ObjType, 0, OBJTYPELEN);
}

void OSSMemS::SetID(char* ID) {
	strscpy(ObjID, OBJIDLEN, ID);
}

void OSSMemS::SetType(char* Type) {
	strscpy(ObjType, OBJTYPELEN, Type);
}

short OSSMemS::GetBinarySize() {
	short siz = 0;

	siz = sizeof(ObjID);
	siz += sizeof(ObjType);

	return siz;
}