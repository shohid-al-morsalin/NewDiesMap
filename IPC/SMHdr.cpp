#include "stdafx.h"
#include "SMHdr.h"

CSMHdr::CSMHdr() {
	Clear();
}

void CSMHdr::Clear() {
	HWin = HWin1 = NULL;
	threadID = threadID1 = 0;
	Ack = Ack1 = 0;
	Nak = Nak1 = 0;
	cmd = CSMHdr::CTEXIT;
	for (int i = 0; i < 16; i++) {
		sub[i] = 0;
		ret[i] = 0;
	}
	text[0] = 0;
}

void CSMHdr::Set1(CSMHdr::SMHDRCMD _cmd, float sub0, float sub1, float sub2, float sub3, float sub4, float sub5, float sub6, float sub7) {
	cmd = _cmd;
	sub[0] = sub0;
	sub[1] = sub1;
	sub[2] = sub2;
	sub[3] = sub3;
	sub[4] = sub4;
	sub[5] = sub5;
	sub[6] = sub6;
	sub[7] = sub7;
}

void CSMHdr::Set2(float sub8, float sub9, float sub10, float sub11, float sub12, float sub13, float sub14, float sub15) {
	sub[8] = sub8;
	sub[9] = sub9;
	sub[10] = sub10;
	sub[11] = sub11;
	sub[12] = sub12;
	sub[13] = sub13;
	sub[14] = sub14;
	sub[15] = sub15;
}

void CSMHdr::SetCaller(CString svr) {
	strcpy(Caller, svr.GetBuffer(0));
}