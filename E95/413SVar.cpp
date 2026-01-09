#include "stdafx.h"
#include "413SVar.h"
#include "SRC/BMake.h"

C413SVar::C413SVar() {
	OpMode = 0;
	ServiceState = 0;
	bWaferPresent = 0;  // Bitmap: from LSB : Aligner|Scale|Scanner
}

C413SVar::~C413SVar() {}

long C413SVar::GetLength() {
	long sz = 0;

	sz += CMake::GetLength(OpMode);
	sz += CMake::GetLength(ServiceState);
	sz += CMake::GetLength(bWaferPresent);

	return sz;
}

long C413SVar::Binary(char** msg) {
	long sz = 0;

	sz += CMake::Binary(OpMode, msg);
	sz += CMake::Binary(ServiceState, msg);
	sz += CMake::Binary(bWaferPresent, msg);

	return sz;
}

void C413SVar::Conv(char** msg) {
	CMake::Conv(OpMode, msg);
	CMake::Conv(ServiceState, msg);
	CMake::Conv(bWaferPresent, msg);
}