#ifndef ARRAYTYPES_H
#define ARRAYTYPES_H

//#include <memory.h>
#include "Platform.h"
#include "JNIBridge.h"
#include "EnvWrapper.h"

#ifdef __cplusplus
class EXPORTED AbstractArray
{
public:
	AbstractArray();
	AbstractArray(const AbstractArray &that);
	AbstractArray(int l);
	void freeMemory(); // memleak16
	int getLength();
	AbstractArray &operator=(const AbstractArray &that);

protected:
	void setLength(int l);
	int length;
	//JNIBridge* jni;
	//EnvWrapper* localenv;
};

class EXPORTED DoubleArray : public AbstractArray {
public:
	DoubleArray(double *values, int l);
	DoubleArray();
	DoubleArray(const DoubleArray &that);
	~DoubleArray();
	double *getDoubleValues();
	void setValues(double *vals, int len);
	DoubleArray &operator=(const DoubleArray &that);

private:
	double *data;
};

class EXPORTED IntArray : public AbstractArray {
public:
	IntArray(int *values, int l);
	IntArray();
	IntArray(const IntArray &that);
	~IntArray();
	int *getIntValues();
        unsigned char* getCharValues();
	void setValues(int *vals, int len);
	void setValues(unsigned char* vals, int len);
        void setCharValues(unsigned char* vals, int len);
	IntArray &operator=(const IntArray &that);

private:
	int *data;
        unsigned char* charData;
};

class EXPORTED FloatArray : public AbstractArray {
public:
	FloatArray(float *values, int l);
	FloatArray();
	FloatArray(const FloatArray &that);
	~FloatArray();
	float *getFloatValues();
	void setValues(float *vals, int len);
	FloatArray &operator=(const FloatArray &that);
private:
	float *data;

};

class EXPORTED LongArray : public AbstractArray {
public:
	LongArray(long *values, int l);
	LongArray();
	LongArray(const LongArray &that);
	~LongArray();
	long *getLongValues();
	void setValues(long *vals, int len);
	LongArray &operator=(const LongArray &that);
private:
	long *data;
};

class EXPORTED CharArray : public AbstractArray {
public:
	CharArray(signed char *values, int l);
	CharArray();
	CharArray(const CharArray &that);
	~CharArray();
	signed char *getCharValues();
	void setValues(signed char *vals, int len);
	CharArray &operator=(const CharArray &that);
private:
	signed char *data;
};

// srs: Formerly the ShortArray implemented unsigned short's.
class EXPORTED ShortArray : public AbstractArray {
public:
	ShortArray(short *values, int l);
	ShortArray();
	ShortArray(const ShortArray &that);
	~ShortArray();
	short *getShortValues();
	void setValues(short *vals, int len);
	ShortArray &operator=(const ShortArray &that);
private:
	short *data;
};

// FIXME: these need to be smarter about memory allocation
class EXPORTED ArrayOfArrays : public AbstractArray {
public:
	ArrayOfArrays();
	ArrayOfArrays(AbstractArray *arrays, int l);
	AbstractArray *getArrays();
private:
	AbstractArray *data;
};


// FIXME: these need to be smarter about memory allocation
class EXPORTED ArrayOfObjects : public AbstractArray {
public:
	ArrayOfObjects();
	ArrayOfObjects(void *objects, int l);
	void *getObjects();
	void *getObject();
	void setObjects(void *objects, int len);
	~ArrayOfObjects();
private:
	void *data;
};

//
//class EXPORTED ArrayOfObjects : public AbstractArray {
//public:
//	ArrayOfObjects(void* values, int l, int size);
//	ArrayOfObjects();
//	ArrayOfObjects(const ArrayOfObjects &that);
//	~ArrayOfObjects();
//	void* getObjects();
//	void setObjects(void* vals, int len, int size);
//	ArrayOfObjects &operator=(const ArrayOfObjects &that);
//	//void setObjects(void* vals, int len);
//private:
//	void* *data;
//	int size;
//};

extern "C" {
#endif
	typedef void* DOUBLEARRAY_T;
	typedef void* INTARRAY_T;
	typedef void* FLOATARRAY_T;
	typedef void* LONGARRAY_T;
	typedef void* CHARARRAY_T;
	typedef void* SHORTARRAY_T;
	typedef void* ARRAYOFARRAYS_T;
	typedef void* ABSTRACTARRAY_T;
	typedef void* ARRAYOFOBJECTS_T;
	
	EXPORTED DOUBLEARRAY_T DoubleArray_Create();
	EXPORTED DOUBLEARRAY_T DoubleArray_Create_1(double* vals, int length);
	EXPORTED double* DoubleArray_getDoubleValues(DOUBLEARRAY_T double_array);
	EXPORTED int DoubleArray_getLength(DOUBLEARRAY_T double_array);
	EXPORTED void DoubleArray_setDoubleValues(DOUBLEARRAY_T double_array, double* values, int len);
	EXPORTED void DoubleArray_Destroy(DOUBLEARRAY_T double_array);
	EXPORTED double DoubleArray_getElementAt(DOUBLEARRAY_T double_array, int i);
	EXPORTED int DoubleArray_copyDoubleValues(DOUBLEARRAY_T double_array, double* target, int length);
	
	EXPORTED INTARRAY_T IntArray_Create();
	EXPORTED INTARRAY_T IntArray_Create_1(int* vals, int length);
	EXPORTED int* IntArray_getIntValues(INTARRAY_T int_array);
	EXPORTED int IntArray_getLength(INTARRAY_T int_array);
	EXPORTED void IntArray_setIntValues(INTARRAY_T int_array, int* values, int len);
	EXPORTED void IntArray_Destroy(INTARRAY_T int_array);
	EXPORTED int IntArray_getElementAt(INTARRAY_T int_array, int i);
	EXPORTED int IntArray_copyIntValues(INTARRAY_T int_array, int* target, int length);
	
	EXPORTED FLOATARRAY_T FloatArray_Create();
	EXPORTED FLOATARRAY_T FloatArray_Create_1(float* vals, int length);
	EXPORTED float* FloatArray_getFloatValues(FLOATARRAY_T float_array);
	EXPORTED int FloatArray_getLength(FLOATARRAY_T float_array);
	EXPORTED void FloatArray_setFloatValues(FLOATARRAY_T float_array, float *values, int len);
	EXPORTED void FloatArray_Destroy(FLOATARRAY_T float_array);
	EXPORTED float FloatArray_getElementAt(FLOATARRAY_T float_array, int i);
	EXPORTED int FloatArray_copyFloatValues(FLOATARRAY_T float_array, float* target, int length);

	EXPORTED LONGARRAY_T LongArray_Create();
	EXPORTED LONGARRAY_T LongArray_Create_1(long* vals, int length);
	EXPORTED long* LongArray_getLongValues(LONGARRAY_T long_array);
	EXPORTED int LongArray_getLength(LONGARRAY_T long_array);
	EXPORTED void LongArray_setLongValues(LONGARRAY_T long_array, long *values, int len);
	EXPORTED void LongArray_Destroy(LONGARRAY_T long_array);
	EXPORTED long LongArray_getElementAt(LONGARRAY_T long_array, int i);
	EXPORTED int LongArray_copyLongValues(LONGARRAY_T long_array, long *target, int length);
	
	EXPORTED CHARARRAY_T CharArray_Create();
	EXPORTED CHARARRAY_T CharArray_Create_1(signed char* vals, int length);
	EXPORTED signed char* CharArray_getCharValues(CHARARRAY_T char_array);
	EXPORTED int CharArray_getLength(CHARARRAY_T char_array);
	EXPORTED void CharArray_setCharValues(CHARARRAY_T char_array, signed char *values, int length);
	EXPORTED void CharArray_Destroy(CHARARRAY_T char_array);
	EXPORTED signed char CharArray_getElementAt(CHARARRAY_T char_array, int i);
	EXPORTED int CharArray_copyCharValues(CHARARRAY_T char_array, signed char *target, int length);
	
	EXPORTED SHORTARRAY_T ShortArray_Create();
	EXPORTED SHORTARRAY_T ShortArray_Create_1(short* vals, int length);
	EXPORTED short* ShortArray_getShortValues(SHORTARRAY_T short_array);
	EXPORTED int ShortArray_getLength(SHORTARRAY_T short_array);
	EXPORTED void ShortArray_setShortValues(SHORTARRAY_T short_array, short *values, int length);
	EXPORTED void ShortArray_Destroy(SHORTARRAY_T short_array);
	EXPORTED short ShortArray_getElementAt(SHORTARRAY_T short_array, int i);
	EXPORTED int ShortArray_copyShortValues(SHORTARRAY_T short_array, short *target, int length);

	EXPORTED ARRAYOFARRAYS_T ArrayOfArrays_Create();
	EXPORTED ARRAYOFARRAYS_T ArrayOfArrays_Create_1(ABSTRACTARRAY_T *vals, int length);
	EXPORTED void* ArrayOfArrays_getArrays(ARRAYOFARRAYS_T array_of_arrays);
	EXPORTED int ArrayOfArrays_getLength(ARRAYOFARRAYS_T array_of_arrays);
	EXPORTED void ArrayOfArrays_Destroy(ARRAYOFARRAYS_T array_of_arrays);

	EXPORTED ARRAYOFOBJECTS_T ArrayOfObjects_Create();
	EXPORTED ARRAYOFOBJECTS_T ArrayOfObjects_Create_1(void* vals, int length);
	EXPORTED void* ArrayOfObjects_getObjects(ARRAYOFOBJECTS_T array_of_objects);
	EXPORTED int ArrayOfObjects_getLength(ARRAYOFOBJECTS_T array_of_objects);
	EXPORTED void ArrayOfObjects_Destroy(ARRAYOFOBJECTS_T array_of_objects);
	EXPORTED int ArrayOfObjects_copyArrayOfObjectsValues(ARRAYOFOBJECTS_T array_of_objects, void *objects, int length);
	EXPORTED void ArrayOfObjects_setObjects(ARRAYOFOBJECTS_T array_of_objects, void *objects, int length);
	
#ifdef WIN32
	EXPORTED DOUBLEARRAY_T STDCALL DoubleArray_Create_stdcall();
	EXPORTED DOUBLEARRAY_T STDCALL DoubleArray_Create_1_stdcall(double* vals, int length);
	EXPORTED double* STDCALL DoubleArray_getDoubleValues_stdcall(DOUBLEARRAY_T double_array);
	EXPORTED int STDCALL DoubleArray_getLength_stdcall(DOUBLEARRAY_T double_array);
	EXPORTED void STDCALL DoubleArray_setDoubleValues_stdcall(DOUBLEARRAY_T double_array, double* values, int len);
	EXPORTED void STDCALL DoubleArray_Destroy_stdcall(DOUBLEARRAY_T double_array);
	EXPORTED double STDCALL DoubleArray_getElementAt_stdcall(DOUBLEARRAY_T double_array, int i);
	EXPORTED int STDCALL DoubleArray_copyDoubleValues_stdcall(DOUBLEARRAY_T double_array, double* target, int length);
	
	EXPORTED FLOATARRAY_T STDCALL FloatArray_Create_stdcall();
	EXPORTED FLOATARRAY_T STDCALL FloatArray_Create_1_stdcall(float* vals, int length);
	EXPORTED float* STDCALL FloatArray_getFloatValues_stdcall(FLOATARRAY_T float_array);
	EXPORTED int STDCALL FloatArray_getLength_stdcall(FLOATARRAY_T float_array);
	EXPORTED void STDCALL FloatArray_setFloatValues_stdcall(FLOATARRAY_T float_array, float* values, int len);
	EXPORTED void STDCALL FloatArray_Destroy_stdcall(FLOATARRAY_T float_array);
	EXPORTED float STDCALL FloatArray_getElementAt_stdcall(FLOATARRAY_T float_array, int i);
	EXPORTED int STDCALL FloatArray_copyFloatValues_stdcall(FLOATARRAY_T float_array, float* target, int length);
#endif /* WIN32 */

#ifdef __cplusplus
};
#endif

#endif // ARRAYTYPES_H
