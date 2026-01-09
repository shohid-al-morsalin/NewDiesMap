// TelnetMsg.h: interface for the CTelnetMsg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TELNETMSG_H__5163EAE8_DFBA_4B2D_B949_F87753F29C07__INCLUDED_)
#define AFX_TELNETMSG_H__5163EAE8_DFBA_4B2D_B949_F87753F29C07__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTelnetMsg  
{
public:
	unsigned int idx;
	short Axis[8];		// Allocated for 8 axis only [6/4/2012 Administrator]
	CString msg;
	CString ret;

	CTelnetMsg();
	virtual ~CTelnetMsg();

};

#endif // !defined(AFX_TELNETMSG_H__5163EAE8_DFBA_4B2D_B949_F87753F29C07__INCLUDED_)
