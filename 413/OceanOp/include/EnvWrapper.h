#ifndef ENVWRAPPER_H
#define ENVWRAPPER_H

#include "JNIBridge.h"

#ifdef __cplusplus
class EXPORTED EnvWrapper {
public:
	EnvWrapper(JNIBridge *br);

	virtual ~EnvWrapper();

	// Static methods

	jobject CallStaticObjectMethod(JNIEnv* pJNIEnv, jclass cl, jmethodID methodID, ...);

	jint CallStaticIntMethod(JNIEnv* pJNIEnv, jclass cl, jmethodID methodID, ...);

	jdouble CallStaticDoubleMethod(JNIEnv* pJNIEnv, jclass cl, jmethodID methodID, ...);

	jboolean CallStaticBooleanMethod(JNIEnv* pJNIEnv, jclass cl, jmethodID methodID, ...);

	jlong CallStaticLongMethod(JNIEnv* pJNIEnv, jclass cl, jmethodID methodID, ...);

	void CallStaticVoidMethod(JNIEnv* pJNIEnv, jclass cl, jmethodID methodID, ...);

	jshort CallStaticShortMethod(JNIEnv* pJNIEnv, jclass cl, jmethodID methodID, ...);

	jbyte CallStaticByteMethod(JNIEnv* pJNIEnv, jclass cl, jmethodID methodID, ...);

	// Instance methods

	jobject CallObjectMethod(JNIEnv* pJNIEnv, jobject object, jmethodID methodID, ...);

	jdouble CallDoubleMethod(JNIEnv* pJNIEnv, jobject object, jmethodID methodID, ...);

	jboolean CallBooleanMethod(JNIEnv* pJNIEnv, jobject object, jmethodID methodID, ...);

	jint CallIntMethod(JNIEnv* pJNIEnv, jobject object, jmethodID methodID, ...);

	jlong CallLongMethod(JNIEnv* pJNIEnv, jobject object, jmethodID methodID, ...);

	void CallVoidMethod(JNIEnv* pJNIEnv, jobject object, jmethodID methodID, ...);
	
	jbyte CallByteMethod(JNIEnv* pJNIEnv, jobject object, jmethodID methodID, ...);
	
	jfloat CallFloatMethod(JNIEnv* pJNIEnv, jobject object, jmethodID methodID, ...);
        
        jshort CallShortMethod(JNIEnv* pJNIEnv, jobject object, jmethodID methodID, ...);

	// Constructors

	jbyteArray NewByteArray(JNIEnv* pJNIEnv, jsize len);

	jintArray NewIntArray(JNIEnv* pJNIEnv, jsize len);

	jdoubleArray NewDoubleArray(JNIEnv* pJNIEnv, jsize len);

	jfloatArray NewFloatArray(JNIEnv* pJNIEnv, jsize len);

	jobjectArray NewObjectArray(JNIEnv* pJNIEnv, jsize len, jclass clazz, jobject init);

	jobject NewObject(JNIEnv* pJNIEnv, jclass cl, jmethodID methodID, ...);

	jstring NewStringUTF(JNIEnv* pJNIEnv, const char *utf);

	// Accessors

	jsize GetStringUTFLength(JNIEnv* pJNIEnv, jstring str);

	const char* GetStringUTFChars(JNIEnv* pJNIEnv, jstring str, jboolean *isCopy);

	jclass FindClass(JNIEnv* pJNIEnv, const char *name);

	jmethodID GetStaticMethodID(JNIEnv* pJNIEnv, jclass clazz, const char *name, const char *sig);

	jmethodID GetMethodID(JNIEnv* pJNIEnv, jclass clazz, const char *name, const char *sig);

	jfieldID GetFieldID(JNIEnv* pJNIEnv, jclass clazz, const char *name, const char *sig);

	jfieldID GetStaticFieldID(JNIEnv* pJNIEnv, jclass clazz, const char *name, const char *sig);

	jbyte GetStaticByteField(JNIEnv* pJNIEnv, jclass clazz, jfieldID fieldID);

	jshort GetStaticShortField(JNIEnv* pJNIEnv, jclass clazz, jfieldID fieldID);

	jint GetStaticIntField(JNIEnv* pJNIEnv, jclass clazz, jfieldID fieldID);

	jlong GetStaticLongField(JNIEnv* pJNIEnv, jclass clazz, jfieldID fieldID);

	jdouble GetStaticDoubleField(JNIEnv* pJNIEnv, jclass clazz, jfieldID fieldID);

	jobject GetStaticObjectField(JNIEnv* pJNIEnv, jclass clazz, jfieldID fieldID);

	jdouble *GetDoubleArrayElements(JNIEnv* pJNIEnv, jdoubleArray array, jboolean *isCopy);

	jfloat *GetFloatArrayElements(JNIEnv* pJNIEnv, jfloatArray array, jboolean *isCopy);

	jbyte *GetByteArrayElements(JNIEnv* pJNIEnv, jbyteArray array, jboolean *isCopy);

	jint *GetIntArrayElements(JNIEnv* pJNIEnv, jintArray array, jboolean *isCopy);

	jboolean* GetBooleanArrayElements(JNIEnv* pJNIEnv, jbooleanArray array, jboolean *isCopy);

	jshort* GetShortArrayElements(JNIEnv* pJNIEnv, jshortArray array, jboolean *isCopy);

	void SetDoubleArrayRegion(JNIEnv* pJNIEnv, jdoubleArray array, jsize start, jsize len, jdouble *buf);

	void SetFloatArrayRegion(JNIEnv* pJNIEnv, jfloatArray array, jsize start, jsize len, jfloat *buf);

	void SetByteArrayRegion(JNIEnv* pJNIEnv, jbyteArray array, jsize start, jsize len, jbyte *buf);

	void SetIntArrayRegion(JNIEnv* pJNIEnv, jintArray array, jsize start, jsize len, jint *buf);
#ifndef LINUX
	void SetIntArrayRegion(JNIEnv* pJNIEnv, jintArray array, jsize start, jsize len, int *buf);
#endif

	void SetObjectArrayElement(JNIEnv* pJNIEnv, jobjectArray array, jsize index, jobject val);

	jsize GetArrayLength(JNIEnv* pJNIEnv, jarray array);

	jobject GetObjectArrayElement(JNIEnv* pJNIEnv, jobjectArray array, jsize index);

	void ReleaseDoubleArrayElements(JNIEnv* pJNIEnv, jdoubleArray array, jdouble *elems, jint mode);

	void ReleaseFloatArrayElements(JNIEnv* pJNIEnv, jfloatArray array, jfloat *elems, jint mode);

	void ReleaseIntArrayElements(JNIEnv* pJNIEnv, jintArray array, jint *elems, jint mode);

	void ReleaseByteArrayElements(JNIEnv* pJNIEnv, jbyteArray array, jbyte *elems, jint mode);

	void ReleaseBooleanArrayElements(JNIEnv* pJNIEnv, jbooleanArray array, jboolean *elems, jint mode);

	void ReleaseShortArrayElements(JNIEnv* pJNIEnv, jshortArray array, jshort *elems, jint mode);

	void ReleaseStringUTFChars(JNIEnv* pJNIEnv, jstring str, const char* chars);

	// References

	void DeleteGlobalRef(JNIEnv* pJNIEnv, jobject gref);

	// Reflection

	jboolean IsInstanceOf(JNIEnv* pJNIEnv, jobject obj, jclass clazz);

	// Exceptions

	void CheckExceptions(JNIEnv* pJNIEnv);

	jint monitorEnter(JNIEnv* pJNIEnv, jobject obj);

	jint monitorExit(JNIEnv* pJNIEnv, jobject obj);

	JNIEnv* AttachCurrentThread();
	void DetachCurrentThread();

	//void attachJVM();
	JNIEnv* attachJVM();
	void detachJVM();
	void mutex_lock(const char* loc);
	void mutex_unlock();
	jobject NewGlobalRef(JNIEnv* pJNIEnv, jobject obj);

private:
	JNIBridge *bridge;
	int verbose;
	int exceptionCheck;
	//JNIEnv *localenv;

};
#endif

#endif // ENVWRAPPER_H
