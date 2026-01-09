// LTwrVar.h: interface for the LTwrVar class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

class LTwrVar {
public:
	void Conv(char** msg);
	long Binary(char** msg);
	long GetLength();
	enum LTLED { OFF, ON, BLINK, BUZZER1, BUZZER2 };
	LTLED Red, Green, Blue, Amber;

	LTwrVar();
	virtual ~LTwrVar();
};
