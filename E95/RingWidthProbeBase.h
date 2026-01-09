#pragma once
// 08132024 Mahedi Kamal // added class
class CRingWidthProbeBase
{
public:
	CRingWidthProbeBase() {}
	virtual ~CRingWidthProbeBase() {
		Deinitialize();
	}

	virtual BOOL Deinitialize() {
		return TRUE;
	}
	virtual BOOL Initialize(CListBox* pMessage) {
		pMessage->InsertString(pMessage->GetCount(), "Ring Width system (simu) started OK");
		return TRUE;
	}
};

