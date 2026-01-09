#pragma once
// TracTimer.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTracTimer window
struct  NODE// : public CObject
{
public:
	UINT	m_Trid;
	UINT	m_Dsper;
	UINT	m_Totsmp;
	UINT    m_Currentsmp;
};

class CTimer :public CObject {
public:
	NODE* pNode;

public:
	CTimer();
	~CTimer();
public:
	static void CALLBACK StdTimerProc(HWND hwnd, UINT uMsg, UINT nIDEvent, DWORD dwTime);
};

class CTraceTracker : public  CTimer {
public:
	UINT m_trid;
	UINT m_Totsmp;
	UINT m_Smpln;
	CUIntArray m_svid;

public:
	void StartTrace(UINT nIDEvent, UINT uElapse, UINT uKillCounter);

	CTraceTracker();
	CTraceTracker(UINT nTrid, UINT nTotsmp);
	virtual ~CTraceTracker();

	const CTraceTracker& operator= (const CTraceTracker& a) {
		m_trid = a.m_trid;
		m_Totsmp = a.m_Totsmp;
		m_Smpln = a.m_Smpln;
		int svidcnt = a.m_svid.GetSize();
		m_svid.SetSize(svidcnt, 1);

		for (int i = 0; i < svidcnt; i++)
			m_svid.SetAtGrow(i, a.m_svid.GetAt(i));
		return *this;
	}
	const BOOL operator== (const CTraceTracker& a) {
		int i, svidcnt1, svidcnt2;
		UINT svid1, svid2;

		if (this->m_trid != a.m_trid)
			return FALSE;
		if (this->m_Totsmp != a.m_Totsmp)
			return FALSE;
		if (this->m_Smpln != a.m_Smpln)
			return FALSE;
		svidcnt1 = this->m_svid.GetSize();
		svidcnt2 = a.m_svid.GetSize();

		if (svidcnt1 != svidcnt2)
			return FALSE;

		for (i = 0; i < svidcnt1; i++) {
			svid1 = this->m_svid.GetAt(i);
			svid2 = a.m_svid.GetAt(i);

			if (svid1 != svid2)
				return FALSE;
		}
		return TRUE;
	}
};

/////////////////////////////////////////////////////////////////////////////
