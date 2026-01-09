#pragma once
class CDrLockMgr {
	HANDLE hThread = NULL;
	BOOL bStarted = FALSE;
	BOOL bStopped = TRUE;
	BOOL bPaused = FALSE;

	BOOL bDoorLockCheck = FALSE;
	int nDoorCheckInterval = 1000;

	static UINT DoorLockProc(LPVOID pMgr);
	void Yld(UINT dly) {
		ULONGLONG tick = GetTickCount64();
		while ((GetTickCount64() - tick) < dly) {
			MSG msg;
			while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			if (bStopped) break;
		}
	}
public:
	CDrLockMgr();
	~CDrLockMgr();

	void Pause() {
		bPaused = TRUE;
	}
	void Continue() {
		bPaused = FALSE;
	}
};
