#pragma once

struct SData {
	int n = 0;
	double meanX = 0, meanY = 0;
	double* X = NULL, * Y = NULL;

	void Dealloc();
	bool Alloc(int nn);
	void means();
	void center();
	void scale();

	SData();
	SData(int n);
	SData(int N, double dataX[], double dataY[]);
	~SData();
};

struct SCircle {
	double a = 0; //- the X - coordinate of the center of the fitting circle
	double b = 0; //- the Y - coordinate of the center of the fitting circle
	double r = 0; //- the radius of the fitting circle
	double s = 0; //- the root mean square error(the estimate of sigma)
	double j = 0; //- the total number of iterations
	double g = 0;
	double Gx = 0;
	double Gy = 0;

	int i = 0;

	SCircle();
	SCircle(double aa, double bb, double rr);
};

struct SMatInv {
	void swap(double& a, double& b);
	SCircle Perturb(SCircle& New, SCircle& Old, double range);
	void eigen2x2(double a, double b, double c, double& d1, double& d2, double& Vx, double& Vy);
	double SigmaWithLargeCircleOption(SData& data, SCircle& circle);
	void GradientHessian(SData& data, SCircle& circle, double& F1, double& F2, double& A11, double& A22, double& A12);
	BOOL MatrixInversion(int n, double* a);

public:
	void PolyfitFlt(int Norder, float* pdata, short np);
	void Matrix2x2Inverse(float mat[2][2]);

	SCircle CircleFitByHyper(SData& data);
	SCircle CircleFitByPratt(SData& data);
	SCircle CircleFitByKasa(SData& data);
	SCircle CircleFitByTaubin(SData& data);

	int CircleFitByLevenbergMarquardtFull(SData& data, SCircle& circleIni, double LambdaIni, SCircle& circle);
	int CircleFitByLevenbergMarquardtReduced(SData& data, SCircle& circleIni, double LambdaIni, SCircle& circle);
	int CircleFitByChernovLesort(SData& data, SCircle& circleIni, double LambdaIni, SCircle& circle);
	int CircleFitByChernovHoussam(SData& data, SCircle& circleIni, double LambdaIni, SCircle& circle);

	int TestAlgebricFit();
	int TestGeometricFit();
	BOOL LSQFit(int n, int* xx, int* yy, float& a, float& b);
};

/*
references
https://people.cas.uab.edu/~mosya/cl/index.html
https://people.cas.uab.edu/~mosya/cl/CPPcircle.html
*/
