#define   WM_USER_ASYNC_SELECT (WM_USER + 0x201)
#define   UWM_USER_ONLINEMODE  (WM_USER + 0x202)
#define   UWM_USER_BMPCHANGE   (WM_USER + 0x203)
#define   UWM_USER_TOOLEDITRPY (WM_USER + 0x204)
#define	  UWM_USER_HOMEFINISHED (WM_USER + 0x205)
#define   UWM_USER_BUTTONCHANGE (WM_USER + 0x206)
#define   WM_HOST_MESSAGE_RECD  (WM_USER + 0x207)
#define	  WM_ALARM_DATA_CHANGED (WM_USER + 0x208)
#define   ACTIVE			1
#define   PASSIVE			0
#define   DATALENSIZE		4

//#define	  OFF				0
//#define	  ON				1

#define   DATADEFAULTSIZE	0x0A

#define   MAX_RECVSTRING	10240

#define   DATAMSG			0x00
#define   SELECTREQ			0x01
#define	  SELECTRSP			0x02
#define   DESELECTREQ		0x03
#define   DESELECTRSP		0x04
#define   LINKREQ			0x05
#define   LINKREP			0x06
#define   REJECTREQ			0x07
#define   SEPERATEREQ		0x09

#define	  PTYPE_POS			4
#define   STYPE_POS			5

#define   PRI_HOST          1
#define   PRI_EQUIP         2

#define   svLIST            0x01
#define   svASCII           0x41
#define   svBINARY		    0x21
#define	  svBOOLEAN			0x25
#define   svUNSIGN1		    0xA5		// 1BYTE UNSIGNED
#define   svUNSIGN2		    0xA9		// 2BYTE UNSIGNED
#define   svUNSIGN4         0xB1        // 4BYTE UNSIGNED
#define   svUNSIGN8         0xA1        // 8BYTE UNSIGNED
#define   svINT1			0x65		// 1BYTE SIGNED
#define   svINT2			0x69		// 2BYTE SIGNED
#define   svINT4			0x71        // 4BYTE SIGNED
#define   svINT8			0x61        // 8BYTE SIGNED
#define   svFLOAT4          0x91        // 4Byte FLOAT
#define   svFLOAT8          0x81        // 8Byte FLOAT

#define   EV_WAIT           0x00
#define   EV_PROC           0x01
#define   EV_SEND           0x03
#define   EV_RESULT         0x04
#define   EV_ABORT			0x05

//#define   EQPOFFLINE        0
//#define   OFFLINE           0
//#define   LOCAL_ONLINE      1
//#define   REMOTE_ONLINE     2
//#define   ATTEMPT_ONLINE	  3
//#define   HOSTOFFLINE       10

// Loader count
//#define   LOADPORTMAX		2
