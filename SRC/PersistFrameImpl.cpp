
/*************************************************************************
 FILE       : PersistWndImpl.cpp

 Author		: Stefan Chekanov (stefant@iname.com)

 Description: Implements persistent Frame windows (MDI, MDI Child and SDI)
			  common behaviour.

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

#include "stdafx.h"
#include "PersistFrameImpl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// static
TCHAR CPersistFrameImpl::s_profileHeading[]		= _T( "Window size" );
TCHAR CPersistFrameImpl::s_profileRect[]		= _T( "Rectangle" );
TCHAR CPersistFrameImpl::s_profileIcon[]		= _T( "Iconized" );
TCHAR CPersistFrameImpl::s_profileMax[]			= _T( "Maximized" );
TCHAR CPersistFrameImpl::m_profileBarState[]	= _T( "BarState" );

//////////////////////////////////////////////////////////////////////

CPersistFrameImpl::CPersistFrameImpl( CFrameWnd* pWnd, LPCTSTR szHeading /*= NULL*/, bool bBarStates /*= false*/ )
{
	m_pWnd = pWnd;

	m_wndpl.length = sizeof(WINDOWPLACEMENT);
	
	if( szHeading != NULL ) {
		SetProfileHeading( szHeading );
	}
	else {
		SetProfileHeading( s_profileHeading );
	}

	m_bManageBarStates = false;
}
//////////////////////////////////////////////////////////////////////

CPersistFrameImpl::~CPersistFrameImpl()
{
}
//////////////////////////////////////////////////////////////////////

//	Load the window state from registry and apply the new state on the window
void	CPersistFrameImpl::Load()
{
	ASSERT( m_pWnd != NULL );

    WINDOWPLACEMENT	m_wndpl;
	CString			strText;

	BOOL	bRet = m_pWnd->GetWindowPlacement( &m_wndpl );
	CRect	rect = m_wndpl.rcNormalPosition;

	//	Gets current window position and minimized/maximized status from Registry
	strText = AfxGetApp()->GetProfileString( m_profileHeading, s_profileRect );
	if( ! strText.IsEmpty() ) {
		rect.left	= _ttoi( (const char*) strText.Mid(0,4) );
		rect.top	= _ttoi( (const char*) strText.Mid(5,4) );
		rect.right	= _ttoi( (const char*) strText.Mid(10,4) );
		rect.bottom = _ttoi( (const char*) strText.Mid(15,4) );
	}

	BOOL	bIconic = AfxGetApp()->GetProfileInt( m_profileHeading, s_profileIcon, 0 );
	BOOL	bMaximized = AfxGetApp()->GetProfileInt( m_profileHeading, s_profileMax, 0 );   
    UINT flags;

	if( bIconic ) {
		m_nCmdShow = SW_SHOWMINNOACTIVE;
		if( bMaximized ) {
			flags = WPF_RESTORETOMAXIMIZED;
		}
		else {
			flags = WPF_SETMINPOSITION;
		}
	}
	else {
		if( bMaximized ) {
			m_nCmdShow = SW_SHOWMAXIMIZED;
			flags = WPF_RESTORETOMAXIMIZED;
		}
		else {
			m_nCmdShow = SW_NORMAL;
			flags = WPF_SETMINPOSITION;
		}
	}

	//	Make the window have new settings
	m_wndpl.showCmd = m_nCmdShow;
	m_wndpl.flags = flags;
	m_wndpl.ptMinPosition = CPoint(0, 0);
	m_wndpl.ptMaxPosition = CPoint( -::GetSystemMetrics(SM_CXBORDER),
									-::GetSystemMetrics(SM_CYBORDER) );
	m_wndpl.rcNormalPosition = rect;

	m_pWnd->SetWindowPlacement( &m_wndpl );

	if( m_bManageBarStates )
	{
		CString	cTmp;
		cTmp.Format( "%s\\%s", m_profileHeading, m_profileBarState );
		m_pWnd->LoadBarState( cTmp );
	}
}
//////////////////////////////////////////////////////////////////////

//	Save the window state into registry
void	CPersistFrameImpl::Save()
{
	ASSERT( m_pWnd != NULL );

	CString			strText;
	BOOL			bIconic = FALSE;
	BOOL			bMaximized = FALSE;

	//	Gets current window position and minimized/maximized status
	BOOL	bRet = m_pWnd->GetWindowPlacement( &m_wndpl );
	if( m_wndpl.showCmd == SW_SHOWNORMAL ) {
		bIconic = FALSE;
		bMaximized = FALSE;
	}
	else if( m_wndpl.showCmd == SW_SHOWMAXIMIZED) {
		bIconic = FALSE;
		bMaximized = TRUE;
	} 
	else if( m_wndpl.showCmd == SW_SHOWMINIMIZED) {
		bIconic = TRUE;
		if( m_wndpl.flags ) {
			bMaximized = TRUE;
		}
		else {
			bMaximized = FALSE;
		}
	}
	
	//	Save the state into Registry
	strText.Format( "%04d %04d %04d %04d",
			m_wndpl.rcNormalPosition.left,
			m_wndpl.rcNormalPosition.top,
			m_wndpl.rcNormalPosition.right,
			m_wndpl.rcNormalPosition.bottom );
	AfxGetApp()->WriteProfileString( m_profileHeading, s_profileRect, strText );
	AfxGetApp()->WriteProfileInt( m_profileHeading, s_profileIcon, bIconic );
	AfxGetApp()->WriteProfileInt( m_profileHeading, s_profileMax, bMaximized );

	if( m_bManageBarStates )
	{
		CString	cTmp;
		cTmp.Format( "%s\\%s", m_profileHeading, m_profileBarState );
		m_pWnd->SaveBarState( cTmp );
	}
}
//////////////////////////////////////////////////////////////////////
