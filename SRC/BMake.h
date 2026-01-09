// BMake.h: interface for the CMake class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BMAKE_H__5A4D1B15_18EF_409C_AC55_9691DF72E7F2__INCLUDED_)
#define AFX_BMAKE_H__5A4D1B15_18EF_409C_AC55_9691DF72E7F2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMake  
{
public:
	static long GetLength(short *val);
	static long GetLength(float *val);
	static long GetLength(SYSTEMTIME &val);
	static long GetLength(CTime &val);
	static long GetLength(double &val);
	static long GetLength(float &val);
	static long GetLength(short &val);
	static long GetLength(long &val);
	static long GetLength(int &val);
	static long GetLength(CString &str);

	static long Conv(short *val, long n, char **msg);
	static long Conv(float *val, long n, char **msg);
	static long Conv(char *val, long n, char **msg);
	static long Conv(SYSTEMTIME &val, char **msg);
	static long Conv(CTime &val, char **msg);
	static long Conv(double &val, char **msg);
	static long Conv(float &val, char **msg);
	static long Conv(short &val, char **msg);
	static long Conv(long &val, char **msg);
	static long Conv(int &val, char **msg);
	static long Conv(CString &str, char **msg);
	static long Binary(short *val, long n, char **msg);
	static long Binary(float *val, long n, char **msg);
	static long Binary(char *val, long n, char **msg);
	static long Binary(SYSTEMTIME &val, char **msg);
	static long Binary(CTime &val, char **msg);
	static long Binary(double &val, char **msg);
	static long Binary(float &val, char **msg);
	static long Binary(short &val, char **msg);
	static long Binary(long &val, char **msg);
	static long Binary(int &val, char **msg);
	static long Binary(CString &str, char **msg);
	CMake();
	virtual ~CMake();

};

#endif // !defined(AFX_BMAKE_H__5A4D1B15_18EF_409C_AC55_9691DF72E7F2__INCLUDED_)
