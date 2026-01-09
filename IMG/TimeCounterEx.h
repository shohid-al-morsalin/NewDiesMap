#pragma once

#pragma warning( disable : 4710 )

// Helper class used to show execution time
class CTimeCounter
{
	BOOL bOK;
	__int64 BeginTime[16];

public:
	CTimeCounter()
	{
		if (!QueryPerformanceFrequency(&m_nFreq))
		{
			bOK = FALSE;
		}
		else
		{
			bOK = TRUE;
		}
	}

	void Start(int nTimer)
	{
		if ((nTimer >= 0) || (nTimer < 16))
		{
			if (bOK)
			{
				QueryPerformanceCounter(&m_nBeginTime[nTimer]);
			}
			else
			{
				BeginTime[nTimer] = GetTickCount64();
			}
		}
	}

	// Value return in nano seconds [2/13/2015 128]
	// ms = 1e3 milli second [2/13/2015 128]
	// mcs = 1e6 micro second [2/13/2015 128]
	// ns = 1e9 nano second [2/13/2015 128]
	__int64 GetExecutionTime(int nTimer, int res)
	{
		__int64 ltime;
		if (bOK)
		{
			LARGE_INTEGER nEndTime;
			QueryPerformanceCounter(&nEndTime);
			ltime = (nEndTime.QuadPart - m_nBeginTime[nTimer].QuadPart) * res / m_nFreq.QuadPart;
		}
		else
		{
			ltime = GetTickCount64() - BeginTime[nTimer];
		}
		return ltime;
	}
	__int64 GetExecutionTimeSEC(int nTimer)
	{
		__int64 ltime;
		if (bOK)
		{
			LARGE_INTEGER nEndTime;
			QueryPerformanceCounter(&nEndTime);
			ltime = (nEndTime.QuadPart - m_nBeginTime[nTimer].QuadPart) / m_nFreq.QuadPart;
		}
		else
		{
			ltime = GetTickCount64() - BeginTime[nTimer];
		}
		return ltime;
	}
	__int64 GetExecutionTimeMS(int nTimer)
	{
		__int64 ltime;
		if (bOK)
		{
			LARGE_INTEGER nEndTime;
			QueryPerformanceCounter(&nEndTime);
			ltime = (nEndTime.QuadPart - m_nBeginTime[nTimer].QuadPart) * 1000 / m_nFreq.QuadPart;
		}
		else
		{
			ltime = GetTickCount64() - BeginTime[nTimer];
		}
		return ltime;
	}
	__int64 GetExecutionTimeMCS(int nTimer)
	{
		__int64 ltime;
		if (bOK)
		{
			LARGE_INTEGER nEndTime;
			QueryPerformanceCounter(&nEndTime);
			ltime = (nEndTime.QuadPart - m_nBeginTime[nTimer].QuadPart) * 1000000 / m_nFreq.QuadPart;
		}
		else
		{
			ltime = GetTickCount64() - BeginTime[nTimer];
		}
		return ltime;
	}

	~CTimeCounter()
	{
	}

protected:
	LARGE_INTEGER m_nFreq;
	LARGE_INTEGER m_nBeginTime[16];
};
