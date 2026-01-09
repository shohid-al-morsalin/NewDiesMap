/********************************************************/
/* WARNING:												*/
/* This code cannot be used in any application			*/
/* without permission of the author						*/
/* for more information please read the license in the	*/
/* Numerical Recipes in C book or go to www.nr.com		*/
/* this is merely an example of how to use it			*/
/********************************************************/

#include "StdAfx.h"
#include <math.h>
#include ".\fourier.h"
#include "Spec.h"

#define SWAP(a,b) ftempr=(a);(a)=(b);(b)=ftempr

CFourier::CFourier(void) {
	pi = 4 * atan((double)1); vector = NULL;
}

CFourier::~CFourier(void) {
	if (vector != NULL) {
		delete[] vector;
	}
}

// FFT 1D
//data -> float array that represent the array of complex samples
//number_of_complex_samples -> number of samples (N^2 order number)
//sign -> 1 to calculate FFT and -1 to calculate Reverse FFT
void CFourier::ComplexFFT(CSpec* pSpec, int number_of_samples, int sample_rate, int sign) {
	//variables for the fft
	int m, n, i, j;
	unsigned long mmax, istep;
	double wtemp, wr, wpr, wpi, wi, theta, tempr, tempi;
	double ftempr;

	//the complex array is real+complex so the array
	//as a size n = 2* number of complex samples
	//real part is the data[index] and
	//the complex part is the data[index+1]

	//new complex array of size n=2*sample_rate
	if (vector != NULL) {
		delete[] vector;
	}

	vector = new double[2 * sample_rate];

	//put the real array in a complex array
	//the complex part is filled with 0's
	//the remaining vector with no data is filled with 0's
	for (n = 0; n < sample_rate; n++) {
		if (n < number_of_samples) {
			vector[2 * n] = pSpec->pPt[n].Y1;
		}
		else {
			vector[2 * n] = 0;
		}
		vector[2 * n + 1] = 0;
	}

	//binary inversion (note that the indexes
	//start from 0 witch means that the
	//real part of the complex is on the even-indexes
	//and the complex part is on the odd-indexes)
	n = sample_rate << 1;
	j = 0;
	for (i = 0; i < n / 2; i += 2) {
		if (j > i) {
			SWAP(vector[j], vector[i]);
			SWAP(vector[j + 1], vector[i + 1]);
			if ((j / 2) < (n / 4)) {
				SWAP(vector[(n - (i + 2))], vector[(n - (j + 2))]);
				SWAP(vector[(n - (i + 2)) + 1], vector[(n - (j + 2)) + 1]);
			}
		}
		m = n >> 1;
		while (m >= 2 && j >= m) {
			j -= m;
			m >>= 1;
		}
		j += m;
	}
	//end of the bit-reversed order algorithm

	//Danielson-Lanzcos routine
	mmax = 2;
	while (n > int(mmax)) {
		istep = mmax << 1;
		theta = sign * (2 * pi / mmax);
		wtemp = sin(0.5 * theta);
		wpr = -2.0 * wtemp * wtemp;
		wpi = sin(theta);
		wr = 1.0;
		wi = 0.0;
		for (m = 1; m<int(mmax); m += 2) {
			for (i = m; i <= n; i += istep) {
				j = i + mmax;
				tempr = wr * vector[j - 1] - wi * vector[j];
				tempi = wr * vector[j] + wi * vector[j - 1];
				vector[j - 1] = float(vector[i - 1] - tempr);
				vector[j] = float(vector[i] - tempi);
				vector[i - 1] += float(tempr);
				vector[i] += float(tempi);
			}
			wr = (wtemp = wr) * wpr - wi * wpi + wr;
			wi = wi * wpr + wtemp * wpi + wi;
		}
		mmax = istep;
	}
	//end of the algorithm

// 	*(data++) = 0;
	// Find peak with highest power [11/25/2012 Yuen]
	pSpec->ReAllocS(sample_rate / 2);
	float MaxV = float(sqrt(pow(vector[0], 2) + pow(vector[1], 2))) * 0.5;
	CFFTPoint* data = pSpec->pV;
	for (int k = 0; k < sample_rate; k += 2) {
		data->V = float(sqrt(pow(vector[k], 2) + pow(vector[k + 1], 2))) * 0.5;
		if (data->V > MaxV) {
			MaxV = data->V;
		}
		data->P = 0;
		if (data->V < 0) {
			data->V = 0;
		}
		data++;
	}
	pSpec->MaxV = MaxV;

	// 	//determine the fundamental frequency
	// 	//look for the maximum absolute value in the complex array
	// 	fundamental_frequency=0;
	// 	for(i=2; i<=sample_rate; i+=2)
	// 	{
	// 		if((pow(vector[i],2)+pow(vector[i+1],2))>(pow(vector[fundamental_frequency],2)+pow(vector[fundamental_frequency+1],2))){
	// 			fundamental_frequency=i;
	// 		}
	// 	}
	//
	// 	//since the array of complex has the format [real][complex]=>[absolute value]
	// 	//the maximum absolute value must be ajusted to half
	// 	fundamental_frequency=(long)floor((float)fundamental_frequency/2);
}