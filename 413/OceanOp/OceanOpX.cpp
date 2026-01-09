// OceanOp.cpp: implementation of the COceanOp class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Dos.h"
#include "OceanOp.h"
#include "OOIDrv32.h"
#include "math.h"
#include <complex>
//#include "MappedData.h"
#include "FFT.h"
//#include "WaferCal.h"

using namespace std;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//#define NUMBEROFMAPPEDPOINTS 2048

#define PI 3.1415926535897932384626433832795
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COceanOp::COceanOp()
{
//	NFFTSP = 2048;

	int i;

	oopars.lambdamax = 833;
	oopars.lambdamin = 416;
	oopars.SignalMax = 3900;
	oopars.SignalMin = 390;
	oopars.fThkFactor = 1.0;
	oopars.fPeakMin = 1.0;
	oopars.fThkMin = 3.0;
	oopars.fThkMax = 20.0;
	oopars.m_RefrIndex = 1.5;
	oopars.nFFTBeg = (int) (2 * oopars.fThkMin * oopars.fThkFactor * oopars.m_RefrIndex
		 * 1e3* (1.0/oopars.lambdamin - 1.0/oopars.lambdamax));
	oopars.nFFTEnd = (int) (2 * oopars.fThkMax * oopars.fThkFactor * oopars.m_RefrIndex
		 * 1e3 * (1.0/oopars.lambdamin - 1.0/oopars.lambdamax));
	oopars.boxcar = 0;
	oopars.fCalCoeff = 1.0;
	oopars.nMeastime = 1;
	oopars.nMethod = 0;
	oopars.nPort = 4;
	oopars.nRepeat = 4;

	for(i=0;i<2048;i++)
	{
		DataSpectrum[i]=1.0;
	}
}

COceanOp::~COceanOp()
{
	WORD adctype = OOI_GetADCType();
	if(adctype==NIR512)
	{
		NIRADC_EnableCooler(FALSE);
		NIRADC_EnableHighGain(FALSE);
		NIRADC_EnableFan(FALSE);
	}

	if (Registered)
	{
		OOI_UnRegister(hInstance);
	}
}

void COceanOp::DoSingleScan()
{
	int Resp;
	sp.cmd=CMD_NONE;
	sp.boxcar = oopars.boxcar;
	sp.dsf = oopars.nMeastime;
	sp.fdc = 0;
	sp.average = oopars.nRepeat;
	sp.correct_dark = FALSE;
	sp.extrig = 0;
	Resp=OOI_DoScan(&sp); 
	if(Resp!=ER_NOERROR)
	{
		return;
	}
}

void COceanOp::Register(HWND wnd, long MessageID)
{
	int Res = OOI_Register(&ooip); 
	if( Res != ER_NOERROR)
	{
		TRACE("Init:Registration Err (%d)\n",Res);
		Registered = true;
	}
}

int COceanOp::Result(double &PolyThickness, float **FFTArray, int &len)
{
	int status_temp;
	status_temp = GetSpectrum(DataSpectrum,NULL);
	if(DataSpectrum[1] == 0)
	{
		DataSpectrum[1] = DataSpectrum[2];
	}
	if(DataSpectrum[0] == 0)
	{
		DataSpectrum[0] = DataSpectrum[1];
	}
	int i;
	int j;

	for(i=0;i<Nwave;i++)
	{
		switch(oopars.nMethod%4)
		{
		case 0: 
			m_Vnorm[i] = DataSpectrum[i+BEGSPECTPIX];  // raw data, no conversion [5/3/2011 XPMUser] 
			break;
		case 1:
			if (DataSpectrum[i] <= BackgroundSpectrum[i])
			{
				m_Vnorm[i] = 0;
			}
			else
			{
				m_Vnorm[i] = DataSpectrum[i+BEGSPECTPIX] - BackgroundSpectrum[i+BEGSPECTPIX];
			}
			break;
		case 2:
			if (CalibrationSpectrum[i+BEGSPECTPIX]<=0)
			{
				m_Vnorm[i] = 0;
			}
			else
			{
				m_Vnorm[i] = (oopars.fCalCoeff*DataSpectrum[i+BEGSPECTPIX]/CalibrationSpectrum[i+BEGSPECTPIX]);
			}
			break;
		case 3:
			if (CalibrationSpectrum[i+BEGSPECTPIX] <= BackgroundSpectrum[i+BEGSPECTPIX])
			{
				m_Vnorm[i] = 0;
			}
			else
			{
				m_Vnorm[i] = (oopars.fCalCoeff*(DataSpectrum[i+BEGSPECTPIX] - BackgroundSpectrum[i+BEGSPECTPIX])
					/(CalibrationSpectrum[i+BEGSPECTPIX] - BackgroundSpectrum[i+BEGSPECTPIX]));
			}
			break;
		default: 
			m_Vnorm[i] = DataSpectrum[i+BEGSPECTPIX]; 
			break;
		}
	}

	j = 0;
	double lambdatemp;
	USB2000CONSTANTS Calibconst;
	WORD adctype;
	adctype=OOI_GetADCType();
	if (adctype == USB2000PLUS)
	{
		if(!USB2000Plus_GetCoefficients(&Calibconst)) 
		{
			return OOIFAILED;
		}
	}
	else
	{
		if(!USB2000_GetCoefficients(&Calibconst)) 
		{
			return OOIFAILED;
		}
	}
	// not elegant but should work, reverse order in order to assure increasing kspace
	for(i=NUMBEROFPIX -1;i>=0;i--)
	{
		int k = (i+BEGSPECTPIX);
		lambdatemp = Calibconst.WLIntercept;
		lambdatemp += Calibconst.WLFirst * k;
		lambdatemp += Calibconst.WLSecond* k * k;
		lambdatemp += Calibconst.WLThird * k * k * k;
		if(lambdatemp <= oopars.lambdamax && lambdatemp >= oopars.lambdamin)
		{
			m_kspace[j] = 1.0/lambdatemp;
			m_signal[j] = m_Vnorm[i];
			j++;
		}
		else
		{
			m_kspace[j] = 0;
			m_signal[j] = 0;
		}
	}
	numberofgoodpixels = j;

	if (numberofgoodpixels>0)
	{
		double aver = 0;
		for(i=0;i<numberofgoodpixels;i++)
		{
			aver += m_signal[i];
		}
		aver /= numberofgoodpixels;
		for(i=0;i<numberofgoodpixels;i++)
		{
			m_signal[i] -= aver;
		}
	}
	
	j=0;

	int N = numberofgoodpixels;

	if(m_kspace[N-1] > m_kspace[0])
	{
		double fXMin = m_kspace[0];
		double fXMax = m_kspace[N-1];

		double fXStep = (fXMax - fXMin)/(NUMBEROFMAPPEDPOINTS - 1);
		m_signal2[0] = m_signal[0];
		m_signal2[NUMBEROFMAPPEDPOINTS - 1] = m_signal[N - 1];

		double fXTemp = fXMin;
		int j = 0;
		for (int i=1; i < NUMBEROFMAPPEDPOINTS - 1; i++)
		{
			fXTemp += fXStep;
			while(!(fXTemp >= m_kspace[j] && fXTemp <= m_kspace[j + 1]))
			{
				j++;
				if(j >= N - 1)
				{
					break;
				}
			}
			m_signal2[i] = (m_signal[j] + (fXTemp - m_kspace[j])
				* (m_signal[j + 1] - m_signal[j]) / (m_kspace[j + 1] - m_kspace[j]));
		}
		for (i = 0; i < NUMBEROFMAPPEDPOINTS; i++)
		{
			if (i<=NUMBEROFMAPPEDPOINTS)  ///apodization
			{
				m_signal2[i] *= (2.0*i/NUMBEROFMAPPEDPOINTS);
			}
			else
			{
				m_signal2[i] *= (2.0*(NUMBEROFMAPPEDPOINTS-1-i)/NUMBEROFMAPPEDPOINTS);
			}
			m_signal2Im[i] = 0;
			resultRe[i] = 0;
			resultIm[i] = 0;
		}
	}
	CFFT FFT;
	FFT.fft_double(NUMBEROFMAPPEDPOINTS, 0, m_signal2, m_signal2Im, resultRe, resultIm);
	for (i=0; i < NFFTSP; i++)
	{
// 		if ((i < oopars.nFFTBeg) || (i > oopars.nFFTEnd)) 
// 		{
// 			FFTData[i] = 0;
// 			m_P[i] = 0;
// 		}
// 		else
// 		{
		CFFTPoint *p = &nFFTData.Pt[i];
		p->x = i;
		p->y1 = sqrt(resultRe[i]*resultRe[i] + resultIm[i]*resultIm[i]);
//		m_P[i] = p->y1;
// 		}
	}

	int jPmax = oopars.nFFTBeg;

	float fSum = 0;
	int count=0;
	for(j=oopars.nFFTBeg;j<=oopars.nFFTEnd;j++)
	{
		if (j<NFFTSP)
		{
			fSum += nFFTData.Pt[j].y1;
			count++;
			if (nFFTData.Pt[j].y1>nFFTData.Pt[jPmax].y1) 
			{
				jPmax = j;
			}
		}
	}
	if (count>0)
	{
		fSum /= count;
	}
	if ((jPmax<NFFTSP)&&
		(jPmax>oopars.nFFTBeg)&&
		(jPmax<=oopars.nFFTEnd)&&
		//(FFTData.Pt[jPmax].y1>=oopars.fPeakMin*fSum)&&
		(oopars.fThkFactor > 0)&&
		(m_kspace[N-1]>m_kspace[0])
		)
	{
		float fSum1 = 0;
		float fSum2 = 0;
		for (i=-3;i<3;i++)
		{
			if ((jPmax+i>=oopars.nFFTBeg)&&(jPmax+i<=oopars.nFFTEnd))
			{
				int n = jPmax+1;
				CFFTPoint *p = &nFFTData.Pt[n];
				fSum1 += /*1.0*(jPmax+i)*/n*p->y1/*FFTData.Pt[jPmax+i]*/;
				fSum2 += p->y1/*FFTData.Pt[jPmax+i]*/;
			}
		}
		if (fSum2>0)
		{
			PolyThickness =  fSum1 / fSum2 / oopars.fThkFactor / oopars.m_RefrIndex / 2e3 
				/ (1.0/oopars.lambdamin - 1.0/oopars.lambdamax);
		}
		else
		{
			PolyThickness = 0;
		}
	}
	else
	{
		PolyThickness = 0;
	}

	return status_temp;
}

int COceanOp::StartTF(CMeParam *p)
{
	GetSystemTime(&AuxilaryStartTime);
	hInstance = AfxGetInstanceHandle();
	hWnd2 = AfxGetMainWnd()->m_hWnd;
	StatusFlag = DLL_OOI_BUSY;

	ooip.ssize=sizeof(OOI_PARAM);
	ooip.dev=0;
	ooip.cmd=CMD_NONE;
	ooip.fdc = 0;
	ooip.dsf = oopars.nMeastime; 
	ooip.boxcar = oopars.boxcar; 
	ooip.average=oopars.nRepeat;
	ooip.chan_ena[0]=1;
	ooip.chan_ena[1]=0;
	ooip.chan_ena[2]=0;
	ooip.chan_ena[3]=0;
	ooip.chan_ena[4]=0;
	ooip.chan_ena[5]=0;
	ooip.chan_ena[6]=0;
	ooip.chan_ena[7]=0;
	ooip.changed=TRUE;
	ooip.ovrr_ok=TRUE;
	ooip.cont=FALSE;
	ooip.flash_cont=FALSE;
	ooip.scan_dark=FALSE;
	ooip.correct_dark=0;
	ooip.extrig=0;
	ooip.hWnd=hWnd2;
	ooip.host=hInstance;

	if(Registered==FALSE)
	{
		int Res = OOI_Register(&ooip); 
		if( Res == ER_NOERROR)
		{
			Registered=TRUE;
		}
	}
	WORD adctype = OOI_GetADCType();
	if(adctype==NIR512)
	{
		NIRADC_EnableCooler(TRUE);
		NIRADC_EnableHighGain(TRUE);
		NIRADC_EnableFan(TRUE);
	}
	ooip.UseUSecIntegrationTime = TRUE;
	ooip.USecIntegrationTime = 1000 * oopars.nMeastime;

	OOI_ParamSet(&ooip);
	OOI_SingleScan(hInstance); //ER_NOERROR 

	return 0;
}

int COceanOp::GetSpectrum(float* Inten, float*Freq)
{
	for(int i=0;i<NUMBEROFPIX;i++)
	{
		Inten[i]=ooip.chdat[0][i];
	}
	if (Inten[0]<=0)
	{
		Inten[0] = Inten[1];
	}
	return 0;
}

int COceanOp::GetDataNumber(BOOL bFFT)
{
  if(!bFFT)
  {
	  return numberofgoodpixels;
  }
  else
  {
	  //return NFFTSP;
	  return (oopars.nFFTEnd - oopars.nFFTBeg + 1);
  }
}

void COceanOp::SetThkRange2(float fThkMin, float fThkMax, int Method, float fCoeff, float RI,float lmin, float lmax, int meastime, int Boxcar, float TFScalingFactor)
{
	COceanBase::SetThkRange2(fThkMin,fThkMax,Method,fCoeff,RI,lmin,lmax,meastime,Boxcar,TFScalingFactor);
	sp.dsf=meastime;
}
