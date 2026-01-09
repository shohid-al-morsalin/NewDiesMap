#pragma once

#define WM_GETTHICKNESS				(WM_USER+140)
#define WM_GETINFO					(WM_USER+141)
#define WM_SETPARS					(WM_USER+142)
#define WM_GETTHICKNESS2			(WM_USER+143)
#define WM_GETINFO2					(WM_USER+144)
#define WM_SETPARS2					(WM_USER+145)
#define WM_USER_ASYNC_SELECT2		(WM_USER+146)

#define SAMPLE_NONE					(0)
#define SAMPLE_BARE					(11)
#define SAMPLE_TAPE_SI				(12)
#define SAMPLE_PATTERN_SI			(13)
#define SAMPLE_TAPE_PATTERN_SI		(14)
#define SAMPLE_PATTERN_SI_TAPE		(15)
#define SAMPLE_TAPE_TAPE2_PATTERN_SI (16)
#define SAMPLE_TOTAL				(17)
#define SAMPLE_PATTERN_SI_TAPE2_TAPE (18)
#define SAMPLE_WARP_MEASUREMENT		(19)
#define SAMPLE_ROUGHNESS			(20)
#define SAMPLE_TRENCH_DEPTH			(21)
#define SAMPLE_STRESS_MEASUREMENT	(22)

#define WAFERCAL_BUTTON_L_MOUSEDOWN	(WM_USER+226)
#define WAFERCAL_BUTTON_L_MOUSEUP	(WM_USER+227)
#define WM_COMM_BREAK_DETECTED		WM_USER+228	// A break was detected on input.
#define WM_COMM_CTS_DETECTED		WM_USER+229	// The CTS (clear-to-send) signal changed state.
#define WM_COMM_DSR_DETECTED		WM_USER+230	// The DSR (data-set-ready) signal changed state.
#define WM_COMM_ERR_DETECTED		WM_USER+231	// A line-status error occurred. Line-status errors are CE_FRAME, CE_OVERRUN, and CE_RXPARITY.
#define WM_COMM_RING_DETECTED		WM_USER+232	// A ring indicator was detected.
#define WM_COMM_RLSD_DETECTED		WM_USER+233	// The RLSD (receive-line-signal-detect) signal changed state.
#define WM_COMM_RXCHAR				WM_USER+234// A character was received and placed in the input buffer.
#define WM_COMM_RXFLAG_DETECTED		WM_USER+235	// The event character was received and placed in the input buffer.
#define WM_COMM_TXEMPTY_DETECTED	WM_USER+236	// The last character in the output buffer was sent.

#define WM_PROCJOB_COMPLETED		WM_USER+237
#define WM_PRCSJOB_FAILED			WM_USER+238
#define WM_PRCSJOB_CANCELLED		WM_USER+239

// #define CIDR_STARTED				(4000+641)
// #define CIDR_STARTFAILED			(4000+642)
