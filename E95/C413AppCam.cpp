#include "stdafx.h"
#include <math.h>
#include "413App.h"
#include "ThinFilmProp.h"
#include "EchoProbeProp.h"
#include "PatternProp.h"
#include "LocationProp.h"
#include "SICam.h"
#include "ImgWnd.h"
#include "ChartWnd.h"
#include "MPoint.h"
#include "Data.h"
#include "EnlargeDlg.h"
#include "SiCamSettingDlg.h"
#include "..\128\CAS\CasMgr.h"
#include "..\128\CAS\NCas.h"
#include "..\413\Echoscope\Peaks.h"
#include "..\413\EchoScope\EchoProbe.h"
#include "..\413\OceanOp\OceanBase.h"
#include "PattMatch.h"
#include "PeakParam.h"
#include "OOPARS.h"
#include "GlobalDeviceContainer.h"
#include "GlobalUIContainer.h"
#include "SetupPropComm.h"
#include "ThkValues.h"
#include "ProcJob413ProcessingDlg.h"
#include "..\413\SIBase.h"
#include "..\413\MC2.h"
#include "MC3.h"
#include "FFUFailDlg.h"

void C413AppCam::LoadPicture(CString name, SIPackedDIB *pDIB)
{
	FILE *fp;
	
	fp = fopen(name,"rb");
	if (fp)
	{
		BITMAPFILEHEADER bmfh;
		
		fread(&bmfh,1,sizeof(BITMAPFILEHEADER),fp);
		fread(&pDIB->BmpInfo,1,sizeof(BITMAPINFOHEADER),fp);
		
		// Check file type signature
		if (bmfh.bfType!='MB') return ;		// File is not BMP
		
		// Assign some short variables:
		short BPP=pDIB->BmpInfo.biBitCount/8;
		long Width=pDIB->BmpInfo.biWidth;
		long Height = abs(pDIB->BmpInfo.biHeight); // absolute value
		long BytesPerRow = Width * BPP;
		BytesPerRow += (4-BytesPerRow%4) % 4;	// int alignment
		
		if (BPP>=3) 
		{
			fseek(fp,bmfh.bfOffBits,SEEK_SET);
			if (pDIB->pDIBBits)
			{
				delete [] pDIB->pDIBBits;
				pDIB->pDIBBits = NULL;
			}
			pDIB->pDIBBits = new BYTE[BytesPerRow*Height+BytesPerRow];
			if (pDIB->BmpInfo.biHeight>0)
			{
				for (int n=Height-1;n>=0;n--)
				{
					fread (pDIB->pDIBBits+BytesPerRow*n,1,BytesPerRow,fp);
				}
				pDIB->BmpInfo.biHeight=-Height;
			}
			else
			{
				fread (pDIB->pDIBBits,1,BytesPerRow*Height,fp);
			}
		}
		// so, we always have a up-bottom raster (that is negative height for windows):
		fclose(fp);
	}
}
