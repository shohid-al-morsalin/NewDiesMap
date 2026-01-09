// TraceTracker.cpp: implementation of the CTraceTracker class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TraceTracker.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern HWND m_gHwnd;
VOID CALLBACK TimerPro( HWND hwnd,  UINT uMsg,   UINT idEvent,   DWORD dwTime );

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
VOID CALLBACK TimerPro( HWND hwnd,  UINT uMsg,   UINT idEvent,   DWORD dwTime , UINT curcnt , UINT maxcnt )
{
	char data[10];
	memset( data , 0 , sizeof data );
	sprintf( data , "timer %d", idEvent );
	MessageBox( NULL , data , "TEIMRE",  MB_OK );
	curcnt++;
}

/*
CTimer::CTimer()
{
	m_CurrentCount = 0;
	m_MaxCount = 0;
	m_TimerID = 0;
}
CTimer::~CTimer()
{

}

void CTimer::TimerStart( UINT nIDEvent , UINT uElapse , UINT uKillCounter )
{
	m_TimerID = nIDEvent;
	m_MaxCount = uKillCounter;
	//::SetTimer( m_gHwnd , nIDEvent , uElapse * 1000 , TimerPro );
}

void CTimer::KillTime()
{
	::KillTimer( m_gHwnd , m_TimerID );
	//MessageBox( NULL , "I AM KIKK ","", MB_OK );
}
 */
/*
CTraceTracker::CTraceTracker()
{

}

CTraceTracker::~CTraceTracker()
{

}

void CTraceTracker::StartTrace(UINT nIDEvent, UINT uElapse , UINT uKillCounter )
{
    m_Timer.TimerStart( nIDEvent , uElapse ,uKillCounter );
}
*/