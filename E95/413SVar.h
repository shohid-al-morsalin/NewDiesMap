#pragma once

class C413SVar {
public:
	BOOL bWaferPresent; // indicate whether wafer is present at station
	short OpMode, ServiceState;
	void Conv(char** msg);
	long Binary(char** msg);
	long GetLength();

	C413SVar();
	virtual ~C413SVar();
};
