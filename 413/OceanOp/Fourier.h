#pragma once

class CSpec;

class CFourier {
public:
	double pi;
	unsigned long int fundamental_frequency;
	double* vector;
	CFourier(void);
	~CFourier(void);
	// FFT 1D
	void ComplexFFT(CSpec* pSpec, int number_of_samples, int sample_rate, int sign);
};
