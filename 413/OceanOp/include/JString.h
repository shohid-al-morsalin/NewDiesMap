/**
 * File: String.h
 * Simplified wrapper for java/lang/String
 * Copyright (C) 2006-2008 Ocean Optics, Inc.
 */


#ifndef JAVA_STRING_H
#define JAVA_STRING_H

#include "Platform.h"
#include <string.h>
//#include <comdef.h>

#ifdef __cplusplus

#ifndef LINUX
#pragma warning(disable : 4251)
#endif

class EXPORTED JString
{
public: 
	JString();
	JString(const JString &that);
	~JString();
	char *getASCII();
	//BSTR  getBSTR();
	int getLength();
	void setASCII(const char *ascii);

	JString &operator=(const JString &that);

private:
	char*   buffer;
	//_bstr_t bstr_buffer;
	int     length;
};

extern "C" {
#endif
	typedef void* JSTRING;

	EXPORTED JSTRING JString_Create();
	EXPORTED void  JString_Destroy(JSTRING jstr);
	EXPORTED char* JString_getASCII(JSTRING jstr);
	//EXPORTED BSTR  JString_getBSTR(JSTRING jstr);
	EXPORTED int   JString_getLength(JSTRING jstr);
	EXPORTED void  JString_setASCII(JSTRING jstr, char* ascii);
	
#ifdef WIN32
	// Use the following function prototypes when calling functions from Visual Basic 6
	EXPORTED JSTRING STDCALL JString_Create_stdcall();
	EXPORTED void STDCALL  JString_Destroy_stdcall(JSTRING jstr);
	EXPORTED char* STDCALL JString_getASCII_stdcall(JSTRING jstr);	
	//EXPORTED BSTR  STDCALL JString_getBSTR_stdcall(JSTRING jstr);	
	EXPORTED int STDCALL   JString_getLength_stdcall(JSTRING jstr);
	EXPORTED void STDCALL  JString_setASCII_stdcall(JSTRING jstr, char* ascii);
#endif /* WIN32 */
#ifdef __cplusplus
};
#endif
#endif // JAVA_STRING_H
