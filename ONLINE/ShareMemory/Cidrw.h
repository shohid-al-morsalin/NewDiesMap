#pragma once

#include "ONLINE/GEM/OSSBASE.H"
#include "ONLINE/GEM/SecsItem.h"
#include "ReadWriteHead.h"
#include <map>

/*
	Notes : 1) The CIDRW can be ReadOnly or ReadWrite
			2) The CIDRW can have all heads of Single Type.
			3) The CIDRW can have heads for RFID,BCR etc...
			4) Not designed for inherited.

*/
class CCidrw : public COSSBASE {
public:
	enum CIDRW_STATE {
		NO_CIDRW, cidINITIALIZING, cidRUNNING
	};
	enum RUNNING_SUBSTATES {
		NOT_RUNNING, cidOPERATING = cidRUNNING, MAINT
	};
	enum ALARM_STATE {
		NOT_ALARM_STATE, NO_ALARMS = cidRUNNING, ALARMS
	};
	enum OPER_STATE {
		brNOT_OPERATING, brIDLE = cidOPERATING, brBUSY, brMAINTENANCE
	};
	enum RW_TYPE {
		READ_ONLY, READ_WRITE
	};
	enum ERR_STATUS {
		NORMAL, EX_ERR, COMM_ERR, HW_ERR, TAG_ERR
	};
private:
	/**Attributes required for E99*/
	CIDRW_STATE			m_CidrwState;
	RUNNING_SUBSTATES	m_RunSubState;
	ALARM_STATE			m_AlarmStatus;		/**Required Current Alarm State			*/
	OPER_STATE			m_OperationStat;	/**Required Current Operation Status	*/
	ERR_STATUS			m_Err;
	/**Implementation*/
	std::vector<CRWHead*>	m_Heads;		/**ReadWriteHeads						*/
	RW_TYPE				m_Type;
	void GetAllAttribIDs(std::vector<string>& AttribIDs);
	std::map<std::string, int> AttribMap;
	int InitAttributes(void);
	bool IsValidAtrib(const std::string&);
	bool GetAttributes(std::vector<string>& Attribs, SecsItemList* pAttrib, SecsItemList* pRes);
	bool GetAttribute(const std::string& Attrib, std::vector<SecsItem*>&);
	void init();
	// Should use ??
	virtual BOOL  GetAttr(CString service, void* buf, short len) {
		return TRUE;
	}
	virtual BOOL  SetAttr(CString service, void* val) {
		return TRUE;
	}
	virtual void  GetAttrName(CStringArray& nArray) {};
	virtual void  ClearAll() {};
	void GetPMInfo(SecsItemList* pStat);
	void GetAlarmStat(SecsItemList* pStat);
	void GetOpStat(SecsItemList* pStat);
	void GetHeadStat(SecsItemList* pStat);
	bool m_bMaintReqd;
	bool PerformDiag(const string& Action/*,CRWHead::HEAD_STATE &Stat*/, SecsItemList*);
	bool GetStatus(SecsItemList*);
	bool IsInit() { return (m_CidrwState == cidINITIALIZING); };
	bool IsRunning() { return m_CidrwState == cidRUNNING; };
	bool IsOper() { return (m_RunSubState == cidOPERATING); };
	bool IsMaint() { return (m_OperationStat == brMAINTENANCE); };
	bool IsIdle() { return (m_OperationStat == brIDLE); };
	bool IsBusy() { return (m_OperationStat == brBUSY); };
public:
	bool ProcessCmd(int HeadID, const string&, const vector<string>&, SecsItemList*);
	void GetErrCode(string& Err);
	CCidrw();
	virtual ~CCidrw();
	/**Services for E99 (Public interface)*/

	/** ChangeState Service S18F13 with SSCMD ="ChangeState",TargetID = "00" */
	bool ChangeState(OPER_STATE NewState, SecsItemList* pStat);

	/** GetAttributes Service S18F1 */
	bool GetAttributes(int HeadID, vector<string>& Attribs, SecsItemList* pAttrib, SecsItemList* pRes);

	/** GetStatus Service S18F13  with SSCMD ="GetStatus",TargetID = 0/1/2/.../n*/
	bool GetStatus(UINT HeadID, SecsItemList*);

	/** Perform Diagnostics Service S18F13  with SSCMD ="PerformDiagnostics",TargetID = 0/1/2/.../n*/
	bool PerformDiagnostics(int HeadID, const string& Action/*,CRWHead::HEAD_STATE &Stat*/, SecsItemList*);

	/** ReadData Service S18F5  */
	bool ReadDataSeg(UINT HeadID, string& CarrierID, int DataSize, int DataSeg);

	/** ReadID Service S18F9  */
	bool ReadID(UINT HeadID, string& CarrierID, SecsItemList*);

	/** GetStatus Service S18F13  with SSCMD ="Reset"*/
	bool Reset(SecsItemList*);

	/** SetAttributes Service S18F3  */
	bool SetAttributes(int HeadID, const vector<string>&, const vector<string>&, SecsItemList*);

	/** WriteData Service S18F7 */
	bool WriteDataSeg(UINT HeadID, const string& CarrierID, int DataSize, int DataSeg, SecsItemList*);

	/** WriteID Service S18F11 */
	bool WriteID(UINT HeadID, const string& CarrierID, SecsItemList*);
	/** End Services Interface */

	/***Iterface with Equipment */

	/*** General Utility **/
	bool SetRunning();
	bool SetOperating();
	bool SetMaint();
	bool SetIdle();
	bool SetBusy();
	bool SetInit();
	/*** End General Utility **/

	bool SetAlarmStatP(bool bNewStat);
	bool GetAlarmStatP();
};
