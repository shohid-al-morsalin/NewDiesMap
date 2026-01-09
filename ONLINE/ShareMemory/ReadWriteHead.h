// ReadWriteHead.h: interface for the CReadWriteHead class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "../GEM/OSSBASE.h"
#include "../GEM/SecsItem.h"
#include <map>
#include <vector>
#include <string>
using namespace std;
enum { NORMAL_OPERATION, EXECUTION_ERR, COMM_ERR, HW_ERR, TAG_FAULT };

class CRWHead : public COSSBASE {
public:
	enum HEAD_STATE { NO_CIDRW, CIDRW_HEAD };
	enum OPER_STATE { NO_STATE, OPERATING = CIDRW_HEAD, NOT_OPERATING };
	enum OPER_SUBSTATES { bsNO_SUBSTATE, bsIDLE, bsBUSY };
	enum HEAD_COND {
		NO_ALARMS,
		NEEDS_MAINT,
		READ_WRITE_FAULT,
		READ_WRITE_RATE_FAULT,
		NO_POWER
	};
	enum HEAD_TYPE { BCR, RFID };
	CRWHead(int HeadNum);
	virtual ~CRWHead();
	string		m_HeadID;				/**R Head Number	0-31			*/
	virtual bool PerformDiag(const string& Action/*,CRWHead::HEAD_STATE &Stat*/, SecsItemList*) = 0;
	virtual bool ReadData(string& Data, int DataSeg, UINT DataSize) = 0;
	virtual bool WriteData(const string& Data, int DataSeg, UINT DataSize) = 0;
	virtual bool ReadID(string& CarrierID) = 0;
	virtual bool WriteID(const string& CarrierID) = 0;
	virtual bool GetAttributes(vector<string>& Attribs, SecsItemList* pAttrib, SecsItemList* pRes) = 0;

	virtual bool WriteDataSeg(const string&, int, int, SecsItemList* pStat) = 0;
	virtual bool WriteID(const string& CarrierID, SecsItemList* pStat) = 0;

	virtual bool ReadDataSeg(string& Data, int DataSize, int DataSeg) = 0;
	virtual bool ReadID(string& CarrierID, SecsItemList*) = 0;
	bool SetBusy();
	bool SetIdle();
	bool AlarmPresent();
	bool AlarmCleared();
	void GetPMInfo(SecsItemList* pStat);
	void GetAlarmStat(SecsItemList* pStat);
	void GetOpStat(SecsItemList* pStat);
	void GetHeadStat(SecsItemList* pStat);
	void GetStatus(SecsItemList*);
protected:
	HEAD_STATE		m_HeadStatus;			/**R The Current State				*/
	UINT			m_NumCycles;			/**O Number of read WriteOperations	*/
	SYSTEMTIME		m_HeadDateInstalled;	/**O Date This Head was installed	*/
	HEAD_COND		m_HeadCondition;		/**O Current Maintenance state		*/
	string			m_MaintData;			/**O Maintenance data				*/
	OPER_STATE		m_OperatingStatus;
	OPER_SUBSTATES	m_OpSubStates;
	map<string, int> AttribMap;
	int InitAttributes(void);
	bool IsValidAtrib(const string&);
	void GetAllAttribIDs(vector<string>& AttribIDs);
	virtual bool GetAttribute(const string& Attrib, vector<SecsItem*>& Res) = 0;;
};

class  RWHeadBcr : public CRWHead {
public:
	virtual BOOL  GetAttr(CString service, void* buf);
	bool GetAttributes(vector<string>& Attribs, SecsItemList* pAttrib, SecsItemList* pRes);
	virtual BOOL  SetAttr(CString service, void* val);
	virtual void  GetAttrName(CStringArray& nArray);
	virtual void  ClearAll();
	RWHeadBcr(int HeadNum);
	virtual ~RWHeadBcr();
	virtual bool ReadData(string& Data, int DataSeg, UINT DataSize);
	virtual bool WriteData(const string& Data, int DataSeg, UINT DataSize);
	virtual bool ReadID(string& CarrierID);
	virtual bool WriteID(const string& CarrierID);
	bool PerformDiag(const string& Action,/*CRWHead::HEAD_STATE &Stat, */SecsItemList*);
	int InitBcrAttribs(void);
	bool GetAttribute(const string& Attrib, vector<SecsItem*>& Res);

	bool WriteDataSeg(const string&, int, int, SecsItemList* pStat);
	bool WriteID(const string& CarrierID, SecsItemList* pStat);

	bool ReadDataSeg(string& Data, int DataSize, int DataSeg);
	bool ReadID(string& CarrierID, SecsItemList*);
};

class  RWHeadRfid : public CRWHead {
public:
	virtual BOOL  GetAttr(CString service, void* buf);
	bool GetAttributes(vector<string>& Attribs, SecsItemList* pAttrib, SecsItemList* pRes);
	virtual BOOL  SetAttr(CString service, void* val);
	virtual void  GetAttrName(CStringArray& nArray);
	virtual void  ClearAll();
	RWHeadRfid(int HeadNum);
	virtual ~RWHeadRfid();
	virtual bool ReadData(string& Data, int DataSeg, UINT DataSize);
	virtual bool WriteData(const string& Data, int DataSeg, UINT DataSize);
	virtual bool ReadID(string& CarrierID);
	virtual bool WriteID(const string& CarrierID);
	bool PerformDiag(const string& Action,/*CRWHead::HEAD_STATE &Stat,*/ SecsItemList*);
	int InitRfidAttribs(void);
	bool GetAttribute(const string& Attrib, vector<SecsItem*>& Res);

	bool WriteDataSeg(const string&, int, int, SecsItemList* pStat);
	bool WriteID(const string& CarrierID, SecsItemList* pStat);

	bool ReadDataSeg(string& Data, int DataSize, int DataSeg);
	bool ReadID(string& CarrierID, SecsItemList*);
};