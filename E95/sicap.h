#ifndef __SICAP_H__
#define __SICAP_H__

#pragma once

#include <sidefs.h>
#include <siipl.h>

#undef SIAPI
#ifdef SIEXPORTS
#define SIAPI __declspec(dllexport)
#else
#define SIAPI __declspec(dllimport)
#endif

typedef struct {
	int iCameraID;
	SIPackedDIB PackedDIB;
	SIMonoImage BayerImage;
	SIMonoImage BlackRefImage;

	int iEnableCustomImagingrPipeline;
	int iEnableCaptureCallback;
	int iEnableDisplayCallback;
	SI_FUNC_MONO_IMAGE fpCustomImagingProc;
	SI_FUNC_MONO_IMAGE fpCaptureCallback;
	SI_FUNC_DISPLAY_PACKEDDIB fpDisplayCallback;
}SIU_CaptureContext;

#ifdef __cplusplus
extern "C" {
#endif

	SIAPI SIResult SI_ReadConfig(const char* lpszFileName, SIU_Config* pCfg);
	SIAPI SIResult SI_DetectCameraModel(int iCamID, SICameraModel* pModel);
	SIAPI SIResult SI_BeginCapture(const SIU_Config* pCfg, SIU_CaptureContext* pCxt);
	SIAPI SIResult SI_EndCapture(SIU_CaptureContext* pCxt);
	SIAPI SIResult SI_GetImagingProfile(SIU_CaptureContext* pCxt, SIU_ImagingProfile* pImgProf);
	SIAPI SIResult SI_SetImagingProfile(SIU_CaptureContext* pCxt, const SIU_ImagingProfile* pImgProf);
	SIAPI SIResult SI_CaptureDIB(SIU_CaptureContext* pCxt);
	SIAPI SIResult SI_StartThreadedCapture(SIU_CaptureContext* pCxt);
	SIAPI SIResult SI_StopThreadedCapture(SIU_CaptureContext* pCxt);
	SIAPI SIResult SI_GetThreadedCaptureFrameRate(SIU_CaptureContext* pCxt, SI32f* pRate);

	SIAPI SIResult SI_CaptureBlackReferenceImage(SIU_CaptureContext* pCxt);
	SIAPI SIResult SI_EnableBlackReferenceImage(SIU_CaptureContext* pCxt, int iEnable);
	SIAPI SIResult SI_GetLastCapturedBinImage(SIU_CaptureContext* pCxt);
	SIAPI SIResult SI_RetrieveCapturedBlackReferenceImage(SIU_CaptureContext* pCxt);
	SIAPI SIResult SI_LoadBlackReferenceImageFromFile(SIU_CaptureContext* pCxt, const char* lpszFileName);

#ifdef __cplusplus
};
#endif

#endif