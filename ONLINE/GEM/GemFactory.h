// GemFactory.h: interface for the CGemFactory class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "SecsMessage.h"
#include "CediListCtroller.h"
#include "ONLINE/general_utils.h"
#include "ONLINE/INC/Secshead.h"
#include "ONLINE/ShareMemory/DataBank.h"
#include "ONLINE/ShareMemory/SharedMemQue.h"
#include "ONLINE/ShareMemory/ShareMemory.h"

#include <vector>
using namespace std;

#define ECIDTXTLEN	128

class CMeParam;
class CRecipe413;
class SecsItemList;
class DataConverter;

struct MParam;

typedef struct tagSV {
	int m_TYPE;
	int m_DataSize;

	union // it should be union otherwise it will fail
	{
		char			    m_ASCII[75];
		char                m_BINARY;
		unsigned __int64	m_UNSIGNED8;
		UINT                m_UNSIGNED4;
		unsigned short      m_UNSIGNED2;
		unsigned char       m_UNSIGNED1;
		UINT                m_UINT;
		char			    m_INT1;
		short			    m_INT2;
		int				    m_INT4;
		__int64			    m_INT8;
		bool                m_BOOLEAN;
		float               m_Float4;
		double              m_Float8;
	};

	BYTE buffer[64];
}
SVID_VALUE;

class CGemRtn {
public:
	BOOL bOK1, bOK2;
	BOOL bIsEcho, bIsThinF, bIsTrim;

	CGemRtn() {
		bOK1 = bOK2 = bIsEcho = bIsThinF = bIsTrim = FALSE;
	}
};

class CGemFactory {
	char ctmpStr[128];  // temporary variable [8/1/2010 XPMUser]
	CString tmpStr;  // temprary variable [8/1/2010 XPMUser]
	BOOL bJobAlertSupport; //  [9/6/2007 LYF]
	BOOL bJobEventSupport; //  [9/6/2007 LYF]

	BOOL SendData(HWND ReplyWnd, string& Data);
public:

	CSharedMemQue m_gEvtQue;
	static CPtrList m_pTraceList; // Time Tracer S6F1

public:
	SecsItemList* GenMeaTimeout(CRecipe413* pRcp);
	SecsItemList* GenPatMatParam(CRecipe413* pRcp);
	SecsItemList* GenSubSamplingParam(CRecipe413* pRcp);
	SecsItemList* GenMeSetAll(CRecipe413* pRcp);
	SecsItemList* GenTrimParam(CRecipe413* pRcp);
	SecsItemList* GenThinFParam(CRecipe413* pRcp);
	SecsItemList* GenEchoParam(CRecipe413* pRcp);
	SecsItemList* GenMeSet(short i, CMeParam* pMe, CRecipe413* pRcp, CGemRtn& GR);
	SecsItemList* GenMeItem(short n, MParam* pM, CGemRtn& GR);
	SecsItemList* GenMeItemEcho(MParam* pM);
	static SecsItemList* ResultWafer(CEvt* pEvt, DataConverter& Data, UINT& Type);
	static SecsItemList* ResultSingle(CEvt* pEvt, DataConverter& Data, UINT& Type);
	void Clamp(short port);
	int mk_S2F22(GEM_HEADER* pHeader);
	int EventCMS(short phase, CEvt* pEvt, GEM_HEADER& shead);
	int EventLineMode(short phase, CEvt* pEvt, GEM_HEADER& shead);
	char* ToChar(double val);
	char* ToChar(float val);
	char* ToChar(UINT val);
	char* ToChar(int val);
	static BOOL MakeSlotMap(char* buf, short PortNo);
	SecsItem* MakeReport(CEvt* pEvt, UINT CEID, UINT Id);
	static sScanData LastScan;
	BOOL CanCommunicate(BYTE Stream, BYTE Function);
	static void Log(CString msg, short lvl = 5);
	short UIMode;
	void EventSink(short EventID, short Make);
	int mk_S18F14(GEM_HEADER* GHead);
	int mk_S18F12(GEM_HEADER* GHead);
	int mk_S18F10(GEM_HEADER* GHead);
	int mk_S18F8(GEM_HEADER* GHead);
	int mk_S18F6(GEM_HEADER* GHead);
	int mk_S18F4(GEM_HEADER* GHead);
	int mk_S18F2(GEM_HEADER* GHead);
	int Stream18(GEM_HEADER*);
	int Stream7(GEM_HEADER* pHeader);
	int mk_S3F28(GEM_HEADER* pHeader);
	BOOL LogMessages(std::string Message, char* File = NULL, int Line = 0, MsgType Type = INFO_MESSAGE);
	BOOL LogMessages(CString& Message);
	void GetObjectAttr(UINT nType, vector < string >& ObjID, vector < string >& AttrIDs, SecsMessage& OutMsg);

	BOOL GetPrJobAttr(string PrJobId, string AttrID, SecsItemList* pList);
	BOOL GetCjobAttr(const string& PrJobId, string AttrID, SecsItemList* pList);
	BOOL GetCarrierAttr(string CarrierID, string AttrID, SecsItemList* pList);
	BOOL GetSubstAttr(string SubstID, string AttrID, SecsItemList* pList);
	BOOL GetSubstLocAttr(string SubstID, string AttrID, SecsItemList* pList);

	BOOL GetPrJobObj(string& PrJobId);
	BOOL GetCjobObj(string& PrJobId);
	BOOL GetCarrierObj(string& CarrierID);
	BOOL GetSubstObj(string& SubstID);
	BOOL GetSubstLocObj(string& SubstLocID);

	int SetObjectAttr(UINT nType, vector < string >& ObjID, vector < string >& AttrIDs, vector < SecsItem* > AttrValue,
		SecsItemList* pResultList, SecsItemList* pErrorList);

	int SetSubstAttr(string SubstID, string AttrID, SecsItem* pAttrValue);

	void GetEcnrData(vector < UINT >& EcidLst, SecsMessage& OutMsg);
	int CjobEventNotify(short phase, CEvt* event, GEM_HEADER* shead);
	static void GetCurrTime(string& TimeStr);
	int mk_S3F22(GEM_HEADER* pHeader);
	int mk_S3F24(GEM_HEADER* pHeader);
	int mk_S16F7(GEM_HEADER* shead, CEvt* ehead);
	int mk_S3F26(GEM_HEADER* pHeader);
	void GetEcData(vector < UINT >& SvList, SecsMessage& OutMsg);
	void GetSvData(vector < UINT >& SvList, SecsMessage& OutMsg);
	void GetSvStat(vector < UINT >& SvList, SecsMessage& OutMsg);
	short SetSecsTime(char* TimeVal, int StrLen);
	int mk_S6F24(GEM_HEADER* shead);
	int mk_S2F44(GEM_HEADER* pHeader);
	int Other_Events(short phase, CEvt* event, GEM_HEADER* shead);
	int PrJobAlert_Notify(short phase, CEvt* event, GEM_HEADER* shead);
	int PrJobAlert_Notify2(short phase, CEvt* event, GEM_HEADER* shead);
	int evt_attempt_local(int phase, CEvt* event, GEM_HEADER* shead);
	int evt_init_data(int phase, CEvt* event, GEM_HEADER* shead);
	int mk_S9F9(GEM_HEADER* shead);
	int evt_Reply_Timeout(int phase, CEvt* event, GEM_HEADER* shead);
	int mk_S9F3(GEM_HEADER* shead);
	int mk_S1F1(GEM_HEADER* shead, CEvt* ehead);
	int evt_attempt_online(int phase, CEvt* event, GEM_HEADER* shead);
	int evt_ETS_MSG_Recognized(short phase, CEvt* event, GEM_HEADER* shead);
	int evt_ETS_SendMsgToHost(short phase, CEvt* event, GEM_HEADER* shead);
	int mk_S5F6(GEM_HEADER* pHeader);
	int mk_S5F4(GEM_HEADER* pHeader);
	short GetDeviceFromHost(GEM_HEADER* pHeader);
	int mk_S9F1(GEM_HEADER* shead);
	int mk_S6F22(GEM_HEADER* shead);
	int mk_S6F20(GEM_HEADER* shead);
	int mk_S6F16(GEM_HEADER* shead);
	int mk_S1F12(GEM_HEADER* shead);
	int mk_S2F17(GEM_HEADER* shead);
	BOOL set_Sparameter(int rno, CString pVal);
	static BOOL GetPassiveIPaddress(char* ip);
	static BOOL SetPassiveIPaddress(const char* ip);

	static SecsItem* get_svval(short PortID, unsigned int svid, CEvt* p);
	short check_ecid(UINT ecid, const char* pVal);
	BOOL set_ecval(UINT ecid, const char* pVal);
	BOOL Check_Trid(UINT trid);
	BOOL DeleteTrid(UINT trid);

	int Stream1(GEM_HEADER* pHeader);
	int Stream2(GEM_HEADER* pHeader);
	int Stream3(GEM_HEADER* pHeader);
	int Stream5(GEM_HEADER* pHeader);
	int Stream6(GEM_HEADER* pHeader);
	int Stream10(GEM_HEADER* pHeader);
	int Stream14(GEM_HEADER* pHeader);
	int Stream16(GEM_HEADER* pHeader);

	int mk_S1F2(GEM_HEADER* pHeader);
	int mk_S1F4(GEM_HEADER* pHeader);
	int mk_S1F13(GEM_HEADER* shead, CEvt* ehead);
	int mk_S1F14(GEM_HEADER* shead);
	int mk_S1F16(GEM_HEADER* shead);
	int mk_S1F18(GEM_HEADER* shead);

	int mk_S2F14(GEM_HEADER* pHeader);
	int mk_S2F16(GEM_HEADER* pHeader);
	int mk_S2F18(GEM_HEADER* pHeader);
	int mk_S2F24(GEM_HEADER* pHeader);
	int mk_S2F26(GEM_HEADER* pHeader);
	int mk_S2F30(GEM_HEADER* pHeader);
	int mk_S2F32(GEM_HEADER* pHeader);
	int mk_S2F34(GEM_HEADER* pHeader);
	int mk_S2F36(GEM_HEADER* pHeader);
	int mk_S2F38(GEM_HEADER* pHeader);
	int mk_S2F40(GEM_HEADER* pHeader);
	int mk_S6F6(GEM_HEADER* pHeader);

	int mk_S2F42(GEM_HEADER* pHeader);
	int mk_S3F18(GEM_HEADER* pHeader);

	int mk_S5F1(GEM_HEADER* pHeader, CEvt* ehead);
	int mk_S5F8(GEM_HEADER* pHeader);

	int mk_S6F1(GEM_HEADER* shead, CEvt* ehead);
	int mk_S6F11(GEM_HEADER* shead, CEvt* ehead);

	int mk_S7F1(GEM_HEADER* shead, CEvt* Evt);
	int mk_S7F2(GEM_HEADER* shead);
	int mk_S7F3(GEM_HEADER* shead, CEvt* Evt);
	int mk_S7F4(GEM_HEADER* shead);
	int mk_S7F5(GEM_HEADER* shead, CEvt* Evt);
	int mk_S7F6(GEM_HEADER* shead);
	int mk_S7F18(GEM_HEADER* shead);
	int mk_S7F20(GEM_HEADER* shead);
	int mk_S7F26(GEM_HEADER* shead);

	int mk_S9F5(GEM_HEADER* shead);
	int mk_S9F7(GEM_HEADER* shead);
	int mk_S9F11(GEM_HEADER* shead, CEvt* pEvt);

	//int mk_S10F2(GEM_HEADER * pHeader);
	int mk_S10F1(GEM_HEADER* pHeader, CEvt* ehead);
	int mk_S10F4(GEM_HEADER* pHeader);
	int mk_S10F6(GEM_HEADER* pHeader);
	int mk_S10F7(GEM_HEADER* shead);
	int mk_S10F10(GEM_HEADER* pHeader);

	int mk_S14F2(GEM_HEADER* pHeader);
	int mk_S14F12(GEM_HEADER* pHeader);
	int mk_S14F10(GEM_HEADER* pHeader);
	int mk_S14F8(GEM_HEADER* pHeader);
	int mk_S14F6(GEM_HEADER* pHeader);
	int mk_S14F4(GEM_HEADER* pHeader);

	int mk_S16F12(GEM_HEADER* pHeader);
	int mk_S16F6(GEM_HEADER* pHeader);
	int mk_s16f9(GEM_HEADER* shead, CEvt* ehead);
	int mk_S16F14(GEM_HEADER* pHeader);
	int mk_S16F16(GEM_HEADER* pHeader);

	int mk_S16F18(GEM_HEADER* pHeader);
	int mk_S16F20(GEM_HEADER* pHeader);
	int mk_S16F22(GEM_HEADER* pHeader);
	int mk_S16F28(GEM_HEADER* shead);
	int mk_S16F26(GEM_HEADER* pHeader);
	int mk_S16F4(GEM_HEADER* pHeader);
	int mk_S16F2(GEM_HEADER* pHeader);

	int	mk_S18F00(GEM_HEADER* shead);
	int mk_S16F00(GEM_HEADER* shead);
	int mk_S14F00(GEM_HEADER* shead);
	int mk_S10F00(GEM_HEADER* shead);
	int mk_S07F00(GEM_HEADER* shead);
	int mk_S06F00(GEM_HEADER* shead);
	int mk_S05F00(GEM_HEADER* shead);
	int mk_S04F00(GEM_HEADER* shead);
	int mk_S03F00(GEM_HEADER* shead);
	int mk_S02F00(GEM_HEADER* shead);
	int mk_S01F00(GEM_HEADER* shead);

	int evt_timetracer(int phase, CEvt* event, GEM_HEADER* shead); //

	int evt_line_agvon(int phase, CEvt* event, GEM_HEADER* shead); //
	int evt_line_agvoff(int phase, CEvt* event, GEM_HEADER* shead); //
	int evt_line_bcron(int phase, CEvt* event, GEM_HEADER* shead); //
	int evt_line_bcroff(int phase, CEvt* event, GEM_HEADER* shead); //
	int evt_line_localevt(int phase, CEvt* event, GEM_HEADER* shead); //
	int evt_line_localonline(int phase, CEvt* event, GEM_HEADER* shead); //
	int evt_line_remoteevt(int phase, CEvt* event, GEM_HEADER* shead); //
	int evt_line_remoteonline(int phase, CEvt* event, GEM_HEADER* shead);
	int evt_line_offline(int phase, CEvt* event, GEM_HEADER* shead);
	int evt_line_hostoff(int phase, CEvt* event, GEM_HEADER* shead);
	int evt_line_commestablish(int phase, CEvt* event, GEM_HEADER* shead);
	int ProcessUIDataRequest(CEvt* event);
	int SendVARData(CEvt* event);
	int SendCEIDData(CEvt* event);
	int SendCJData(CEvt* event);
	int SendPJData(CEvt* event);
	int DumpVidData(CEvt* event);
	int DumpCeidData(CEvt* event);

	int evt_cms_cassettenopresent(int phase, CEvt* event, GEM_HEADER* shead);
	int evt_cms_cassettepresent(int phase, CEvt* event, GEM_HEADER* shead);
	int evt_cms_cassettefilenopresent(int phase, CEvt* event, GEM_HEADER* shead);
	int evt_cms_cassettefilepresent(int phase, CEvt* event, GEM_HEADER* shead);
	int evt_cms_slotmapverify(int phase, CEvt* event, GEM_HEADER* shead);
	int evt_cms_inservice(int phase, CEvt* event, GEM_HEADER* shead);
	int evt_cms_outofservice(int phase, CEvt* event, GEM_HEADER* shead);
	int evt_cms_maintenance(int phase, CEvt* event, GEM_HEADER* shead);
	int evt_cms_working(int phase, CEvt* event, GEM_HEADER* shead);
	int evt_cms_idle(int phase, CEvt* event, GEM_HEADER* shead);
	int evt_cms_watingforhostcarrierid(int phase, CEvt* event, GEM_HEADER* shead);
	int evt_cms_watingforhostslotmap(int phase, CEvt* event, GEM_HEADER* shead);

	int evt_cms_readytoload(int phase, CEvt* event, GEM_HEADER* shead);
	int evt_cms_transferblocked(int phase, CEvt* event, GEM_HEADER* shead); //
	int evt_cms_loadingcompleted(int phase, CEvt* event, GEM_HEADER* shead);
	int evt_cms_readcarrieridcompleted(short phase, CEvt* event, GEM_HEADER* shead);
	int evt_cms_idverifyok(int phase, CEvt* event, GEM_HEADER* shead);
	int evt_cms_dockingcomplete(int phase, CEvt* event, GEM_HEADER* shead);
	int evt_cms_foupopen(int phase, CEvt* event, GEM_HEADER* shead);
	int evt_cms_foupopenfail(int phase, CEvt* event, GEM_HEADER* shead);
	int evt_cms_foupclose(int phase, CEvt* event, GEM_HEADER* shead);
	int evt_cms_undockingcomplete(int phase, CEvt* event, GEM_HEADER* shead);
	int evt_cms_slotmapReadOk(int phase, CEvt* event, GEM_HEADER* shead);
	int evt_cms_readytounload(int phase, CEvt* event, GEM_HEADER* shead);
	int evt_cms_carrierunclamped(int phase, CEvt* event, GEM_HEADER* shead);
	int evt_cms_unloadcomplete(int phase, CEvt* event, GEM_HEADER* shead);
	int evt_cms_carriercancel(int phase, CEvt* event, GEM_HEADER* shead);
	int evt_cms_carriercomplete(int phase, CEvt* event, GEM_HEADER* shead);
	int evt_cms_waferpickup(int phase, CEvt* event, GEM_HEADER* shead);
	int evt_cms_wafermeasurefinished(int phase, CEvt* event, GEM_HEADER* shead);
	int evt_cms_waferreturnback(int phase, CEvt* event, GEM_HEADER* shead);

	int evt_remote_stop(int phase, CEvt* event, GEM_HEADER* shead);
	int evt_remote_pause(int phase, CEvt* event, GEM_HEADER* shead);
	int evt_remote_resume(int phase, CEvt* event, GEM_HEADER* shead);
	int evt_remote_abort(int phase, CEvt* event, GEM_HEADER* shead);
	int evt_remote_cancel(int phase, CEvt* event, GEM_HEADER* shead);
	int evt_pjm_PJEN(int phase, CEvt* event, GEM_HEADER* shead);

	int evt_pjm_Process(int phase, CEvt* event, GEM_HEADER* shead);

	int Evt_Spool_Unload(short phase, CEvt* event, GEM_HEADER* shead);
	int evt_alarm(int phase, CEvt* event, GEM_HEADER* shead);
	int Evt_Spool_Activated(short phase, CEvt* event, GEM_HEADER* shead);
	int Evt_Spool_Deactivated(short phase, CEvt* event, GEM_HEADER* shead);
	int evt_processing(int phase, CEvt* event, GEM_HEADER* shead);
	int evt_setup(int phase, CEvt* event, GEM_HEADER* shead);
	int evt_excuting(int phase, CEvt* event, GEM_HEADER* shead);
	int evt_selected(int phase, CEvt* event, GEM_HEADER* shead);
	int evt_lotidread(int phase, CEvt* event, GEM_HEADER* shead);
	int EvtGetTimeFromHost(int phase, CEvt* event, GEM_HEADER* shead);
	int ExcessMsgLen(int phase, CEvt* event, GEM_HEADER* shead);

	int EvtUploadRecipe(int phase, CEvt* event, GEM_HEADER* shead);
	int	EvtRcpDownload(int phase, CEvt* event, GEM_HEADER* shead);
	int EvtRcpChange(int phase, CEvt* event, GEM_HEADER* shead);
	int EvtMBEnqRecipe(int phase, CEvt* event, GEM_HEADER* shead);
	int Event_Proc(short phase, CEvt* pEvt, GEM_HEADER& shead);
	BOOL Initial_ListContrll();
	unsigned int AtoUint(unsigned char* pData);
	USHORT AtoUshort(unsigned char* pData);
	UINT ByteSwap(unsigned char* pData);
	int Get_DataLen(BYTE* pData);
	BOOL SendBlock(GEM_HEADER* pHeader);
	void Set_DataLen(GEM_HEADER* ead, long size);
	int set_head(int wbit, char strm, char func, GEM_HEADER* head, int size, BYTE primarymode);
	void Message_Seperation(GEM_HEADER* pHeader);
	CGemFactory();
	virtual ~CGemFactory();
private:
	SecsItem* MakeErrorList(UCHAR ErrCode);
	SecsItem* MakeErrorReport(vector < UINT > ErrorCodes);
	BOOL GetAllAttributes(short ObjectType, vector<string>& AttrIds);
	BOOL IsObject(short ObjType, string ObjID);
	BOOL GetAllObjectIDs(short ObjectType, vector<string>& ObjIds);
	BOOL GetObjectAttr(short ObjType, const string& ObjID, const string& AttrId, SecsItemList* pAttr);
	void InitGlobals(void);
	CPRJob* MakePrJob(PrjobRepNew& ProcessJob, vector<UINT>& ErrCodes);
};

extern CGemFactory* m_pFactory;
extern UINT TimeFormat;
extern UINT EstablishCommunicationTimeout;
