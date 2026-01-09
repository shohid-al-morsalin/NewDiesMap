///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
//  Module:  OOIDRV32.H
//  Project: Ocean Optics 16/32 bit device drivers
//  Purpose:  This file is the C/C++ header for the device drivers
//
//  Note: This source file is designed to be viewed/printed using 10 pt Courier New
//			with tabs set at every 2 spaces
//
//	This header file is Copyright?1998-2003, Ocean Optics, Inc.
//
//  Ocean Optics, Inc.
//  380 Main Street
//	Dunedin, FL  34698
//
//	http://www.OceanOptics.com
//
//  For job opportunities writing software for Ocean Optics, send a cover letter,
//		resume, sample source code and salary requirements to Jobs@OceanOptics.com
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
//
//  Borland Builder Users Please Note:
//  The structures members are aligned at 8 Byte (Quad Word) intervals
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef __OOIDRV
#define __OOIDRV

///////////////////////////////////////////////////////////////////////////////////
//// Define C linkage if using a C++ compiler
///////////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
extern "C" {
#endif

	///////////////////////////////////////////////////////////////////////////////////
	//// Determine callling convention baesd on compiler
	///////////////////////////////////////////////////////////////////////////////////
#ifdef __BORLANDC__
#ifdef __WIN32__
#define EXPORTED __declspec(dllexport) __stdcall
#else
#define EXPORTED FAR PASCAL __export
#endif // WIN32
#else // __BORLANDC__
#ifdef WIN32
#define EXPORTED __declspec(dllexport)
#else
#define EXPORTED FAR PASCAL __export
#endif // WIN32
#endif //__BORLANDC__

///////////////////////////////////////////////////////////////////////////////////
//// Messages sent to application
///////////////////////////////////////////////////////////////////////////////////
#define OOI_STATE				WM_USER + 100	// driver state has changed
#define OOI_DATAREADY			WM_USER + 101	// data is ready in the driver
#define OOI_TIMEOUT				WM_USER + 102	// scan timed out
#define OOI_OVERRUN				WM_USER + 103	// data buffer overrun
#define OOI_SADSYNC				WM_USER + 104	// SAD500 synchronization error
#define OOI_USB_STATE_CHANGE	WM_USER + 200	// USB2000 plugged in

#define USB_STATE_ATTACHED		0
#define USB_STATE_DETACHED		1

///////////////////////////////////////////////////////////////////////////////////
//// Data buffer management status values
///////////////////////////////////////////////////////////////////////////////////
#define BUF_INACTIVE		0	// channel not active
#define BUF_EMPTY			1	// no data in buffer
#define BUF_SCAN			2	// no data, scan in progress
#define BUF_READY			3	// valid data present
#define BUF_INVALID			4	// data will be invalid
#define BUF_TIMEOUT			5	// scan timed out
#define BUF_SADSYNC			6	// SAD500 synchronization error

///////////////////////////////////////////////////////////////////////////////////
//// Spectrometer channel selection
///////////////////////////////////////////////////////////////////////////////////
#define NONE		-1	// no channel
#define MASTER		0	// master channel
#define SLAVE1		1	// first slave channel
#define SLAVE2		2	// second slave channel
#define SLAVE3		3	// third slave channel
#define SLAVE4		4	// fourth slave channel
#define SLAVE5		5	// fifth slave channel
#define SLAVE6		6	// sixth slave channel
#define SLAVE7		7	// seventh slave channel
#define MAX_BUF		8	// maximum number of channel buffers

///////////////////////////////////////////////////////////////////////////////////
//// Driver error codes
///////////////////////////////////////////////////////////////////////////////////
#define ER_NOERROR			0	// no error
#define ER_TIMEOUT			2	// scan timed out
#define ER_BUSY				3	// driver busy acquiring data
#define ER_SADSYNC			4	// SAD500 synchronization error
#define ER_MEMORY			5	// memory buffers not allocated
#define ER_UNKNOWN			6	// unknown application instance
#define ER_TIMER			7	// no timers available
#define ER_ERROR			8	// unknown error
#define ER_NOTHREAD			9	// unable to create Single Scan thread
#define ER_NIR_NOT_FOUND	10	// unable to find NIR attached to system
#define ER_USB2000_NOT_FOUND 11	// unable to find USB2000 attached to system
#define ER_ADC1000USB_NOT_FOUND 12	// unable to find ADC1000-USB attached to system
#define ER_HR2000_NOT_FOUND	13	// unable to find HR2000 attached to system

///////////////////////////////////////////////////////////////////////////////////
//// Driver commands
///////////////////////////////////////////////////////////////////////////////////
#define CMD_NONE		0	// no specific command
#define CMD_SETUP		1	// force hardware setup
#define CMD_STATUS		2	// send status messages
#define CMD_TEST		9	// return test data

///////////////////////////////////////////////////////////////////////////////////
//// Driver messages
///////////////////////////////////////////////////////////////////////////////////
#define MSG_NONE		0	// no specific message

///////////////////////////////////////////////////////////////////////////////////
//// External trigger modes
///////////////////////////////////////////////////////////////////////////////////
#define TRIG_NORMAL		0	// not triggered
#define TRIG_SOFTWARE	1	// external software trigger
#define TRIG_SYNC		2	// external synchronization (S2000 only)
#define TRIG_HARD		3	// external hardware trigger (S2000 only)

///////////////////////////////////////////////////////////////////////////////////
//// Spectrometer Types
///////////////////////////////////////////////////////////////////////////////////
#define S1000		0x0000	// S1000/PC1000/PC1000 spectrometers
#define S2000BT		0x0100	// S2000BT spectrometer
#define S2000		0x0200	// S2000/PC2000 spectrometers
#define S1024DW		0x0300	// S1024DW spectrometer (SAD500 only!)
#define NIR512		0x0400	// NIR512 IR Spectrometer
#define NIR256		0x0500	// NIR256 IR Spectrometer
#define S4000		0x0600
#define QE65000		0x0700	// QE65000
///////////////////////////////////////////////////////////////////////////////////
//// A/D adapter types
///////////////////////////////////////////////////////////////////////////////////
#define ADC500			0x0000 // ADC500/PC1000
#define DAQ700			0x0001 // DAQ700
#define ADC1000			0x0002 // ADC1000/PC2000
#define SAD500			0x0003 // SAD500
#define USB2000			0x0004 // USB2000
#define SERIALUSB		0x0005 // SERIAL USB2000
#define ADC2000PCI		0x0006 // ADC2000-PCI
#define ADC1000USB		0x0007 // ADC1000-USB
#define SERIALADC		0x0008 // all serial A/D converters
#define HR2000			0x0009 // HR2000
#define PC104			0x000A // PC104
#define NIRADC			0x000B // NIR-USB ADC
#define HR4000		0x000C
#define HR2000PLUS	0x000D // HR2000Plus
#define QEADC	0x000E // QE65000
#define USB2000PLUS	0x000F // USB2000Plus
#define USB4000		0x0010 // USB4000
#define NO_ADC			0x00FF // No A/D adapter

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
//// The following constants are for use with Ocean Optics SAD500 ONLY!!!
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
//// SAD500 return codes
///////////////////////////////////////////////////////////////////////////////////
#define SAD_RETURN_SUCCESS						1		// success
#define SAD_RETURN_FAILURE						0		// failure
#define SAD_RETURN_COM_FAILURE					-1		// com port not initialized
#define SAD_RETURN_TIMEOUT						-2		// scan timeout
#define SAD_RETURN_COM_CAN_NOT_FIND_COM_DLL		-3		// can not find serial library
#define SAD_RETURN_BAD_HEADER					-4		// bad scan header
#define SAD_RETURN_BUSY_SCANNING				-5		// driver busy scanning
#define SAD_RETURN_NOT_ALLOWED					-6		// function execution not allowed
#define SAD_RETURN_OUT_OF_SYNC					-7		// communication out of sync
#define SAD_RETURN_BAD_CHECKSUM					-8		// bad checksum
#define SAD_RETURN_DEVICE_NOT_COMPATIBLE		-9		// issued command not supported by device
#define SAD_RETURN_PARAMETER_BAD				-9999	// invalid paramter

///////////////////////////////////////////////////////////////////////////////////
//// SAD500 serial ports
///////////////////////////////////////////////////////////////////////////////////
#define SAD_COM_1	1 // COM1
#define SAD_COM_2	2 // COM2
#define SAD_COM_3	3 // COM3
#define SAD_COM_4	4 // COM4
#define SAD_COM_5	5 // COM5
#define SAD_COM_6	6 // COM6
#define SAD_COM_7	7 // COM7
#define SAD_COM_8	8 // COM8

///////////////////////////////////////////////////////////////////////////////////
//// SAD500 close commands
///////////////////////////////////////////////////////////////////////////////////
#define SAD_CLOSE_NO_SAVE				0 // close with save
#define SAD_CLOSE_STORE_ALL_BUT_BAUD	1 // save all parameters but baud
#define SAD_CLOSE_STORE_ALL				2 // save all parameters including baud
#define SAD_CLOSE_STORE_BAUD_DEFAULT	3 // save all parameter and default baud

///////////////////////////////////////////////////////////////////////////////////
//// SAD500 data compression modes
///////////////////////////////////////////////////////////////////////////////////
#define SAD_COMPRESSION_OFF		0	// binary data compression off
#define SAD_COMPRESSION_ON		1	// binary data compression on

///////////////////////////////////////////////////////////////////////////////////
//// SAD500 correlated doubling sampling modes
///////////////////////////////////////////////////////////////////////////////////
#define SAD_CDS_OFF				0	// correlated double sampling off
#define SAD_CDS_ON				1	// correlated double on

///////////////////////////////////////////////////////////////////////////////////
//// SAD500 checksum modes
///////////////////////////////////////////////////////////////////////////////////
#define SAD_CHECKSUM_OFF		0	// checksum off
#define SAD_CHECKSUM_ON			1	// checksum on

///////////////////////////////////////////////////////////////////////////////////
//// SAD500 memory types

///////////////////////////////////////////////////////////////////////////////////
#define SAD_MEMORY_BOTH			0	// both fast and slow memory
#define SAD_MEMORY_FAST			1	// fast memory only
#define SAD_MEMORY_SLOW			2	// slow memory only

///////////////////////////////////////////////////////////////////////////////////
//// SAD500 spectrometer channels
///////////////////////////////////////////////////////////////////////////////////
#define SAD_MASTER		0	// master channel
#define SAD_SLAVE1		1	// first slave channel
#define SAD_SLAVE2		2	// second slave channel
#define SAD_SLAVE3		3	// third slave channel
#define SAD_SLAVE4		4	// fourth slave channel
#define SAD_SLAVE5		5	// fifth slave channel
#define SAD_SLAVE6		6	// sixth slave channel
#define SAD_SLAVE7		7	// seventh slave channel

///////////////////////////////////////////////////////////////////////////////////
//// SAD500 scan modes
///////////////////////////////////////////////////////////////////////////////////
#define SAD_SCAN_RETURN_IMMEDIATELY		0	// scans returned immediately
#define SAD_SCAN_TO_FAST_MEMORY			1	// scans store in fast memory
#define SAD_SCAN_TO_SLOW_MEMORY			2	// scans store in slow memory

///////////////////////////////////////////////////////////////////////////////////
//// SAD500 pixel modes
///////////////////////////////////////////////////////////////////////////////////
#define SAD_PIXELMODE_ALL								0x0000	// transmit all pixels
#define SAD_PIXELMODE_EVERY_N							0x0001	// transmit every N pixels
#define SAD_PIXELMODE_EVERY_N_AVERAGED					0x0002	// transmit every N pixels with averaging
#define SAD_PIXELMODE_X_TO_Y_EVERY_N					0x0003	// transmit pixel X to Y every N pixels
#define SAD_PIXELMODE_SELECTED							0x0004	// transmit up to 81 selected pixels
// the modes below are identical to the modes above, except the data is compressed
#define SAD_PIXELMODE_ALL_COMPRESSED					0x0100
#define SAD_PIXELMODE_EVERY_N_COMPRESSED				0x0101
#define SAD_PIXELMODE_EVERY_N_AVERAGED_COMPRESSED		0x0102
#define SAD_PIXELMODE_X_TO_Y_EVERY_N_COMPRESSED			0x0103
#define SAD_PIXELMODE_SELECTED_COMPRESSED				0x0104
// the modes below are identical to the modes above, except CDS is active
#define SAD_PIXELMODE_ALL_CDS							0x0200	// transmit all pixels
#define SAD_PIXELMODE_EVERY_N_CDS						0x0201	// transmit every N pixels
#define SAD_PIXELMODE_EVERY_N_AVERAGED_CDS				0x0202	// transmit every N pixels with averaging
#define SAD_PIXELMODE_X_TO_Y_EVERY_N_CDS				0x0203	// transmit pixel X to Y every N pixels
#define SAD_PIXELMODE_SELECTED_CDS						0x0204	// transmit up to 81 selected pixels
#define SAD_PIXELMODE_ALL_COMPRESSED_CDS				0x0300
#define SAD_PIXELMODE_EVERY_N_COMPRESSED_CDS			0x0301
#define SAD_PIXELMODE_EVERY_N_AVERAGED_COMPRESSED_CDS	0x0302
#define SAD_PIXELMODE_X_TO_Y_EVERY_N_COMPRESSED_CDS		0x0303
#define SAD_PIXELMODE_SELECTED_COMPRESSED_CDS			0x0304

///////////////////////////////////////////////////////////////////////////////////
//// SAD500 interpolation of missing pixels
///////////////////////////////////////////////////////////////////////////////////
#define SAD_DO_NOT_INTERPOLATE_MISSING_PIXELS	0	// do not interpolate
#define SAD_INTERPOLATE_MISSING_PIXELS			1	// interpolate missing pixels

///////////////////////////////////////////////////////////////////////////////////
//// SAD500 external trigger modes
///////////////////////////////////////////////////////////////////////////////////
#define SAD_EXTRIG_NORMAL		0	// normal, no triggering
#define SAD_EXTRIG_SOFTWARE		1	// external software trigger
#define SAD_EXTRIG_SYNC			2	// external synchronization
#define SAD_EXTRIG_HARDWARE		3	// external hardware trigger

///////////////////////////////////////////////////////////////////////////////////
//// SAD500 baud rates
///////////////////////////////////////////////////////////////////////////////////
#define SAD_BAUD_2400			0	// 2400 baud
#define SAD_BAUD_4800			1	// 4800 baud
#define SAD_BAUD_9600			2	// 9600 baud
#define SAD_BAUD_19200			3	// 19200 baud
#define SAD_BAUD_38400			4	// 38400 baud
#define SAD_BAUD_57600			5	// 57600 baud
#define SAD_BAUD_115200			6	// 115200 baud

///////////////////////////////////////////////////////////////////////////////////
//// SAD500 error values
///////////////////////////////////////////////////////////////////////////////////
#define SAD_ERROR_LOAD_BOOT_TABLE			0x8000	// can not load boot table
#define SAD_ERROR_SLOW_MEM_FULL				0x4000	// slow memory full
#define SAD_ERROR_CREATE_BOOT_TABLE			0x2000	// can not create boot table
#define SAD_ERROR_MUST_MOVE_BLOCK			0x1000	// must move bad block table
#define SAD_ERROR_CAN_NOT_READ_BLOCK		0x0800	// can not read memory block
#define SAD_ERROR_CAN_NOT_FIND_MEMORY		0x0400	// can not find start of next scan
#define SAD_ERROR_BAD_FORMAT				0x0200	// bad pixel mode format
#define SAD_ERROR_CAN_NOT_WRITE_BLOCK		0x0100	// can not write memory block
#define SAD_ERROR_UNKNOWN8					0x0080	// reserved error value
#define SAD_ERROR_UNKNOWN7					0x0040	// reserved error value
#define SAD_ERROR_UNKNOWN6					0x0020	// reserved error value
#define SAD_ERROR_UNKNOWN5					0x0010	// reserved error value
#define SAD_ERROR_UNKNOWN4					0x0008	// reserved error value
#define SAD_ERROR_UNKNOWN3					0x0004	// reserved error value
#define SAD_ERROR_UNKNOWN2					0x0002	// reserved error value
#define SAD_ERROR_UNKNOWN1					0x0001	// reserved error value
#define SAD_ERROR_NONE						0x0000	// no error

///////////////////////////////////////////////////////////////////////////////////
//// SAD500 scan receive status
///////////////////////////////////////////////////////////////////////////////////
#define SAD_SCAN_OK			0	// scan received OK
#define SAD_SCAN_NOT_OK		1	// scan not received OK

///////////////////////////////////////////////////////////////////////////////////
//// SAD500 memory slots
///////////////////////////////////////////////////////////////////////////////////
#define SAD_SLOT_SERIALNUMBER				0
#define SAD_SLOT_WLINTERCEPT				1
#define SAD_SLOT_WLFIRST					2
#define SAD_SLOT_WLSECOND					3
#define SAD_SLOT_WLTHIRD					4
#define SAD_SLOT_STRAYLIGHT					5
#define SAD_SLOT_NL0						6
#define SAD_SLOT_NL1						7
#define SAD_SLOT_NL2						8
#define SAD_SLOT_NL3						9
#define SAD_SLOT_NL4						10
#define SAD_SLOT_NL5						11
#define SAD_SLOT_NL6						12
#define SAD_SLOT_NL7						13
#define SAD_SLOT_NLORDER					14

///////////////////////////////////////////////////////////////////////////////////
//// Serial A/D converter types
///////////////////////////////////////////////////////////////////////////////////
#define SERIALADC_SAD500					0
#define SERIALADC_USB2000					1
#define SERIALADC_ADC1000USB				2
#define SERIALADC_HR2000					3
#define SERIALADC_NIRADC					4

///////////////////////////////////////////////////////////////////////////////////
//// USB2000 memory slots
///////////////////////////////////////////////////////////////////////////////////
#define USB2000_SLOT_SERIALNUMBER			0
#define USB2000_SLOT_WLINTERCEPT			1
#define USB2000_SLOT_WLFIRST				2
#define USB2000_SLOT_WLSECOND				3
#define USB2000_SLOT_WLTHIRD				4
#define USB2000_SLOT_STRAYLIGHT				5
#define USB2000_SLOT_NL0					6
#define USB2000_SLOT_NL1					7
#define USB2000_SLOT_NL2					8
#define USB2000_SLOT_NL3					9
#define USB2000_SLOT_NL4					10
#define USB2000_SLOT_NL5					11
#define USB2000_SLOT_NL6					12
#define USB2000_SLOT_NL7					13
#define USB2000_SLOT_NLORDER				14

///////////////////////////////////////////////////////////////////////////////////
//// ADC1000-USB memory slots
///////////////////////////////////////////////////////////////////////////////////
#define ADC1000USB_SLOT_SERIALNUMBER		0
#define ADC1000USB_SLOT_CHANNELSENABLED		1
#define ADC1000USB_SLOT_CH0_WLINTERCEPT		2
#define ADC1000USB_SLOT_CH0_WLFIRST			3
#define ADC1000USB_SLOT_CH0_WLSECOND		4
#define ADC1000USB_SLOT_CH0_WLTHIRD			5
#define ADC1000USB_SLOT_CH1_WLINTERCEPT		6
#define ADC1000USB_SLOT_CH1_WLFIRST			7
#define ADC1000USB_SLOT_CH1_WLSECOND		8
#define ADC1000USB_SLOT_CH1_WLTHIRD			9
#define ADC1000USB_SLOT_CH2_WLINTERCEPT		10
#define ADC1000USB_SLOT_CH2_WLFIRST			11
#define ADC1000USB_SLOT_CH2_WLSECOND		12
#define ADC1000USB_SLOT_CH2_WLTHIRD			13
#define ADC1000USB_SLOT_CH3_WLINTERCEPT		14
#define ADC1000USB_SLOT_CH3_WLFIRST			15
#define ADC1000USB_SLOT_CH3_WLSECOND		16
#define ADC1000USB_SLOT_CH3_WLTHIRD			17
#define ADC1000USB_SLOT_CH4_WLINTERCEPT		18
#define ADC1000USB_SLOT_CH4_WLFIRST			19
#define ADC1000USB_SLOT_CH4_WLSECOND		20
#define ADC1000USB_SLOT_CH4_WLTHIRD			21
#define ADC1000USB_SLOT_CH5_WLINTERCEPT		22
#define ADC1000USB_SLOT_CH5_WLFIRST			23
#define ADC1000USB_SLOT_CH5_WLSECOND		24
#define ADC1000USB_SLOT_CH5_WLTHIRD			25
#define ADC1000USB_SLOT_CH6_WLINTERCEPT		26
#define ADC1000USB_SLOT_CH6_WLFIRST			27
#define ADC1000USB_SLOT_CH6_WLSECOND		28
#define ADC1000USB_SLOT_CH6_WLTHIRD			29
#define ADC1000USB_SLOT_CH7_WLINTERCEPT		30
#define ADC1000USB_SLOT_CH7_WLFIRST			31
#define ADC1000USB_SLOT_CH7_WLSECOND		32
#define ADC1000USB_SLOT_CH7_WLTHIRD			33

///////////////////////////////////////////////////////////////////////////////////
//// HR2000 memory slots
///////////////////////////////////////////////////////////////////////////////////
#define HR2000_SLOT_SERIALNUMBER			0
#define HR2000_SLOT_WLINTERCEPT				1
#define HR2000_SLOT_WLFIRST					2
#define HR2000_SLOT_WLSECOND				3
#define HR2000_SLOT_WLTHIRD					4
#define HR2000_SLOT_STRAYLIGHT				5
#define HR2000_SLOT_NL0						6
#define HR2000_SLOT_NL1						7
#define HR2000_SLOT_NL2						8
#define HR2000_SLOT_NL3						9
#define HR2000_SLOT_NL4						10
#define HR2000_SLOT_NL5						11
#define HR2000_SLOT_NL6						12
#define HR2000_SLOT_NL7						13
#define HR2000_SLOT_NLORDER					14

///////////////////////////////////////////////////////////////////////////////////
//// NIR memory slots
///////////////////////////////////////////////////////////////////////////////////
#define NIR_SLOT_SERIALNUMBER				0
#define NIR_SLOT_WLINTERCEPT				1
#define NIR_SLOT_WLFIRST					2
#define NIR_SLOT_WLSECOND					3
#define NIR_SLOT_WLTHIRD					4
#define NIR_SLOT_STRAYLIGHT					5
#define NIR_SLOT_NL0						6
#define NIR_SLOT_NL1						7
#define NIR_SLOT_NL2						8
#define NIR_SLOT_NL3						9
#define NIR_SLOT_NL4						10
#define NIR_SLOT_NL5						11
#define NIR_SLOT_NL6						12
#define NIR_SLOT_NL7						13
#define NIR_SLOT_NLORDER					14

///////////////////////////////////////////////////////////////////////////////////
//// NIRADC Integration time scalars
///////////////////////////////////////////////////////////////////////////////////
#define NIR_INTTIME_MSEC 1
#define NIR_INTTIME_10USEC 0

///////////////////////////////////////////////////////////////////////////////////
//// HRr000 memory slots
///////////////////////////////////////////////////////////////////////////////////
#define HR4000_SLOT_SERIALNUMBER			0
#define HR4000_SLOT_WLINTERCEPT				1
#define HR4000_SLOT_WLFIRST					2
#define HR4000_SLOT_WLSECOND				3
#define HR4000_SLOT_WLTHIRD					4
#define HR4000_SLOT_STRAYLIGHT				5
#define HR4000_SLOT_NL0						6
#define HR4000_SLOT_NL1						7
#define HR4000_SLOT_NL2						8
#define HR4000_SLOT_NL3						9
#define HR4000_SLOT_NL4						10
#define HR4000_SLOT_NL5						11
#define HR4000_SLOT_NL6						12
#define HR4000_SLOT_NL7						13
#define HR4000_SLOT_NLORDER					14
///////////////////////////////////////////////////////////////////////////////////
//// Structures for passing parameters to DLL
///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
//// OOI_PARAM structure for SingleScan/ContinuousScan acquisition mode
///////////////////////////////////////////////////////////////////////////////////
	typedef struct tag_ooi_param {
		short		ssize;				// size of this structure in bytes
		short		dev;				// reserved, must be 0
		short		cmd;				// command, use CMD_ constants
		short		msg;				// message, use MSG_ constants
		short		res;				// reserved, DO NOT ALTER
		short		fdc;				// flash delay in msec
		WORD		dsf;				// sampling frequency (kHz - S1000, msec - S2000)
		short		boxcar;				// boxcar smoothing width
		short		average;			// samples to average
		short		cur_sampl;			// number of samples collected
		short		chan_ena[MAX_BUF];	// spectrometer channel enabled array
		short		chan_stat[MAX_BUF];	// spectrometerchannel buffer status
		BOOL		changed;			// flag indicating parameters have changed
		BOOL		ovrr_ok;			// do not halt on scan overrun
		BOOL		cont;				// single or continuous scan
		BOOL		flash_cont;			// continuous flash control
		BOOL		scan_dark;			// scan dark
		BOOL		correct_dark;		// correct for electrical dark signal
		int			extrig;				// external trigger mode
		HWND		hWnd;				// window to receive Windows messages
		HINSTANCE	host;				// application instance handle
		float FAR* chdat[MAX_BUF];		// spectral data array pointers
		BOOL UseUSecIntegrationTime;
		DWORD USecIntegrationTime;
		int AutonullingStatus;
		int AutonullingDark;
	} OOI_PARAM;

	///////////////////////////////////////////////////////////////////////////////////
	//// SCANPARM structure for DoScan acquisition mode
	///////////////////////////////////////////////////////////////////////////////////
	typedef struct tag_scanparm {
		short		cmd;				// command, use CMD_ constants
		short		fdc;				// flash delay in msec
		WORD		dsf;				// sampling frequency (kHz - S1000, msec - S2000)
		short		boxcar;				// boxcar smoothing width
		short		average;			// samples to average
		short		chan_ena[4];		// spectrometer channel enabled array
		short		scan_dark;			// scan dark
		short		correct_dark;		// correct for electrical dark signal
		short		extrig;				// external trigger mode
		short		upper4chan;			// flag true if want data from S2000 channels 4-7
		float		sdat[4][2048];		// spectral data arrays
		BOOL UseUSecIntegrationTime;
		DWORD USecIntegrationTime;
		int AutonullingStatus;
		int AutonullingDark;
	} SCANPARM;

	///////////////////////////////////////////////////////////////////////////////////
	//// CALSTORAGE structure for Reading / Writing Calibrations
	///////////////////////////////////////////////////////////////////////////////////
	typedef struct tag_calstorage {
		float buffer[2048];
		int size;
	} CALSTORAGE;

	///////////////////////////////////////////////////////////////////////////////////
	//// FULLPARM structure for FullScan acquisition mode
	///////////////////////////////////////////////////////////////////////////////////
	typedef struct tag_fullparm {
		short		cmd;				// command, use CMD_ constants
		short		fdc;				// flash delay in msec
		WORD		dsf;				// sampling frequency (kHz - S1000, msec - S2000)
		short		boxcar;				// boxcar smoothing width
		short		average;			// samples to average
		short		scan_dark;			// scan dark
		short		correct_dark;		// correct for electrical dark signal
		short		extrig;				// external trigger mode
		short		chan_ena[MAX_BUF];	// spectrometer channel enabled array
		float FAR* chdat[MAX_BUF];		// spectral data array pointe
		BOOL UseUSecIntegrationTime;
		DWORD USecIntegrationTime;
		int AutonullingStatus;
		int AutonullingDark;
	} FULLPARM;

	///////////////////////////////////////////////////////////////////////////////////
	//// OOI_FAM_PARAM structure for Flexible acquisition mode
	///////////////////////////////////////////////////////////////////////////////////
	typedef struct tag_ooi_fam_param {
		short		cmd;				// command, use CMD_ constants
		short		fdc[4];				// flash delay (in msec) array
		WORD		dsf[4];				// sampling frequency (kHz - S1000, msec - S2000) array
		short		boxcar[4];			// boxcar smoothing width array
		short		average[4];			// samples to average array
		short		chan_ena[4];		// spectrometer channel enabled array
		short		scan_dark[4];		// scan dark array
		short		correct_dark[4];	// correct for electrical dark signal array
		short		extrig[4];			// external trigger mode array
		short		upper4chan;			// flag true if want data from S2000 channels 4-7
		short		error[4];			// return value array for each OOI_DoScan call
		float		data[4][2048];		// an array of 4 channels x 2048 elements
		BOOL UseUSecIntegrationTime;
		DWORD USecIntegrationTime[4];
		int AutonullingStatus;
		int AutonullingDark;
	} OOI_FAM_PARAM;

	///////////////////////////////////////////////////////////////////////////////////
	//// SADDATA structure for SAD500 acquisition
	///////////////////////////////////////////////////////////////////////////////////
	typedef struct tag_saddata {
		WORD		start_tag;			// start tag, always 0xfff
		WORD		channel;			// spectrometer channel number
		WORD		scan_number;		// current scan number
		WORD		scans_in_memory;	// number of scans in memory
		WORD		integration_time;	// integration time (in msec)
		WORD		integration_counter;// integration time counter
		WORD		pixel_mode;			// SAD500 pixel mode
		WORD		pixel_data[81];		// optional SAD500 pixel mode data (if pixel mode not 0)
		WORD		scan_data[2048];	// spectral data
	} SADDATA;

	///////////////////////////////////////////////////////////////////////////////////
	//// PIXELMODEDATA structure for SAD500 pixel modeds
	///////////////////////////////////////////////////////////////////////////////////
	typedef struct tagPixelModeData {
		WORD		pixel_mode;			// pixel mode, see SAD_PIXELMODE_ constants
		WORD		pixel_data[81];		// optional data for pixel modes
		WORD		num_pixels;			// number of pixels for the SAD_PIXELMODE_SELECTED mode
	} PIXELMODEDATA;

	///////////////////////////////////////////////////////////////////////////////////
	//// USB2000CONSTANTS structure for SAD500 pixel modeds
	///////////////////////////////////////////////////////////////////////////////////
	typedef struct tagUSB2000Constants {
		char		SerialNumber[16];
		float		WLIntercept;
		float		WLFirst;
		float		WLSecond;
		float		WLThird;
		float		SLCoefficient;
		float		NL0;
		float		NL1;
		float		NL2;
		float		NL3;
		float		NL4;
		float		NL5;
		float		NL6;
		float		NL7;
		int			NLOrder;
	} USB2000CONSTANTS;
	///////////////////////////////////////////////////////////////////////////////////
	//// USB2000INFO structure for OOI_IdentifyAllUSB2000 function
	///////////////////////////////////////////////////////////////////////////////////

	typedef struct tagUSB2000Info {
		int			Valid;
		char		SerialNumber[30];
	} USB2000INFO;

	///////////////////////////////////////////////////////////////////////////////////
	//// ADC1000-USB structure for OOI_IdentifyAllADC1000USB function
	///////////////////////////////////////////////////////////////////////////////////

	typedef struct tagADC1000USBINFO {
		int			Valid;
		char		SerialNumber[30];
	} ADC1000USBINFO;

	///////////////////////////////////////////////////////////////////////////////////
	//// ADC1000-USB structure stored constants
	///////////////////////////////////////////////////////////////////////////////////
	typedef struct tagADC1000Constants {
		char		SerialNumber[16];
		BYTE		ChannelsEnabled;
		float		CH0_WLIntercept;
		float		CH0_WLFirst;
		float		CH0_WLSecond;
		float		CH0_WLThird;
		float		CH1_WLIntercept;
		float		CH1_WLFirst;
		float		CH1_WLSecond;
		float		CH1_WLThird;
		float		CH2_WLIntercept;
		float		CH2_WLFirst;
		float		CH2_WLSecond;
		float		CH2_WLThird;
		float		CH3_WLIntercept;
		float		CH3_WLFirst;
		float		CH3_WLSecond;
		float		CH3_WLThird;
		float		CH4_WLIntercept;
		float		CH4_WLFirst;
		float		CH4_WLSecond;
		float		CH4_WLThird;
		float		CH5_WLIntercept;
		float		CH5_WLFirst;
		float		CH5_WLSecond;
		float		CH5_WLThird;
		float		CH6_WLIntercept;
		float		CH6_WLFirst;
		float		CH6_WLSecond;
		float		CH6_WLThird;
		float		CH7_WLIntercept;
		float		CH7_WLFirst;
		float		CH7_WLSecond;
		float		CH7_WLThird;
	} ADC1000USBCONSTANTS;

	///////////////////////////////////////////////////////////////////////////////////
	//// USB2000 O2Coefs structure for Get/SetO2CalibrationCoefficients
	///////////////////////////////////////////////////////////////////////////////////
	typedef struct tagO2Coefs {
		float coef[18]; //array coeffs
	}O2COEFS, * LPO2COEFS;

	typedef struct tagUSB2000Constants HR2000CONSTANTS;
	typedef struct tagUSB2000Info HR2000INFO;
	typedef struct tagUSB2000Constants NIRADCCONSTANTS;
	typedef struct tagUSB2000Info NIRADCINFO;
	typedef struct tagUSB2000Constants HR4000CONSTANTS;
	typedef struct tagUSB2000Info HR4000INFO;
	typedef struct tagUSB2000Constants HR2000PLUSCONSTANTS;
	typedef struct tagUSB2000Info HR2000PLUSINFO;
	typedef struct tagUSB2000Constants QE65000CONSTANTS;
	typedef struct tagUSB2000Info QE65000INFO;
	typedef struct tagUSB2000Info USB2000PLUSINFO;
	typedef struct tagUSB2000Constants USB2000PLUSCONSTANTS;
	typedef struct tagUSB2000Info USB4000INFO;
	typedef struct tagUSB2000Constants USB4000CONSTANTS;

	typedef struct tagNIRADCStatus {
		int			NumPixels;

		int			IntegrationTime;
		BOOL		LampOn;
		BYTE		TrigMode;
		BOOL		BusyScanning;
		BOOL		TimerSwap;
		BOOL		DataReady;
		BOOL		FailFlag;
		BOOL		Ext5VActive;
		BYTE		PacketCount;
		BOOL		HighGainOn;
		BOOL		TEOn;
		BOOL		FanOn;
	} NIRADCSTATUS;

	///////////////////////////////////////////////////////////////////////////////////
	//// SCANTIMES structure for OOI_GetScanTimes
	///////////////////////////////////////////////////////////////////////////////////
	typedef struct tagSCANTIMES {
		DWORD DriverStartTime;
		DWORD ScanRequested;
		DWORD ScanComplete;
		DWORD Reserved1;
		DWORD Reserved2;
		DWORD Reserved3;
		DWORD Reserved4;
		DWORD Reserved5;
		DWORD Reserved6;
		DWORD Reserved7;
		DWORD Reserved8;
	} SCANTIMES;

	///////////////////////////////////////////////////////////////////////////////////
	//// Function prototypes for OOI_ functions
	///////////////////////////////////////////////////////////////////////////////////
	void	EXPORTED	OOI_BufferEmpty(HINSTANCE, short);			// indicate you are done with data
	void	EXPORTED	OOI_Config(short, short, short, short);		// configure hardware
	void	EXPORTED	OOI_ConfigEx(short, short, short, short, short, short); // config hardware
	short	EXPORTED	OOI_ContinuousScan(HINSTANCE);				// perform continuous acquisitions
	void	EXPORTED	OOI_DisableMultichannelSpectra(void);		// disable rotator (ADC1000)
	short	EXPORTED	OOI_DoScan(SCANPARM FAR*);					// perform 1 acquisition
	short	EXPORTED	OOI_DoScan_Array(short FAR*, float FAR*);	// perform 1 acquisition
	long	EXPORTED	OOI_DoScan_Array_Long(long FAR*, float FAR*);// perform 1 acquisition
	void	EXPORTED	OOI_EnableMultichannelSpectra(short);		// enable channel rotator (ADC1000)
	void	EXPORTED	OOI_EnableNLCorrection(short, short);		// enable nonlineaity correction
	void	EXPORTED	OOI_EnableSLCorrection(short, short);		// enable stray light correction
	short	EXPORTED	OOI_Flexible_Acquisition(OOI_FAM_PARAM FAR*); // perform 1 acquisition
	short	EXPORTED	OOI_FullScan(FULLPARM FAR* fp);				// perform 1 acquisition
	WORD	EXPORTED	OOI_GetADCType(void);						// return A/D adapter type
	DWORD	EXPORTED	OOI_GetBaseAddress(void);					// return base address (decimal)
	void	EXPORTED	OOI_GetDriverVersion(char* buffer, int chars);
	int	EXPORTED	OOI_GetIntegrationTime(void);				// return integration time (msec)
	short	EXPORTED	OOI_GetIRQ(void);							// return IRQ
	WORD	EXPORTED	OOI_GetNIDAQCardID(void);					// return NI-DAQ card ID
	short	EXPORTED	OOI_GetNumberOfPixels(void);				// return number of pixels
	WORD	EXPORTED	OOI_GetSAD500BaudRate(void);				// gets SAD500 baud rate
	WORD	EXPORTED	OOI_GetSAD500CompressedMode(void);			// gets SAD500 compressed mode
	WORD	EXPORTED	OOI_GetSAD500PixelResolution(void);			// gets SAD500 Pixel resolution
	WORD	EXPORTED	OOI_GetSerialPort(void);					// return serial port
	WORD	EXPORTED	OOI_GetSpectrometerType(void);				// return spectrometer type
	short	EXPORTED	OOI_ParamSet(OOI_PARAM FAR*);				// lock acquisition paramters
	WORD	EXPORTED	OOI_Register(OOI_PARAM FAR*);				// register application instance
	void	EXPORTED	OOI_S2000StrobeEnable(short);				// enable/disalbe S2000 strobe
	void	EXPORTED	OOI_SetNLCoefs(short, float FAR*);			// set nonlinearity correction coefs
	short	EXPORTED	OOI_SetSAD500BaudRate(WORD);				// sets SAD500 baud rate
	short	EXPORTED	OOI_SetSAD500CompressedMode(WORD);			// sets SAD500 compressed mode
	short	EXPORTED	OOI_SetSAD500PixelResolution(WORD);			// sets SAD500 Pixel resolution
	void	EXPORTED	OOI_SetSLCoef(short, float);					// set stray light correction coefs
	short	EXPORTED	OOI_SingleScan(HINSTANCE);					// perform single acquisition
	void	EXPORTED	OOI_Stat(OOI_PARAM FAR*);					// return acquisition status
	void	EXPORTED	OOI_Stop(HINSTANCE);						// stop continuous acquisitions
	void	EXPORTED	OOI_StrobeEnable(short);					// enables/disables strobe for S2000 and S1024DW
	void	EXPORTED	OOI_UnRegister(HINSTANCE);					// unregister application instance
	void	EXPORTED	OOI_GetCurrentUSB2000SerialNumber(char* sn);// outdated, use OOI_GetCurrentUSBSerialNumber
	BOOL	EXPORTED	OOI_SetCurrentUSB2000SerialNumber(char* sn);// outdated, use OOI_SetCurrentUSBSerialNumber
	void	EXPORTED	OOI_IdentifyAllUSB2000(USB2000INFO* info);
	void	EXPORTED	OOI_IdentifyAllADC1000USB(ADC1000USBINFO* info);
	void	EXPORTED	OOI_GetCurrentUSBSerialNumber(char* sn);
	BOOL	EXPORTED	OOI_SetCurrentUSBSerialNumber(char* sn);
	int		EXPORTED	OOI_GetPCICardID(void);
	HANDLE	EXPORTED	OOI_SingleScan_Thread(HINSTANCE);			// Creates OOI_SingleFullScanUSB2000 as a thread
	BOOL	EXPORTED	OOI_GetD3State(void);
	BOOL	EXPORTED	OOI_AcquisitionThreadActive(void);
	void	EXPORTED	OOI_IdentifyAllHR2000(HR2000INFO* info);
	int		EXPORTED	OOI_GetTrigTimeout(void);
	void	EXPORTED	OOI_SetTrigTimeout(int TimeOut);
	BOOL	EXPORTED	OOI_EnableDriverMessages(BOOL OnOff);
	void	EXPORTED	OOI_IdentifyAllNIR512(NIRADCINFO* info);
	void	EXPORTED	OOI_IdentifyAllNIR256(NIRADCINFO* info);
	void EXPORTED OOI_IdentifyAllHR4000(HR4000INFO* info);
	void EXPORTED OOI_IdentifyAllHR2000Plus(HR2000PLUSINFO* info);
	void EXPORTED OOI_IdentifyAllQE65000(QE65000INFO* info);

	void EXPORTED OOI_IdentifyAllUSB4000(USB4000INFO* info);
	void EXPORTED OOI_IdentifyAllUSB2000Plus(USB2000PLUSINFO* info);

	void EXPORTED OOI_GetScanTimes(SCANTIMES* st);
	DWORD EXPORTED OOI_GetMaximumScopeModeIntensity();

	//void EXPORTED OOI_DigitalOutEnable(WORD on);

	///////////////////////////////////////////////////////////////////////////////////
	//// Function prototypes for SAD_ functions
	///////////////////////////////////////////////////////////////////////////////////
	short	EXPORTED	SAD_ClearMemory(WORD);						// clear memory
	short	EXPORTED	SAD_Close(WORD);							// close SAD500
	int		EXPORTED	SAD_DetermineSerialADCType(void);			// deterimes
	short	EXPORTED	SAD_DumpFastMemory(void);					// dump fast memory scans to slow memory
	short	EXPORTED	SAD_EnableISR(WORD, void FAR*);				// sets ISR-like behavior
	short	EXPORTED	SAD_EnterDoubleSecretMode(WORD);
	WORD	EXPORTED	SAD_GetADCRate(void);						// get A/D rate in kHz
	WORD	EXPORTED	SAD_GetBaudRate(void);						// get SAD500 and computer baud rate
	WORD	EXPORTED	SAD_GetBoxcarWidth(void);					// get pixel boxcar width

	WORD	EXPORTED	SAD_GetChecksumMode(void);					// get checksum mode
	WORD	EXPORTED	SAD_GetCompressedMode(void);				// gets compressed data transfer
	WORD	EXPORTED	SAD_GetError(void);							// gets and clears error value
	WORD	EXPORTED	SAD_GetExternalTrigger(void);				// get external trigger mode
	short	EXPORTED	SAD_GetFastMemoryAvailable(void);			// get fast mem. available (scans)
	WORD	EXPORTED	SAD_GetFullPixelMode(PIXELMODEDATA FAR*);	// gets full pixel mode
	WORD	EXPORTED	SAD_GetIntegrationCounter(void);			// retreives integration period counter
	WORD	EXPORTED	SAD_GetIntegrationTime(void);				// get integration time in msec
	WORD	EXPORTED	SAD_GetInterpolateMissingPixels(void);		// gets interpolation of missing pix.
	short	EXPORTED	SAD_GetNumberOfScansInMemory(WORD);			// get num. scans in memory
	WORD	EXPORTED	SAD_GetNumberOfScansToStore(void);			// get number of to store
	WORD	EXPORTED	SAD_GetPixelMode(void);						// get pixel transfer mode
	WORD	EXPORTED	SAD_GetS1024DWCDSMode(void);				// gets S1024DW correlated doubling sampleing mode
	WORD	EXPORTED	SAD_GetScanMode(void);						// get whether scans are returned or stored in memory
	WORD	EXPORTED	SAD_GetScansToAdd(void);					// get number of scans to add
	short	EXPORTED	SAD_GetSlowMemoryAvailable(void);			// get slow mem. available (kbytes)
	WORD	EXPORTED	SAD_GetSpectrometerChannel(void);			// get spectrometer channel
	WORD	EXPORTED	SAD_GetStrobeEnable(void);					// get strobe enable (S0)
	WORD	EXPORTED	SAD_GetVersion(void);						// retreives microcode version
	short	EXPORTED	SAD_Init(WORD, WORD);						// initialize SAD500
	short	EXPORTED	SAD_ReadoutOneScan(WORD, SADDATA FAR*);		// readout one scan
	short	EXPORTED	SAD_ReadoutScansInMemory(WORD, WORD FAR*);	// readout stored spectra
	short	EXPORTED	SAD_Reset(void);							// reset A/D
	short	EXPORTED	SAD_Scan(SADDATA FAR*);						// start spectral acquisition
	short	EXPORTED	SAD_ScanReceivedOK(WORD);					// scan readout ok
	WORD	EXPORTED	SAD_ScanWithAverage(SADDATA FAR*, WORD, float far*);
	short	EXPORTED	SAD_SetADCRate(WORD);						// set A/D rate in kHz
	short	EXPORTED	SAD_SetBaudRate(WORD);						// sets SAD500 and computer baud rate
	short	EXPORTED	SAD_SetBoxcarWidth(WORD);					// set pixel boxcar width
	short	EXPORTED	SAD_SetChecksumMode(WORD);					// sets checksum mode
	short	EXPORTED	SAD_SetCompressedMode(WORD);				// sets/clears compressed data transfer
	short	EXPORTED	SAD_SetExternalTrigger(WORD);				// sets external trigger mode
	short	EXPORTED	SAD_SetFlashDelay(WORD);					// sets flash delay for adc1000-usb in serial mode
	short	EXPORTED	SAD_SetIntegrationTime(WORD);				// sets integration time in msec
	short	EXPORTED	SAD_SetInterpolateMissingPixels(WORD);		// sets interpolation of missing pix.
	short	EXPORTED	SAD_SetNumberOfScansToStore(WORD);			// sets number of to store
	short	EXPORTED	SAD_SetPixelMode(PIXELMODEDATA FAR*);		// sets pixel transfer mode
	short	EXPORTED	SAD_SetS1024DWCDSMode(WORD);				// set S1024DW correlated doubling sampleing mode
	short	EXPORTED	SAD_SetScanMode(WORD);						// sets whether scans are returned or stored
	short	EXPORTED	SAD_SetScansToAdd(WORD);					// sets number of scans to add
	short	EXPORTED	SAD_SetSerialPort(WORD);
	short	EXPORTED	SAD_SetSpectrometerChannel(WORD);			// sets spectrometer channel
	short	EXPORTED	SAD_SetStrobeEnable(WORD);					// enables strobe (toggles S0)
	short	EXPORTED	SAD_GetStoredString(int slot, char* string);
	short	EXPORTED	SAD_SetStoredString(int slot, char* string);
	short	EXPORTED	SAD_SetStoredFloat(int slot, float value);
	short	EXPORTED	SAD_GetStoredFloat(int slot, float* value);
	short	EXPORTED	SAD_SetStoredInt(int slot, int value);
	short	EXPORTED	SAD_GetStoredInt(int slot, int* value);
	short	EXPORTED	SAD_SetRamanMode(BOOL mode);
	BOOL	EXPORTED	SAD_GetRamanMode(void);

	///////////////////////////////////////////////////////////////////////////////////
	//// Function prototypes for SAD_ double-secret functions
	///////////////////////////////////////////////////////////////////////////////////
	short	EXPORTED	SAD_ChangeSlowReadPointer(WORD);			// change slow memory read pointer
	short	EXPORTED	SAD_ClearSlowMemory(WORD, WORD);				// clears slow memory BLOCK and PAGE
	WORD	EXPORTED	SAD_GetDebugMode(void);						// gets debug mode enabled
	WORD	EXPORTED	SAD_GetNumberOfBadMemoryBlocks(void);		// gets number of bad flash memory blocks
	WORD	EXPORTED	SAD_GetPrintDebugMode(void);				// gets debug print mode enabled
	short	EXPORTED	SAD_ReadSlowMemory(WORD, WORD, WORD FAR*, WORD FAR*); // read out slow memory BLOCK and PAGE
	short	EXPORTED	SAD_ToggleDebugMode(void);					// toggles debug mode (ascii xmit and receive)
	short	EXPORTED	SAD_ToggleDebugPrintMode(void);				// toggles debug print mode

	///////////////////////////////////////////////////////////////////////////////////
	//// Function prototypes for USBISSUV
	///////////////////////////////////////////////////////////////////////////////////
	BOOL EXPORTED USBISSUV_WritePotValue(BYTE pot, BYTE value);
	BOOL EXPORTED USBIISUV_ReadPotValues(BYTE* values);
	BOOL EXPORTED USB4000_ISSUV_LampEnable(short ena);

	///////////////////////////////////////////////////////////////////////////////////
	//// Function prototypes for USB2000_ and USB2000-specific commands
	///////////////////////////////////////////////////////////////////////////////////
	BOOL	EXPORTED	USB2000_GetCoefficients(USB2000CONSTANTS* con);
	BOOL	EXPORTED	USB2000_SetCoefficients(USB2000CONSTANTS* con);
	BOOL	EXPORTED	USB2000_IdentifyUSB2000(int DevNum, char* SerialNumber);
	BOOL	EXPORTED	USB2000_GetFirmwareVersion(char* ver);
	BOOL	EXPORTED	USB2000_SetStoredString(int slot, char* string);
	BOOL	EXPORTED	USB2000_GetStoredString(int slot, char* string);
	BOOL	EXPORTED	USB2000_SetO2CalibrationCoefficients(LPO2COEFS O2);
	BOOL	EXPORTED	USB2000_GetO2CalibrationCoefficients(LPO2COEFS O2);
	BOOL	EXPORTED	USB2000_SetLS450Modulation(BOOL Pulsed);
	BOOL	EXPORTED	USB2000_GetLS450Modulation(void);
	BOOL	EXPORTED	USB2000_IsPluginDetected(BYTE PluginID);
	float	EXPORTED	USB2000_GetTemperature(void);
	void	EXPORTED	USB2000_GetTemperatureCoefficients(float* first, float* second);
	void	EXPORTED	USB2000_SetTemperatureCoefficients(float first, float second);
	BOOL	EXPORTED	USB2000_SetLS450DA(WORD daValue);
	BOOL EXPORTED USB2000_ISSUV_LampOn(BOOL on);
	BOOL EXPORTED USB2000_ISSUV_SetVisLampOn(BOOL on);
	BOOL EXPORTED USB2000_ISSUV_SetShutterOpened(BOOL open);
	BOOL EXPORTED USB2000_ISSUV_StoreCurrentPotValues(void);
	BOOL EXPORTED USB2000_ISSUV_SetVisLampIntensity(short value);
	short EXPORTED USB2000_ISSUV_GetVisLampIntensity(void);
	BOOL EXPORTED USB2000_ISSUV_SetUVLampIntensity(short value);
	short EXPORTED USB2000_ISSUV_GetUVLampIntensity(void);
	BOOL EXPORTED USB2000_ISSUV_GetVisLampOn();
	BOOL EXPORTED USB2000_ISSUV_GetLampOn();
	short EXPORTED USB2000_I2C_Read(short address, short numbytes, BYTE* array);
	short EXPORTED USB2000_I2C_Write(short address, short numbytes, BYTE* array);

	BOOL EXPORTED USB2000_ReadFloatArray(CALSTORAGE FAR*);
	BOOL EXPORTED USB2000_WriteFloatArray(CALSTORAGE FAR*);

	///////////////////////////////////////////////////////////////////////////////////
	//// Function prototypes for ADC1000USB_ and ADC1000USB-specific commands
	///////////////////////////////////////////////////////////////////////////////////
	BOOL	EXPORTED	ADC1000USB_GetCoefficients(ADC1000USBCONSTANTS* con);
	BOOL	EXPORTED	ADC1000USB_SetCoefficients(ADC1000USBCONSTANTS* con);
	BOOL	EXPORTED	ADC1000USB_IdentifyADC1000USB(int DevNum, char* SerialNumber);
	void	EXPORTED	ADC1000USB_GetFirmwareVersion(char* ver);
	BOOL EXPORTED ADC1000USB_DAC_Initialize(void);
	WORD EXPORTED ADC1000USB_DAC_ReadValue(short channel);
	BOOL EXPORTED ADC1000USB_DAC_WriteValue(short channel, WORD value);

	///////////////////////////////////////////////////////////////////////////////////
	//// Function prototypes for ADC2000PCI and ADC2000PCI-specific commands
	///////////////////////////////////////////////////////////////////////////////////
	void	EXPORTED	ADC2000PCI_WriteDIO(BYTE bits, BYTE value);
	WORD	EXPORTED	ADC2000PCI_ReadDIO(void);
	void	EXPORTED	ADC2000PCI_AnalogOut(int channel, WORD value);
	BOOL	EXPORTED	ADC2000PCI_IsPCIPlus(void);

	///////////////////////////////////////////////////////////////////////////////////
	//// Function prototypes for HR2000_ and HR2000-specific commands
	///////////////////////////////////////////////////////////////////////////////////
	BOOL	EXPORTED	HR2000_GetCoefficients(HR2000CONSTANTS* con);
	BOOL	EXPORTED	HR2000_SetCoefficients(HR2000CONSTANTS* con);
	BOOL	EXPORTED	HR2000_IdentifyHR2000(int DevNum, char* SerialNumber);
	void	EXPORTED	HR2000_GetFirmwareVersion(char* ver);
	BOOL	EXPORTED	HR2000_SetStoredString(int slot, char* string);
	BOOL	EXPORTED	HR2000_GetStoredString(int slot, char* string);

	///////////////////////////////////////////////////////////////////////////////////
	//// Function prototypes for NIRADC_ and NIRADC-specific commands
	///////////////////////////////////////////////////////////////////////////////////
	BOOL	EXPORTED	NIRADC_GetCoefficients(NIRADCCONSTANTS* con);
	BOOL	EXPORTED	NIRADC_SetCoefficients(NIRADCCONSTANTS* con);
	BOOL	EXPORTED	NIRADC_IdentifyNIR512(int DevNum, char* SerialNumber);
	BOOL	EXPORTED	NIRADC_IdentifyNIR256(int DevNum, char* SerialNumber);

	BOOL	EXPORTED	NIRADC_SetStoredString(int slot, char* string);
	BOOL	EXPORTED	NIRADC_GetStoredString(int slot, char* string);
	BOOL	EXPORTED	NIRADC_EnableCooler(BOOL OnOff);
	BOOL	EXPORTED	NIRADC_EnableHighGain(BOOL OnOff);
	BOOL	EXPORTED	NIRADC_EnableFan(BOOL OnOff);

	BOOL	EXPORTED	NIRADC_GetStatus(NIRADCSTATUS* Stat);
	BOOL	EXPORTED	NIRADC_GetFirmwareVersion(char* ver);

	BOOL	EXPORTED	NIRADC_SetIntegrationTimeScalar(BOOL msec);
	BOOL	EXPORTED	NIRADC_GetIntegrationTimeScalar();
	BOOL	EXPORTED	NIRADC_SetTECSetPoint(float temp);
	float	EXPORTED	NIRADC_GetTECSetPoint();
	float	EXPORTED	NIRADC_GetTECTemperature();
	float	EXPORTED	NIRADC_GetBoardTemperature();

	///////////////////////////////////////////////////////////////////////////////////
	//// Function prototypes for HR4000_ and HR4000-specific commands
	///////////////////////////////////////////////////////////////////////////////////
	BOOL	EXPORTED	HR4000_GetCoefficients(HR4000CONSTANTS* con);
	BOOL	EXPORTED	HR4000_SetCoefficients(HR4000CONSTANTS* con);
	BOOL	EXPORTED	HR4000_IdentifyHR4000(int DevNum, char* SerialNumber);
	void	EXPORTED	HR4000_GetFirmwareVersion(char* ver);
	BOOL	EXPORTED	HR4000_SetStoredString(int slot, char* string);
	BOOL	EXPORTED	HR4000_GetStoredString(int slot, char* string);
	double EXPORTED HR4000_GetBoardTemperature();
	BOOL EXPORTED HR4000_SetPowerDown(BOOL on);
	int EXPORTED HR4000_WriteI2C(BYTE address, BYTE numBytes, BYTE* data);
	int EXPORTED HR4000_ReadI2C(BYTE address, BYTE numBytes, BYTE* data);
	BOOL EXPORTED HR4000_WriteSPI(int numBytes, BYTE* data);
	BOOL EXPORTED HR4000_ReadSPI(int numBytes, BYTE* data);
	int EXPORTED HR4000_ReadAnalogInput();
	double EXPORTED HR4000_ReadAnalogInputVoltage();
	BOOL EXPORTED HR4000_SetAnalogOutput(int value);

	BOOL EXPORTED HR4000_SetAnalogOutputVoltage(double volts);
	BOOL EXPORTED HR4000_GetPSOCVersion(char* ver);
	BOOL EXPORTED HR4000_WriteFPGA(BYTE reg, int value);
	int EXPORTED HR4000_ReadFPGA(BYTE reg);
	int EXPORTED HR4000_GetMasterClockDivisor();
	BOOL EXPORTED HR4000_SetMasterClockDivisor(int value);
	BOOL EXPORTED HR4000_GetFPGAFirmwareVersion(char* str);
	int EXPORTED HR4000_GetContinuousStrobeBase();
	BOOL EXPORTED HR4000_SetContinuousStrobeBase(int value);
	int EXPORTED HR4000_GetContinuousStrobeClock();
	BOOL EXPORTED HR4000_SetContinuousStrobeClock(int value);
	short EXPORTED HR4000_GetIntegrationTimeBaseClock();
	BOOL EXPORTED HR4000_SetIntegrationTimeBaseClock(short value);
	int EXPORTED HR4000_GetIntegrationClockTimer();
	BOOL EXPORTED HR4000_SetIntegrationClockTimer(short value);
	int EXPORTED HR4000_GetShutterClock();
	BOOL EXPORTED HR4000_SetShutterClock(int value);
	int EXPORTED HR4000_GetHardwareTriggerDelay();
	BOOL EXPORTED HR4000_SetHardwareTriggerDelay(int value);
	short EXPORTED HR4000_GetHardwareTriggerMode();
	BOOL EXPORTED HR4000_SetHardwareTriggerMode(short value);
	int EXPORTED HR4000_GetSingleStrobeHigh();
	BOOL EXPORTED HR4000_SetSingleStrobeHigh(int value);
	int EXPORTED HR4000_GetSingleStrobeLow();
	BOOL EXPORTED HR4000_SetSingleStrobeLow(int value);
	short EXPORTED HR4000_GetStrobeEnableFPGA();
	BOOL EXPORTED HR4000_SetStrobeEnableFPGA(short value);
	short EXPORTED HR4000_GetGPIOMux();
	BOOL EXPORTED HR4000_SetGPIOMux(short value);
	BOOL EXPORTED HR4000_SetGPIOMuxReg(short value);
	BOOL EXPORTED HR4000_SetGPIOMuxAlt(short value);
	BOOL EXPORTED HR4000_GetGPIOPinMux(short pin);
	short EXPORTED HR4000_GetGPIOIO();
	BOOL EXPORTED HR4000_SetGPIOIO(short value);
	short EXPORTED HR4000_GetGPIOPinIO(short pin);
	BOOL EXPORTED HR4000_SetGPIOPinIO(short pin, short value);
	BOOL EXPORTED HR4000_SetGPIOPinOutput(short pin);
	BOOL EXPORTED HR4000_SetGPIOPinInput(short pin);
	BOOL EXPORTED HR4000_SetGPIODataReg(short value);
	short EXPORTED HR4000_GetGPIODataReg();
	short EXPORTED HR4000_GetGPIOPinData(short pin);
	BOOL EXPORTED HR4000_SetGPIOPinData(short pin, short value);
	double EXPORTED HR4000_GetMasterClockTime();
	double EXPORTED HR4000_GetIntegrationClockTime();
	double EXPORTED HR4000_GetContinuousStrobeBaseTime();
	double EXPORTED HR4000_GetContinuousStrobeTime();
	double EXPORTED HR4000_GetHardwareTriggerDelayTime();
	double EXPORTED HR4000_GetSingleStrobeHighTime();
	double EXPORTED HR4000_GetSingleStrobeLowTime();

	BOOL	EXPORTED	HR2000Plus_GetCoefficients(HR2000PLUSCONSTANTS* con);
	BOOL	EXPORTED	HR2000Plus_SetCoefficients(HR2000PLUSCONSTANTS* con);
	BOOL	EXPORTED	HR2000Plus_IdentifyHR2000Plus(int DevNum, char* SerialNumber);
	void	EXPORTED	HR2000Plus_GetFirmwareVersion(char* ver);
	BOOL	EXPORTED	HR2000Plus_SetStoredString(int slot, char* string);
	BOOL	EXPORTED	HR2000Plus_GetStoredString(int slot, char* string);
	double EXPORTED HR2000Plus_GetBoardTemperature();
	double EXPORTED HR2000Plus_GetBoardTemperature();
	BOOL EXPORTED HR2000Plus_SetPowerDown(BOOL on);
	int EXPORTED HR2000Plus_WriteI2C(BYTE address, BYTE numBytes, BYTE* data);

	int EXPORTED HR2000Plus_ReadI2C(BYTE address, BYTE numBytes, BYTE* data);
	BOOL EXPORTED HR2000Plus_WriteSPI(int numBytes, BYTE* data);
	BOOL EXPORTED HR2000Plus_ReadSPI(int numBytes, BYTE* data);
	int EXPORTED HR2000Plus_ReadAnalogInput();
	double EXPORTED HR2000Plus_ReadAnalogInputVoltage();
	BOOL EXPORTED HR2000Plus_SetAnalogOutput(int value);
	BOOL EXPORTED HR2000Plus_SetAnalogOutputVoltage(double volts);
	BOOL EXPORTED HR2000Plus_GetPSOCVersion(char* ver);
	BOOL EXPORTED HR2000Plus_WriteFPGA(BYTE reg, int value);
	int EXPORTED HR2000Plus_ReadFPGA(BYTE reg);
	int EXPORTED HR2000Plus_GetMasterClockDivisor();
	BOOL EXPORTED HR2000Plus_SetMasterClockDivisor(int value);
	BOOL EXPORTED HR2000Plus_GetFPGAFirmwareVersion(char* str);
	int EXPORTED HR2000Plus_GetContinuousStrobeBase();
	BOOL EXPORTED HR2000Plus_SetContinuousStrobeBase(int value);
	int EXPORTED HR2000Plus_GetContinuousStrobeClock();
	BOOL EXPORTED HR2000Plus_SetContinuousStrobeClock(int value);
	short EXPORTED HR2000Plus_GetIntegrationTimeBaseClock();
	BOOL EXPORTED HR2000Plus_SetIntegrationTimeBaseClock(short value);
	int EXPORTED HR2000Plus_GetIntegrationClockTimer();
	BOOL EXPORTED HR2000Plus_SetIntegrationClockTimer(short value);
	int EXPORTED HR2000Plus_GetShutterClock();
	BOOL EXPORTED HR2000Plus_SetShutterClock(int value);
	int EXPORTED HR2000Plus_GetHardwareTriggerDelay();
	BOOL EXPORTED HR2000Plus_SetHardwareTriggerDelay(int value);
	short EXPORTED HR2000Plus_GetHardwareTriggerMode();
	BOOL EXPORTED HR2000Plus_SetHardwareTriggerMode(short value);
	int EXPORTED HR2000Plus_GetSingleStrobeHigh();
	BOOL EXPORTED HR2000Plus_SetSingleStrobeHigh(int value);
	int EXPORTED HR2000Plus_GetSingleStrobeLow();
	BOOL EXPORTED HR2000Plus_SetSingleStrobeLow(int value);
	short EXPORTED HR2000Plus_GetStrobeEnableFPGA();
	BOOL EXPORTED HR2000Plus_SetStrobeEnableFPGA(short value);
	short EXPORTED HR2000Plus_GetGPIOMux();
	BOOL EXPORTED HR2000Plus_SetGPIOMux(short value);
	BOOL EXPORTED HR2000Plus_SetGPIOMuxReg(short value);
	BOOL EXPORTED HR2000Plus_SetGPIOMuxAlt(short value);
	BOOL EXPORTED HR2000Plus_GetGPIOPinMux(short pin);
	short EXPORTED HR2000Plus_GetGPIOIO();
	BOOL EXPORTED HR2000Plus_SetGPIOIO(short value);
	BOOL EXPORTED HR2000Plus_SetFPGA(short value);
	short EXPORTED HR2000Plus_GetGPIOPinIO(short pin);
	BOOL EXPORTED HR2000Plus_SetGPIOPinIO(short pin, short value);
	BOOL EXPORTED HR2000Plus_SetGPIOPinOutput(short pin);
	BOOL EXPORTED HR2000Plus_SetGPIOPinInput(short pin);
	BOOL EXPORTED HR2000Plus_SetGPIODataReg(short value);
	short EXPORTED HR2000Plus_GetGPIODataReg();
	short EXPORTED HR2000Plus_GetGPIOPinData(short pin);
	BOOL EXPORTED HR2000Plus_SetGPIOPinData(short pin, short value);
	double EXPORTED HR2000Plus_GetMasterClockTime();
	double EXPORTED HR2000Plus_GetIntegrationClockTime();
	double EXPORTED HR2000Plus_GetContinuousStrobeBaseTime();
	double EXPORTED HR2000Plus_GetContinuousStrobeTime();
	double EXPORTED HR2000Plus_GetHardwareTriggerDelayTime();
	double EXPORTED HR2000Plus_GetSingleStrobeHighTime();
	double EXPORTED HR2000Plus_GetSingleStrobeLowTime();

	BOOL	EXPORTED	QE65000_GetCoefficients(QE65000CONSTANTS* con);
	BOOL	EXPORTED	QE65000_SetCoefficients(QE65000CONSTANTS* con);
	BOOL	EXPORTED	QE65000_IdentifyQE65000(int DevNum, char* SerialNumber);
	void	EXPORTED	QE65000_GetFirmwareVersion(char* ver);
	BOOL	EXPORTED	QE65000_SetStoredString(int slot, char* string);
	BOOL	EXPORTED	QE65000_GetStoredString(int slot, char* string);
	double EXPORTED QE65000_GetBoardTemperature();
	BOOL EXPORTED QE65000_SetPowerDown(BOOL on);
	int EXPORTED QE65000_WriteI2C(BYTE address, BYTE numBytes, BYTE* data);
	int EXPORTED QE65000_ReadI2C(BYTE address, BYTE numBytes, BYTE* data);
	BOOL EXPORTED QE65000_WriteSPI(int numBytes, BYTE* data);
	BOOL EXPORTED QE65000_ReadSPI(int numBytes, BYTE* data);
	int EXPORTED QE65000_ReadAnalogInput();
	double EXPORTED QE65000_ReadAnalogInputVoltage();
	BOOL EXPORTED QE65000_SetAnalogOutput(int value);
	BOOL EXPORTED QE65000_SetAnalogOutputVoltage(double volts);
	BOOL EXPORTED QE65000_GetPSOCVersion(char* ver);
	BOOL EXPORTED QE65000_WriteFPGA(BYTE reg, int value);
	int EXPORTED QE65000_ReadFPGA(BYTE reg);
	int EXPORTED QE65000_GetMasterClockDivisor();
	BOOL EXPORTED QE65000_SetMasterClockDivisor(int value);
	BOOL EXPORTED QE65000_GetFPGAFirmwareVersion(char* str);
	int EXPORTED QE65000_GetContinuousStrobeBase();
	BOOL EXPORTED QE65000_SetContinuousStrobeBase(int value);
	int EXPORTED QE65000_GetContinuousStrobeClock();
	BOOL EXPORTED QE65000_SetContinuousStrobeClock(int value);
	short EXPORTED QE65000_GetIntegrationTimeBaseClock();
	BOOL EXPORTED QE65000_SetIntegrationTimeBaseClock(short value);
	int EXPORTED QE65000_GetIntegrationClockTimer();
	BOOL EXPORTED QE65000_SetIntegrationClockTimer(short value);
	int EXPORTED QE65000_GetShutterClock();
	BOOL EXPORTED QE65000_SetShutterClock(int value);
	int EXPORTED QE65000_GetSingleStrobeHigh();
	BOOL EXPORTED QE65000_SetSingleStrobeHigh(int value);
	int EXPORTED QE65000_GetSingleStrobeLow();
	BOOL EXPORTED QE65000_SetSingleStrobeLow(int value);
	short EXPORTED QE65000_GetStrobeEnableFPGA();
	BOOL EXPORTED QE65000_SetStrobeEnableFPGA(short value);
	short EXPORTED QE65000_GetGPIOMux();
	BOOL EXPORTED QE65000_SetGPIOMux(short value);
	BOOL EXPORTED QE65000_SetGPIOMuxReg(short value);
	BOOL EXPORTED QE65000_SetGPIOMuxAlt(short value);
	BOOL EXPORTED QE65000_GetGPIOPinMux(short pin);
	short EXPORTED QE65000_GetGPIOIO();
	BOOL EXPORTED QE65000_SetFPGA(short value);
	short EXPORTED QE65000_GetGPIOPinIO(short pin);
	BOOL EXPORTED QE65000_SetGPIOPinIO(short pin, short value);
	BOOL EXPORTED QE65000_SetGPIOPinOutput(short pin);
	BOOL EXPORTED QE65000_SetGPIOPinInput(short pin);
	BOOL EXPORTED QE65000_SetGPIODataReg(short value);
	short EXPORTED QE65000_GetGPIODataReg();
	short EXPORTED QE65000_GetGPIOPinData(short pin);
	BOOL EXPORTED QE65000_SetGPIOPinData(short pin, short value);
	double EXPORTED QE65000_GetMasterClockTime();
	double EXPORTED QE65000_GetIntegrationClockTime();
	double EXPORTED QE65000_GetContinuousStrobeBaseTime();
	double EXPORTED QE65000_GetContinuousStrobeTime();
	double EXPORTED QE65000_GetSingleStrobeHighTime();
	double EXPORTED QE65000_GetSingleStrobeLowTime();
	double EXPORTED QE65000_GetMainBoardTemperature();
	double EXPORTED QE65000_GetDetectorBoardTemperature();
	BOOL EXPORTED QE65000_SetFanEnable(BOOL on);
	BOOL EXPORTED QE65000_SetTECEnable(BOOL on);
	BOOL EXPORTED QE65000_SetTECSetPoint(double temp);
	double EXPORTED QE65000_GetTECTemperature();

	///////////////////////////////////////////////////////////////////////////////////
	//// Function prototypes for USB4000_ and USB4000-specific commands
	///////////////////////////////////////////////////////////////////////////////////
	BOOL	EXPORTED	USB4000_GetCoefficients(USB4000CONSTANTS* con);
	BOOL	EXPORTED	USB4000_SetCoefficients(USB4000CONSTANTS* con);
	BOOL	EXPORTED	USB4000_IdentifyUSB4000(int DevNum, char* SerialNumber);
	void	EXPORTED	USB4000_GetFirmwareVersion(char* ver);
	BOOL	EXPORTED	USB4000_SetStoredString(int slot, char* string);
	BOOL	EXPORTED	USB4000_GetStoredString(int slot, char* string);
	double EXPORTED USB4000_GetBoardTemperature();
	BOOL EXPORTED USB4000_SetPowerDown(BOOL on);
	int EXPORTED USB4000_WriteI2C(BYTE address, BYTE numBytes, BYTE* data);
	int EXPORTED USB4000_ReadI2C(BYTE address, BYTE numBytes, BYTE* data);
	BOOL EXPORTED USB4000_WriteSPI(int numBytes, BYTE* data);
	BOOL EXPORTED USB4000_ReadSPI(int numBytes, BYTE* data);
	int EXPORTED USB4000_ReadAnalogInput();
	double EXPORTED USB4000_ReadAnalogInputVoltage();
	BOOL EXPORTED USB4000_SetAnalogOutput(int value);
	BOOL EXPORTED USB4000_SetAnalogOutputVoltage(double volts);
	BOOL EXPORTED USB4000_GetPSOCVersion(char* ver);
	BOOL EXPORTED USB4000_WriteFPGA(BYTE reg, int value);
	int EXPORTED USB4000_ReadFPGA(BYTE reg);
	int EXPORTED USB4000_GetMasterClockDivisor();
	BOOL EXPORTED USB4000_SetMasterClockDivisor(int value);
	BOOL EXPORTED USB4000_GetFPGAFirmwareVersion(char* str);
	int EXPORTED USB4000_GetContinuousStrobeBase();
	BOOL EXPORTED USB4000_SetContinuousStrobeBase(int value);
	int EXPORTED USB4000_GetContinuousStrobeClock();
	BOOL EXPORTED USB4000_SetContinuousStrobeClock(int value);
	short EXPORTED USB4000_GetIntegrationTimeBaseClock();
	BOOL EXPORTED USB4000_SetIntegrationTimeBaseClock(short value);

	int EXPORTED USB4000_GetIntegrationClockTimer();
	BOOL EXPORTED USB4000_SetIntegrationClockTimer(short value);
	int EXPORTED USB4000_GetShutterClock();
	BOOL EXPORTED USB4000_SetShutterClock(int value);
	int EXPORTED USB4000_GetHardwareTriggerDelay();
	BOOL EXPORTED USB4000_SetHardwareTriggerDelay(int value);
	short EXPORTED USB4000_GetHardwareTriggerMode();
	BOOL EXPORTED USB4000_SetHardwareTriggerMode(short value);
	int EXPORTED USB4000_GetSingleStrobeHigh();
	BOOL EXPORTED USB4000_SetSingleStrobeHigh(int value);
	int EXPORTED USB4000_GetSingleStrobeLow();
	BOOL EXPORTED USB4000_SetSingleStrobeLow(int value);
	short EXPORTED USB4000_GetStrobeEnableFPGA();
	BOOL EXPORTED USB4000_SetStrobeEnableFPGA(short value);
	short EXPORTED USB4000_GetGPIOMux();
	BOOL EXPORTED USB4000_SetGPIOMux(short value);
	BOOL EXPORTED USB4000_SetGPIOMuxReg(short value);
	BOOL EXPORTED USB4000_SetGPIOMuxAlt(short value);
	BOOL EXPORTED USB4000_GetGPIOPinMux(short pin);
	short EXPORTED USB4000_GetGPIOIO();
	BOOL EXPORTED USB4000_SetGPIOIO(short value);
	short EXPORTED USB4000_GetGPIOPinIO(short pin);
	BOOL EXPORTED USB4000_SetGPIOPinIO(short pin, short value);
	BOOL EXPORTED USB4000_SetGPIOPinOutput(short pin);
	BOOL EXPORTED USB4000_SetGPIOPinInput(short pin);
	BOOL EXPORTED USB4000_SetGPIODataReg(short value);
	short EXPORTED USB4000_GetGPIODataReg();
	short EXPORTED USB4000_GetGPIOPinData(short pin);
	BOOL EXPORTED USB4000_SetGPIOPinData(short pin, short value);
	double EXPORTED USB4000_GetMasterClockTime();
	double EXPORTED USB4000_GetIntegrationClockTime();
	double EXPORTED USB4000_GetContinuousStrobeBaseTime();
	double EXPORTED USB4000_GetContinuousStrobeTime();
	double EXPORTED USB4000_GetHardwareTriggerDelayTime();
	double EXPORTED USB4000_GetSingleStrobeHighTime();
	double EXPORTED USB4000_GetSingleStrobeLowTime();
	int EXPORTED USB4000_GetLastAutonullingStatusValue();
	int EXPORTED USB4000_GetLastAutonullingDarkValue();
	double EXPORTED USB4000_GetAutonullingSaturationValue();
	double EXPORTED USB4000_GetExternalTemperatureCelsius();
	float EXPORTED USB4000_GetTemperatureCoefficient(int index);
	BOOL EXPORTED USB4000_SetTemperatureCoefficients(float first, float second);
	BOOL EXPORTED USB4000_GetO2ProbeCalibrationCoefficients(LPO2COEFS O2);
	BOOL EXPORTED USB4000_GetLS450Modulation();
	BOOL EXPORTED USB4000_SetLS450Modulation(int pulsed);
	BOOL EXPORTED USB4000_SetLS450Da(int daValue);
	BOOL EXPORTED USB4000_SetO2ProbeCalibrationCoefficients(LPO2COEFS O2);

	BOOL	EXPORTED	USB2000Plus_GetCoefficients(USB2000PLUSCONSTANTS* con);
	BOOL	EXPORTED	USB2000Plus_SetCoefficients(USB2000PLUSCONSTANTS* con);
	BOOL	EXPORTED	USB2000Plus_IdentifyUSB2000Plus(int DevNum, char* SerialNumber);
	void	EXPORTED	USB2000Plus_GetFirmwareVersion(char* ver);
	BOOL	EXPORTED	USB2000Plus_SetStoredString(int slot, char* string);
	BOOL	EXPORTED	USB2000Plus_GetStoredString(int slot, char* string);
	double EXPORTED USB2000Plus_GetBoardTemperature();
	double EXPORTED USB2000Plus_GetBoardTemperature();
	BOOL EXPORTED USB2000Plus_SetPowerDown(BOOL on);
	int EXPORTED USB2000Plus_WriteI2C(BYTE address, BYTE numBytes, BYTE* data);
	int EXPORTED USB2000Plus_ReadI2C(BYTE address, BYTE numBytes, BYTE* data);
	BOOL EXPORTED USB2000Plus_WriteSPI(int numBytes, BYTE* data);
	BOOL EXPORTED USB2000Plus_ReadSPI(int numBytes, BYTE* data);
	int EXPORTED USB2000Plus_ReadAnalogInput();
	double EXPORTED USB2000Plus_ReadAnalogInputVoltage();
	BOOL EXPORTED USB2000Plus_SetAnalogOutput(int value);
	BOOL EXPORTED USB2000Plus_SetAnalogOutputVoltage(double volts);
	BOOL EXPORTED USB2000Plus_GetPSOCVersion(char* ver);
	BOOL EXPORTED USB2000Plus_WriteFPGA(BYTE reg, int value);
	int EXPORTED USB2000Plus_ReadFPGA(BYTE reg);
	int EXPORTED USB2000Plus_GetMasterClockDivisor();
	BOOL EXPORTED USB2000Plus_SetMasterClockDivisor(int value);
	BOOL EXPORTED USB2000Plus_GetFPGAFirmwareVersion(char* str);
	int EXPORTED USB2000Plus_GetContinuousStrobeBase();
	BOOL EXPORTED USB2000Plus_SetContinuousStrobeBase(int value);
	int EXPORTED USB2000Plus_GetContinuousStrobeClock();
	BOOL EXPORTED USB2000Plus_SetContinuousStrobeClock(int value);
	short EXPORTED USB2000Plus_GetIntegrationTimeBaseClock();
	BOOL EXPORTED USB2000Plus_SetIntegrationTimeBaseClock(short value);
	int EXPORTED USB2000Plus_GetIntegrationClockTimer();
	BOOL EXPORTED USB2000Plus_SetIntegrationClockTimer(short value);
	int EXPORTED USB2000Plus_GetShutterClock();
	BOOL EXPORTED USB2000Plus_SetShutterClock(int value);
	int EXPORTED USB2000Plus_GetHardwareTriggerDelay();
	BOOL EXPORTED USB2000Plus_SetHardwareTriggerDelay(int value);
	short EXPORTED USB2000Plus_GetHardwareTriggerMode();
	BOOL EXPORTED USB2000Plus_SetHardwareTriggerMode(short value);
	int EXPORTED USB2000Plus_GetSingleStrobeHigh();
	BOOL EXPORTED USB2000Plus_SetSingleStrobeHigh(int value);
	int EXPORTED USB2000Plus_GetSingleStrobeLow();
	BOOL EXPORTED USB2000Plus_SetSingleStrobeLow(int value);
	short EXPORTED USB2000Plus_GetStrobeEnableFPGA();
	BOOL EXPORTED USB2000Plus_SetStrobeEnableFPGA(short value);
	short EXPORTED USB2000Plus_GetGPIOMux();
	BOOL EXPORTED USB2000Plus_SetGPIOMux(short value);
	BOOL EXPORTED USB2000Plus_SetGPIOMuxReg(short value);
	BOOL EXPORTED USB2000Plus_SetGPIOMuxAlt(short value);
	BOOL EXPORTED USB2000Plus_GetGPIOPinMux(short pin);
	short EXPORTED USB2000Plus_GetGPIOIO();
	BOOL EXPORTED USB2000Plus_SetGPIOIO(short value);
	short EXPORTED USB2000Plus_GetGPIOPinIO(short pin);
	BOOL EXPORTED USB2000Plus_SetGPIOPinIO(short pin, short value);
	BOOL EXPORTED USB2000Plus_SetGPIOPinOutput(short pin);
	BOOL EXPORTED USB2000Plus_SetGPIOPinInput(short pin);
	BOOL EXPORTED USB2000Plus_SetGPIODataReg(short value);
	short EXPORTED USB2000Plus_GetGPIODataReg();
	short EXPORTED USB2000Plus_GetGPIOPinData(short pin);
	BOOL EXPORTED USB2000Plus_SetGPIOPinData(short pin, short value);
	double EXPORTED USB2000Plus_GetMasterClockTime();
	double EXPORTED USB2000Plus_GetIntegrationClockTime();
	double EXPORTED USB2000Plus_GetContinuousStrobeBaseTime();
	double EXPORTED USB2000Plus_GetContinuousStrobeTime();
	double EXPORTED USB2000Plus_GetHardwareTriggerDelayTime();
	double EXPORTED USB2000Plus_GetSingleStrobeHighTime();
	double EXPORTED USB2000Plus_GetSingleStrobeLowTime();
	int EXPORTED USB2000Plus_GetLastAutonullingStatusValue();
	int EXPORTED USB2000Plus_GetLastAutonullingDarkValue();
	double EXPORTED USB2000Plus_GetAutonullingSaturationValue();

	void EXPORTED OOI_SetAutonullingEnabled(BOOL ena);
	BOOL EXPORTED OOI_GetAutonullingEnabled();

	///////////////////////////////////////////////////////////////////////////////////
	//// LS450 Calls
	///////////////////////////////////////////////////////////////////////////////////

	float EXPORTED OOI_GetExternalTemperatureCelsius();
	void EXPORTED OOI_GetTemperatureCoefficients(float* first, float* second);
	BOOL EXPORTED OOI_SetTemperatureCoefficients(float first, float second);
	BOOL EXPORTED OOI_GetO2ProbeCalibrationCoefficients(LPO2COEFS O2);
	BOOL EXPORTED OOI_GetLS450Modulation();
	BOOL EXPORTED OOI_SetLS450Modulation(int pulsed);
	BOOL EXPORTED OOI_SetLS450Da(int daValue);
	BOOL EXPORTED OOI_SetO2ProbeCalibrationCoefficients(LPO2COEFS O2);

	///////////////////////////////////////////////////////////////////////////////////
	//// Function prototypes backward compatibility ONLY and provide limitied functionality
	///////////////////////////////////////////////////////////////////////////////////
	void	EXPORTED	SS_BufferEmpty(HINSTANCE, short);
	void	EXPORTED	SS_Config(short, short, short, short);
	short	EXPORTED	SS_ContinuousScan(HINSTANCE);
	short	EXPORTED	SS_DoScan(SCANPARM FAR*);
	short	EXPORTED	SS_ParamSet(OOI_PARAM FAR*);
	WORD	EXPORTED	SS_Register(OOI_PARAM FAR*);
	short	EXPORTED	SS_SingleScan(HINSTANCE);
	void	EXPORTED	SS_Stat(OOI_PARAM FAR*);
	void	EXPORTED	SS_Stop(HINSTANCE);
	void	EXPORTED	SS_UnRegister(HINSTANCE);

	///////////////////////////////////////////////////////////////////////////////////
	//// __stdcall function prototypes Visual Basic and Excel
	///////////////////////////////////////////////////////////////////////////////////
#ifndef __BORLANDC__  // do not include these __stdcall functions for Borland compilers
#ifdef WIN32 // only include these __stdcall functions for the 32-bit driver

///////////////////////////////////////////////////////////////////////////////////
//// Function prototypes for OOI_ functions
///////////////////////////////////////////////////////////////////////////////////
	void	EXPORTED __stdcall OOI_BufferEmpty_stdcall(HINSTANCE, short);	// indicate you are done with data
	void	EXPORTED __stdcall OOI_Config_stdcall(short, short, short, short); // configure hardware
	void	EXPORTED __stdcall OOI_ConfigEx_stdcall(short, short, short, short, short, short);// config hardware
	short	EXPORTED __stdcall OOI_ContinuousScan_stdcall(HINSTANCE);		// perform continuous acquisitions
	void	EXPORTED __stdcall OOI_DisableMultichannelSpectra_stdcall(void);// disable rotator _stdcall(ADC1000)
	short	EXPORTED __stdcall OOI_DoScan_stdcall(SCANPARM FAR*);			// perform 1 acquisition
	short	EXPORTED __stdcall OOI_DoScan_Array_stdcall(short FAR*, float FAR*);		// perform 1 acquisition
	long	EXPORTED __stdcall OOI_DoScan_Array_Long_stdcall(long FAR*, float FAR*); // perform 1 acquisition
	void	EXPORTED __stdcall OOI_EnableMultichannelSpectra_stdcall(short);// enable channel rotator _stdcall(ADC1000)
	void	EXPORTED __stdcall OOI_EnableNLCorrection_stdcall(short, short);	// enable nonlineaity correction
	void	EXPORTED __stdcall OOI_EnableSLCorrection_stdcall(short, short); // enable stray light correction
	short	EXPORTED __stdcall OOI_Flexible_Acquisition_stdcall(OOI_FAM_PARAM FAR*);// perform 1 acquisition
	short	EXPORTED __stdcall OOI_FullScan_stdcall(FULLPARM FAR* fp);		// perform 1 acquisition
	WORD	EXPORTED __stdcall OOI_GetADCType_stdcall(void);				// return A/D adapter type

	DWORD	EXPORTED __stdcall OOI_GetBaseAddress_stdcall(void);			// return base address _stdcall(decimal)
	void	EXPORTED __stdcall OOI_GetDriverVersion_stdcall(char* buffer, int chars);
	WORD	EXPORTED __stdcall OOI_GetIntegrationTime_stdcall(void);		// return integration time _stdcall(msec)
	short	EXPORTED __stdcall OOI_GetIRQ_stdcall(void);					// return IRQ
	WORD	EXPORTED __stdcall OOI_GetNIDAQCardID_stdcall(void);			// return NI-DAQ card ID
	short	EXPORTED __stdcall OOI_GetNumberOfPixels_stdcall(void);			// return number of pixels
	WORD	EXPORTED __stdcall OOI_GetSAD500BaudRate_stdcall(void);			// gets SAD500 baud rate
	WORD	EXPORTED __stdcall OOI_GetSAD500CompressedMode_stdcall(void);	// gets SAD500 compressed mode
	WORD	EXPORTED __stdcall OOI_GetSAD500PixelResolution_stdcall(void);	// gets SAD500 Pixel resolution
	WORD	EXPORTED __stdcall OOI_GetSerialPort_stdcall(void);				// return serial port
	WORD	EXPORTED __stdcall OOI_GetSpectrometerType_stdcall(void);		// return spectrometer type
	short	EXPORTED __stdcall OOI_ParamSet_stdcall(OOI_PARAM FAR*);		// lock acquisition paramters
	WORD	EXPORTED __stdcall OOI_Register_stdcall(OOI_PARAM FAR*);		// register application instance
	void	EXPORTED __stdcall OOI_S2000StrobeEnable_stdcall(short);		// enable/disalbe S2000 strobe
	void	EXPORTED __stdcall OOI_SetNLCoefs_stdcall(short, float FAR*);	// set nonlinearity correction coefs
	short	EXPORTED __stdcall OOI_SetSAD500BaudRate_stdcall(WORD);			// sets SAD500 baud rate
	short	EXPORTED __stdcall OOI_SetSAD500CompressedMode_stdcall(WORD);	// sets SAD500 compressed mode
	short	EXPORTED __stdcall OOI_SetSAD500PixelResolution_stdcall(WORD);	// sets SAD500 Pixel resolution
	void	EXPORTED __stdcall OOI_SetSLCoef_stdcall(short, float);			// set stray light correction coefs
	short	EXPORTED __stdcall OOI_SingleScan_stdcall(HINSTANCE);			// perform single acquisition
	void	EXPORTED __stdcall OOI_Stat_stdcall(OOI_PARAM FAR*);			// return acquisition status
	void	EXPORTED __stdcall OOI_Stop_stdcall(HINSTANCE);					// stop continuous acquisitions
	void	EXPORTED __stdcall OOI_StrobeEnable_stdcall(short);				// enables/disables strobe for S2000 and S1024DW
	void	EXPORTED __stdcall OOI_UnRegister_stdcall(HINSTANCE);			// unregister application instance
	void	EXPORTED __stdcall OOI_GetCurrentUSB2000SerialNumber_stdcall(char* sn);	// outdated, use OOI_GetCurrentUSBSerialNumber
	BOOL	EXPORTED __stdcall OOI_SetCurrentUSB2000SerialNumber_stdcall(char* sn);	// outdated, use OOI_SetCurrentUSBSerialNumber
	void	EXPORTED __stdcall OOI_IdentifyAllUSB2000_stdcall(USB2000INFO* info);
	void	EXPORTED __stdcall OOI_IdentifyAllADC1000USB_stdcall(ADC1000USBINFO* info);
	void	EXPORTED __stdcall OOI_GetCurrentUSBSerialNumber_stdcall(char* sn);
	BOOL	EXPORTED __stdcall OOI_SetCurrentUSBSerialNumber_stdcall(char* sn);
	HANDLE	EXPORTED __stdcall OOI_SingleScan_Thread_stdcall(HINSTANCE fsp);// Creates OOI_SingleFullScanUSB2000 as a thread
	BOOL	EXPORTED __stdcall OOI_GetD3State_stdcall(void);
	int		EXPORTED __stdcall OOI_GetPCICardID_stdcall(void);
	void	EXPORTED __stdcall OOI_IdentifyAllHR2000_stdcall(HR2000INFO* info);
	int 	EXPORTED __stdcall OOI_GetTrigTimeout_stdcall(void);
	void	EXPORTED __stdcall OOI_SetTrigTimeout_stdcall(int TimeOut);
	BOOL	EXPORTED __stdcall OOI_EnableDriverMessages_stdcall(BOOL OnOff);
	void	EXPORTED	__stdcall OOI_IdentifyAllNIR512_stdcall(NIRADCINFO* info);
	void	EXPORTED	__stdcall OOI_IdentifyAllNIR256_stdcall(NIRADCINFO* info);
	void EXPORTED __stdcall OOI_IdentifyAllHR4000_stdcall(HR4000INFO* info);
	void EXPORTED __stdcall OOI_IdentifyAllHR2000Plus_stdcall(HR2000PLUSINFO* info);
	void EXPORTED __stdcall OOI_IdentifyAllQE65000_stdcall(QE65000INFO* info);
	void EXPORTED __stdcall OOI_GetScanTimes_stdcall(SCANTIMES* st);
	DWORD EXPORTED __stdcall OOI_GetMaximumScopeModeIntensity_stdcall();
	///////////////////////////////////////////////////////////////////////////////////
	//// Function prototypes for SAD_ functions
	///////////////////////////////////////////////////////////////////////////////////
	short	EXPORTED __stdcall SAD_ClearMemory_stdcall(WORD);				// clear memory
	short	EXPORTED __stdcall SAD_Close_stdcall(WORD);						// close SAD500
	int		EXPORTED __stdcall SAD_DetermineSerialADCType_stdcall(void);	// deterimes
	short	EXPORTED __stdcall SAD_DumpFastMemory_stdcall(void);			// dump fast memory scans to slow memory
	short	EXPORTED __stdcall SAD_EnableISR_stdcall(WORD, void FAR*);		// sets ISR-like behavior
	short	EXPORTED __stdcall SAD_EnterDoubleSecretMode_stdcall(WORD);
	WORD	EXPORTED __stdcall SAD_GetADCRate_stdcall(void);				// get A/D rate in kHz
	WORD	EXPORTED __stdcall SAD_GetBaudRate_stdcall(void);				// get SAD500 and computer baud rate
	WORD	EXPORTED __stdcall SAD_GetBoxcarWidth_stdcall(void);			// get pixel boxcar width
	WORD	EXPORTED __stdcall SAD_GetChecksumMode_stdcall(void);			// get checksum mode
	WORD	EXPORTED __stdcall SAD_GetCompressedMode_stdcall(void);			// gets compressed data transfer
	WORD	EXPORTED __stdcall SAD_GetError_stdcall(void);					// gets and clears error value
	WORD	EXPORTED __stdcall SAD_GetExternalTrigger_stdcall(void);		// get external trigger mode
	short	EXPORTED __stdcall SAD_GetFastMemoryAvailable_stdcall(void);	// get fast mem. available _stdcall(scans)
	WORD	EXPORTED __stdcall SAD_GetFullPixelMode_stdcall(PIXELMODEDATA FAR*);// gets full pixel mode
	WORD	EXPORTED __stdcall SAD_GetIntegrationCounter_stdcall(void);		// retreives integration period counter
	WORD	EXPORTED __stdcall SAD_GetIntegrationTime_stdcall(void);		// get integration time in msec
	WORD	EXPORTED __stdcall SAD_GetInterpolateMissingPixels_stdcall(void);// gets interpolation of missing pix.
	short	EXPORTED __stdcall SAD_GetNumberOfScansInMemory_stdcall(WORD);	// get num. scans in memory
	WORD	EXPORTED __stdcall SAD_GetNumberOfScansToStore_stdcall(void);	// get number of to store
	WORD	EXPORTED __stdcall SAD_GetPixelMode_stdcall(void);				// get pixel transfer mode
	WORD	EXPORTED __stdcall SAD_GetS1024DWCDSMode_stdcall(void);			// gets S1024DW correlated doubling sampleing mode
	WORD	EXPORTED __stdcall SAD_GetScanMode_stdcall(void);				// get whether scans are returned or stored in memory
	WORD	EXPORTED __stdcall SAD_GetScansToAdd_stdcall(void);				// get number of scans to add
	short	EXPORTED __stdcall SAD_GetSlowMemoryAvailable_stdcall(void);	// get slow mem. available _stdcall(kbytes)
	WORD	EXPORTED __stdcall SAD_GetSpectrometerChannel_stdcall(void);	// get spectrometer channel
	WORD	EXPORTED __stdcall SAD_GetStrobeEnable_stdcall(void);			// get strobe enable _stdcall(S0)
	WORD	EXPORTED __stdcall SAD_GetVersion_stdcall(void);				// retreives microcode version
	short	EXPORTED __stdcall SAD_Init_stdcall(WORD, WORD);					// initialize SAD500
	short	EXPORTED __stdcall SAD_ReadoutOneScan_stdcall(WORD, SADDATA FAR*);// readout one scan
	short	EXPORTED __stdcall SAD_ReadoutScansInMemory_stdcall(WORD, WORD FAR*);// readout stored spectra
	short	EXPORTED __stdcall SAD_Reset_stdcall(void);						// reset A/D
	short	EXPORTED __stdcall SAD_Scan_stdcall(SADDATA FAR*);				// start spectral acquisition
	short	EXPORTED __stdcall SAD_ScanReceivedOK_stdcall(WORD);			// scan readout ok
	WORD	EXPORTED __stdcall SAD_ScanWithAverage_stdcall(SADDATA FAR*, WORD, float far*);
	short	EXPORTED __stdcall SAD_SetADCRate_stdcall(WORD);				// set A/D rate in kHz
	short	EXPORTED __stdcall SAD_SetBaudRate_stdcall(WORD);				// sets SAD500 and computer baud rate
	short	EXPORTED __stdcall SAD_SetBoxcarWidth_stdcall(WORD);			// set pixel boxcar width
	short	EXPORTED __stdcall SAD_SetChecksumMode_stdcall(WORD);			// sets checksum mode
	short	EXPORTED __stdcall SAD_SetCompressedMode_stdcall(WORD);			// sets/clears compressed data transfer
	short	EXPORTED __stdcall SAD_SetExternalTrigger_stdcall(WORD);		// sets external trigger mode
	short	EXPORTED __stdcall SAD_SetFlashDelay_stdcall(WORD);				// sets flash delay for adc1000-usb in serial mode
	short	EXPORTED __stdcall SAD_SetIntegrationTime_stdcall(WORD);		// sets integration time in msec
	short	EXPORTED __stdcall SAD_SetInterpolateMissingPixels_stdcall(WORD);// sets interpolation of missing pix.
	short	EXPORTED __stdcall SAD_SetNumberOfScansToStore_stdcall(WORD);	// sets number of to store
	short	EXPORTED __stdcall SAD_SetPixelMode_stdcall(PIXELMODEDATA FAR*);// sets pixel transfer mode
	short	EXPORTED __stdcall SAD_SetS1024DWCDSMode_stdcall(WORD);			// set S1024DW correlated doubling sampleing mode
	short	EXPORTED __stdcall SAD_SetScanMode_stdcall(WORD);				// sets whether scans are returned or stored
	short	EXPORTED __stdcall SAD_SetScansToAdd_stdcall(WORD);				// sets number of scans to add
	short	EXPORTED __stdcall SAD_SetSerialPort_stdcall(WORD);
	short	EXPORTED __stdcall SAD_SetSpectrometerChannel_stdcall(WORD);	// sets spectrometer channel
	short	EXPORTED __stdcall SAD_SetStrobeEnable_stdcall(WORD);			// enables strobe _stdcall(toggles S0)
	short	EXPORTED __stdcall SAD_GetStoredString_stdcall(int slot, char* string);
	short	EXPORTED __stdcall SAD_SetStoredString_stdcall(int slot, char* string);
	short	EXPORTED __stdcall SAD_SetStoredFloat_stdcall(int slot, float value);
	short	EXPORTED __stdcall SAD_GetStoredFloat_stdcall(int slot, float* value);
	short	EXPORTED __stdcall SAD_SetStoredInt_stdcall(int slot, int value);
	short	EXPORTED __stdcall SAD_GetStoredInt_stdcall(int slot, int* value);
	short	EXPORTED __stdcall SAD_SetRamanMode_stdcall(BOOL mode);
	BOOL	EXPORTED __stdcall SAD_GetRamanMode_stdcall(void);

	///////////////////////////////////////////////////////////////////////////////////
	//// Function prototypes for SAD_ double-secret functions
	///////////////////////////////////////////////////////////////////////////////////
	short	EXPORTED __stdcall SAD_ChangeSlowReadPointer_stdcall(WORD);		// change slow memory read pointer
	short	EXPORTED __stdcall SAD_ClearSlowMemory_stdcall(WORD, WORD);		// clears slow memory BLOCK and PAGE
	WORD	EXPORTED __stdcall SAD_GetDebugMode_stdcall(void);				// gets debug mode enabled
	WORD	EXPORTED __stdcall SAD_GetNumberOfBadMemoryBlocks_stdcall(void);// gets number of bad flash memory blocks
	WORD	EXPORTED __stdcall SAD_GetPrintDebugMode_stdcall(void);			// gets debug print mode enabled
	short	EXPORTED __stdcall SAD_ReadSlowMemory_stdcall(WORD, WORD, WORD FAR*, WORD FAR*);// read out slow memory BLOCK and PAGE
	short	EXPORTED __stdcall SAD_ToggleDebugMode_stdcall(void);			// toggles debug mode _stdcall(ascii xmit and receive)
	short	EXPORTED __stdcall SAD_ToggleDebugPrintMode_stdcall(void);		// toggles debug print mode

	///////////////////////////////////////////////////////////////////////////////////
	//// Function prototypes for USB2000_ and USB2000-specific commands
	///////////////////////////////////////////////////////////////////////////////////
	BOOL	EXPORTED __stdcall USB2000_GetCoefficients_stdcall(USB2000CONSTANTS* con);
	BOOL	EXPORTED __stdcall USB2000_SetCoefficients_stdcall(USB2000CONSTANTS* con);
	BOOL	EXPORTED __stdcall USB2000_IdentifyUSB2000_stdcall(int DevNum, char* SerialNumber);
	void	EXPORTED __stdcall USB2000_GetFirmwareVersion_stdcall(char* ver);
	BOOL	EXPORTED __stdcall USB2000_SetStoredString_stdcall(int slot, char* string);
	BOOL	EXPORTED __stdcall USB2000_GetStoredString_stdcall(int slot, char* string);
	BOOL	EXPORTED __stdcall USB2000_SetO2CalibrationCoefficients_stdcall(LPO2COEFS O2);
	BOOL	EXPORTED __stdcall USB2000_GetO2CalibrationCoefficients_stdcall(LPO2COEFS O2);
	BOOL	EXPORTED __stdcall USB2000_SetLS450Modulation_stdcall(BOOL Pulsed);
	BOOL	EXPORTED __stdcall USB2000_GetLS450Modulation_stdcall(void);
	BOOL	EXPORTED __stdcall USB2000_IsPluginDetected_stdcall(BYTE PluginID);
	float	EXPORTED __stdcall USB2000_GetTemperature_stdcall(void);
	void	EXPORTED __stdcall USB2000_GetTemperatureCoefficients_stdcall(float* first, float* second);
	void	EXPORTED __stdcall USB2000_SetTemperatureCoefficients_stdcall(float first, float second);
	BOOL	EXPORTED __stdcall USB2000_SetLS450DA_stdcall(WORD daValue);
	BOOL EXPORTED __stdcall USB2000_ISSUV_LampOn_stdcall(BOOL on);
	BOOL EXPORTED __stdcall USB2000_ISSUV_SetVisLampOn_stdcall(BOOL on);
	BOOL EXPORTED __stdcall USB2000_ISSUV_SetShutterOpened_stdcall(BOOL open);
	BOOL EXPORTED __stdcall USB2000_ISSUV_StoreCurrentPotValues_stdcall(void);
	BOOL EXPORTED __stdcall USB2000_ISSUV_SetVisLampIntensity_stdcall(short value);
	short EXPORTED __stdcall USB2000_ISSUV_GetVisLampIntensity_stdcall(void);
	BOOL EXPORTED __stdcall USB2000_ISSUV_SetUVLampIntensity_stdcall(short value);
	short EXPORTED __stdcall USB2000_ISSUV_GetUVLampIntensity_stdcall(void);
	short EXPORTED __stdcall USB2000_I2C_Read_stdcall(short address, short numbytes, BYTE* array);
	short EXPORTED __stdcall USB2000_I2C_Write_stdcall(short address, short numbytes, BYTE* array);

	///////////////////////////////////////////////////////////////////////////////////
	//// Function prototypes for ADC1000USB_ and ADC1000USB-specific commands
	///////////////////////////////////////////////////////////////////////////////////
	BOOL	EXPORTED __stdcall ADC1000USB_GetCoefficients_stdcall(ADC1000USBCONSTANTS* con);
	BOOL	EXPORTED __stdcall ADC1000USB_SetCoefficients_stdcall(ADC1000USBCONSTANTS* con);
	BOOL	EXPORTED __stdcall ADC1000USB_IdentifyADC1000USB_stdcall(int DevNum, char* SerialNumber);
	void	EXPORTED __stdcall ADC1000USB_GetFirmwareVersion_stdcall(char* ver);
	BOOL	EXPORTED __stdcall ADC1000USB_DAC_Initialize_stdcall(void);
	WORD	EXPORTED __stdcall ADC1000USB_DAC_ReadValue_stdcall(short channel);
	BOOL	EXPORTED __stdcall ADC1000USB_DAC_WriteValue_stdcall(short channel, WORD value);

	///////////////////////////////////////////////////////////////////////////////////
	//// Function prototypes for ADC2000PCI and ADC2000PCI-specific commands
	///////////////////////////////////////////////////////////////////////////////////
	void	EXPORTED __stdcall ADC2000PCI_WriteDIO_stdcall(BYTE bits, BYTE value);
	WORD	EXPORTED __stdcall ADC2000PCI_ReadDIO_stdcall(void);
	void	EXPORTED __stdcall ADC2000PCI_AnalogOut_stdcall(int channel, WORD value);
	BOOL	EXPORTED __stdcall ADC2000PCI_IsPCIPlus_stdcall(void);

	///////////////////////////////////////////////////////////////////////////////////
	//// Function prototypes for HR2000_ and HR2000-specific commands
	///////////////////////////////////////////////////////////////////////////////////
	BOOL	EXPORTED __stdcall HR2000_GetCoefficients_stdcall(HR2000CONSTANTS* con);
	BOOL	EXPORTED __stdcall HR2000_SetCoefficients_stdcall(HR2000CONSTANTS* con);
	BOOL	EXPORTED __stdcall HR2000_IdentifyHR2000_stdcall(int DevNum, char* SerialNumber);
	void	EXPORTED __stdcall HR2000_GetFirmwareVersion_stdcall(char* ver);
	BOOL	EXPORTED __stdcall HR2000_SetStoredString_stdcall(int slot, char* string);
	BOOL	EXPORTED __stdcall HR2000_GetStoredString_stdcall(int slot, char* string);

	///////////////////////////////////////////////////////////////////////////////////
	//// Function prototypes for NIRADC_ and NIRADC-specific commands
	///////////////////////////////////////////////////////////////////////////////////
	BOOL	EXPORTED __stdcall NIRADC_GetCoefficients_stdcall(NIRADCCONSTANTS* con);
	BOOL	EXPORTED __stdcall NIRADC_SetCoefficients_stdcall(NIRADCCONSTANTS* con);

	BOOL	EXPORTED __stdcall NIRADC_IdentifyNIR512_stdcall(int DevNum, char* SerialNumber);
	BOOL	EXPORTED __stdcall NIRADC_IdentifyNIR256_stdcall(int DevNum, char* SerialNumber);

	BOOL	EXPORTED __stdcall NIRADC_SetStoredString_stdcall(int slot, char* string);
	BOOL	EXPORTED __stdcall NIRADC_GetStoredString_stdcall(int slot, char* string);
	BOOL	EXPORTED __stdcall NIRADC_EnableCooler_stdcall(BOOL OnOff);
	BOOL	EXPORTED __stdcall NIRADC_EnableHighGain_stdcall(BOOL OnOff);
	BOOL	EXPORTED __stdcall NIRADC_EnableFan_stdcall(BOOL OnOff);
	BOOL	EXPORTED __stdcall NIRADC_GetStatus_stdcall(NIRADCSTATUS* Stat);
	BOOL	EXPORTED __stdcall NIRADC_GetFirmwareVersion_stdcall(char* ver);

	BOOL	EXPORTED __stdcall NIRADC_SetIntegrationTimeScalar_stdcall(BOOL msec);
	BOOL	EXPORTED __stdcall NIRADC_GetIntegrationTimeScalar_stdcall();
	BOOL	EXPORTED __stdcall NIRADC_SetTECSetPoint_stdcall(float temp);
	float	EXPORTED __stdcall NIRADC_GetTECSetPoint_stdcall();
	float	EXPORTED __stdcall NIRADC_GetTECTemperature_stdcall();
	float	EXPORTED __stdcall NIRADC_GetBoardTemperature_stdcall();

	///////////////////////////////////////////////////////////////////////////////////
	//// Function prototypes for HR4000_ and HR4000-specific commands
	///////////////////////////////////////////////////////////////////////////////////
	BOOL	EXPORTED __stdcall HR4000_GetCoefficients_stdcall(HR4000CONSTANTS* con);
	BOOL	EXPORTED __stdcall HR4000_SetCoefficients_stdcall(HR4000CONSTANTS* con);
	BOOL	EXPORTED __stdcall HR4000_IdentifyHR2000_stdcall(int DevNum, char* SerialNumber);
	void	EXPORTED __stdcall HR4000_GetFirmwareVersion_stdcall(char* ver);
	BOOL	EXPORTED __stdcall HR4000_SetStoredString_stdcall(int slot, char* string);
	BOOL	EXPORTED __stdcall HR4000_GetStoredString_stdcall(int slot, char* string);
	double EXPORTED __stdcall HR4000_GetBoardTemperature_stdcall();
	BOOL EXPORTED __stdcall HR4000_SetPowerDown_stdcall(BOOL on);
	int EXPORTED __stdcall HR4000_WriteI2C_stdcall(BYTE address, BYTE numBytes, BYTE* data);
	int EXPORTED __stdcall HR4000_ReadI2C_stdcall(BYTE address, BYTE numBytes, BYTE* data);
	BOOL EXPORTED __stdcall HR4000_WriteSPI_stdcall(int numBytes, BYTE* data);
	BOOL EXPORTED __stdcall HR4000_ReadSPI_stdcall(int numBytes, BYTE* data);

	int EXPORTED __stdcall HR4000_ReadAnalogInput_stdcall();
	double EXPORTED __stdcall HR4000_ReadAnalogInputVoltage_stdcall();
	BOOL EXPORTED __stdcall HR4000_SetAnalogOutput_stdcall(int value);
	BOOL EXPORTED __stdcall HR4000_SetAnalogOutputVoltage_stdcall(double volts);
	BOOL EXPORTED __stdcall HR4000_GetPSOCVersion_stdcall(char* ver);
	BOOL EXPORTED __stdcall HR4000_WriteFPGA_stdcall(BYTE reg, int value);
	int EXPORTED __stdcall HR4000_ReadFPGA_stdcall(BYTE reg);
	int EXPORTED __stdcall HR4000_GetMasterClockDivisor_stdcall();
	BOOL EXPORTED __stdcall HR4000_SetMasterClockDivisor_stdcall(int value);
	BOOL EXPORTED __stdcall HR4000_GetFPGAFirmwareVersion_stdcall(char* str);
	int EXPORTED __stdcall HR4000_GetContinuousStrobeBase_stdcall();
	BOOL EXPORTED __stdcall HR4000_SetContinuousStrobeBase_stdcall(int value);
	int EXPORTED __stdcall HR4000_GetContinuousStrobeClock_stdcall();
	BOOL EXPORTED __stdcall HR4000_SetContinuousStrobeClock_stdcall(int value);
	short EXPORTED __stdcall HR4000_GetIntegrationTimeBaseClock_stdcall();
	BOOL EXPORTED __stdcall HR4000_SetIntegrationTimeBaseClock_stdcall(short value);
	int EXPORTED __stdcall HR4000_GetIntegrationClockTimer_stdcall();
	BOOL EXPORTED __stdcall HR4000_SetIntegrationClockTimer_stdcall(short value);
	int EXPORTED __stdcall HR4000_GetShutterClock_stdcall();
	BOOL EXPORTED __stdcall HR4000_SetShutterClock_stdcall(int value);
	int EXPORTED __stdcall HR4000_GetHardwareTriggerDelay_stdcall();
	BOOL EXPORTED __stdcall HR4000_SetHardwareTriggerDelay_stdcall(int value);
	short EXPORTED __stdcall HR4000_GetHardwareTriggerMode_stdcall();
	BOOL EXPORTED __stdcall HR4000_SetHardwareTriggerMode_stdcall(short value);
	int EXPORTED __stdcall HR4000_GetSingleStrobeHigh_stdcall();
	BOOL EXPORTED __stdcall HR4000_SetSingleStrobeHigh_stdcall(int value);
	int EXPORTED __stdcall HR4000_GetSingleStrobeLow_stdcall();
	BOOL EXPORTED __stdcall HR4000_SetSingleStrobeLow_stdcall(int value);
	short EXPORTED __stdcall HR4000_GetStrobeEnableFPGA_stdcall();
	BOOL EXPORTED __stdcall HR4000_SetStrobeEnableFPGA_stdcall(short value);
	short EXPORTED __stdcall HR4000_GetGPIOMux_stdcall();
	BOOL EXPORTED __stdcall HR4000_SetGPIOMux_stdcall(short value);
	BOOL EXPORTED __stdcall HR4000_SetGPIOMuxReg_stdcall(short value);
	BOOL EXPORTED __stdcall HR4000_SetGPIOMuxAlt_stdcall(short value);
	BOOL EXPORTED __stdcall HR4000_GetGPIOPinMux_stdcall(short pin);
	short EXPORTED __stdcall HR4000_GetGPIOIO_stdcall();
	BOOL EXPORTED __stdcall HR4000_SetGPIOIO_stdcall(short value);
	short EXPORTED __stdcall HR4000_GetGPIOPinIO_stdcall(short pin);
	BOOL EXPORTED __stdcall HR4000_SetGPIOPinIO_stdcall(short pin, short value);
	BOOL EXPORTED __stdcall HR4000_SetGPIOPinOutput_stdcall(short pin);
	BOOL EXPORTED __stdcall HR4000_SetGPIOPinInput_stdcall(short pin);
	BOOL EXPORTED __stdcall HR4000_SetGPIODataReg_stdcall(short value);
	short EXPORTED __stdcall HR4000_GetGPIODataReg_stdcall();
	short EXPORTED __stdcall HR4000_GetGPIOPinData_stdcall(short pin);
	BOOL EXPORTED __stdcall HR4000_SetGPIOPinData_stdcall(short pin, short value);
	double EXPORTED __stdcall HR4000_GetMasterClockTime_stdcall();
	double EXPORTED __stdcall HR4000_GetIntegrationClockTime_stdcall();
	double EXPORTED __stdcall HR4000_GetContinuousStrobeBaseTime_stdcall();
	double EXPORTED __stdcall HR4000_GetContinuousStrobeTime_stdcall();
	double EXPORTED __stdcall HR4000_GetHardwareTriggerDelayTime_stdcall();
	double EXPORTED __stdcall HR4000_GetSingleStrobeHighTime_stdcall();
	double EXPORTED __stdcall HR4000_GetSingleStrobeLowTime_stdcall();

	///////////////////////////////////////////////////////////////////////////////////
	//// Function prototypes for HR2000Plus_ and HR2000Plus-specific commands
	///////////////////////////////////////////////////////////////////////////////////
	BOOL	EXPORTED __stdcall HR2000Plus_GetCoefficients_stdcall(HR2000PLUSCONSTANTS* con);
	BOOL	EXPORTED __stdcall HR2000Plus_SetCoefficients_stdcall(HR2000PLUSCONSTANTS* con);
	BOOL	EXPORTED __stdcall HR2000Plus_IdentifyHR2000Plus_stdcall(int DevNum, char* SerialNumber);
	void	EXPORTED __stdcall HR2000Plus_GetFirmwareVersion_stdcall(char* ver);
	BOOL	EXPORTED __stdcall HR2000Plus_SetStoredString_stdcall(int slot, char* string);
	BOOL	EXPORTED __stdcall HR2000Plus_GetStoredString_stdcall(int slot, char* string);
	double EXPORTED __stdcall HR2000Plus_GetBoardTemperature_stdcall();
	BOOL EXPORTED __stdcall HR2000Plus_SetPowerDown_stdcall(BOOL on);
	int EXPORTED __stdcall HR2000Plus_WriteI2C_stdcall(BYTE address, BYTE numBytes, BYTE* data);
	int EXPORTED __stdcall HR2000Plus_ReadI2C_stdcall(BYTE address, BYTE numBytes, BYTE* data);
	BOOL EXPORTED __stdcall HR2000Plus_WriteSPI_stdcall(int numBytes, BYTE* data);
	BOOL EXPORTED __stdcall HR2000Plus_ReadSPI_stdcall(int numBytes, BYTE* data);
	int EXPORTED __stdcall HR2000Plus_ReadAnalogInput_stdcall();
	double EXPORTED __stdcall HR2000Plus_ReadAnalogInputVoltage_stdcall();
	BOOL EXPORTED __stdcall HR2000Plus_SetAnalogOutput_stdcall(int value);
	BOOL EXPORTED __stdcall HR2000Plus_SetAnalogOutputVoltage_stdcall(double volts);
	BOOL EXPORTED __stdcall HR2000Plus_GetPSOCVersion_stdcall(char* ver);
	BOOL EXPORTED __stdcall HR2000Plus_WriteFPGA_stdcall(BYTE reg, int value);
	int EXPORTED __stdcall HR2000Plus_ReadFPGA_stdcall(BYTE reg);
	int EXPORTED __stdcall HR2000Plus_GetMasterClockDivisor_stdcall();
	BOOL EXPORTED __stdcall HR2000Plus_SetMasterClockDivisor_stdcall(int value);
	BOOL EXPORTED __stdcall HR2000Plus_GetFPGAFirmwareVersion_stdcall(char* str);
	int EXPORTED __stdcall HR2000Plus_GetContinuousStrobeBase_stdcall();
	BOOL EXPORTED __stdcall HR2000Plus_SetContinuousStrobeBase_stdcall(int value);
	int EXPORTED __stdcall HR2000Plus_GetContinuousStrobeClock_stdcall();
	BOOL EXPORTED __stdcall HR2000Plus_SetContinuousStrobeClock_stdcall(int value);
	short EXPORTED __stdcall HR2000Plus_GetIntegrationTimeBaseClock_stdcall();
	BOOL EXPORTED __stdcall HR2000Plus_SetIntegrationTimeBaseClock_stdcall(short value);
	int EXPORTED __stdcall HR2000Plus_GetIntegrationClockTimer_stdcall();
	BOOL EXPORTED __stdcall HR2000Plus_SetIntegrationClockTimer_stdcall(short value);
	int EXPORTED __stdcall HR2000Plus_GetShutterClock_stdcall();
	BOOL EXPORTED __stdcall HR2000Plus_SetShutterClock_stdcall(int value);
	int EXPORTED __stdcall HR2000Plus_GetHardwareTriggerDelay_stdcall();
	BOOL EXPORTED __stdcall HR2000Plus_SetHardwareTriggerDelay_stdcall(int value);
	short EXPORTED __stdcall HR2000Plus_GetHardwareTriggerMode_stdcall();
	BOOL EXPORTED __stdcall HR2000Plus_SetHardwareTriggerMode_stdcall(short value);
	int EXPORTED __stdcall HR2000Plus_GetSingleStrobeHigh_stdcall();
	BOOL EXPORTED __stdcall HR2000Plus_SetSingleStrobeHigh_stdcall(int value);
	int EXPORTED __stdcall HR2000Plus_GetSingleStrobeLow_stdcall();
	BOOL EXPORTED __stdcall HR2000Plus_SetSingleStrobeLow_stdcall(int value);
	short EXPORTED __stdcall HR2000Plus_GetStrobeEnableFPGA_stdcall();
	BOOL EXPORTED __stdcall HR2000Plus_SetStrobeEnableFPGA_stdcall(short value);
	short EXPORTED __stdcall HR2000Plus_GetGPIOMux_stdcall();
	BOOL EXPORTED __stdcall HR2000Plus_SetGPIOMux_stdcall(short value);
	BOOL EXPORTED __stdcall HR2000Plus_SetGPIOMuxReg_stdcall(short value);
	BOOL EXPORTED __stdcall HR2000Plus_SetGPIOMuxAlt_stdcall(short value);
	BOOL EXPORTED __stdcall HR2000Plus_GetGPIOPinMux_stdcall(short pin);
	short EXPORTED __stdcall HR2000Plus_GetGPIOIO_stdcall();
	BOOL EXPORTED __stdcall HR2000Plus_SetGPIOIO_stdcall(short value);
	short EXPORTED __stdcall HR2000Plus_GetGPIOPinIO_stdcall(short pin);
	BOOL EXPORTED __stdcall HR2000Plus_SetGPIOPinIO_stdcall(short pin, short value);
	BOOL EXPORTED __stdcall HR2000Plus_SetGPIOPinOutput_stdcall(short pin);
	BOOL EXPORTED __stdcall HR2000Plus_SetGPIOPinInput_stdcall(short pin);
	BOOL EXPORTED __stdcall HR2000Plus_SetGPIODataReg_stdcall(short value);
	short EXPORTED __stdcall HR2000Plus_GetGPIODataReg_stdcall();
	short EXPORTED __stdcall HR2000Plus_GetGPIOPinData_stdcall(short pin);
	BOOL EXPORTED __stdcall HR2000Plus_SetGPIOPinData_stdcall(short pin, short value);
	double EXPORTED __stdcall HR2000Plus_GetMasterClockTime_stdcall();
	double EXPORTED __stdcall HR2000Plus_GetIntegrationClockTime_stdcall();
	double EXPORTED __stdcall HR2000Plus_GetContinuousStrobeBaseTime_stdcall();
	double EXPORTED __stdcall HR2000Plus_GetContinuousStrobeTime_stdcall();
	double EXPORTED __stdcall HR2000Plus_GetHardwareTriggerDelayTime_stdcall();
	double EXPORTED __stdcall HR2000Plus_GetSingleStrobeHighTime_stdcall();
	double EXPORTED __stdcall HR2000Plus_GetSingleStrobeLowTime_stdcall();

	BOOL	EXPORTED __stdcall	QE65000_GetCoefficients_stdcall(QE65000CONSTANTS* con);
	BOOL	EXPORTED __stdcall	QE65000_SetCoefficients_stdcall(QE65000CONSTANTS* con);
	BOOL	EXPORTED __stdcall	QE65000_IdentifyQE65000_stdcall(int DevNum, char* SerialNumber);
	void	EXPORTED __stdcall	QE65000_GetFirmwareVersion_stdcall(char* ver);
	BOOL	EXPORTED __stdcall	QE65000_SetStoredString_stdcall(int slot, char* string);
	BOOL	EXPORTED __stdcall	QE65000_GetStoredString_stdcall(int slot, char* string);
	double EXPORTED __stdcall QE65000_GetBoardTemperature_stdcall();
	BOOL EXPORTED __stdcall QE65000_SetPowerDown_stdcall(BOOL on);
	int EXPORTED __stdcall QE65000_WriteI2C_stdcall(BYTE address, BYTE numBytes, BYTE* data);
	int EXPORTED __stdcall QE65000_ReadI2C_stdcall(BYTE address, BYTE numBytes, BYTE* data);
	BOOL EXPORTED __stdcall QE65000_WriteSPI_stdcall(int numBytes, BYTE* data);
	BOOL EXPORTED __stdcall QE65000_ReadSPI_stdcall(int numBytes, BYTE* data);
	int EXPORTED __stdcall QE65000_ReadAnalogInput_stdcall();
	double EXPORTED __stdcall QE65000_ReadAnalogInputVoltage_stdcall();
	BOOL EXPORTED __stdcall QE65000_SetAnalogOutput_stdcall(int value);
	BOOL EXPORTED __stdcall QE65000_SetAnalogOutputVoltage_stdcall(double volts);
	BOOL EXPORTED __stdcall QE65000_GetPSOCVersion_stdcall(char* ver);
	BOOL EXPORTED __stdcall QE65000_WriteFPGA_stdcall(BYTE reg, int value);
	int EXPORTED __stdcall QE65000_ReadFPGA_stdcall(BYTE reg);
	int EXPORTED __stdcall QE65000_GetMasterClockDivisor_stdcall();
	BOOL EXPORTED __stdcall QE65000_SetMasterClockDivisor_stdcall(int value);
	BOOL EXPORTED __stdcall QE65000_GetFPGAFirmwareVersion_stdcall(char* str);
	int EXPORTED __stdcall QE65000_GetContinuousStrobeBase_stdcall();
	BOOL EXPORTED __stdcall QE65000_SetContinuousStrobeBase_stdcall(int value);
	int EXPORTED __stdcall QE65000_GetContinuousStrobeClock_stdcall();
	BOOL EXPORTED __stdcall QE65000_SetContinuousStrobeClock_stdcall(int value);
	short EXPORTED __stdcall QE65000_GetIntegrationTimeBaseClock_stdcall();
	BOOL EXPORTED __stdcall QE65000_SetIntegrationTimeBaseClock_stdcall(short value);
	int EXPORTED __stdcall QE65000_GetIntegrationClockTimer_stdcall();
	BOOL EXPORTED __stdcall QE65000_SetIntegrationClockTimer_stdcall(short value);
	int EXPORTED __stdcall QE65000_GetShutterClock_stdcall();
	BOOL EXPORTED __stdcall QE65000_SetShutterClock_stdcall(int value);
	int EXPORTED __stdcall QE65000_GetSingleStrobeHigh_stdcall();
	BOOL EXPORTED __stdcall QE65000_SetSingleStrobeHigh_stdcall(int value);
	int EXPORTED __stdcall QE65000_GetSingleStrobeLow_stdcall();
	BOOL EXPORTED __stdcall QE65000_SetSingleStrobeLow_stdcall(int value);
	short EXPORTED __stdcall QE65000_GetStrobeEnableFPGA_stdcall();
	BOOL EXPORTED __stdcall QE65000_SetStrobeEnableFPGA_stdcall(short value);
	short EXPORTED __stdcall QE65000_GetGPIOMux_stdcall();
	BOOL EXPORTED __stdcall QE65000_SetGPIOMux_stdcall(short value);
	BOOL EXPORTED __stdcall QE65000_SetGPIOMuxReg_stdcall(short value);
	BOOL EXPORTED __stdcall QE65000_SetGPIOMuxAlt_stdcall(short value);
	BOOL EXPORTED __stdcall QE65000_GetGPIOPinMux_stdcall(short pin);
	short EXPORTED __stdcall QE65000_GetGPIOIO_stdcall();
	BOOL EXPORTED __stdcall QE65000_SetFPGA_stdcall(short value);
	short EXPORTED __stdcall QE65000_GetGPIOPinIO_stdcall(short pin);
	BOOL EXPORTED __stdcall QE65000_SetGPIOPinIO_stdcall(short pin, short value);
	BOOL EXPORTED __stdcall QE65000_SetGPIOPinOutput_stdcall(short pin);
	BOOL EXPORTED __stdcall QE65000_SetGPIOPinInput_stdcall(short pin);
	BOOL EXPORTED __stdcall QE65000_SetGPIODataReg_stdcall(short value);
	short EXPORTED __stdcall QE65000_GetGPIODataReg_stdcall();
	short EXPORTED __stdcall QE65000_GetGPIOPinData_stdcall(short pin);
	BOOL EXPORTED __stdcall QE65000_SetGPIOPinData_stdcall(short pin, short value);
	double EXPORTED __stdcall QE65000_GetMasterClockTime_stdcall();
	double EXPORTED __stdcall QE65000_GetIntegrationClockTime_stdcall();
	double EXPORTED __stdcall QE65000_GetContinuousStrobeBaseTime_stdcall();
	double EXPORTED __stdcall QE65000_GetContinuousStrobeTime_stdcall();
	double EXPORTED __stdcall QE65000_GetSingleStrobeHighTime_stdcall();
	double EXPORTED __stdcall QE65000_GetSingleStrobeLowTime_stdcall();
	double EXPORTED __stdcall QE65000_GetMainBoardTemperature_stdcall();
	double EXPORTED __stdcall QE65000_GetDetectorBoardTemperature_stdcall();
	BOOL EXPORTED __stdcall QE65000_SetFanEnable_stdcall(BOOL on);
	BOOL EXPORTED __stdcall QE65000_SetTECEnable_stdcall(BOOL on);
	BOOL EXPORTED __stdcall QE65000_SetTECSetPoint_stdcall(double temp);
	double EXPORTED __stdcall QE65000_GetTECTemperature_stdcall();

	///////////////////////////////////////////////////////////////////////////////////
	//// Function prototypes for USB4000_ and USB4000-specific commands
	///////////////////////////////////////////////////////////////////////////////////
	BOOL	EXPORTED __stdcall	USB4000_GetCoefficients_stdcall(USB4000CONSTANTS* con);
	BOOL	EXPORTED __stdcall	USB4000_SetCoefficients_stdcall(USB4000CONSTANTS* con);
	BOOL	EXPORTED __stdcall	USB4000_IdentifyUSB4000_stdcall(int DevNum, char* SerialNumber);
	void	EXPORTED __stdcall	USB4000_GetFirmwareVersion_stdcall(char* ver);
	BOOL	EXPORTED __stdcall	USB4000_SetStoredString_stdcall(int slot, char* string);
	BOOL	EXPORTED __stdcall	USB4000_GetStoredString_stdcall(int slot, char* string);
	double EXPORTED __stdcall USB4000_GetBoardTemperature_stdcall();
	BOOL EXPORTED __stdcall USB4000_SetPowerDown_stdcall(BOOL on);
	int EXPORTED __stdcall USB4000_WriteI2C_stdcall(BYTE address, BYTE numBytes, BYTE* data);
	int EXPORTED __stdcall USB4000_ReadI2C_stdcall(BYTE address, BYTE numBytes, BYTE* data);
	BOOL EXPORTED __stdcall USB4000_WriteSPI_stdcall(int numBytes, BYTE* data);
	BOOL EXPORTED __stdcall USB4000_ReadSPI_stdcall(int numBytes, BYTE* data);
	int EXPORTED __stdcall USB4000_ReadAnalogInput_stdcall();
	double EXPORTED __stdcall USB4000_ReadAnalogInputVoltage_stdcall();
	BOOL EXPORTED __stdcall USB4000_SetAnalogOutput_stdcall(int value);
	BOOL EXPORTED __stdcall USB4000_SetAnalogOutputVoltage_stdcall(double volts);
	BOOL EXPORTED __stdcall USB4000_GetPSOCVersion_stdcall(char* ver);
	BOOL EXPORTED __stdcall USB4000_WriteFPGA_stdcall(BYTE reg, int value);
	int EXPORTED __stdcall USB4000_ReadFPGA_stdcall(BYTE reg);
	int EXPORTED __stdcall USB4000_GetMasterClockDivisor_stdcall();
	BOOL EXPORTED __stdcall USB4000_SetMasterClockDivisor_stdcall(int value);
	BOOL EXPORTED __stdcall USB4000_GetFPGAFirmwareVersion_stdcall(char* str);
	int EXPORTED __stdcall USB4000_GetContinuousStrobeBase_stdcall();
	BOOL EXPORTED __stdcall USB4000_SetContinuousStrobeBase_stdcall(int value);
	int EXPORTED __stdcall USB4000_GetContinuousStrobeClock_stdcall();
	BOOL EXPORTED __stdcall USB4000_SetContinuousStrobeClock_stdcall(int value);
	short EXPORTED __stdcall USB4000_GetIntegrationTimeBaseClock_stdcall();
	BOOL EXPORTED __stdcall USB4000_SetIntegrationTimeBaseClock_stdcall(short value);
	int EXPORTED __stdcall USB4000_GetIntegrationClockTimer_stdcall();
	BOOL EXPORTED __stdcall USB4000_SetIntegrationClockTimer_stdcall(short value);
	int EXPORTED __stdcall USB4000_GetShutterClock_stdcall();
	BOOL EXPORTED __stdcall USB4000_SetShutterClock_stdcall(int value);
	int EXPORTED __stdcall USB4000_GetHardwareTriggerDelay_stdcall();
	BOOL EXPORTED __stdcall USB4000_SetHardwareTriggerDelay_stdcall(int value);
	short EXPORTED __stdcall USB4000_GetHardwareTriggerMode_stdcall();
	BOOL EXPORTED __stdcall USB4000_SetHardwareTriggerMode_stdcall(short value);
	int EXPORTED __stdcall USB4000_GetSingleStrobeHigh_stdcall();
	BOOL EXPORTED __stdcall USB4000_SetSingleStrobeHigh_stdcall(int value);
	int EXPORTED __stdcall USB4000_GetSingleStrobeLow_stdcall();
	BOOL EXPORTED __stdcall USB4000_SetSingleStrobeLow_stdcall(int value);
	short EXPORTED __stdcall USB4000_GetStrobeEnableFPGA_stdcall();
	BOOL EXPORTED __stdcall USB4000_SetStrobeEnableFPGA_stdcall(short value);
	short EXPORTED __stdcall USB4000_GetGPIOMux_stdcall();
	BOOL EXPORTED __stdcall USB4000_SetGPIOMux_stdcall(short value);
	BOOL EXPORTED __stdcall USB4000_SetGPIOMuxReg_stdcall(short value);
	BOOL EXPORTED __stdcall USB4000_SetGPIOMuxAlt_stdcall(short value);
	BOOL EXPORTED __stdcall USB4000_GetGPIOPinMux_stdcall(short pin);
	short EXPORTED __stdcall USB4000_GetGPIOIO_stdcall();
	BOOL EXPORTED __stdcall USB4000_SetGPIOIO_stdcall(short value);
	short EXPORTED __stdcall USB4000_GetGPIOPinIO_stdcall(short pin);
	BOOL EXPORTED __stdcall USB4000_SetGPIOPinIO_stdcall(short pin, short value);
	BOOL EXPORTED __stdcall USB4000_SetGPIOPinOutput_stdcall(short pin);
	BOOL EXPORTED __stdcall USB4000_SetGPIOPinInput_stdcall(short pin);
	BOOL EXPORTED __stdcall USB4000_SetGPIODataReg_stdcall(short value);
	short EXPORTED __stdcall USB4000_GetGPIODataReg_stdcall();
	short EXPORTED __stdcall USB4000_GetGPIOPinData_stdcall(short pin);
	BOOL EXPORTED __stdcall USB4000_SetGPIOPinData_stdcall(short pin, short value);
	double EXPORTED __stdcall USB4000_GetMasterClockTime_stdcall();
	double EXPORTED __stdcall USB4000_GetIntegrationClockTime_stdcall();
	double EXPORTED __stdcall USB4000_GetContinuousStrobeBaseTime_stdcall();
	double EXPORTED __stdcall USB4000_GetContinuousStrobeTime_stdcall();
	double EXPORTED __stdcall USB4000_GetHardwareTriggerDelayTime_stdcall();
	double EXPORTED __stdcall USB4000_GetSingleStrobeHighTime_stdcall();
	double EXPORTED __stdcall USB4000_GetSingleStrobeLowTime_stdcall();
	int EXPORTED __stdcall USB4000_GetLastAutonullingStatusValue_stdcall();
	int EXPORTED __stdcall USB4000_GetLastAutonullingDarkValue_stdcall();
	double EXPORTED __stdcall USB4000_GetAutonullingSaturationValue_stdcall();
	BOOL EXPORTED __stdcall USB4000_ISSUV_LampEnable_stdcall(short ena);

	BOOL	EXPORTED __stdcall	USB2000Plus_GetCoefficients_stdcall(USB2000PLUSCONSTANTS* con);
	BOOL	EXPORTED __stdcall	USB2000Plus_SetCoefficients_stdcall(USB2000PLUSCONSTANTS* con);
	BOOL	EXPORTED __stdcall	USB2000Plus_IdentifyUSB2000Plus_stdcall(int DevNum, char* SerialNumber);
	void	EXPORTED __stdcall	USB2000Plus_GetFirmwareVersion_stdcall(char* ver);
	BOOL	EXPORTED __stdcall	USB2000Plus_SetStoredString_stdcall(int slot, char* string);
	BOOL	EXPORTED __stdcall	USB2000Plus_GetStoredString_stdcall(int slot, char* string);
	double EXPORTED __stdcall USB2000Plus_GetBoardTemperature_stdcall();
	double EXPORTED __stdcall USB2000Plus_GetBoardTemperature_stdcall();
	BOOL EXPORTED __stdcall USB2000Plus_SetPowerDown_stdcall(BOOL on);
	int EXPORTED __stdcall USB2000Plus_WriteI2C_stdcall(BYTE address, BYTE numBytes, BYTE* data);
	int EXPORTED __stdcall USB2000Plus_ReadI2C_stdcall(BYTE address, BYTE numBytes, BYTE* data);
	BOOL EXPORTED __stdcall USB2000Plus_WriteSPI_stdcall(int numBytes, BYTE* data);
	BOOL EXPORTED __stdcall USB2000Plus_ReadSPI_stdcall(int numBytes, BYTE* data);
	int EXPORTED __stdcall USB2000Plus_ReadAnalogInput_stdcall();
	double EXPORTED __stdcall USB2000Plus_ReadAnalogInputVoltage_stdcall();
	BOOL EXPORTED __stdcall USB2000Plus_SetAnalogOutput_stdcall(int value);
	BOOL EXPORTED __stdcall USB2000Plus_SetAnalogOutputVoltage_stdcall(double volts);
	BOOL EXPORTED __stdcall USB2000Plus_GetPSOCVersion_stdcall(char* ver);
	BOOL EXPORTED __stdcall USB2000Plus_WriteFPGA_stdcall(BYTE reg, int value);
	int EXPORTED __stdcall USB2000Plus_ReadFPGA_stdcall(BYTE reg);
	int EXPORTED __stdcall USB2000Plus_GetMasterClockDivisor_stdcall();
	BOOL EXPORTED __stdcall USB2000Plus_SetMasterClockDivisor_stdcall(int value);
	BOOL EXPORTED __stdcall USB2000Plus_GetFPGAFirmwareVersion_stdcall(char* str);
	int EXPORTED __stdcall USB2000Plus_GetContinuousStrobeBase_stdcall();
	BOOL EXPORTED __stdcall USB2000Plus_SetContinuousStrobeBase_stdcall(int value);
	int EXPORTED __stdcall USB2000Plus_GetContinuousStrobeClock_stdcall();
	BOOL EXPORTED __stdcall USB2000Plus_SetContinuousStrobeClock_stdcall(int value);
	short EXPORTED __stdcall USB2000Plus_GetIntegrationTimeBaseClock_stdcall();
	BOOL EXPORTED __stdcall USB2000Plus_SetIntegrationTimeBaseClock_stdcall(short value);
	int EXPORTED __stdcall USB2000Plus_GetIntegrationClockTimer_stdcall();
	BOOL EXPORTED __stdcall USB2000Plus_SetIntegrationClockTimer_stdcall(short value);
	int EXPORTED __stdcall USB2000Plus_GetShutterClock_stdcall();
	BOOL EXPORTED __stdcall USB2000Plus_SetShutterClock_stdcall(int value);
	int EXPORTED __stdcall USB2000Plus_GetHardwareTriggerDelay_stdcall();
	BOOL EXPORTED __stdcall USB2000Plus_SetHardwareTriggerDelay_stdcall(int value);
	short EXPORTED __stdcall USB2000Plus_GetHardwareTriggerMode_stdcall();
	BOOL EXPORTED __stdcall USB2000Plus_SetHardwareTriggerMode_stdcall(short value);
	int EXPORTED __stdcall USB2000Plus_GetSingleStrobeHigh_stdcall();
	BOOL EXPORTED __stdcall USB2000Plus_SetSingleStrobeHigh_stdcall(int value);
	int EXPORTED __stdcall USB2000Plus_GetSingleStrobeLow_stdcall();
	BOOL EXPORTED __stdcall USB2000Plus_SetSingleStrobeLow_stdcall(int value);
	short EXPORTED __stdcall USB2000Plus_GetStrobeEnableFPGA_stdcall();
	BOOL EXPORTED __stdcall USB2000Plus_SetStrobeEnableFPGA_stdcall(short value);
	short EXPORTED __stdcall USB2000Plus_GetGPIOMux_stdcall();
	BOOL EXPORTED __stdcall USB2000Plus_SetGPIOMux_stdcall(short value);
	BOOL EXPORTED __stdcall USB2000Plus_SetGPIOMuxReg_stdcall(short value);
	BOOL EXPORTED __stdcall USB2000Plus_SetGPIOMuxAlt_stdcall(short value);
	BOOL EXPORTED __stdcall USB2000Plus_GetGPIOPinMux_stdcall(short pin);
	short EXPORTED __stdcall USB2000Plus_GetGPIOIO_stdcall();
	BOOL EXPORTED __stdcall USB2000Plus_SetGPIOIO_stdcall(short value);
	short EXPORTED __stdcall USB2000Plus_GetGPIOPinIO_stdcall(short pin);
	BOOL EXPORTED __stdcall USB2000Plus_SetGPIOPinIO_stdcall(short pin, short value);
	BOOL EXPORTED __stdcall USB2000Plus_SetGPIOPinOutput_stdcall(short pin);
	BOOL EXPORTED __stdcall USB2000Plus_SetGPIOPinInput_stdcall(short pin);
	BOOL EXPORTED __stdcall USB2000Plus_SetGPIODataReg_stdcall(short value);
	short EXPORTED __stdcall USB2000Plus_GetGPIODataReg_stdcall();
	short EXPORTED __stdcall USB2000Plus_GetGPIOPinData_stdcall(short pin);
	BOOL EXPORTED __stdcall USB2000Plus_SetGPIOPinData_stdcall(short pin, short value);
	double EXPORTED __stdcall USB2000Plus_GetMasterClockTime_stdcall();
	double EXPORTED __stdcall USB2000Plus_GetIntegrationClockTime_stdcall();
	double EXPORTED __stdcall USB2000Plus_GetContinuousStrobeBaseTime_stdcall();
	double EXPORTED __stdcall USB2000Plus_GetContinuousStrobeTime_stdcall();
	double EXPORTED __stdcall USB2000Plus_GetHardwareTriggerDelayTime_stdcall();
	double EXPORTED __stdcall USB2000Plus_GetSingleStrobeHighTime_stdcall();
	double EXPORTED __stdcall USB2000Plus_GetSingleStrobeLowTime_stdcall();
	int EXPORTED __stdcall USB2000Plus_GetLastAutonullingStatusValue_stdcall();
	int EXPORTED __stdcall USB2000Plus_GetLastAutonullingDarkValue_stdcall();
	double EXPORTED __stdcall USB2000Plus_GetAutonullingSaturationValue_stdcall();
	void EXPORTED __stdcall OOI_SetAutonullingEnabled_stdcall(BOOL ena);
	BOOL EXPORTED __stdcall OOI_GetAutonullingEnabled_stdcall();

	///////////////////////////////////////////////////////////////////////////////////
	//// LS450 Calls
	///////////////////////////////////////////////////////////////////////////////////

	float EXPORTED __stdcall OOI_GetExternalTemperatureCelsius_stdcall();
	void EXPORTED __stdcall OOI_GetTemperatureCoefficients_stdcall(float* first, float* second);
	BOOL EXPORTED __stdcall OOI_SetTemperatureCoefficients_stdcall(float first, float second);
	BOOL EXPORTED __stdcall OOI_GetO2ProbeCalibrationCoefficients_stdcall(LPO2COEFS O2);
	BOOL EXPORTED __stdcall OOI_GetLS450Modulation_stdcall();
	BOOL EXPORTED __stdcall OOI_SetLS450Modulation_stdcall(int pulsed);
	BOOL EXPORTED __stdcall OOI_SetLS450Da_stdcall(int daValue);
	BOOL EXPORTED __stdcall OOI_SetO2ProbeCalibrationCoefficients_stdcall(LPO2COEFS O2);

#endif //WIN32
#endif //__BORLANDC__

#ifdef __cplusplus
};
#endif //__cplusplus

#endif // __OOIDRV
