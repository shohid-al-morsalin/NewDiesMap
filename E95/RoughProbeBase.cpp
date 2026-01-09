#include "stdafx.h"
#include "413App.h"
#include "RoughProbeBase.h"

double CRoughProbeBase::Read(short NAver) {
	ReadVolts(NAver);
	VRd = sqrt(Iva * Iva + Ivb * Ivb);
	VRd -= HWPar.fRdOffset; if (VRd < 0) VRd = 0;
	VRs -= HWPar.fRsOffset; if (VRs < 0) VRs = 0;
	return CalculateRH(VRs, VRd);
}

double CRoughProbeBase::Read() {
	ReadVolts(HWPar.Nmeas);
	VRd = sqrt(Iva * Iva + Ivb * Ivb);
	VRd -= HWPar.fRdOffset; if (VRd < 0) VRd = 0;
	VRs -= HWPar.fRsOffset; if (VRs < 0) VRs = 0;
	return CalculateRH(VRs, VRd);
}

short CRoughProbeBase::ReadVolts(short NAver) {
	VRs = 1.68f; Iva = 0.775f; Ivb = 0;
	VRs += 0.001f * rand() / float(RAND_MAX);
	Iva += 0.0005f * rand() / float(RAND_MAX);
	Ivb += 0.0005f * rand() / float(RAND_MAX);
	return 1;
}

double CRoughProbeBase::CalculateRH(double Rs, double Rd) {
	double a = Lamda4PI2();
	double Rsd = sqrt(Rd / (Rd + Rs));
	if (p413App->bIsRa == true) {
		return  HWPar.kRefl1 * a * Rsd * HWPar.RaFactor;
	}
	return HWPar.kRefl1 * a * Rsd/* * HWPar.sfc*/;
}

double CRoughProbeBase::Lamda4PI2() {
	//double lambda = 0.67, pi = 3.14159265358, theta = 0, a;
	//a = lambda / (4 * pi * cos(theta));
	return 5331.690593578;
}
