// 413ProcThread.h: interface for the C413ProcThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_413PROCTHREAD_H__2D49735D_DE5B_4466_83C8_61D3536225B8__INCLUDED_)
#define AFX_413PROCTHREAD_H__2D49735D_DE5B_4466_83C8_61D3536225B8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class C413TCPServer;

class C413ProcThread  
{
	BOOL bStop;
	CWinThread *pThread;
	C413TCPServer *pTCPSvr;

public:
	void Start();
	void Stop();
	static UINT MeasureProc(LPVOID pObject);
	C413ProcThread();
	virtual ~C413ProcThread();

};

#endif // !defined(AFX_413PROCTHREAD_H__2D49735D_DE5B_4466_83C8_61D3536225B8__INCLUDED_)
