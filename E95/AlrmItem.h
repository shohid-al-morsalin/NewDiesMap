#pragma once

#define ALMTXTLEN			80

class CAlrmItem {
public:
	void SetCEID(int SetAlrm, int ClrAlrm, int EnSetAlrm, int EnClrAlrm);
	void Set(int id, char* idTx);
	void Preset();
	enum ALCD {
		RESET = 0x00, SET = 0x80 //  [9/16/2007 LYF]
	};

	ALCD alCD;  // Alarm status [7/28/2010 Yuen]
	int alID;
	char alTX[ALMTXTLEN + 1];

	BOOL alED;					/** Enable/disable code for this alarm */
	UINT SetAlrmCEID;			/** CEID linked to set of this alarm */
	UINT ClearAlrmCEID;			/** CEID linked with clearing of this alarm */
	BOOL EnableSetAlrmCEID;		/** Reporting status enable/disable code for CEID SetEvtID */
	BOOL EnableClearAlrmCEID;	/** Reporting status enable/disable code for CEID ClearEvtID */

// 	CAlrmItem();
// 	virtual ~CAlrmItem();
};
