#ifndef LABVIEW_WIN32
#ifndef LABVIEW_LINUX
#ifndef LABVIEW_MACOSX

//  This is what we want for building OmniDriver on any OS platform

#ifdef WIN32
#define _CRT_SECURE_NO_DEPRECATE
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif
//#include <memory.h>
#include <string.h>
#include <windows.h>
#include <jni.h>
#include <stdio.h>
#define snprintf _snprintf
#define vsnprintf _vsnprintf
#ifdef __BORLANDC__
//#define EXPORTED __declspec(dllexport) __stdcall
#define EXPORTED __declspec(dllexport)
#else
#define EXPORTED __declspec(dllexport)
#endif //__BORLANDC__
#define STDCALL __stdcall
#define int64 __int64
#define MUTEX_TYPE HANDLE
#ifndef CLASS_TOKEN
#ifdef __CLR_VER // means that it is being compiled as a CLR application
#define CLASS_TOKEN public class
#else
#define CLASS_TOKEN class
#endif
#endif // CLASS_TOKEN
//#define __cplusplus
#endif // WIN32

#ifdef MACOSX
#ifndef CLASS_TOKEN
#define CLASS_TOKEN class
#endif // CLASS_TOKEN
#ifndef __STDAFX
#define __STDAFX
#define TRUE 1
#define FALSE 0
#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif
#define EXPORTED
#include <stdio.h>
#include <string.h>
#include <JavaVM/jni.h>
#include <pthread.h>
#define int64 long long
#define MUTEX_TYPE pthread_mutex_t
#endif // __STDAFX
#endif // MACOSX

#ifdef LINUX
#ifndef CLASS_TOKEN
#define CLASS_TOKEN class
#endif // CLASS_TOKEN
#ifndef __STDAFX
#define __STDAFX
#define TRUE 1
#define FALSE 0
#define true 1
#define false 0
#define EXPORTED
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#define MUTEX_TYPE pthread_mutex_t
#include "jni.h"
#define int64 long long
#endif // __STDAFX
#endif // LINUX

#endif // LABVIEW_MACOSX
#endif // LABVIEW_LINUX
#endif // LABVIEW_WIN32



#ifdef LABVIEW_WIN32

//  This is what we want for LabVIEW when it generates the VI's for OmniDriver on the WIN32 platform

#define WIN32
#define DELETE
#define _DEBUG
#define __cplusplus

#ifdef WIN32
#define _CRT_SECURE_NO_DEPRECATE
//#include <memory.h>
#include <string.h>
#include <windows.h>
#include <jni.h>
#include <stdio.h>
#define snprintf _snprintf
#define vsnprintf _vsnprintf

//#ifdef __BORLANDC__
//#define EXPORTED __declspec(dllexport) __stdcall
//#else
#define EXPORTED __declspec(dllexport)
//#endif //__BORLANDC__

#define STDCALL __stdcall
#define int64 __int64
#define MUTEX_TYPE HANDLE
#ifndef CLASS_TOKEN

//#ifdef __CLR_VER // means that it is being compiled as a CLR application
//#define CLASS_TOKEN public class
//#else
#define CLASS_TOKEN class
//#endif

#endif // CLASS_TOKEN
//#define __cplusplus
#endif // WIN32

#endif // LABVIEW_WIN32

