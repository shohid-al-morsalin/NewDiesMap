#ifndef JNIBRIDGE_H
#define JNIBRIDGE_H

#include "Platform.h"
//#include "BaseJavaClass.h"
#include "JString.h"

#ifdef __cplusplus
class ReferenceManager;
class JString;

class EXPORTED JNIBridge {
public:
	// class is a singleton. this and the next block of functions are for that design pattern
	static JNIBridge* getInstance();
	~JNIBridge();

protected:	
	JNIBridge();
	JNIBridge(const JNIBridge&);
	JNIBridge& operator=(const JNIBridge&);


protected:
	static JNIBridge* pInstance;

public:
	// srs-multithread-fix
	//JNIEnv* getEnv() {return env;}
	JNIEnv* getJavaEnv();

	JavaVM* getJVM() {return jvm;}
	JNIEnv* getLocalEnv();
	int addReference(JNIEnv* pJNIEnv, JString& name, int64 o);
	int removeReference(JNIEnv* pJNIEnv, int64 o);
	JNIEnv* attachCurrentThread();
	void detachCurrentThread();

	void mutex_lock(const char*);
	void mutex_unlock();
	void shutdownJVM();
	void shutdown(); // srsjvmfix
	void unloadDLL(); // srsjvmfix

private:
	// JNIEnv* env; srs-multithread-fix
	JavaVM* jvm;
	char *generateClassPath();
	char *generateLibraryPath();

	MUTEX_TYPE semaphore;
	char semaphoreLocation[50];
};
#endif


#ifdef WIN32
	// srsjvmfix
	extern "C" EXPORTED void* STDCALL JNIBridge_getInstance_stdcall();
	extern "C" EXPORTED void  STDCALL JNIBridge_shutdown_stdcall(void* pJNIBridge);
	extern "C" EXPORTED void  STDCALL JNIBridge_unloadDLL_stdcall(void* pJNIBridge);
#endif

#endif // JNIBridge.h
