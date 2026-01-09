#pragma once

#include <string>

class SecsItemList;
class SecsItem;

class DataConverter {
	SecsItem* pFrom;
	void Clear();
public:
	DataConverter();
	void Val(const char*);
	void Val(unsigned char);
	void Val(unsigned short);
	void Val(unsigned int);
	void Val(unsigned __int64);
	void Val(char);
	void Val(short);
	void Val(int);
	void Val(__int64);
	void Val(unsigned char*, UINT);
	void Val(bool);
	void Val(std::string&);
	void Val(float);
	void Val(double);
	void Val(SecsItemList*);
	virtual ~DataConverter();
	operator unsigned char();
	operator unsigned short();
	operator unsigned int();
	operator unsigned __int64();
	operator char();
	operator short();
	operator int();
	operator __int64();
	operator float();
	operator double();
	operator std::string();
	operator SecsItemList* ();
	operator unsigned char* ();
	operator bool();
	SecsItem* GetType(UINT ItmType);
};
