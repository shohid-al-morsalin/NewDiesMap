// DMCMotorBase.h: interface for the CDMCMotorBase class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
class CDMCMotorBase {
public:
	BOOL bRun;
	CString MotorFile;

public:
	virtual BOOL Send(char* cmd);
	BOOL LoadProgram(CString file);
	virtual BOOL LoadProgramA();
	virtual BOOL Deinitialize();
	virtual BOOL Initialize(CString Filename, CListBox* pMessage);
	virtual BOOL Reload(CString Filename);
	virtual BOOL Stop();
	virtual BOOL Run();

	void Log(CString msg, short lvl = 5);

	CDMCMotorBase();
	virtual ~CDMCMotorBase();
};
