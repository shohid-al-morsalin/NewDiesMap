#include <stdafx.h>
#include "Peaks.h"

void CPeaks::Dump(FILE* fp, short id) {
	int i;
	fprintf(fp, "ID,%d\nTop,%d\n", id, Total[0]);

	for (i = 0; i < Total[0]; i++) {
		fprintf(fp, "%f,%d,%d,%d\n", fCM[0][i] * 1e-4f, fXMaxima[0][i], fMiddle[0][i], Inten[0][i]);
	}
	fprintf(fp, "Bot,%d\n", Total[1]);
	for (i = 0; i < Total[1]; i++) {
		fprintf(fp, "%f,%d,%d,%d\n", fCM[1][i] * 1e-4f, fXMaxima[1][i], fMiddle[1][i], Inten[1][i]);
	}
}
void CPeaks::Clear() {
	Total[0] = Total[1] = 0;
	for (int i = 0; i < MAXPEAKCOUNT; i++) {
		fCM[0][i] = fCM[1][i] = 0;
		Inten[0][i] = Inten[1][i] = 0;
		fXMaxima[0][i] = fXMaxima[1][i] = 0;
		fMiddle[0][i] = fMiddle[1][i] = 0;
	}
}
CPeaks::CPeaks() {
	Clear();
}