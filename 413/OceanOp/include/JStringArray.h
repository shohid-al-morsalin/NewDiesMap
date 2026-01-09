/***
 * JStringArray.h
 * Date: Wed Feb 20 16:37:46 EST 2008
 * Copyright (C) 2006 - 2008 Ocean Optics, Inc.  All rights reserved.
 */

#ifndef JSTRINGARRAY_H
#define JSTRINGARRAY_H
#include "ArrayTypes.h"
#include "JString.h"

#ifdef __cplusplus
class EXPORTED JStringArray : public AbstractArray {
public:
	/* Default constructor */
	JStringArray();

	/* Initialization constructor */
	JStringArray(JString *objects, int length);

	/* Copy constructor */
	JStringArray(const JStringArray &that);

	/* Destructor */
	~JStringArray();

	/* Assignment operator */
	JStringArray &operator=(const JStringArray &that);

	/* Setter */
	void setObjects(JString *objects, int length);

	/* Getter */
	JString *getObjects();

	/* Getter for specific index */
	JString getElementAt(int index);

	/* Utility method for internal use only */
	char *getClassDescriptor();

private:
	JString *data;
};

extern "C" {
#endif /* __cplusplus */
#ifndef EXTERN_TYPEDEF_JSTRINGARRAY_T
#define EXTERN_TYPEDEF_JSTRINGARRAY_T
	typedef void* JSTRINGARRAY_T;
#endif /* EXTERN_TYPEDEF_JSTRINGARRAY_T */

	EXPORTED JSTRINGARRAY_T JStringArray_Create();
	EXPORTED JSTRINGARRAY_T JStringArray_Create_1(JSTRING* objects, int length);
	EXPORTED void JStringArray_Destroy(JSTRINGARRAY_T string_array);
	EXPORTED int JStringArray_getLength(JSTRINGARRAY_T string_array);
	EXPORTED void JStringArray_setObjects(JSTRINGARRAY_T string_array, JSTRING* objects, int length);
	EXPORTED void JStringArray_getElementAt(JSTRINGARRAY_T string_array, int index, JSTRING target);
	EXPORTED int JStringArray_copyObjectsInto(JSTRINGARRAY_T string_array, JSTRING* target, int length);
#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif /* JSTRINGARRAY_H */
