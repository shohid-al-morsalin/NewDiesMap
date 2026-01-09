#ifndef __PEAKS_H__
#define __PEAKS_H__

#define MAXNUMPROBE		2
#define MAXPEAKCOUNT	10

class CPeaks {
public:

	/**
	 * Max # of peaks detected per channel
	 */
	short Total[MAXNUMPROBE];

	/**
	 * Center of mass of each peak per channel
	 */
	float fCM[MAXNUMPROBE][MAXPEAKCOUNT + 1];		// CM, +1 for safety, unit in micron [3/12/2010 Valued Customer]

	/**
	 * Max intensity of each peak per channel
	 */
	int Inten[MAXNUMPROBE][MAXPEAKCOUNT + 1];		// Peak intensity

	/**
	 * Index of maxima position of each peak per channel
	 */
	int fXMaxima[MAXNUMPROBE][MAXPEAKCOUNT + 1];	// XMaxima
	int fMiddle[MAXNUMPROBE][MAXPEAKCOUNT + 1];	// XMaxima

	void Dump(FILE* fp, short id);
	void Clear();
	CPeaks();
};

#endif
