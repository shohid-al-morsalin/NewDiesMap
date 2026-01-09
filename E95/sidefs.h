#ifndef _SIDEFS_H_
#define _SIDEFS_H_

#pragma once

/*===========================================================================

			 SI SDK COMMON DATA STRUCTURES

DESCRIPTION
  This file contains the data structures for Silicon Imaging USB camera SDK.

REFERENCES
  USB MegaCamera API Manual

Copyright (c) 2003 by Silicon Imaging, Incorporated. All Rights Reserved.
===========================================================================*/

#if defined( _WIN32 ) || defined ( _WIN64 )
#define __STDCALL  __stdcall
#define __CDECL    __cdecl
#define __INT64    __int64
#define __UINT64    unsigned __int64
#else
#define __STDCALL
#define __CDECL
#define __INT64    long long
#define __UINT64    unsigned long long
#endif

#define SI_MAX_PATH						1024
#define SI_MAX_INIT_VR				32
#define SI_MAX_IPE_PROFILE		16

#ifdef SIDLL_EXPORTS
#define SIAPI __declspec(dllexport)
#else
#define SIAPI __declspec(dllimport)
#endif

typedef unsigned char   SI8u;
typedef signed char			SI8s;
typedef unsigned short  SI16u;
typedef signed short		SI16s;
typedef unsigned int    SI32u;
typedef signed int			SI32s;
typedef float						SI32f;
typedef __INT64					SI64s;
typedef __UINT64				SI64u;
typedef double					SI64f;
typedef void* SIHandle;

enum SIResult {
	SR_OK = 0,			// No error
	SR_Error = -1,			// Generic, unspecifed error
	SR_FatalError = -2,			// Unrecoverable error
	SR_NotSupported = -3,
	SR_Abort = 1,
	SR_ImageDataOrderErr = -20,
	SR_ImageDataDepthErr = -21,
	SR_ImageChannelNumberErr = -22,
	SR_ImageBufferErr = -23,
	SR_ImageFormatErr = -24,
	SR_ImageBppErr = -25,
	SR_InputImageFormatErr = -26,
	SR_OutputImageFormatErr = -27,
	SR_FileIOErr = -51,
	SR_ReadConfigFileErr = -52,
	SR_LUTAndDataMismatch = -53,

	SR_CameraIDErr = -500,
	SR_CameraOpenErr = -501,
	SR_CameraConfigErr = -502,
	SR_CameraChangeBppErr = -503,
	SR_CameraInitErr = -504,
	SR_CameraModelErr = -505,
	SR_CameraSnapFailure = -506,
	SR_CameraSnapDataErr = -507,
	SR_CameraSnapTimeout = -508,
	SR_CameraDeviceError = -509,

	SR_CameraNotOpen = -600,
	SR_SIUInvalidShutterMode = -601,

	SR_CameraInUse = 500,
};

typedef struct tagSIPoint {
	int iX;
	int iY;
}SIPoint;

typedef struct tagSIRegion {
	int iX, iY, iWidth, iHeight;
}SIRegion;

typedef struct tagSISize {
	int iWidth, iHeight;
}SISize;

enum SIColorSpace {
	SICS_Mono = 0,
	SICS_RGB = 1,
	SICS_CbYCr422 = 2,
};

enum SIPixelFormat {
	SIPF_GRAYSCALE = 0,
	SIPF_BAYER = 1,
	SIPF_RGB = 2,
	SIPF_RGBA = 3,
};

enum SIPixelPacking {
	SIPP_Interleaved,
	SIPP_Planar,
	SIPP_PseudoPlanar,
};

enum SIPixelAlign {
	SIPA_LOOSE,
	SIPA_PTIGHT,
	SIPA_CTIGHT,
};

enum SIColorMode {
	SICM_Auto = 0,
	SICM_Mono = 1,
	SICM_Color = 2,
};

enum SIWhiteBalanceMode {
	SIWB_Off = 0,
	SIWB_Auto = 1,
	SIWB_Manual = 2,
};

enum SIAutoWhiteBalanceAlgorithm {
	SIAWB_GrayWorld = 0,
	SIAWB_WhitePatch = 1,
};

enum SIDeviantPixelDetectAlgorithm {
	SIDPD_MonoImageBasic = 0,
	SIDPD_ColorImageBasic = 1,
	SIDPD_GrayImageBasic = 2,
};

enum SICFAPattern {
	SICFAPattern_BGGR = 0,
	SICFAPattern_GBRG = 1,
	SICFAPattern_RGGB = 2,
	SICFAPattern_GRBG = 3
};

enum SICFAInterpolation {
	SICFAInterpNNP = 0,
	SICFAInterpBilinear = 1,
	SICFAInterpEdgeDirected = 2,
	SICFAInterpFast = 0,
	SICFAInterpGood = 1,
	SICFAInterpBetter = 2,
	SICFAInterpBest = 3,
};

enum SIBitReductionAlgorithm {
	SIBRALGO_Truncate = 0,
};

enum SICameraModel {
	// Note: use odd number for mono and even number for color.
	SIGenericCam = 0,
	SI1280M = 1,
	SI1280C = 2,
	SI3170M = 3,
	SI3170C = 4,
	SI2100M = 5,
	SI2100C = 6,
	SI640M = 7,
	SI640C = 8,
	SI1300M = 9,
	SI1300C = 10,
	SI3300M = 11,
	SI3300C = 12,
	SI6600M = 13,
	SI6600C = 14,
	SI5100M = 15,
	SI5100C = 16
};

typedef enum tagSIGainSelection {
	SISelectIndividualGains = 0,
	SISelectGlobalGain = 1
}SIGainSelection;

typedef enum tagSICAMIF_SizingMode {
	SICAMIF_SM_Unknown = -1,
	SICAMIF_SM_Windowing = 0,
	SICAMIF_SM_Subsampling = 1,
	SICAMIF_SM_Zooming = 2,
}SICAMIF_SizingMode;

typedef enum tagSILUTType {
	SILUT_Custom = 0,
	SILUT_ITURec709 = 1,
	SILUT_SI1920HighLight4x = 2,
	SILUT_SI1920HighLight7x = 3,
}SILUTType;

/*
 * SIU_ImagerFormat
 *	Defines camera sensor read-out interface format.
 */
typedef struct tagSIU_ImagerFormat {
	int iWidth;
	int iHeight;
	int iBpp;
	int iSubSampleMode;
	SICAMIF_SizingMode SizingMode;
	SIPoint Origin;
	SIColorSpace ColorSpace;
}SIU_ImagerFormat;

typedef enum tagSIU_SnapMode {
	SIUSM_SnapLoop = 0,
	SIUSM_LiveDriver = 1,
}SIU_SnapMode;

/*
 * SIU_CameraConfig
 *	Defines SI USB MegaCamera camera configuration.
 */
#define CAMCFG_CLOCK				0x00000001
#define CAMCFG_GLOBALGAIN		0x00000002
#define CAMCFG_RGGBGAIN			0x00000004

typedef struct tagSIU_CameraConfig {
	SI32u iChangeFlag;

	SIU_ImagerFormat ImagerFormat;									// Camera interface format
	int		iClockIndex;														// Clock table index
	int		iGlobalGain;                            // Global gain table index
	int		iRGGBGain[4];                           // RGGB gain table indices
	SIGainSelection		GainSelection;							// effective only for cameras with mutually exclusive gain settings
	int		iBlackOffset;
	int		iTBRange;																// (1280 only)
	int		iExposure;
	int		iFrameTime;
	int		iColumnOffset;
	int		iVerticalBlanking;
	int		iHorizontalBlanking;
	int		iAutoMode;                       // SI2100 and SI5100
	int		iAECBrightnessTarget;				// 2100 only
	int		iAECBrightnessBracket;			// 2100 only
	int		iFastAECThreshold[2];        // SI5100 only
	int		iArmTrigger;
	int		iLiveModeDriverDelay;
	int		iShutterMode;
	int		iGammaMode;
}SIU_CameraConfig;

/*
 * SIU_ImagingProfile
 *	Defines video front end (VFE) imaging profiles.
 */
typedef struct tagSIU_ImagingProfile {
	char	szDescription[64];
	SI32f ColorCorrectionMatrix[3][3];
	float fWhiteBalanceGain[4];
	float fGlobalGain;
	float fSaturationFactor;
	SI32s	iBlackLevel;
	SI32s iBrightness;
	SI32s iContrast;
	SIAutoWhiteBalanceAlgorithm	WhiteBalanceAlgo;
	SILUTType LUTType;
}SIU_ImagingProfile;

/*
 * SIU_ImagingConfig
 *	Defines video front end (VFE) imaging context
 */
typedef struct tagSIU_ImagingConfig {
	int		iDPCEna;
	int		iMaxDPNum;
	int		iDeviantPixelDetectStrength;
	SIDeviantPixelDetectAlgorithm DPDAlgo;
	int		iBlackCorrection;
	int		iEnableColorCorrection;
	int		iEnableAdjustment;
	int		iEnableGammaCorrection;
	int   iLUTMode;
	SIWhiteBalanceMode WhiteBalanceMode;
	SICFAPattern								CFAPattern;
	SICFAInterpolation					CFAInterp;
}SIU_ImagingConfig;

typedef struct tagSIU_VendorRequest {
	int iVRCode;
	int iIndex;
	int iValue;
	int iDirection;
	int iBufferSize;
	unsigned char Buffer[64];
}SIU_VendorRequest;

typedef struct tagSIU_RunStatus {
	SIColorMode ColorMode;
	SIU_SnapMode SnapMode;
	int iSnapDelay;
	int iVerbose;
}SIU_RunStatus;

typedef struct tagSIUCConfig {
	SIU_CameraConfig		CameraConfig;
	SIU_ImagingConfig		ImagingConfig;
	int iActiveImagingProfile;
	int iInitVRUsed;
	int iTotalImagingProfile;
	SIU_RunStatus				RunStatus;
	SIU_ImagingProfile	ImagingProfile[SI_MAX_IPE_PROFILE];
	SIU_VendorRequest		InitVR[SI_MAX_INIT_VR];
}SIU_Config;

typedef struct tagSIBayerImageInfo {
	SICFAPattern CFAPattern;
}SIBayerImageInfo;

typedef struct tagSIColorImageInfo {
	SIPixelPacking PixelPacking;
}SIColorImageInfo;

typedef struct _SIImage {
	SI32u iWidth;
	SI32u iHeight;
	SI32u iPitch;
	SI32u iBitsPerPixel[4];
	SIPixelFormat PixelFormat;
	SIPixelAlign PixelAlign;
	union {
		SI8u* pData8u;
		SI16u* pData16u;
		SI16s* pData16s;
		SI8u* pPlane8u[3];
		SI16u* pPlane16u[3];
		SI16s* pPlane16s[3];
	}Data;

	union {
		SIBayerImageInfo BayerImageInfo;
		SIColorImageInfo ColorImageInfo;
	}AdvancedPixelFormatInfo;
}SIImage;

typedef struct _SI_CFAInterpolationAdvancedConfig {}SI_CFAInterpolationAdvancedConfig;

typedef struct _SI_CFAInterpolationConfig {
	SI32u								Enable;
	SICFAPattern				CFAPattern;
	SICFAInterpolation	CFAInterp;
	SI32u								UseAdvancedConfig;
	SI_CFAInterpolationAdvancedConfig AdvancedConfig;
}SI_CFAInterpolationConfig;

typedef struct _SI_WhiteBalanceConfig {
	SI32u												Enable;
	SIWhiteBalanceMode					WhiteBalanceMode;
	SIAutoWhiteBalanceAlgorithm	WhiteBalanceAlgo;
	SI32f												WhiteBalanceGain[4];
}SI_WhiteBalanceConfig;

typedef struct _SI_ColorCorrectionConfig {
	SI32u												Enable;
	SI32f												ColorCorrectionMatrix[3][3];
}SI_ColorCorrectionConfig;

typedef struct _SI_IPEInput {
	SISize	ImageSize;
	SI32u		Bpp;
	SIColorSpace ColorSpace;
}SI_IPEInput;

typedef struct _SIIPEConfig {
	SIHandle									IPEHandle;
	SIImage										InputImage;
	SI_CFAInterpolationConfig CFAInterpConfig;
	SI_WhiteBalanceConfig			WBConfig;
}SIIPEConfig;

typedef struct tagSIMonoImage {
	int iWidth;
	int iHeight;
	int iPitch;
	int iBpp;
	union {
		SI8u* pData8u;
		SI16u* pData16u;
		SI16s* pData16s;
	}Data;
}SIMonoImage;

typedef enum {
	SICO_Pixel = 0,
	SICO_Planar = 1,
}SIColorOrder;

typedef struct tagSIColorImage {
	int iWidth;
	int iHeight;
	int iPitch;
	int iBpp;
	union {
		SI8u* pData8u;
		SI16u* pData16u;
		SI16s* pData16s;
		SI8u* pPlane8u[3];
		SI16u* pPlane16u[3];
		SI16s* pPlane16s[3];
	}Data;
	SIColorOrder ColorOrder;
}SIColorImage;

typedef struct tagSIPackedDIB {
	BITMAPINFOHEADER	BmpInfo;
	RGBQUAD				ColorTable[256];
	PBYTE				pDIBBits;
}SIPackedDIB;

typedef struct tagSI_ImagingPipeline {
	SIU_ImagerFormat	ImagerFormat;
	SIU_RunStatus		RunStatus;
	SIU_ImagingProfile	ImagingProfile;
	SIU_ImagingConfig	ImagingConfig;

	SIMonoImage				BayerImage;
	SIColorImage			RGBImageP3;
	SIColorImage			RGBImageC3;
	SIPackedDIB				PackedDIB;
}SI_ImagingPipeline;

typedef SIResult(*SI_FUNC_MONO_IMAGE)(SIMonoImage* pImg);
typedef SIResult(*SI_FUNC_DISPLAY_PACKEDDIB)(const SIPackedDIB* const pImg);

#endif