#include "stdafx.h"
#include "Status.h"

short Equipment::GGetEquipStatus()
{
	switch(Status.OpMode) {
	case EqpStatus::IDLE:
		return 1;
		break;
	case EqpStatus::WORKING:
		return 2;
		break;
	case EqpStatus::MAINT:
		return 3;
		break;
	case EqpStatus::INIT:
		return 4;
		break;
	case EqpStatus::SETUP:
		return 5;
		break;
	}
	return 0;
}

CString Equipment::GetEquipStatusStr()
{
	switch(Status.OpMode) {
	case EqpStatus::IDLE:
		return "Idle";
		break;
	case EqpStatus::WORKING:
		return "Working";
		break;
	case EqpStatus::MAINT:
		return "Maintainence";
		break;
	case EqpStatus::INIT:
		return "Initializing";
		break;
	case EqpStatus::SETUP:
		return "Setting up";
		break;
	}
	return "?";
}
