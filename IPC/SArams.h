#ifndef _SARAMS_
#define _SARAMS_
struct SArams {
	enum ARAMSSTATE {
		NOSTATE, PRODUCTIVE, STANDBY,
		ENGINEERING, SCHEDULEDDOWN,
		UNSCHEDULEDDOWN, NONSCHEDULED,
		INITIALIZE, SETTINGUP
	};

	ARAMSSTATE CurrentState;

	double ScheduledDown;

	double UnscheduledDown;

	double Engineering;

	double NonScheduled;

	double Standby;

	double Productive;

	BOOL bEqpSetDownTime;

	BOOL bShutDown;

	static CString GetEqpStateStr(ARAMSSTATE state) {
		switch (state) {
		case NOSTATE:
			return "NO STATE";
		case PRODUCTIVE:
			return "PRODUCTIVE";
		case STANDBY:
			return "STANDBY";
		case ENGINEERING:
			return "ENGINEERING";
		case SCHEDULEDDOWN:
			return "SCHEDULE DOWN";
		case UNSCHEDULEDDOWN:
			return "UNSCHEDULED DOWN";
		case NONSCHEDULED:
			return "NOT SCHEDULED";
		case INITIALIZE:
			return "INITIALIZE";
		case SETTINGUP:
			return "SETTING UP";
		}
		return "UNKNOWN";
	}

	void Preset() {
		CurrentState = NOSTATE;
		Engineering = 0;
		NonScheduled = 0;
		Productive = 0;
		ScheduledDown = 0;
		Standby = 0;
		UnscheduledDown = 0;
		bShutDown = TRUE;
		bEqpSetDownTime = FALSE;
	}
};
#endif
