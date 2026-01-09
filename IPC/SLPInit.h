#pragma once
struct SLPInit {
	short unit, action;
	CString CarrierID;
	CString RecipeName;
	BOOL bNoClose[4];  // Max 4 load port

	SLPInit() {
		unit = 0;
		action = 0;
		CarrierID = "";
		RecipeName = "";
		for (int i = 0; i < 4; i++) {
			bNoClose[i] = FALSE;
		}
	}
};
