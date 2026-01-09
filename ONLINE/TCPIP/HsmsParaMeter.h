#pragma once

class CHsmsParaMeter {
	/*****************************************************************/
	// PASSIVE , ACTINVE  MODE
	// VAR , FUNCTION
	/*****************************************************************/

public:
	enum HSMSSSSM { NOTCONNECTED, ATTEMPTCONNECT, NOTSELECTED, SELECTED, CONNECTED };
	enum COMMSM { DISABLED, WAITCRFROMHOST, WAITDELAY, WAITCRA, COMMUNICATING, NOTCOMMUNICATING, ENABLED };
public:
	// 	static  DWORD	 m_T3Timer ;
	// 	static  DWORD	 m_T5Timer ;
	// 	static  DWORD	 m_T6Timer ;
	// 	static  DWORD	 m_T7Timer ;
	// 	static  DWORD	 m_T8Timer ;
	// 	static  short	 m_DeviceId ;
	// 	static  long	 m_SystemByte;
	// 	static  short	 m_SysByteInc;
	//
	// 	static  int      m_SocketPortNo;
	// 	static  int      m_ConnectionMode ;
	// 	static  CString	 m_ActIP;
	// 	static  int      m_ActPort;

	enum DEFONLINE { DEF_LOCAL, DEF_REMOTE };
	// 	static  short DefOnlineState;
	enum DEFOFFLINE { DEF_HOST_OFF, DEF_EQP_OFF };
	// 	static  short DefOfflineState;

	static  int      m_StartMode;
	static  BYTE     m_HostMode;
	static  BOOL	 m_ActiveSocket;
	// 	static	HSMSSSSM HSMSSS;
	// 	static	COMMSM   CommState;
	static	int m_ChgmodePossible;
public:

	CHsmsParaMeter();
	/*
	CHsmsParaMeter::CHsmsParaMeter( DWORD nT3 = 45 , DWORD nT5 = 10 , DWORD nT6 = 5 ,
								DWORD nT7 = 10 ,DWORD nT8 = 1 , int nPort = 5000 ,
								int nConnectMode = PASSIVE );
	 */

	 /*****************************************************************/
	 //
	 // PASSIVE MODE COMMUNICATION
	 // VAR , FUNCTION
	 /*****************************************************************/
public:
	static SOCKET m_sClientSk;

public:
	//static void ParaMeter_Setting();
	static void Set_ActPort(int pPort);
	static void Set_ActIP(CString pIP);
	static BOOL Filter_ParaVal(char* input, char* output, UINT NumParams, UINT ParamLoc);

public:
	static void Log(CString msg, short lvl = 5);
	static BOOL IsOnline();
	//static short GGetLineMode(); //  [9/14/2007 LYF]
	//static CString CHsmsParaMeter::GetLineModeStr(); //  [9/14/2007 LYF]
	//static int GetDataFromSocket( BYTE *lpszString , int nMaxLen );
	static int Pass_QCntChk();

	static int  Pass_RecvString(BYTE* nData, int nMaxLen, short* pTime, BOOL bWait = FALSE);
	static int  Pass_RevChar(BYTE* nData, int nLen);
	static void Pass_DescardRevString(int len);
	static int  Pass_SndString(void* pData, int Len);

	static void Pass_SetDataLen(BYTE* nData, int nLen);
	static int  Pass_GetDataLen(BYTE* nData);

	//	static EqpStatus::CONTROLSTATE GGetDefOflMode();
	//	static EqpStatus::CONTROLSTATE GGetDefOnlMode();
	virtual ~CHsmsParaMeter();
};

// extern DWORD	m_T3Timer;
// extern DWORD	m_T5Timer;
// extern DWORD	m_T6Timer;
// extern DWORD	m_T7Timer;
// extern DWORD	m_T8Timer;

extern short	m_DeviceId;
extern long		m_SystemByte;
extern short	m_SysByteInc;

extern int		m_SocketPortNo;
extern int		m_ConnectionMode;

extern char		m_cActIP[24];
extern int		m_ActPort;
extern int		m_StartMode;

extern short	m_DefOnlineState;
extern short	m_DefOfflineState;

extern CHsmsParaMeter::COMMSM hmCommState;
extern CHsmsParaMeter::HSMSSSSM hmHSMSSS;
