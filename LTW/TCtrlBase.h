// TCtrlBase.h: interface for the CTCtrlBase class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
class CTCtrlBase {
public:
	enum LIGHTSIGNAL {
		OFF, ON, BLINK, BEEP1, BEEP2, NOSTATE
	};
public:
	void Get(char& Rd, char& Am, char& Gr, char& Bl);
	char GetState(CTCtrlBase::LIGHTSIGNAL st);
	void Set(LIGHTSIGNAL Red, LIGHTSIGNAL Amber, LIGHTSIGNAL Green, LIGHTSIGNAL Blue = OFF);
	virtual BOOL SetConditionAllOff();
	virtual BOOL Init();
	BOOL bUpdate;
	LIGHTSIGNAL GetState(char st);
	void Set(char Rd, char Am, char Gr, char Bl = '0');

	CTCtrlBase();
	virtual ~CTCtrlBase();
	virtual void Update();

protected:
	LIGHTSIGNAL Red;
	LIGHTSIGNAL Green;
	LIGHTSIGNAL Amber;
	LIGHTSIGNAL Blue;
};