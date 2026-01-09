#ifndef _STDACTION_H_
#define _STDACTION_H_
#include "Constant.h"
// standard action
#define SUBLEN				16
#define RETLEN				16
struct StdAction {
	enum ACTION {
		ROBOTHOMED, SCANDATAREADY, SCANDATASKIPPED, READSCANDATA, BATCHRUNEND, READMSG, HELLO, STARTPROCESSING, STARTUNIT,
		/*UPDATEARAMS,*/ GOONLINEREMOTE, GOONLINELOCAL, GOOFFLINEEQP, UNCLAMPFOUP // Sub[0] = PortNo, text = CarrierID
	};

	ACTION cmd;

	float sub[SUBLEN + 1]; // Why multiple float values

	float ret[RETLEN + 1]; // Why multiple float values

	char text[TEXTLEN + 1];
};
#endif