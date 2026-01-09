/*! \file siipl.h
		\brief Silicon Imaing (c) Image Processing Library header file.
*/
#ifndef __SIIPL_H__
#define __SIIPL_H__

#pragma once

#include <sidefs.h>

#undef SIAPI
#if defined (SIIPL_EXPORTS)
#define SIAPI __declspec(dllexport)
#elif defined (SIIPL_SLIB)
#define SIAPI extern
#else
#define SIAPI __declspec(dllimport)
#endif

typedef struct tagSILUTInfo {
	SILUTType Type;
	int iInputBpp;
	int iOutputBpp;
	int iChannel;
	SI32f fPower;
	union {
		struct {
			SI32s iWhite;
			SI32s iBlack;
			SI32f fCutoff;
			SI32f fSlope;
			SI32f fFudge;
			SI32f fGamma;
		}SIHighLight;
	}LUTParam;

	union {
		SI16u* Lut16;
		SI8u* Lut8;
	}Lut[4];
}SILUTInfo;

#ifdef __cplusplus
extern "C" {
#endif
	/*! \fn SIResult SI_AllocMonoImage(SIMonoImage * pImg);
			\brief Allocates aligned memory for a mono image.
	*/
	SIAPI SIResult			SI_AllocMonoImage(SIMonoImage* pImg);
	SIAPI	void					SI_FreeMonoImage(SIMonoImage* pImg);
	SIAPI SIResult			SI_AllocColorImage(SIColorImage* pImg);
	SIAPI void					SI_FreeColorImage(SIColorImage* pImg);
	SIAPI BITMAPINFO* SI_AllocDIB(int iWidth, int iHeight, int iBpp);
	SIAPI void					SI_FreeDIB(BITMAPINFO* pBmp);
	SIAPI SIResult			SI_CopyMonoImage(const SIMonoImage* pSrc, SIMonoImage* pDst);

	SIAPI SIResult	SI_YCrCb422ToRGB(SIMonoImage* pSrc, SIColorImage* pDst);
	SIAPI SIResult	SI_BayerToRGBP3(SIMonoImage* pSrc, SIColorImage* pDst, SICFAPattern CFAPat, SICFAInterpolation CFAInterp);
	SIAPI SIResult	SI_ConvertRGBP3ToC3(const SIColorImage* pSrc, SIColorImage* pDst);
	SIAPI SIResult	SI_ConvertRGBToDIB(SIColorImage* pClrImg, BITMAPINFO* pBmp);

	SIAPI void			SI_BuildColorMatrix(SI32f SatFactor, SI32f ClrMat[3][3]);
	SIAPI SIResult	SI_CalculateAutoWhiteBalance(const SIColorImage* pSrc, float fWBCoef[4], SIAutoWhiteBalanceAlgorithm awb);
	SIAPI SIResult	SI_SetGain_Color(SIColorImage* pSrc, SIColorImage* pDst, SI32f fGains[4]);
	SIAPI SIResult	SI_SetGain_Mono(SIMonoImage* pSrc, SIMonoImage* pDst, SI32f fGain);
	SIAPI SIResult	SI_ColorProcessing(SIColorImage* pSrc, SIColorImage* pDst, const float fSatMat[3][3], const float fRGBGain[3],
		float fGlobalGain, int iGlobalOffset, float fWBCoef[3], SIAutoWhiteBalanceAlgorithm WBAlgo);
	SIAPI SIResult	SI_ApplyColorCorrectionMatrix(SIColorImage* pSrc, SIColorImage* pDst, const float M[3][3]);
	SIAPI SIResult	SI_BrightnessAndContrast_Color(SIColorImage* pSrc, SIColorImage* pDst, int iBrightness, int iContrast);
	SIAPI SIResult	SI_BrightnessAndContrast_Mono(SIMonoImage* pSrc, SIMonoImage* pDst, int iBrightness, int iContrast);
	SIAPI SIResult	SI_SetBlackLevel_Mono(SIMonoImage* pSrc, SIMonoImage* pDst, SI32s iBlackLevel);
	SIAPI SIResult	SI_SetBlackLevel_Color(SIColorImage* pSrc, SIColorImage* pDst, SI32s iBlackLevel[3]);

	SIAPI SIResult	SI_RGBGainAndOffset(SIColorImage* pSrc, SIColorImage* pDst, SI32f fGain[3], SI32s iOffset[3]);
	SIAPI void			SI_OffsetMonoImage(const SIMonoImage* pSrc, SIMonoImage* pDst, int iOffset);
	SIAPI SIResult	SI_SubtractBlackReference(SIMonoImage* pImg, SIMonoImage* pBlackRef);
	SIAPI SIResult	SI_SwapBR(SIColorImage* pSrc, SIColorImage* pDst);
	SIAPI SIResult	SI_DeviantPixelDetection(const SIMonoImage* pBayer, SICFAPattern CFAPat,
		SIDeviantPixelDetectAlgorithm DPDAlgo, SI32u iStrength,
		SI32u iMaxDPCount, SIPoint* pDPList, SI32u* iDPCount);

	SIAPI SIResult	SI_DeviantPixelCorrection(SIMonoImage* pBayer, int iDPNum, SIPoint* pDPList);
	SIAPI SIResult	SI_BitReduction_Mono(SIMonoImage* pSrc, SIMonoImage* pDst, SIBitReductionAlgorithm bralgo);
	SIAPI SIResult	SI_BitReduction_Color(SIColorImage* pSrc, SIColorImage* pDst, SIBitReductionAlgorithm bralgo);

	SIAPI SIResult	SI_LoadBinFile(const char* lpszFileName, SIMonoImage* pImg);
	SIAPI SIResult  SI_SaveBinFile(const SIMonoImage* pImg, const char* lpszFileName);
	SIAPI SIResult	SI_SavePackedDIBAsBmp(const SIPackedDIB* pPackedDIB, const char* lpszFileName);
	SIAPI SIResult	SI_SaveAsBmp_Color(const SIColorImage* pImg, const char* lpszFileName);
	SIAPI SIResult	SI_SaveAsBmp_Mono(const SIMonoImage* pImg, const char* lpszFileName);
	SIAPI SIResult SI_SaveBayerToDNG(const char* szFileName, const SIMonoImage* pImg, SICFAPattern CFAPat,
		const SIU_CameraConfig* pCfg, const SIU_ImagingProfile* pProf);
	SIAPI SIResult	SI_SaveAsTIFF(const char* szFileName, const SIImage* pImg);
	SIAPI SIResult	SI_LoadDNGToBayer(const char* szFileName, SIMonoImage* pImg, SIU_ImagingConfig* pCfg);

	SIAPI SIResult	SI_InitImagingPipeline(SI_ImagingPipeline* pSIIP);
	SIAPI SIResult	SI_ExitImagingPipeline(SI_ImagingPipeline* pSIIP);
	SIAPI SIResult	SI_RunImagingPipeline(SI8u* pBayer, SI_ImagingPipeline* pSIIP);

	SIAPI SIResult SI_CreateLUT(SILUTInfo* pLutInfo);
	SIAPI SIResult SI_ReleaseLUT(SILUTInfo* pLutInfo);
	SIAPI SIResult SI_ApplyLUTColor(const SIColorImage* pSrc, SIColorImage* pDst, SILUTInfo* pLutInfo);
	SIAPI SIResult SI_ApplyLUTMono(const SIMonoImage* pSrc, SIMonoImage* pDst, SILUTInfo* pLutInfo);

#ifdef __cplusplus
};
#endif

#endif