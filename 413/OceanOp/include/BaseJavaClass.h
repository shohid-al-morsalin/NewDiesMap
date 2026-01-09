#ifndef __BASEJAVACLASS
#define __BASEJAVACLASS
// Since all platforms need these defines, as do customers that use these
// product, include the preprocessor macros here.  This include file
// is included by all generated headers
#ifndef INCLUDES_IN_IMPL
#define INCLUDES_IN_IMPL
#endif
#ifndef INCLUDES_IN_HEADER
#define INCLUDES_IN_HEADER
#endif
#ifndef CLASS_PREDECLARATIONS
#define CLASS_PREDECLARATIONS
#endif
/*
#ifndef INTERFACE_PREDECLARATIONS
#define INTERFACE_PREDECLARATIONS
#endif
*/
#include "JNIBridge.h"
#include "ArrayTypes.h"
#include "EnvWrapper.h"
//#include "ReferenceManager.h" this causes C2504 error

#ifdef WIN32
//#pragma managed(push, off)
#endif

// added for TableModelEvent under Windows
#ifdef DELETE
#undef DELETE
#endif

#ifdef __cplusplus
CLASS_TOKEN EXPORTED BaseJavaClass {
public:
	BaseJavaClass();
	virtual ~BaseJavaClass(void);
	char* getClassName() { return className; }
	jclass findClass(JNIEnv* pJNIEnv);

	jclass getClass() { return javaClass; }
	void setObject(JNIEnv* pJNIEnv, jobject j);
	void setObjectFirstTime(jobject j); // same as setObject(), but does not attempt to release prior javaObject
	//void setObjectForReturn(jobject obj);
	jobject getObject() { return javaObject; }
	JavaVM* getJVM() { return jni->getJVM(); }
	JNIEnv* getEnv() { return jni->getJavaEnv(); }
	jobject newGlobalRef(JNIEnv* pJNIEnv, jobject obj, int isObject);
	void deleteGlobalRef(JNIEnv* pJNIEnv, jobject obj);

	int isWrappedObjectNull(void);
	int canCastInto(JNIEnv* pJNIEnv, BaseJavaClass &obj);
	void castInto(JNIEnv* pJNIEnv, BaseJavaClass &obj);
protected:
	JNIBridge* jni;
	char className[1000];
	jclass javaClass;
	jobject javaObject;
	EnvWrapper* localenv;
private:

};


extern "C" {
#endif

	typedef void* BASEJAVACLASS;

	EXPORTED BASEJAVACLASS BaseJavaClass_Create();
	EXPORTED void BaseJavaClass_Destroy(BASEJAVACLASS baseJavaClass);

	EXPORTED int BaseJavaClass_isWrappedObjectNull(BASEJAVACLASS baseJavaClass);
	EXPORTED int BaseJavaClass_canCastInto(JNIEnv* pJNIEnv, BASEJAVACLASS baseJavaClass, BASEJAVACLASS ptrOut);
	EXPORTED void BaseJavaClass_castInto(JNIEnv* pJNIEnv, BASEJAVACLASS baseJavaClass, BASEJAVACLASS ptrOut);
	
	EXPORTED void BaseJavaClass_setObject(JNIEnv* pJNIEnv, BASEJAVACLASS baseJavaClass, jobject j);
	EXPORTED jobject BaseJavaClass_getObject(BASEJAVACLASS baseJavaClass);
#ifdef __cplusplus
};
#endif

#ifdef WIN32
//#pragma managed(pop)
#endif

#endif // __BASEJAVACLASS
