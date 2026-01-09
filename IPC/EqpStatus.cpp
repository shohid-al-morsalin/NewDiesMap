#include "stdafx.h"

#include "EqpStatus.h"
#include "GlobalDeviceContainer.h"

CString EqpStatus::GetControlStateStr(CONTROLSTATE state) {
	if (pGDev->SubModel == CGlobalDeviceContainer::UMC) {
		switch (state) {
		case 0:
			return "NO STATE";
		case 1:
			return "OFFLINE EQP";
		case 2:
			return "ATTEMPT ONLINE";
		case 3:
			return "OFFLINE HOST";
		case 4:
			return "LOCAL";
		case 5:
			return "REMOTE";
		}
	}
	else {
		switch (state) {
		case NOSECS:
			return "NO STATE";
		case REMOTE:
			return "REMOTE";
		case LOCAL:
			return "LOCAL";
		case OFFLINEEQP:
			return "OFFLINE EQP";
		case OFFLINEHOST:
			return "OFFLINE HOST";
		case ATTEMPTONLINE:
			return "ATTEMPT ONLINE";
		}
	}
	return "UNKNOWN";
}

BOOL EqpStatus::IsOnline() {
	switch (LineMode) {
	case LOCAL:
	case REMOTE:
		return TRUE;
		break;
	}
	return FALSE;
};