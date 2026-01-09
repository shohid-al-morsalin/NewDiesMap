#ifndef _OSSMEMS_H_
#define _OSSMEMS_H_

#define OBJIDLEN			64
#define OBJTYPELEN			64

interface OSSAttr {
public:
	enum ATTRITYPE {
		UNKN, STR, IN2, IN4, FLOT, DBLE, CHR
	};
public:
	ATTRITYPE GetAttrType(CString AttrName);
	void ClearAllAttr();
	void GetAttrName(CStringArray& AttrNameList);
	BOOL GetAttr(CString AttrName, CString& Attr);
};

struct OSSMemS : public OSSAttr {
	char ObjID[OBJIDLEN + 1];
	char ObjType[OBJTYPELEN + 1];

public:

	void Preset();
	void SetID(char* ID);
	void SetType(char* Type);
	short GetBinarySize();
};
#endif