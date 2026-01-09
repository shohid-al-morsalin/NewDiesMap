// TCtrlDB.cpp: implementation of the CTCtrlDB class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <conio.h>
#include "TCtrlDB.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define IOPORT		0x30A

CTCtrlDB::CTCtrlDB() {
	PortValue = 0;
	// Clear All lights
	//_outp(IOPORT,0xFF);
}

CTCtrlDB::~CTCtrlDB() {
	// Clear All lights
	//_outp(IOPORT,0xFF);
}

void CTCtrlDB::Update() {
#define GRN			0x80
#define RED			0x40
#define AMB			0x20

	PortValue = 0;
	if (Red == ON) {
		PortValue |= RED;
	}
	if (Amber == ON) {
		PortValue |= AMB;
	}
	if (Green == ON) {
		PortValue |= GRN;
	}

	PortValue = ~PortValue;
	//_outp(IOPORT,PortValue);
}