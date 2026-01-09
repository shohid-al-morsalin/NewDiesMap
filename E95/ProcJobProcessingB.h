#pragma once

#include "ProcJobCommon.h"

class CMsgItem;
class CInfoPanelMain;
class CProcJobInfo;
class CCasMgr;
class CEchoData;

class CProcJobProcessingB : public CProcJobCommon {
public:
	void PrepareToStart();
	virtual CWnd* GetThis();
	virtual void ScrnToClnt(LPRECT lpRect);
	virtual CWnd* GetDialogItem(UINT ID);
	CProcJobProcessingB();
	~CProcJobProcessingB();
	BOOL ShowTreeView();
	void ShowReportView();
	void ShowChartView();
	void ShowHeatView();
protected:
	LRESULT MsgInQue(CMsgItem* pMsg);
	LRESULT OnTabSelected(WPARAM wP, LPARAM lP);
public:
	BOOL NoOtherProcJobRuning();
	void RememberPrJob(short unit, CProcJobInfo* pInfo);
	virtual void ScanDataArrived(CEchoData* pScanData) = 0;

	enum VIEWWIN { SCANLINE, HEAT, REPORT, MAP3D, MAP2D, MAPLINE, TREND, SPECTRUM };
	void CreateView(VIEWWIN View);
	void ShowView(VIEWWIN View);
	virtual void SetProcessingHalt(CMsgItem* pMsg) = 0;
	virtual void SetProcessingFail(CMsgItem* pMsg) = 0;
	virtual BOOL SetProcessingComplete(CMsgItem* pMsg) = 0;
	virtual afx_msg void ReloadCassette() = 0;

	CCasMgr* m_pCasMgr;
	BOOL bTreeUnderconstruction;
	BOOL bProcessing;
	BOOL bRecovery;

	enum DISPTOMODULE { NOMODULE, RTMODULE, HTMODULE, ECHOMODULE };  // ECHOMODULE is 413 [6/28/2010 Yuen]
	DISPTOMODULE DispatchTo;
};
