#pragma once

class CChartWnd : public CStatic {
	// Construction
public:
	CChartWnd();

	short* pData;
	short nData;

	short Ymax, Ymin;


	short* pData2;//Tushar
	short Ymax2, Ymin2;//Tushar
	BOOL isDoublePlot = false;//Tushar
	short nData2;//Tushar

	// Attributes
public:

	// Operations
public:

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CChartWnd)
		//}}AFX_VIRTUAL

	// Implementation
public:
	void Set(short* pData, short n);
	void Set2(short* pData,short* pData2, short n);//Tushar
	//void setIsDoublePlot(bool isDoublePlot);//tushar
	virtual ~CChartWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CChartWnd)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
