#include "stdafx.h"
#include "Stat.h"

SStat::SStat() {
	Clear();
}

SStat::~SStat() {
}

void SStat::Dump(FILE* fp) {
	if (!fp) return;
	fprintf(fp, "Statistics:\n");
	fprintf(fp, "Average,%.3f\n", fAve);
	fprintf(fp, "Std.Dev,%.4f\n", fStdev);
	fprintf(fp, "TTV,%.3f\n", fMax - fMin);
	fprintf(fp, "Maximum,%.3f\n", fMax);
	fprintf(fp, "Minimum,%.3f\n\n", fMin);
}

void SStat::Calculate() {
	if (N == 0) { fAve = 0; fStdev = 0; fMax = fMin = 0; return; }
	double v = N * sum2 - sum * sum;
	fAve = float(sum / double(N));
	if ((N > 1) && (v >= 0)) fStdev = float(sqrt(v / (N * (N - 1))));
	else fStdev = 0;
}

void SStat::Clear() {
	N = 0;
	sum = sum2 = 0;
	fStdev = fAve = 0;
	fMin = 0; fMax = 0;
	fWarp = fBow = fMaxDieTTV = fStress = 0;
	fP2PMaxTTV = 0; //20120222 SCOTT ADD
}

void SStat::Add(float v) {
	if (N == 0) fMax = fMin = v;
	sum += v; sum2 += (v * v);
	if (v < fMin) fMin = v;
	if (v > fMax) fMax = v;
	N++;
}