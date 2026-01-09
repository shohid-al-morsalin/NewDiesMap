#pragma once

#include "combase.h"

class CComSMIF : public CComBase {
public:

	void Decode(CString& msg);

	BOOL SetDefault();

	virtual BOOL Connect(short port);

	virtual BOOL Disconnect();

	CComSMIF();
	virtual ~CComSMIF();
};
