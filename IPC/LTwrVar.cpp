// LTwrVar.cpp: implementation of the LTwrVar class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LTwrVar.h"
#include "..\SRC\BMake.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LTwrVar::LTwrVar() {
	Red = OFF;
	Green = OFF;
	Blue = OFF;
	Amber = OFF;
}

LTwrVar::~LTwrVar() {}

long LTwrVar::GetLength() {
	short led;
	long sz = 0;
	sz += CMake::GetLength(led) * 4;

	return sz;
}

long LTwrVar::Binary(char** msg) {
	short led;
	long sz = 0;

	led = (short)Red;
	sz += CMake::Binary(led, msg);
	led = (short)Green;
	sz += CMake::Binary(led, msg);
	led = (short)Blue;
	sz += CMake::Binary(led, msg);
	led = (short)Amber;
	sz += CMake::Binary(led, msg);

	return sz;
}

void LTwrVar::Conv(char** msg) {
	short led;
	CMake::Conv(led, msg);
	Red = (LTwrVar::LTLED)led;
	CMake::Conv(led, msg);
	Green = (LTwrVar::LTLED)led;
	CMake::Conv(led, msg);
	Blue = (LTwrVar::LTLED)led;
	CMake::Conv(led, msg);
	Amber = (LTwrVar::LTLED)led;
}