#include "stdafx.h"

#include "DOSUtil.h"

#include "Arams.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

// CArams *pARAMS = NULL;

CArams::CArams() {
	StartDate = CTime::GetCurrentTime();
	LastDate = StartDate;

	Attr.Preset();
}

CArams::~CArams() {}

BOOL CArams::Load(CString filename) {
	return TRUE;	// Disable state preservation [10/23/2011 Administrator]

	CSingleLock Lock(&CS);
	Lock.Lock();
	CString savefile;

	DosUtil.GetProgramDir(savefile);
	savefile += "\\Arams.bin";

	CFile theFile;
	if (theFile.Open(savefile, CFile::modeRead)) {
		CArchive archive(&theFile, CArchive::load);
		Serialize(archive);
		archive.Close();
		theFile.Close();
		return TRUE;
	}
	return FALSE;
}

void CArams::Save(CString filename) {
	return;	// Disable state preservation [10/23/2011 Administrator]

	CSingleLock Lock(&CS);
	Lock.Lock();
	CString savefile;

	DosUtil.GetProgramDir(savefile);

	savefile += "\\Arams.bin";

	CFile theFile;
	if (theFile.Open(savefile, CFile::modeCreate | CFile::modeWrite)) {
		CArchive archive(&theFile, CArchive::store);
		Serialize(archive);
		archive.Close();
		theFile.Close();
		return;
	}
}

// YFL: Never call this directly [2/7/2003]
void CArams::Serialize(CArchive& ar) {
	short magic;
	if (ar.IsStoring()) {
		magic = 1;
		ar << magic;
		int state;
		state = (int)Attr.CurrentState;
		ar << state;
		ar << Attr.Engineering;
		ar << Attr.NonScheduled;
		ar << Attr.Productive;
		ar << Attr.ScheduledDown;
		ar << Attr.Standby;
		ar << Attr.UnscheduledDown;
		ar << StartDate;
		ar << LastDate;
		ar << Attr.bShutDown;
		ar << Attr.bEqpSetDownTime;
	}
	else {
		ar >> magic;
		int state;
		ar >> state;
		Attr.CurrentState = (SArams::ARAMSSTATE)state;
		if (Attr.CurrentState == SArams::PRODUCTIVE) {
			Attr.CurrentState = SArams::STANDBY;
		}
		ar >> Attr.Engineering;
		ar >> Attr.NonScheduled;
		ar >> Attr.Productive;
		ar >> Attr.ScheduledDown;
		ar >> Attr.Standby;
		ar >> Attr.UnscheduledDown;
		ar >> StartDate;
		ar >> LastDate;
		ar >> Attr.bShutDown;
		ar >> Attr.bEqpSetDownTime;
	}
}

void CArams::UpdateState() {
	CSingleLock Lock(&CS);
	Lock.Lock();
	CTime nowTime;
	nowTime = CTime::GetCurrentTime();
	switch (Attr.CurrentState) {
	case  SArams::PRODUCTIVE:
		Attr.Productive += (difftime(nowTime.GetTime(), LastDate.GetTime()));
		break;
	case  SArams::STANDBY:
		Attr.Standby += (difftime(nowTime.GetTime(), LastDate.GetTime()));
		break;
	case SArams::ENGINEERING:
		Attr.Engineering += (difftime(nowTime.GetTime(), LastDate.GetTime()));
		break;
	case SArams::NONSCHEDULED:
		Attr.NonScheduled += (difftime(nowTime.GetTime(), LastDate.GetTime()));
		break;
	case SArams::SCHEDULEDDOWN:
		Attr.ScheduledDown += (difftime(nowTime.GetTime(), LastDate.GetTime()));
		break;
	case SArams::UNSCHEDULEDDOWN:
		Attr.UnscheduledDown += (difftime(nowTime.GetTime(), LastDate.GetTime()));
		break;
	}
	LastDate = nowTime;
}

BOOL CArams::IsProductiveState() {
	switch (Attr.CurrentState) {
	case SArams::PRODUCTIVE:
	case  SArams::STANDBY:
		return TRUE;
		break;
	}
	return FALSE;
}

void CArams::SetCurrentState(SArams::ARAMSSTATE state, BOOL bEqpSet) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	Attr.CurrentState = state;
	Attr.bEqpSetDownTime = bEqpSet;
}

BOOL CArams::IsUnscheduledDown() {
	if (Attr.CurrentState == SArams::UNSCHEDULEDDOWN) {
		return TRUE;
	}
	return FALSE;
}

BOOL CArams::IsEqpSetDownTime() {
	return Attr.bEqpSetDownTime;
}

void CArams::SetAttr(SArams& Ar) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	Attr = Ar;
}

SArams::ARAMSSTATE CArams::GetCurrentState() {
	return Attr.CurrentState;
}

CString CArams::GetCurrentStateStr() {
	switch (Attr.CurrentState) {
	case SArams::STANDBY:
		return "Standby";
		break;
	case SArams::PRODUCTIVE:
		return "Productive";
		break;
	case SArams::SCHEDULEDDOWN:
		return "Scheduled Down";
		break;
	case SArams::UNSCHEDULEDDOWN:
		return "Unscheduled Down";
		break;
	case SArams::NONSCHEDULED:
		return "Non-scheduled";
		break;
	case SArams::ENGINEERING:
		return "Engineering";
		break;
	}
	return "?";
}

// NOTE: Only called by ARAMS Server YFL [2/7/2003]
void CArams::Init(CString filename) {
	if (!Load(filename)) {
		Save(filename);
	}

	SArams::ARAMSSTATE backState;
	if ((Attr.CurrentState == SArams::PRODUCTIVE) ||
		Attr.CurrentState == SArams::STANDBY) {
		// count program shut down time as NONSCHEDULED if it is not down
		backState = Attr.CurrentState;
		Attr.CurrentState = SArams::NONSCHEDULED;
		UpdateState();
		Attr.CurrentState = backState;
	}
	if (Attr.CurrentState == SArams::NOSTATE) {
		Attr.CurrentState = SArams::STANDBY;
		UpdateState();
	}
	UpdateState();

	LastDate = CTime::GetCurrentTime();
	Attr.bShutDown = FALSE;
}

// Only called by ARAMS server
void CArams::Deinit(CString filename) {
	Attr.bShutDown = TRUE;
	UpdateState();
	Save(filename);
}

BOOL CArams::IsEngineeringState() {
	switch (Attr.CurrentState) {
	case SArams::ENGINEERING:
		return TRUE;
		break;
	}
	return FALSE;
}

BOOL CArams::IsNonScheduleState() {
	switch (Attr.CurrentState) {
	case SArams::NONSCHEDULED:
		return TRUE;
		break;
	}
	return FALSE;
}

BOOL CArams::IsOperatable() {
	switch (Attr.CurrentState) {
	case SArams::PRODUCTIVE:
	case SArams::STANDBY:
	case SArams::ENGINEERING:
		return TRUE;
		break;
	}
	return FALSE;
}