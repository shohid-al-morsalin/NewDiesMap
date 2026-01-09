// BMake.cpp: implementation of the CMake class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BMake.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMake::CMake() {}

CMake::~CMake() {}

long CMake::Binary(CString& str, char** msg) {
	short sz;
	long len = str.GetLength();
	sz = sizeof(len);
	memcpy(*msg, &len, sz);
	*msg += sz;
	if (len) {
		memcpy(*msg, str.GetBuffer(0), len);
		*msg += len;
	}
	return sz + len;
}

long CMake::Conv(CString& str, char** msg) {
	long len;
	short sz;
	sz = sizeof(len);
	memcpy(&len, *msg, sz);
	*msg += sz;
	if (len) {
		char* p = new char[len + 1];
		if (p) {
			memcpy(p, *msg, len);
			*(p + len) = 0;
			*msg += len;
			str = p;
			delete[] p;
			return sz + len;
		}
	}
	return sz;
}

long CMake::Binary(int& val, char** msg) {
	long sz;
	sz = sizeof(val);
	memcpy(*msg, &val, sz);
	*msg += sz;
	return sz;
}

long CMake::Conv(int& val, char** msg) {
	long sz;
	sz = sizeof(val);
	memcpy(&val, *msg, sz);
	*msg += sz;
	return sz;
}

long CMake::Binary(long& val, char** msg) {
	long sz;
	sz = sizeof(val);
	memcpy(*msg, &val, sz);
	*msg += sz;
	return sz;
}

long CMake::Conv(long& val, char** msg) {
	long sz;
	sz = sizeof(val);
	memcpy(&val, *msg, sz);
	*msg += sz;
	return sz;
}

long CMake::Binary(short& val, char** msg) {
	long sz;
	sz = sizeof(val);
	memcpy(*msg, &val, sz);
	*msg += sz;
	return sz;
}

// retrieve data from msg [7/3/2010 Yuen]
long CMake::Conv(short& val, char** msg) {
	long sz;
	sz = sizeof(val);
	memcpy(&val, *msg, sz);
	*msg += sz;
	return sz;
}

// populate msg with data [7/3/2010 Yuen]
long CMake::Binary(float& val, char** msg) {
	long sz;
	sz = sizeof(val);
	memcpy(*msg, &val, sz);
	*msg += sz;
	return sz;
}

long CMake::Conv(float& val, char** msg) {
	long sz;
	sz = sizeof(val);
	memcpy(&val, *msg, sz);
	*msg += sz;
	return sz;
}

long CMake::Binary(double& val, char** msg) {
	long sz;
	sz = sizeof(val);
	memcpy(*msg, &val, sz);
	*msg += sz;
	return sz;
}

long CMake::Conv(double& val, char** msg) {
	long sz;
	sz = sizeof(val);
	memcpy(&val, *msg, sz);
	*msg += sz;
	return sz;
}

long CMake::Binary(CTime& val, char** msg) {
	long sz;
	sz = sizeof(val);
	memcpy(*msg, &val, sz);
	*msg += sz;
	return sz;
}

long CMake::Conv(CTime& val, char** msg) {
	long sz;
	sz = sizeof(val);
	memcpy(&val, *msg, sz);
	*msg += sz;
	return sz;
}

long CMake::Binary(char* val, long n, char** msg) {
	n = n * sizeof(char);
	memcpy(*msg, val, n);
	*msg += n;
	return n;
}

long CMake::Conv(char* val, long n, char** msg) {
	n = n * sizeof(char);
	memcpy(val, *msg, n);
	*msg += n;
	*(val + n) = 0;
	return n;
}

long CMake::Binary(float* val, long n, char** msg) {
	n = n * sizeof(float);
	memcpy(*msg, val, n);
	*msg += n;
	return n;
}

long CMake::Conv(float* val, long n, char** msg) {
	n = n * sizeof(float);
	memcpy(val, *msg, n);
	*msg += n;
	return n;
}

long CMake::Binary(short* val, long n, char** msg) {
	n = n * sizeof(short);
	memcpy(*msg, val, n);
	*msg += n;
	return n;
}

long CMake::Conv(short* val, long n, char** msg) {
	n = n * sizeof(short);
	memcpy(val, *msg, n);
	*msg += n;
	return n;
}

long CMake::Binary(SYSTEMTIME& val, char** msg) {
	long sz;
	sz = sizeof(val);
	memcpy(*msg, &val, sz);
	*msg += sz;
	return sz;
}

long CMake::Conv(SYSTEMTIME& val, char** msg) {
	long sz;
	sz = sizeof(val);
	memcpy(&val, *msg, sz);
	*msg += sz;
	return sz;
}

long CMake::GetLength(SYSTEMTIME& val) {
	return sizeof(val);
}

long CMake::GetLength(CString& str) {
	return sizeof(long) + str.GetLength();
}

long CMake::GetLength(int& val) {
	return sizeof(val);
}

long CMake::GetLength(long& val) {
	return sizeof(val);
}

long CMake::GetLength(short& val) {
	return sizeof(val);
}

long CMake::GetLength(float& val) {
	return sizeof(val);
}

long CMake::GetLength(double& val) {
	return sizeof(val);
}
long CMake::GetLength(CTime& val) {
	return sizeof(val);
}

long CMake::GetLength(float* val) {
	return sizeof(val);
}

long CMake::GetLength(short* val) {
	return sizeof(val);
}