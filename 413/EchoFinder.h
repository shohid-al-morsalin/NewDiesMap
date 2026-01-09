// EchoFinder.h: interface for the CEchoFinder class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
class CEchoFinder {
	HANDLE m_hThread; // thread for measuring continuously

public:
	HWND hWnd;
	double OldPos;
	BOOL bEchoStopFlag;

public:
	static DWORD MeasureProcEcho(LPVOID pParam);
	void static CALLBACK OnCalculatingEnd(int, LPVOID);

public:
	BOOL StopEchoThread();
	BOOL StartEchoThread();
	virtual void PostDrawChartMessage(void* pObject) = 0;
	virtual void PostErrorMessage(CString MsgStr) = 0;
	BOOL Deinitialize();
	BOOL Initialize();

	CEchoFinder();
	virtual ~CEchoFinder();
};