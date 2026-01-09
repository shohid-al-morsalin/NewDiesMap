#pragma once

class CWMap {
public:
	float max = 0;
	float min = 0;
	float* buff = NULL;
	float* scale = NULL;
	int ht = 0;
	int wd = 0;
	short level = 0;

public:
	BOOL AutoLevel(short lv);
	BOOL BackFill(float con);
	BOOL Create(int w, int h);
	BOOL Expand();
	BOOL Fill();
	BOOL Set(int x, int y, float v);
	BOOL SetScale(float* sc, short lv);
	CString GetLevelStr(short lv);
	float Get(int x, int y);
	float GetQN(int x, int y);
	short GetIndex(short l);
	void MaxMin();

public:
	CWMap();
	virtual ~CWMap();
};
