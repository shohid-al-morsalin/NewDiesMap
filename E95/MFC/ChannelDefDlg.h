#if !defined(AFX_CHANNELDEFDLG_H__CF316D01_BE8F_11D3_889C_0020182D5632__INCLUDED_)
#define AFX_CHANNELDEFDLG_H__CF316D01_BE8F_11D3_889C_0020182D5632__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChannelDefDlg.h : header file
//
#include <vector>

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CChannelDefDlg dialog


class CChannelDefDlg : public CDialog
{
	BOOL VisMask[37];	// start counting with 1c
	CButton *pBtnList[36];
	std::vector<CString> NameList;
	short mass[36];
	// Attributes
public:
	void SetNames(const std::vector<CString> &Names)
	{
		NameList.assign(Names.begin(),Names.end());
	}
	void SetNames(const std::vector<CString> &Names,const std::vector<BOOL> &Visibility)
	{
		NameList.assign(Names.begin(),Names.end());
		for(int Index =0;Index < int(Visibility.size());Index++)
		{
			VisMask[Index+1] = Visibility[Index];
		}
	}
	void GetSelNames(std::vector<CString> &Names);

	
// Construction
	CChannelDefDlg(CWnd* pParent = NULL);   // standard constructor
private:
	void ClearSlot(short which);
	void UpdateSlot(short slt);

// Dialog Data
	//{{AFX_DATA(CChannelDefDlg)
	enum { IDD = IDD_ELEMENT };
	CButton	m_ch36;
	CButton	m_ch35;
	CButton	m_ch34;
	CButton	m_ch33;
	CButton	m_ch9;
	CButton	m_ch8;
	CButton	m_ch7;
	CButton	m_ch6;
	CButton	m_ch5;
	CButton	m_ch4;
	CButton	m_ch32;
	CButton	m_ch31;
	CButton	m_ch30;
	CButton	m_ch3;
	CButton	m_ch29;
	CButton	m_ch28;
	CButton	m_ch27;
	CButton	m_ch26;
	CButton	m_ch25;
	CButton	m_ch24;
	CButton	m_ch23;
	CButton	m_ch22;
	CButton	m_ch21;
	CButton	m_ch20;
	CButton	m_ch2;
	CButton	m_ch19;
	CButton	m_ch18;
	CButton	m_ch17;
	CButton	m_ch16;
	CButton	m_ch15;
	CButton	m_ch14;
	CButton	m_ch13;
	CButton	m_ch12;
	CButton	m_ch11;
	CButton	m_ch10;
	CButton	m_ch1;
	BOOL	m_chn1;
	BOOL	m_chn10;
	BOOL	m_chn11;
	BOOL	m_chn12;
	BOOL	m_chn13;
	BOOL	m_chn14;
	BOOL	m_chn15;
	BOOL	m_chn16;
	BOOL	m_chn17;
	BOOL	m_chn18;
	BOOL	m_chn19;
	BOOL	m_chn2;
	BOOL	m_chn20;
	BOOL	m_chn21;
	BOOL	m_chn22;
	BOOL	m_chn23;
	BOOL	m_chn24;
	BOOL	m_chn25;
	BOOL	m_chn26;
	BOOL	m_chn27;
	BOOL	m_chn28;
	BOOL	m_chn29;
	BOOL	m_chn3;
	BOOL	m_chn30;
	BOOL	m_chn31;
	BOOL	m_chn32;
	BOOL	m_chn4;
	BOOL	m_chn5;
	BOOL	m_chn6;
	BOOL	m_chn7;
	BOOL	m_chn8;
	BOOL	m_chn9;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChannelDefDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CChannelDefDlg)
	afx_msg void OnChannel1();
	afx_msg void OnChannel10();
	afx_msg void OnChannel11();
	afx_msg void OnChannel12();
	afx_msg void OnChannel13();
	afx_msg void OnChannel14();
	afx_msg void OnChannel15();
	afx_msg void OnChannel16();
	afx_msg void OnChannel17();
	afx_msg void OnChannel18();
	afx_msg void OnChannel19();
	afx_msg void OnChannel2();
	afx_msg void OnChannel20();
	afx_msg void OnChannel21();
	afx_msg void OnChannel22();
	afx_msg void OnChannel23();
	afx_msg void OnChannel24();
	afx_msg void OnChannel25();
	afx_msg void OnChannel26();
	afx_msg void OnChannel27();
	afx_msg void OnChannel28();
	afx_msg void OnChannel29();
	afx_msg void OnChannel3();
	afx_msg void OnChannel30();
	afx_msg void OnChannel31();
	afx_msg void OnChannel32();
	afx_msg void OnChannel4();
	afx_msg void OnChannel5();
	afx_msg void OnChannel6();
	afx_msg void OnChannel7();
	afx_msg void OnChannel8();
	afx_msg void OnChannel9();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnClearall();
	afx_msg void OnChannel33();
	afx_msg void OnChannel34();
	afx_msg void OnChannel35();
	afx_msg void OnChannel36();
	afx_msg void OnCleartrend();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHANNELDEFDLG_H__CF316D01_BE8F_11D3_889C_0020182D5632__INCLUDED_)
