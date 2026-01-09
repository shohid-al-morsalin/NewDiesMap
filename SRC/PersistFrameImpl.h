
#ifndef __PERSISTWNDIMPL_H__
#define __PERSISTWNDIMPL_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


/*************************************************************************
 FILE       : PersistFrameImpl.h

 Author		: Stefan Chekanov (stefant@iname.com)

 Description: Definition of CPersistFrameImpl class which implements persistent 
			  Frame windows (MDI, MDI Child and SDI) common behaviour.

Copyright(c) 1998,1999,2000
by Stefan Chekanov (stefant@iname.com)

This code may be used in compiled form in any way you desire. This
file may be redistributed unmodified by any means PROVIDING it is 
not sold for profit without the authors written consent, and 
providing that this notice and the authors name is included. If 
the source code in this file is used in any commercial application 
then a simple email to the author would be nice.

This file is provided "as is" with no expressed or implied warranty.
The author accepts no liability if it causes any damage.

*************************************************************************/
/* #    Revisions    # */

//////////////////////////////////////////////////////////////////////

class CPersistFrameImpl
{
public:
	CPersistFrameImpl( CFrameWnd* pWnd = NULL, LPCTSTR szHeading = NULL, bool bBarStates = false );
	virtual ~CPersistFrameImpl();

	virtual	void	Load();
	virtual	void	Save();

	UINT				getCmdShow() {
		return	m_nCmdShow;
	}

	LPWINDOWPLACEMENT	getWindowsPlacement() {
		return	&m_wndpl;
	}

	//	Set the window for which this object load/save state info
	void	SetWindow( CFrameWnd* pWnd ) {
		m_pWnd = pWnd;
	}

	//	Set the registry section where the state info to be saved/loaded from
	void	SetProfileHeading( LPCTSTR szHeading ) {
		m_profileHeading = szHeading;
	}

	//	Specify whether positions of control bars should be saved/restored too.
	void	SetManageBarStates( bool bManage ) {
		m_bManageBarStates = bManage;
	}

private:
	CFrameWnd*		m_pWnd;
	UINT			m_nCmdShow;
    WINDOWPLACEMENT	m_wndpl;

	CString		m_profileHeading;			//	The assigned Profile Heading
	bool		m_bManageBarStates;			//	True if we should manage bar states too
											//	By default it is false

    static TCHAR	s_profileHeading[];		//	Default profile heading
    static TCHAR	s_profileRect[];
    static TCHAR	s_profileIcon[];
    static TCHAR	s_profileMax[];
	static TCHAR	m_profileBarState[];
};
//////////////////////////////////////////////////////////////////////

#endif	//	__PERSISTWNDIMPL_H__
