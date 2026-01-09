#pragma once

class CFsmCoreSocket;
class CFmsRWUPCoreSocket;

class CHSMSComThread : public CWinThread {
	DECLARE_DYNCREATE(CHSMSComThread)
	BOOL bAbort;
	BYTE* pdwRecvString;

public:

	CHSMSComThread(); // protected constructor used by dynamic creation

	CHSMSComThread(HWND hwnd);

	virtual ~CHSMSComThread();
	void ByteSwap(void* pVoid, int nNumBytes);
	// Attributes
public:

	HWND m_hWnd;
	CFsmCoreSocket* m_pListenSocket;
	short m_Time[34];
	BOOL* pbRun;
	static BOOL m_AcceptFlag;

	// Operations
public:
	void SendSelectReq();
	void SendSeparateReq();
	void SeparateReq();
	void Select_Rsp(BYTE* pData);
	void Deselect_Rsp(BYTE* pData);
	void StopThread();
	BOOL OfflineEvent();
	void SelectRequest();
	BOOL CreateSocket();
	BOOL AttemptConnect();
	void Log(CString msg, short lvl = 5);
	void DiscardMessage(UINT  Len);
	void ResumeEvents();
	void PauseEvents();
	void ProcessExcessLen(UINT  Len);
	void ProcessMessage(UINT DataLength);
	void AbortTrans(UINT SysByte, int Stream, int Function);
	int LinkTestReq(void);
	void PostMessageDlg(int msg, BOOL flag);
	void LinkTest_Rsp(BYTE* pData);
	int GetSType(BYTE* pData);
	BOOL Progress_PassiveMode();
	void Set_GemDataLen(BYTE* nData, int nLen);
	BOOL Progress_ActiveMode();
	BOOL Init_ActiveMode();
	int Get_GemDataLen(BYTE* nData);
	UINT GetSysByte(BYTE* nData);

	virtual int Run();

	void MainProcedure();

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHSMSComThread)
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CHSMSComThread)
	// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

extern CFmsRWUPCoreSocket* m_pClientSocket;
