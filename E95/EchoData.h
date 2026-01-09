// EchoData.h: interface for the CEchoData class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
class CEchoData {
public:
	long Binary(char** msg);
	void Conv(char** msg);
	long GetLength();

	CEchoData();
	virtual ~CEchoData();
};
