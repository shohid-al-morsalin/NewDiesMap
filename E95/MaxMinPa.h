// MaxMinPa.h: interface for the CMaxMinPa class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
#include "MParam.h"
#include "MaxMin.h"
#include "Recipe413.h"

class CMaxMinPa {
public:
	CMaxMin MM[/*MAXPATTERNCNT**/MaXPA + 6];

	CMaxMinPa();
	virtual ~CMaxMinPa();
};