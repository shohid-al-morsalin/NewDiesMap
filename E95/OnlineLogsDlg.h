#pragma once
#include "afxcmn.h"
#include "afxdtctl.h"

#include "../online/mfc/dib256.h"

#include "../online/mfc/ColorEdit.h"
#include "../online/mfc/ColorButton.h"
#include "../online/mfc/ColorList.h"
#include "../online/mfc/ImageStatic.h"

// COnlineLogsDlg dialog

class COnlineLogsDlg : public CDialog {
	DECLARE_DYNAMIC(COnlineLogsDlg)

	UINT m_CurrMessage;

	struct MessageData {
		std::string FileName;
		UINT OffSet, Pos;
	};
	std::vector<MessageData> m_MsgDat;
	int m_Recordcnt;
	int m_CurRecordcnt;
	void WriteDataToList(CListCtrl& List, const char* Data);
	void Display_OnScreen();
	void Display_GemData(COleDateTime pStime, COleDateTime pEtime);
	void SetCol(int MsgType);
public:
	COnlineLogsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~COnlineLogsDlg();

	// Dialog Data
	enum { IDD = IDD_ONLINELOGSDLG };

protected:
	virtual void PostNcDestroy();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CColorEdit m_Time;
	CColorEdit m_Wbit;
	CColorEdit m_Sysbyte;
	CColorEdit m_Stream;
	CColorEdit m_Function;
	CColorEdit m_RecordCount;
	CColorButton m_LogSearch;
	CColorEdit	m_CodeKey;
	CColorEdit	m_Msg;
	CColorEdit	m_Type;
	CColorEdit	m_System;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedLogdown();
protected:
	// Log message display
	CListCtrl m_ListData;
public:
	// Holds list of files used
	CListCtrl m_LogFiles;
	// Start time for logfilter
	CDateTimeCtrl m_STimePicker;
	// End time for log filtering
	CDateTimeCtrl m_ETimePicker;
	// End hour fro log filtering
	CSpinButtonCtrl m_EHour_Spin;
	// Start hour for log filter
	CSpinButtonCtrl m_SHour_Spin;
	// Start minutes to filter logs
	CSpinButtonCtrl m_SMin_Spin;
	afx_msg void OnBnClickedFirst();
	afx_msg void OnBnClickedPrev10();
	afx_msg void OnBnClickedPrev5();
	afx_msg void OnBnClickedPrev();
	afx_msg void OnBnClickedNext();
	afx_msg void OnBnClickedNext5();
	afx_msg void OnBnClickedNext10();
	afx_msg void OnBnClickedLast();
	virtual BOOL OnInitDialog();
	// End interval for Log filter
	CSpinButtonCtrl m_EMin_Spin;
};
