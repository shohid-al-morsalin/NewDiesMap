#pragma once
#include "RingWidthProbeBase.h"
// 08132024 Mahedi Kamal // added class
class CRingWidthProbe : public CRingWidthProbeBase
{
public:
	BOOL Deinitialize();
	BOOL Initialize(CListBox* pMessage);
	CRingWidthProbe() {}
	~CRingWidthProbe() {
		Deinitialize();
	}
};

