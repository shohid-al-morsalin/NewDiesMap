#include "stdafx.h"

#include "CamLumi.h"

CCamLumi::CCamLumi() {
	CamType = TYLUMICAM;
	hCamera = NULL;
	bIsMono = TRUE;
	pFrame = NULL;
	bpr = 0;
}

CCamLumi::~CCamLumi() {
	if (pFrame) {
		delete[] pFrame;
	}

	Close(0);
}

BOOL CCamLumi::SetExposure(short CamNum, short Exposure) {
	if (hCamera) {
		if (LucamSetProperty(hCamera, LUCAM_PROP_EXPOSURE, Exposure / 10.0f, 0)) {
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CCamLumi::SetAutoMode(short CamNum, short Prop) {
	return TRUE;
}

BOOL CCamLumi::Capture(SIU_CaptureContext* pCxt) {
	if (hCamera && pFrame) {
		if (LucamTakeVideo(hCamera, 1, pFrame)) {
			int wd = pCxt->PackedDIB.BmpInfo.biWidth;
			int ht = abs(pCxt->PackedDIB.BmpInfo.biHeight);
			BYTE* p = pFrame;
			BYTE* q = pCxt->PackedDIB.pDIBBits;
			if (bIsMono) {
				for (int y = 0; y < ht; y++) {
					BYTE* p1 = p;
					BYTE* q1 = q;
					for (int x = 0; x < wd; x++) {
						*q1 = *p1; q1++;
						*q1 = *p1; q1++;
						*q1 = *p1; q1++; p1++;
					}
					p += wd;
					q += bpr;
				}
			}
			else {
				LUCAM_CONVERSION lcConversion;
				lcConversion.CorrectionMatrix = LUCAM_CM_FLUORESCENT;
				lcConversion.DemosaicMethod = LUCAM_DM_HIGHER_QUALITY;
				LucamConvertFrameToRgb24(hCamera, q, pFrame, lffFormat.width / lffFormat.subSampleX, lffFormat.height / lffFormat.subSampleY, lffFormat.pixelFormat, &lcConversion);
			}
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CCamLumi::SetGlobalGain(short iCNum, int Gain) {
	if (hCamera) {
		if (LucamSetProperty(hCamera, LUCAM_PROP_GAIN, Gain / 10.0f, 0)) {
			if (!bIsMono) {
				if (!LucamSetProperty(hCamera, LUCAM_PROP_GAIN_RED, 1.0, 0)) {
					return FALSE;
				}

				if (!LucamSetProperty(hCamera, LUCAM_PROP_GAIN_GREEN1, 1.0, 0)) {
					return FALSE;
				}

				if (!LucamSetProperty(hCamera, LUCAM_PROP_GAIN_GREEN2, 1.0, 0)) {
					return FALSE;
				}

				if (!LucamSetProperty(hCamera, LUCAM_PROP_GAIN_BLUE, 1.0, 0)) {
					return FALSE;
				}
			}
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CCamLumi::ReadConfig(const char* lpszFileName, SIU_Config* pCfg, CCamBase::CAMERAID ID, short iCNum) {
	return TRUE;
}

BOOL CCamLumi::BeginCapture(const SIU_Config* pCfg, SIU_CaptureContext* pCxt, CCamBase::CAMERAID ID) {
	if (!pCfg) {
		ASSERT(0);
		return FALSE;
	}
	if (!hCamera) {
		return FALSE;
	}

	lsSnapshot.exposure = pCfg->CameraConfig.iExposure;
	lsSnapshot.gain = pCfg->CameraConfig.iGlobalGain;
	lsSnapshot.gainRed = 1.0;
	lsSnapshot.gainBlue = 1.0;
	lsSnapshot.gainGrn1 = 1.0;
	lsSnapshot.gainGrn2 = 1.0;
	lsSnapshot.useStrobe = FALSE;
	lsSnapshot.strobeDelay = 0;

	if (LucamGetFormat(hCamera, &lffFormat, &fFrameRate)) {
		lsSnapshot.format = lffFormat;
		lsSnapshot.timeout = 0;
		lsSnapshot.shutterType = LUCAM_SHUTTER_TYPE_GLOBAL;
		lsSnapshot.exposureDelay = 0;
		lsSnapshot.flReserved1 = 0.0;
		lsSnapshot.flReserved2 = 0.0;
		lsSnapshot.ulReserved1 = 0;
		lsSnapshot.ulReserved2 = 0;

		lsSnapshot.format.binningX = lffFormat.binningX;
		lsSnapshot.format.binningY = lffFormat.binningY;
		lsSnapshot.format.flagsX = lffFormat.flagsX;
		lsSnapshot.format.flagsY = lffFormat.flagsY;
		lsSnapshot.format.height = lffFormat.height;
		lsSnapshot.format.pixelFormat = lffFormat.pixelFormat;
		lsSnapshot.format.subSampleX = lffFormat.subSampleX;
		lsSnapshot.format.subSampleY = lffFormat.subSampleY;
		lsSnapshot.format.width = lffFormat.width;
		lsSnapshot.format.xOffset = lffFormat.xOffset;
		lsSnapshot.format.yOffset = lffFormat.yOffset;
		lsSnapshot.shutterType = LUCAM_SHUTTER_TYPE_GLOBAL;
		lsSnapshot.strobeDelay = 0.0;
		lsSnapshot.strobeFlags = 0;
		lsSnapshot.timeout = 10000.0;
		lsSnapshot.ulReserved1 = 0;
		lsSnapshot.ulReserved2 = 0;
		lsSnapshot.useHwTrigger = FALSE;
		lsSnapshot.useStrobe = FALSE;

		int iSize = (lffFormat.width * lffFormat.height) / (lffFormat.subSampleX * lffFormat.subSampleY);
		if (lffFormat.pixelFormat == LUCAM_PF_16) {
			iSize *= 2;
		}
		pFrame = new BYTE[iSize];
		if (LucamStreamVideoControl(hCamera, START_STREAMING, NULL)) {
			BITMAPINFOHEADER* pB = &pCxt->PackedDIB.BmpInfo;
			pB->biBitCount = 24;
			pB->biHeight = -abs((long)lffFormat.height) / lffFormat.subSampleY;
			pB->biWidth = lffFormat.width / lffFormat.subSampleX;
			pB->biPlanes = 1;
			pB->biSize = sizeof(BITMAPINFOHEADER);
			bpr = pB->biWidth * 3;	// convert to RGB [6/2/2014 FSMT]
			bpr += (4 - bpr % 4) % 4;
			pB->biSizeImage = bpr * abs(pB->biHeight);
			if (pCxt->PackedDIB.pDIBBits) {
				// Prevent multiple entry [6/2/2014 FSMT]
				delete[] pCxt->PackedDIB.pDIBBits;
				pCxt->PackedDIB.pDIBBits = NULL;
			}
			pCxt->PackedDIB.pDIBBits = new BYTE[pB->biSizeImage]; // Preallocate storage [6/2/2014 FSMT]
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CCamLumi::EndCapture(SIU_CaptureContext* pCxt, CCamBase::CAMERAID ID) {
	if (pFrame) {
		delete[] pFrame;
		pFrame = NULL;
	}
	if (hCamera) {
		if (LucamStreamVideoControl(hCamera, STOP_STREAMING, NULL)) {
			LucamDestroyDisplayWindow(hCamera);
			if (pCxt->PackedDIB.pDIBBits) {
				delete[] pCxt->PackedDIB.pDIBBits;
				pCxt->PackedDIB.pDIBBits = NULL;
			}
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CCamLumi::Open(short iCNum, SIU_Config* pCfg) {
	if (!hCamera) {
		short n = LucamNumCameras();
		if (n < 1) {
			return FALSE;
		}
		hCamera = LucamCameraOpen(iCNum);
		if (!hCamera) {
			return FALSE;
		}
		// Get camera id
		if (LucamGetCameraId(hCamera, &ulCameraId)) {
			// Get current video frame format
			if (LucamGetFormat(hCamera, &lffFormat, &fFrameRate)) {
				// Get color format
				FLOAT fValue;
				LONG lFlags;
				if (LucamGetProperty(hCamera, LUCAM_PROP_COLOR_FORMAT, &fValue, &lFlags)) {
					bIsMono = FALSE;
					if (fValue == LUCAM_CF_MONO) {
						bIsMono = TRUE;
					}

					ULONG nbFrameRate = 6, nbReturnFrameRate = 0;
					FLOAT frameRates[6];
					nbReturnFrameRate = LucamEnumAvailableFrameRates(hCamera, nbFrameRate, frameRates);

					float fCurFrameRate;
					LUCAM_FRAME_FORMAT lffFormat;
					if (LucamGetFormat(hCamera, &lffFormat, &fCurFrameRate)) {
						if (bIsMono) {
							lffFormat.subSampleX = lffFormat.subSampleY = 2;
						}
						else {
							lffFormat.subSampleX = lffFormat.subSampleY = 4;
						}
						if (LucamSetFormat(hCamera, &lffFormat, fCurFrameRate)) {
							return TRUE;
						}
					}
				}
			}
		}
		Close(iCNum);
		hCamera = NULL;
		return FALSE;
	}
	return FALSE;
}

BOOL CCamLumi::Close(short iCNum) {
	BOOL ret = FALSE;
	if (hCamera) {
		if (LucamCameraClose(hCamera)) {
			ret = TRUE;
		}
		hCamera = NULL;
	}
	return ret;
}