#ifndef _EQPSTATUS_H_
#define _EQPSTATUS_H_

struct EqpStatus {
	enum CONTROLSTATE {
		// Notes Never change the sequence [11/25/2002]
		NOSECS, REMOTE, LOCAL, OFFLINEEQP, OFFLINEHOST, ATTEMPTONLINE
	};

	// 	static CString GetControlStateStr(CONTROLSTATE state)
	// 	{
	// 		switch(state)
	// 		{
	// 		case NOSECS:
	// 			return "NO STATE";
	// 		case REMOTE:
	// 			return "REMOTE";
	// 		case LOCAL:
	// 			return "LOCAL";
	// 		case OFFLINEEQP:
	// 			return "OFFLINE EQP";
	// 		case OFFLINEHOST:
	// 			return "OFFLINE HOST";
	// 		case ATTEMPTONLINE:
	// 			return "ATTEMPT ONLINE";
	// 		}
	// 		return "UNKNOWN";
	// 	}

	short MaxLP;
	float bp750;
	float bp810;
	float stressconst;

	CONTROLSTATE LineMode;

public:
	static CString GetControlStateStr(CONTROLSTATE state);
	BOOL IsOnline();
	// 	{
	// 		switch (LineMode)
	// 		{
	// 		case LOCAL:
	// 		case REMOTE:
	// 			return TRUE;
	// 			break;
	// 		}
	// 		return FALSE;
	// 	};
};

struct RbtStatus {
	enum PROCESSSTATE {
		pIDLE, pWORKING, pERRORS
	};

	PROCESSSTATE ProcState;
};
#endif