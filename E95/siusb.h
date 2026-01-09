////////////////////////////////////////////////////////////////////////////////
// siusb.h
//
// Header file for Silicon Imaging camera API
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __SIUSB_H__
#define __SIUSB_H__

#define BULK_READ 0
#define BULK_WRITE 1
#define ISO_READ 2
#define ISO_WRITE 3
#define START_ISO_STREAM 4
#define STOP_ISO_STREAM  5
#define READ_BURST  6
#define READ_BUFFER 7

#define SUBSAMP_OFF  0
#define SUBSAMP_MONO_ON  1
#define SUBSAMP_COLOR_ON 2

#define SI_STATUS_SUCCESS  0
#define SI_STATUS_FAILURE  1
#define SI_STATUS_FAILURE_TIMEOUT 2
#define SI_STATUS_FAILURE_FUNCTION_NOT_SUPPORTED  3
#define SI_STATUS_GENERIC_CAMERA 4
#define SI_STATUS_INCORRECT_SIZE_RETURNED  5
#define SI_STATUS_DEVICE_ERROR 6

#define MANUAL_EXP_GAIN_CONTROL 3
#define AUTO_EXP_GAIN_CONTROL  4
#define AUTO_WHITE_BALANCE_EXP_CONTROL  5

#define TIFF_MONO_8  0
#define TIFF_MONO_12 1
#define TIFF_RGB_8  2
#define TIFF_RGB_16 3

// cursor types
#define CURSOR_SMALL_CROSSHAIR 1
#define CURSOR_LARGE_CROSSHAIR  2
#define CURSOR_THIN_WIDTH 4
#define CURSOR_THICK_WIDTH 8

// 5100 image sizes
#define QSXGA 10
#define SXGA 11
#define VGA 12
#define HF 13

#define CONTINUOUS_SHUTTER_MODE 0
#define SNAPSHOT_ROLLING_SHUTTER_MODE 1
#define SNAPSHOT_GLOBAL_RESET_MODE  2

struct CLOCKSETTING {
	int iFrequency;
	char iCommand[4];
	int iLineTime;
	int iLineRate;
	float iRateFull;
	float iRate_19x10;
	float iRate_12x10;
	float iRate_10x7;
	float iRate_6x4;
	float iRate_3x2;
};

#ifdef __cplusplus
extern "C" {
#endif

	//-------------------------USB Command Functions---------------------------------------------

	__declspec(dllimport) int SI_OpenCamera(int iCamNumber, PCHAR devname);
	__declspec(dllimport) int SI_CloseCamera(int iCamNumber);
	__declspec(dllimport) int SI_ResetPipe(int iCamNumber, int iPipe);
	__declspec(dllimport) int SI_AbortPipe(int iCamNumber, int iPipe);
	__declspec(dllimport) int SI_GetPipeInfo(int iCamNumber, unsigned char* InterfaceInfo, unsigned int* iInterfaceSize);

	//-------------------------Image Transfer Functions-------------------------------------------

	__declspec(dllimport) int SI_SnapBulkImage(int iCamNumber, int iColumns, int iRows, int iPixelDepth, unsigned char* ImageBuffer, int* iBytesRead);
	__declspec(dllimport) int SI_SnapBulkImageB(int iCamNumber, int iColumns, int iRows, int iPixelDepth, unsigned char* ImageBuffer, int* iBytesRead);
	__declspec(dllimport) int SI_SnapBulkImageC(int iCamNumber, int iColumns, int iRows, int iPixelDepth, unsigned char* ImageBuffer, int* iBytesRead);
	__declspec(dllimport) int SI_SnapBulkImage(int iCamNumber, int iColumns, int iRows, int iPixelDepth, unsigned char* ImageBuffer, int* iBytesRead);
	__declspec(dllimport) int SI_SnapBulkLiveStart(int iCamNumber, int iColumns, int iRows, int iPixelDepth, unsigned char* ImageBuffer,
		int iSequenceCount, HANDLE hEvent1, HANDLE hEvent2, HANDLE hEvent3);
	__declspec(dllimport) int SI_SnapBulkLiveStartDual(int iCamNumber, int iColumns, int iRows, int iPixelDepth, unsigned char* ImageBuffer,
		int iSequenceCount, HANDLE hEvent1, HANDLE hEvent2, HANDLE hEvent3);
	__declspec(dllimport) int SI_SnapBulkLiveStop(int iCamNumber);
	__declspec(dllimport) int SI_SnapBulkLiveWait(int iCamNumber, int iBytesRead);
	__declspec(dllimport) void SI_SetImageBuffers(int iCamNumber, unsigned char* pBuffer1, unsigned char* pBuffer2);
	__declspec(dllimport) int SI_SnapBulkDoubleBuffer(int iCamNumber, int iColumns, int iRows, int iPixelDepth, int iCaptureBuffer, HANDLE hEvent, int* iBytesRead);
	__declspec(dllimport) int SI_SaveBinFileB(unsigned char* pImg, int iSize, char* lpszFileName);
	__declspec(dllimport) int SI_SaveTiffFile(unsigned char* pImg, int iColumns, int iRows, int iImageMode, char* lpszFileName);
	__declspec(dllimport) int SI_SaveDngFile(unsigned char* pImg, int iColumns, int iRows, int iBitDepth, BOOL iColor, char* lpszFileName);
	__declspec(dllimport) void SI_WriteCursor(unsigned char* ImageBuffer, int iPixelDepth, int iWidth, int iHeight, int iCursorType,
		int iXPosition, int iYPosition, int iRedLevel, int iGreenLevel, int iBlueLevel);
	__declspec(dllexport) int SI_GetUSBTransferData(int iCamNumber, LPVOID lpBuffer, ULONG iSize);

	//-------------------------Camera Control Functions-------------------------------------------

	__declspec(dllimport) int SI_TransferControlData(int iCamNumber, int iVendorRequestCode, int iIndex, int iValue,
		int iDirection, int iBufferSize, unsigned char* buffer);
	__declspec(dllimport) int SI_InitWindowMode(int iCamNumber, int iStartRow, int iEndRow, int iStartColumn, int iEndColumn,
		int iFrameTime, int iExposureTime);
	__declspec(dllimport) int SI_SetRowStartEnd(int iCamNumber, int iStartRow, int iEndRow);
	__declspec(dllimport) int SI_SetColStartEnd(int iCamNumber, int iStartColumn, int iEndColumn);
	__declspec(dllimport) int SI_SetExposure(int iCamNumber, int iExposure, int iRange, int iCount);
	__declspec(dllimport) int SI_SetAutoExposure(int iCamNumber, int iWidth, int iHeight, unsigned char* buffer);
	__declspec(dllimport) int SI_SetExposureValue(int iCamNumber, int iExposure);
	__declspec(dllimport) int SI_SetAECBrightnessTargets(int iCamNumber, int iLowTarget, int iHighTarget);
	__declspec(dllimport) int SI_SetBlackLevels(int iCamNumber, int iRedLevel, int iGreenBLevel, int iGreenRLevel, int iBlueLevel);
	__declspec(dllimport) int SI_GetRGBAverages(int iCamNumber, int* iBAverage, int* iGBAverage,
		int* iGRAverage, int* iRAverage);
	__declspec(dllimport) int SI_SetAutoModes(int iCamNumber, int iMode);
	__declspec(dllimport) int SI_SetMaxAGCGain(int iCamNumber, int iMaxAGCGain);
	__declspec(dllimport) int SI_SetAWBThreshold(int iCamNumber,
		int iWhiteBalanceThresh, int iWhiteBalanceSpeed);
	__declspec(dllimport) int SI_SetAECThreshold(int iCamNumber, int iLowThreshold, int iHighThreshold);
	__declspec(dllimport) int SI_SetVerticalBlanking(int iCamNumber, int iVertBlanking);
	__declspec(dllimport) int SI_SetHorizontalBlanking(int iCamNumber, int iHorizBlanking);
	__declspec(dllimport) int SI_SetShutterDelay(int iCamNumber, int iShutterDelay);
	__declspec(dllimport) int SI_SetFrameTime(int iCamNumber, int iFrameTime);
	__declspec(dllimport) int SI_SetWindowPositionExposure(int iCamNumber, int iStartRow, int iStartColumn, int iExposureTime);
	__declspec(dllimport) int SI_SetWindowPosition(int iCamNumber, int iStartRow, int iStartColumn);
	__declspec(dllimport) int SI_ReadStatusClock(int iCamNumber, int* iStatus, int* iClock);
	__declspec(dllimport) int SI_SetClock(int iCamNumber, int iClockMode, CLOCKSETTING* ClockSettings);
	__declspec(dllimport) int SI_ReadClockFile(char* cCamClockFile, int* iClockTableSize,
		CLOCKSETTING* ClockSettings);
	__declspec(dllimport) int SI_SetClockB(int iCamNumber, int iClockIndex);
	__declspec(dllimport) int SI_ReadVersion(int iCamNumber, char* cCamVersion);
	__declspec(dllimport) int SI_ReadSerialNumber(int iCamNumber, char* cSerialNumber);
	__declspec(dllimport) int SI_SetGlobalGain(int iCamNumber, int iGain);
	__declspec(dllimport) int SI_CreateGainTable(int iCamNumber);
	__declspec(dllimport) int SI_GetGainString(int iCamNumber, int iGainIndex, char** cGain);
	__declspec(dllimport) int SI_SetGlobalGainB(int iCamNumber, int iGainIndex);
	__declspec(dllimport) int SI_SetRGBGain(int iCamNumber, int iRedGain, int iGreenGain, int iBlueGain);
	__declspec(dllimport) int SI_SetRGBGainB(int iCamNumber, int iRedGainIndex, int iGreenGainIndex, int iBlueGainIndex);
	__declspec(dllimport) int SI_SetRGGBGain(int iCamNumber, int iRedGain, int iGreenGain1, int iGreenGain2, int iBlueGain);
	__declspec(dllimport) int SI_SetRGGBGainB(int iCamNumber, int iRedGainIndex, int iGreenGain1Index, int iGreenGain2Index, int iBlueGainIndex);
	__declspec(dllimport) int SI_SetGlobalShutterMode(int iCamNumber, int iShutterMode);
	__declspec(dllimport) int SI_ArmTriggerWait(int iCamNumber);
	__declspec(dllimport) int SI_DisarmTrigger(int iCamNumber);
	__declspec(dllimport) int SI_SetJamSync(int iCamNumber, bool bEnableJamSync);
	__declspec(dllimport) int SI_TransferFirmware(int iCamNumber, char* cFirmwareFile, int iDirection);
	__declspec(dllimport) int SI_SetSubsampleMode(int iCamNumber, int iSubSampMode);
	__declspec(dllimport) int SI_SetPixelDepth(int iCamNumber, int iPixelDepth);
	__declspec(dllimport) int SI_SetADC(int iCamNumber, int iADC);
	__declspec(dllimport) int SI_SetColumnOffset(int iCamNumber, int iColOffset);
	__declspec(dllimport) int SI_SetStartColumn(int iCamNumber, int iStartColumn);
	__declspec(dllimport) int SI_SetStartRow(int iCamNumber, int iStartRow);
	__declspec(dllimport) int SI_SetBlackOffset(int iCamNumber, int iBlackOffset);
	__declspec(dllimport) int SI_SetGammaMode(int iCamNumber, bool bGammaMode);
	__declspec(dllimport) int SI_TransferDefBalanceOffset(int iCamNumber, unsigned char* cBalOffsetArray,
		int iDirection);
	__declspec(dllimport) int SI_SetDefBalanceOffset(int iCamNumber);
	__declspec(dllimport) int SI_SetGPIO(int iCamNumber, int iMode, int iIONumber);
	__declspec(dllimport) int SI_SetAsyncControlMode(int iCamNumber, BOOL bControlMode);
	__declspec(dllimport) int SI_SetDriverDelay(int iCamNumber, int iDriverDelay);
	__declspec(dllimport) int SI_SetZoomMode(int iCamNumber, int iRowOrigin, int iColumnOrigin,
		int iSubSampMode);
	__declspec(dllimport)  void SI_CreateCamToRGBConversionMatrix(double cam_xyz[3][3], double cam_rgb[3][3]);
	__declspec(dllexport)  void SI_ConvertRGBMatrixToXYZ(double rgb_cam[3][3], double cam_xyz[3][3]);
	__declspec(dllimport) void initsiusb();

#ifdef __cplusplus
}
#endif

// full frame commands
#define FULL_FRAME_LIVE_MODE         0x01
#define FULL_FRAME_SINGLE_SHOT_MODE  0x03

// window commands
#define WINDOW_LIVE_MODE
#define WINDOW_SINGLE_SHOT_MODE
#define WINDOW_SETUP_MODE

// long integration commands
#define FULL_FRAME_INTEGRATION_BEGIN   0x0b
#define FULL_FRAME_INTEGRATION_READOUT 0x03
#define WINDOW_INTEGRATION_BEGIN   0x9b
#define WINDOW_INTEGRATION_READOUT 0x13

#define CAM_PRESET_1		1
#define CAM_PRESET_2		2
#define CAM_PRESET_3		3
#define CAPTURE_MODE_LIVE	6
#define CAPTURE_MODE_WINDOW	7
#define SENSOR_REGISTER_COUNT     64

#define WINDOW_MODE			0x90
#define INTEGRATION_MODE    0x04
#define SINGLE_SHOT_MODE    0x02
#define RUN_MODE            0x01
#define PROGRAM_MODE        0x00

#define PIPE_OUT_SENSOR_COMMANDS 2
#define PIPE_IN_SENSOR_COMMANDS  3
#define PIPE_OUT_BULK_VIDEO_DATA  1
#define PIPE_IN_BULK_VIDEO_DATA  0

#define MAX_BULK_CAM_TRANSFER  512

#define GPIO_1 1
#define GPIO_2 2
#define GPIO_OFF  0
#define GPIO_ON  1

#endif