#include "stdafx.h"

#include <iostream>
#include "MatInv.h"

#define Pi 3.14159265358
#define REAL_MAX            FLT_MAX
#define REAL_MIN            FLT_MIN
#define REAL_TOLERANCE     (FLT_MIN * 100)
#define REAL_EPSILON        1.192092896e-07F

// #define REAL_MAX numeric_limits<reals>::max()
// #define REAL_MIN numeric_limits<reals>::min()
// #define REAL_EPSILON numeric_limits<reals>::epsilon();

const double One = 1.0, Two = 2.0, Three = 3.0, Four = 4.0, Five = 5.0;
const double Six = 6.0, Ten = 10.0;

template<typename T>
inline T SQR(T t) { return t * t; };

//********************* SimulateRandom ****************************

void SimulateRandom(SData& data, double Window) {
	/*
			  Simulate data points with uniform distribution
			  in the square |x|<Window, |y|<Window

	  input:
			  nPoints  the number of data points
	*/
	//Data data(nPoints);

	for (int i = 0; i < data.n; i++) {
		data.X[i] = Window * (Two * rand() / RAND_MAX - One);
		data.Y[i] = Window * (Two * rand() / RAND_MAX - One);
	}
}

double pythag1(double a, double b) {
	double absa = abs(a), absb = abs(b);
	if (absa > absb) return absa * sqrt(One + SQR(absb / absa));
	else return (absb == 0.0 ? 0.0 : absb * sqrt(One + SQR(absa / absb)));
}

double Sigma(SData& data, SCircle& circle) {
	//****************** Sigma ************************************
	//   estimate of Sigma = square root of RSS divided by N
	//   gives the root-mean-square error of the geometric circle fit
	double sum = 0., dx, dy;

	for (int i = 0; i < data.n; i++) {
		dx = data.X[i] - circle.a;
		dy = data.Y[i] - circle.b;
		sum += SQR(sqrt(dx * dx + dy * dy) - circle.r);
	}
	return sqrt(sum / data.n);
}

double SigmaReduced(SData& data, SCircle& circle) {
	//****************** SigmaReduced ************************************
	//   estimate of Sigma = square root of RSS divided by N
	//   gives the root-mean-square error of the geometric circle fit
	//
	//   uses only the center of the circle (a,b), not the radius
	//   the function computes the optimal radius here
	int i;
	int n = data.n;
	double sum = 0., dx, dy, r, * D;

	D = new double[n]; if (!D) return 0;

	for (i = 0; i < n; i++) {
		dx = data.X[i] - circle.a;
		dy = data.Y[i] - circle.b;
		D[i] = sqrt(dx * dx + dy * dy);
		sum += D[i];
	}
	r = sum / n;

	for (sum = 0., i = 0; i < n; i++)  sum += SQR(D[i] - r);
	delete[]D;
	return sqrt(sum / n);
}

double SigmaReducedNearLinearCase(SData& data, SCircle& circle) {
	//****************** SigmaReducedNearLinearCase ****************
	//   estimate of Sigma = square root of RSS divided by N
	int i, n = data.n;
	double a0, b0, del, s, c, x, y, z, p, t, g, W, Z;

	a0 = circle.a - data.meanX;  b0 = circle.b - data.meanY;
	del = One / sqrt(a0 * a0 + b0 * b0);
	s = b0 * del;  c = a0 * del;

	for (W = Z = 0., i = 0; i < n; i++) {
		x = data.X[i] - data.meanX;
		y = data.Y[i] - data.meanY;
		z = x * x + y * y;
		p = x * c + y * s;
		t = del * z - Two * p;
		g = t / (One + sqrt(One + del * t));
		W += (z + p * g) / (Two + del * g);
		Z += z;
	}
	W /= n;
	Z /= n;

	return sqrt(Z - W * (Two + del * del * W));
}

double SigmaReducedForCenteredScaled(SData& data, SCircle& circle) {
	//****************** SigmaReducedForCenteredScaled ****************
	//   estimate of Sigma = square root of RSS divided by N
	int i, n = data.n;
	double sum = 0., dx, dy, r;

	for (i = 0; i < n; i++) {
		dx = data.X[i] - circle.a;
		dy = data.Y[i] - circle.b;
		sum += sqrt(dx * dx + dy * dy);
	}
	r = sum / n;

	return sqrt(SQR(circle.a) + SQR(circle.b) - r * r + Two);
}

double OptimalRadius(SData& data, SCircle& circle) {
	//****************** OptimalRadius ******************************
	//     compute the optimal radius of a circle, given its center (a,b)
	double Mr = 0., dx, dy;

	for (int i = 0; i < data.n; i++) {
		dx = data.X[i] - circle.a;
		dy = data.Y[i] - circle.b;
		Mr += sqrt(dx * dx + dy * dy);
	}
	return Mr / data.n;
}

void SMatInv::PolyfitFlt(int Norder, float* pdata, short np) {
	double sum = 0;
	double L, L2;
	int k, i, j;
	int N = Norder + 1;
	double* S;
	double* SX;
	double* RH;
	double* a;
	float fXnorm = 0.25;

	if ((Norder < 2) || (np < Norder)) return;

	S = new double[N * N];
	SX = new double[2 * N + 1];
	RH = new double[N + 1];
	a = new double[N + 1];

	memset(S, 0, (N * N) * sizeof(double));
	memset(SX, 0, (2 * N + 1) * sizeof(double));
	memset(RH, 0, (N + 1) * sizeof(double));
	memset(a, 0, (N + 1) * sizeof(double));

	double sf = 2.0 / (np - 1.0);

	SX[0] = np;
	for (k = 1; k <= 2 * N - 2; k++) {
		sum = 0;
		for (i = 0; i < np; i++) {
			L = 1.0;
			for (j = 0; j < k; j++) {
				L *= fXnorm * (-1 + sf * i);
			}
			sum += L;
		}
		SX[k] = sum;
	}

	for (k = 0; k < N; k++) {
		sum = 0;
		for (i = 0; i < np; i++) {
			L = 1.0;
			for (j = 0; j < k; j++) {
				L *= fXnorm * (-1 + sf * i);
			}
			sum += pdata[i] * L; //20160826 sum += fY[i] * L;
		}
		RH[k] = sum;
	}

	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++) {
			S[i * N + j] = SX[i + j];
		}
	}

	if (!MatrixInversion(N, S)) return;

	for (i = 0; i < N; i++) {
		a[i] = 0;
		for (j = 0; j < N; j++) {
			a[i] += S[i * N + j] * RH[j];
		}
	}

	for (k = 0; k < np; k++) {
		sum = 0;
		L = fXnorm * (-1 + k * sf);
		for (j = 0; j < N; j++) {
			L2 = 1.0;
			for (i = 0; i < j; i++) {
				L2 *= L;
			}
			sum += a[j] * L2;
		}
		pdata[k] = float(sum); //20160826 fY[k] = sum;
	}

	if (S) delete[] S;
	if (SX) delete[] SX;
	if (RH) delete[] RH;
	if (a) delete[] a;
}

void SMatInv::Matrix2x2Inverse(float mat[2][2]) {// mat assume to be 2x2 [11/6/2019 FSM]
	const short dim = 2;
	float a, b, c, d;
	double determinant = 0;

	a = mat[0][0];
	b = mat[0][1];
	c = mat[1][0];
	d = mat[1][1];

	determinant = (double(a) * d - double(b) * c); if (determinant == 0) return;
	mat[0][0] = float(d / determinant);
	mat[0][1] = float(-b / determinant);
	mat[1][0] = float(-c / determinant);
	mat[1][1] = float(a / determinant);
}

SCircle SMatInv::CircleFitByHyper(SData& data) {
	/*
	Circle fit to a given set of data points (in 2D)

	This is an algebraic fit based on the journal article

	A. Al-Sharadqah and N. Chernov, "Error analysis for circle fitting algorithms",
	Electronic Journal of Statistics, Vol. 3, pages 886-911, (2009)

	It is an algebraic circle fit with "hyperaccuracy" (with zero essential bias).
	The term "hyperaccuracy" first appeared in papers by Kenichi Kanatani around 2006

	Input:  data     - the class of data (contains the given points):

		data.n   - the number of data points
		data.X[] - the array of X-coordinates
		data.Y[] - the array of Y-coordinates

	Output:
			circle - parameters of the fitting circle:

		circle.a - the X-coordinate of the center of the fitting circle
		circle.b - the Y-coordinate of the center of the fitting circle
		circle.r - the radius of the fitting circle
		circle.s - the root mean square error (the estimate of sigma)
		circle.j - the total number of iterations

	This method combines the Pratt and Taubin fits to eliminate the essential bias.

	It works well whether data points are sampled along an entire circle or
	along a small arc.

	Its statistical accuracy is theoretically higher than that of the Pratt fit
	and Taubin fit, but practically they all return almost identical circles
	(unlike the Kasa fit that may be grossly inaccurate).

	It provides a very good initial guess for a subsequent geometric fit.

	Nikolai Chernov  (September 2012)

	*/
	int i, iter, IterMAX = 99;

	double Xi, Yi, Zi;
	double Mz, Mxy, Mxx, Myy, Mxz, Myz, Mzz, Cov_xy, Var_z;
	double A0, A1, A2, A22;
	double Dy, xnew, x, ynew, y;
	double DET, Xcenter, Ycenter;

	SCircle circle;

	data.means();   // Compute x- and y- sample means (via a function in the class "data")

//     computing moments

	Mxx = Myy = Mxy = Mxz = Myz = Mzz = 0.;

	for (i = 0; i < data.n; i++) {
		Xi = data.X[i] - data.meanX;   //  centered x-coordinates
		Yi = data.Y[i] - data.meanY;   //  centered y-coordinates
		Zi = Xi * Xi + Yi * Yi;

		Mxy += Xi * Yi;
		Mxx += Xi * Xi;
		Myy += Yi * Yi;
		Mxz += Xi * Zi;
		Myz += Yi * Zi;
		Mzz += Zi * Zi;
	}
	Mxx /= data.n;
	Myy /= data.n;
	Mxy /= data.n;
	Mxz /= data.n;
	Myz /= data.n;
	Mzz /= data.n;

	//    computing the coefficients of the characteristic polynomial

	Mz = Mxx + Myy;
	Cov_xy = Mxx * Myy - Mxy * Mxy;
	Var_z = Mzz - Mz * Mz;

	A2 = Four * Cov_xy - Three * Mz * Mz - Mzz;
	A1 = Var_z * Mz + Four * Cov_xy * Mz - Mxz * Mxz - Myz * Myz;
	A0 = Mxz * (Mxz * Myy - Myz * Mxy) + Myz * (Myz * Mxx - Mxz * Mxy) - Var_z * Cov_xy;
	A22 = A2 + A2;

	//    finding the root of the characteristic polynomial
	//    using Newton's method starting at x=0
	//     (it is guaranteed to converge to the right root)

	for (x = 0., y = A0, iter = 0; iter < IterMAX; iter++)  // usually, 4-6 iterations are enough
	{
		Dy = A1 + x * (A22 + 16. * x * x);
		xnew = x - y / Dy;
		if ((xnew == x) || (!isfinite(xnew))) break;
		ynew = A0 + xnew * (A1 + xnew * (A2 + Four * xnew * xnew));
		if (abs(ynew) >= abs(y))  break;
		x = xnew;  y = ynew;
	}

	//    computing parameters of the fitting circle

	DET = x * x - x * Mz + Cov_xy;
	Xcenter = (Mxz * (Myy - x) - Myz * Mxy) / DET / Two;
	Ycenter = (Myz * (Mxx - x) - Mxz * Mxy) / DET / Two;

	//       assembling the output

	circle.a = Xcenter + data.meanX;
	circle.b = Ycenter + data.meanY;
	circle.r = sqrt(Xcenter * Xcenter + Ycenter * Ycenter + Mz - x - x);
	circle.s = Sigma(data, circle);
	circle.i = 0;
	circle.j = iter;  //  return the number of iterations, too

	return circle;
}

SCircle SMatInv::CircleFitByPratt(SData& data) {
	/*
	Circle fit to a given set of data points (in 2D)

	This is an algebraic fit, due to Pratt, based on the journal article

	V. Pratt, "Direct least-squares fitting of algebraic surfaces",
	Computer Graphics, Vol. 21, pages 145-152 (1987)

	Input:  data     - the class of data (contains the given points):

	data.n   - the number of data points
	data.X[] - the array of X-coordinates
	data.Y[] - the array of Y-coordinates

	Output:
	circle - parameters of the fitting circle:

	circle.a - the X-coordinate of the center of the fitting circle
	circle.b - the Y-coordinate of the center of the fitting circle
	circle.r - the radius of the fitting circle
	circle.s - the root mean square error (the estimate of sigma)
	circle.j - the total number of iterations

	The method is based on the minimization of the function

	F = sum [(x-a)^2 + (y-b)^2 - R^2]^2 / R^2

	This method is more balanced than the simple Kasa fit.

	It works well whether data points are sampled along an entire circle or
	along a small arc.

	It still has a small bias and its statistical accuracy is slightly
	lower than that of the geometric fit (minimizing geometric distances).

	It provides a good initial guess for a subsequent geometric fit.

	Nikolai Chernov  (September 2012)

	*/
	int i, iter, IterMAX = 99;

	double Xi, Yi, Zi;
	double Mz, Mxy, Mxx, Myy, Mxz, Myz, Mzz, Cov_xy, Var_z;
	double A0, A1, A2, A22;
	double Dy, xnew, x, ynew, y;
	double DET, Xcenter, Ycenter;

	SCircle circle;

	data.means();   // Compute x- and y- sample means (via a function in the class "data")

					//     computing moments

	Mxx = Myy = Mxy = Mxz = Myz = Mzz = 0.;

	for (i = 0; i < data.n; i++) {
		Xi = data.X[i] - data.meanX;   //  centered x-coordinates
		Yi = data.Y[i] - data.meanY;   //  centered y-coordinates
		Zi = Xi * Xi + Yi * Yi;

		Mxy += Xi * Yi;
		Mxx += Xi * Xi;
		Myy += Yi * Yi;
		Mxz += Xi * Zi;
		Myz += Yi * Zi;
		Mzz += Zi * Zi;
	}
	Mxx /= data.n;
	Myy /= data.n;
	Mxy /= data.n;
	Mxz /= data.n;
	Myz /= data.n;
	Mzz /= data.n;

	//    computing coefficients of the characteristic polynomial

	Mz = Mxx + Myy;
	Cov_xy = Mxx * Myy - Mxy * Mxy;
	Var_z = Mzz - Mz * Mz;

	A2 = Four * Cov_xy - Three * Mz * Mz - Mzz;
	A1 = Var_z * Mz + Four * Cov_xy * Mz - Mxz * Mxz - Myz * Myz;
	A0 = Mxz * (Mxz * Myy - Myz * Mxy) + Myz * (Myz * Mxx - Mxz * Mxy) - Var_z * Cov_xy;
	A22 = A2 + A2;

	//    finding the root of the characteristic polynomial
	//    using Newton's method starting at x=0
	//     (it is guaranteed to converge to the right root)

	for (x = 0., y = A0, iter = 0; iter < IterMAX; iter++)  // usually, 4-6 iterations are enough
	{
		Dy = A1 + x * (A22 + 16. * x * x);
		xnew = x - y / Dy;
		if ((xnew == x) || (!isfinite(xnew))) break;
		ynew = A0 + xnew * (A1 + xnew * (A2 + Four * xnew * xnew));
		if (abs(ynew) >= abs(y))  break;
		x = xnew;  y = ynew;
	}

	//    computing paramters of the fitting circle

	DET = x * x - x * Mz + Cov_xy;
	Xcenter = (Mxz * (Myy - x) - Myz * Mxy) / DET / Two;
	Ycenter = (Myz * (Mxx - x) - Mxz * Mxy) / DET / Two;

	//       assembling the output

	circle.a = Xcenter + data.meanX;
	circle.b = Ycenter + data.meanY;
	circle.r = sqrt(Xcenter * Xcenter + Ycenter * Ycenter + Mz + x + x);
	circle.s = Sigma(data, circle);
	circle.i = 0;
	circle.j = iter;  //  return the number of iterations, too

	return circle;
}

SCircle SMatInv::CircleFitByKasa(SData& data) {
	/*
	Circle fit to a given set of data points (in 2D)

	This is an algebraic fit, disovered and rediscovered by many people.
	One of the earliest publications is due to Kasa:

	I. Kasa, "A curve fitting procedure and its error analysis",
	IEEE Trans. Inst. Meas., Vol. 25, pages 8-14, (1976)

	Input:  data     - the class of data (contains the given points):

	data.n   - the number of data points
	data.X[] - the array of X-coordinates
	data.Y[] - the array of Y-coordinates

	Output:
	circle - parameters of the fitting circle:

	circle.a - the X-coordinate of the center of the fitting circle
	circle.b - the Y-coordinate of the center of the fitting circle
	circle.r - the radius of the fitting circle
	circle.s - the root mean square error (the estimate of sigma)
	circle.j - the total number of iterations

	The method is based on the minimization of the function

	F = sum [(x-a)^2 + (y-b)^2 - R^2]^2

	This is perhaps the simplest and fastest circle fit.

	It works well when data points are sampled along an entire circle
	or a large part of it (at least half circle).

	It does not work well when data points are sampled along a small arc
	of a circle. In that case the method is heavily biased, it returns
	circles that are too often too small.

	It is the oldest algebraic circle fit (first published in 1972?).
	For 20-30 years it has been the most popular circle fit, at least
	until the more robust Pratt fit (1987) and Taubin fit (1991) were invented.

	Nikolai Chernov  (September 2012)
	*/
	int i;

	double Xi, Yi, Zi;
	double Mxy, Mxx, Myy, Mxz, Myz;
	double B, C, G11, G12, G22, D1, D2;

	SCircle circle;

	data.means();   // Compute x- and y- sample means (via a function in the class "data")

					//     computing moments

	Mxx = Myy = Mxy = Mxz = Myz = 0.;

	for (i = 0; i < data.n; i++) {
		Xi = data.X[i] - data.meanX;   //  centered x-coordinates
		Yi = data.Y[i] - data.meanY;   //  centered y-coordinates
		Zi = Xi * Xi + Yi * Yi;

		Mxx += Xi * Xi;
		Myy += Yi * Yi;
		Mxy += Xi * Yi;
		Mxz += Xi * Zi;
		Myz += Yi * Zi;
	}
	Mxx /= data.n;
	Myy /= data.n;
	Mxy /= data.n;
	Mxz /= data.n;
	Myz /= data.n;

	//    solving system of equations by Cholesky factorization

	G11 = sqrt(Mxx);
	G12 = Mxy / G11;
	G22 = sqrt(Myy - G12 * G12);

	D1 = Mxz / G11;
	D2 = (Myz - D1 * G12) / G22;

	//    computing parameters of the fitting circle

	C = D2 / G22 / Two;
	B = (D1 - G12 * C) / G11 / Two;

	//       assembling the output

	circle.a = B + data.meanX;
	circle.b = C + data.meanY;
	circle.r = sqrt(B * B + C * C + Mxx + Myy);
	circle.s = Sigma(data, circle);
	circle.i = 0;
	circle.j = 0;

	return circle;
}

SCircle SMatInv::CircleFitByTaubin(SData& data) {
	/*
	Circle fit to a given set of data points (in 2D)

	This is an algebraic fit, due to Taubin, based on the journal article

	G. Taubin, "Estimation Of Planar Curves, Surfaces And Nonplanar
	Space Curves Defined By Implicit Equations, With
	Applications To Edge And Range Image Segmentation",
	IEEE Trans. PAMI, Vol. 13, pages 1115-1138, (1991)

	Input:  data     - the class of data (contains the given points):

	data.n   - the number of data points
	data.X[] - the array of X-coordinates
	data.Y[] - the array of Y-coordinates

	Output:
	circle - parameters of the fitting circle:

	circle.a - the X-coordinate of the center of the fitting circle
	circle.b - the Y-coordinate of the center of the fitting circle
	circle.r - the radius of the fitting circle
	circle.s - the root mean square error (the estimate of sigma)
	circle.j - the total number of iterations

	The method is based on the minimization of the function

	sum [(x-a)^2 + (y-b)^2 - R^2]^2
	F = -------------------------------
	sum [(x-a)^2 + (y-b)^2]

	This method is more balanced than the simple Kasa fit.

	It works well whether data points are sampled along an entire circle or
	along a small arc.

	It still has a small bias and its statistical accuracy is slightly
	lower than that of the geometric fit (minimizing geometric distances),
	but slightly higher than that of the very similar Pratt fit.
	Besides, the Taubin fit is slightly simpler than the Pratt fit

	It provides a very good initial guess for a subsequent geometric fit.

	Nikolai Chernov  (September 2012)

	*/
	int i, iter, IterMAX = 99;

	double Xi, Yi, Zi;
	double Mz, Mxy, Mxx, Myy, Mxz, Myz, Mzz, Cov_xy, Var_z;
	double A0, A1, A2, A22, A3, A33;
	double Dy, xnew, x, ynew, y;
	double DET, Xcenter, Ycenter;

	SCircle circle;

	data.means();   // Compute x- and y- sample means (via a function in the class "data")

					//     computing moments

	Mxx = Myy = Mxy = Mxz = Myz = Mzz = 0.;

	for (i = 0; i < data.n; i++) {
		Xi = data.X[i] - data.meanX;   //  centered x-coordinates
		Yi = data.Y[i] - data.meanY;   //  centered y-coordinates
		Zi = Xi * Xi + Yi * Yi;

		Mxy += Xi * Yi;
		Mxx += Xi * Xi;
		Myy += Yi * Yi;
		Mxz += Xi * Zi;
		Myz += Yi * Zi;
		Mzz += Zi * Zi;
	}
	Mxx /= data.n;
	Myy /= data.n;
	Mxy /= data.n;
	Mxz /= data.n;
	Myz /= data.n;
	Mzz /= data.n;

	//      computing coefficients of the characteristic polynomial

	Mz = Mxx + Myy;
	Cov_xy = Mxx * Myy - Mxy * Mxy;
	Var_z = Mzz - Mz * Mz;
	A3 = Four * Mz;
	A2 = -Three * Mz * Mz - Mzz;
	A1 = Var_z * Mz + Four * Cov_xy * Mz - Mxz * Mxz - Myz * Myz;
	A0 = Mxz * (Mxz * Myy - Myz * Mxy) + Myz * (Myz * Mxx - Mxz * Mxy) - Var_z * Cov_xy;
	A22 = A2 + A2;
	A33 = A3 + A3 + A3;

	//    finding the root of the characteristic polynomial
	//    using Newton's method starting at x=0
	//     (it is guaranteed to converge to the right root)

	for (x = 0., y = A0, iter = 0; iter < IterMAX; iter++)  // usually, 4-6 iterations are enough
	{
		Dy = A1 + x * (A22 + A33 * x);
		xnew = x - y / Dy;
		if ((xnew == x) || (!isfinite(xnew))) break;
		ynew = A0 + xnew * (A1 + xnew * (A2 + xnew * A3));
		if (abs(ynew) >= abs(y))  break;
		x = xnew;  y = ynew;
	}

	//       computing paramters of the fitting circle

	DET = x * x - x * Mz + Cov_xy;
	Xcenter = (Mxz * (Myy - x) - Myz * Mxy) / DET / Two;
	Ycenter = (Myz * (Mxx - x) - Mxz * Mxy) / DET / Two;

	//       assembling the output

	circle.a = Xcenter + data.meanX;
	circle.b = Ycenter + data.meanY;
	circle.r = sqrt(Xcenter * Xcenter + Ycenter * Ycenter + Mz);
	circle.s = Sigma(data, circle);
	circle.i = 0;
	circle.j = iter;  //  return the number of iterations, too

	return circle;
}

int SMatInv::CircleFitByLevenbergMarquardtFull(
	SData& data, SCircle& circleIni, double LambdaIni, SCircle& circle) {
	/*
	<------------------ Input ------------------->  <-- Output -->

	Geometric circle fit to a given set of data points (in 2D)

	Input:  data     - the class of data (contains the given points):

	data.n   - the number of data points
	data.X[] - the array of X-coordinates
	data.Y[] - the array of Y-coordinates

	circleIni - parameters of the initial circle ("initial guess")

	circleIni.a - the X-coordinate of the center of the initial circle
	circleIni.b - the Y-coordinate of the center of the initial circle
	circleIni.r - the radius of the initial circle

	LambdaIni - the initial value of the control parameter "lambda"
	for the Levenberg-Marquardt procedure
	(common choice is a small positive number, e.g. 0.001)

	Output:
	integer function value is a code:
	0:  normal termination, the best fitting circle is
	successfully found
	1:  the number of outer iterations exceeds the limit (99)
	(indicator of a possible divergence)
	2:  the number of inner iterations exceeds the limit (99)
	(another indicator of a possible divergence)
	3:  the coordinates of the center are too large
	(a strong indicator of divergence)

	circle - parameters of the fitting circle ("best fit")

	circle.a - the X-coordinate of the center of the fitting circle
	circle.b - the Y-coordinate of the center of the fitting circle
	circle.r - the radius of the fitting circle
	circle.s - the root mean square error (the estimate of sigma)
	circle.i - the total number of outer iterations (updating the parameters)
	circle.j - the total number of inner iterations (adjusting lambda)

	Algorithm:  Levenberg-Marquardt running over the full parameter space (a,b,r)

	See a detailed description in Section 4.5 of the book by Nikolai Chernov:
	"Circular and linear regression: Fitting circles and lines by least squares"
	Chapman & Hall/CRC, Monographs on Statistics and Applied Probability, volume 117, 2010.

	Nikolai Chernov,  February 2014
	*/
	int code, i, iter, inner, IterMAX = 99;

	double factorUp = 10., factorDown = 0.04, lambda, ParLimit = 1.e+6;
	double dx, dy, ri, u, v;
	double Mu, Mv, Muu, Mvv, Muv, Mr, UUl, VVl, Nl, F1, F2, F3, dX, dY, dR;
	double epsilon = 3.e-8;
	double G11, G22, G33, G12, G13, G23, D1, D2, D3;

	SCircle Old, New;

	//       starting with the given initial circle (initial guess)

	New = circleIni;

	//       compute the root-mean-square error via function Sigma; see Utilities.cpp

	New.s = Sigma(data, New);

	//       initializing lambda, iteration counters, and the exit code

	lambda = LambdaIni;
	iter = inner = code = 0;

NextIteration:

	Old = New;
	if (++iter > IterMAX) { code = 1;  goto enough; }

	//       computing moments

	Mu = Mv = Muu = Mvv = Muv = Mr = 0.;

	for (i = 0; i < data.n; i++) {
		dx = data.X[i] - Old.a;
		dy = data.Y[i] - Old.b;
		ri = sqrt(dx * dx + dy * dy);
		u = dx / ri;
		v = dy / ri;
		Mu += u;
		Mv += v;
		Muu += u * u;
		Mvv += v * v;
		Muv += u * v;
		Mr += ri;
	}
	Mu /= data.n;
	Mv /= data.n;
	Muu /= data.n;
	Mvv /= data.n;
	Muv /= data.n;
	Mr /= data.n;

	//       computing matrices

	F1 = Old.a + Old.r * Mu - data.meanX;
	F2 = Old.b + Old.r * Mv - data.meanY;
	F3 = Old.r - Mr;

	Old.g = New.g = sqrt(F1 * F1 + F2 * F2 + F3 * F3);

try_again:

	UUl = Muu + lambda;
	VVl = Mvv + lambda;
	Nl = One + lambda;

	//         Cholesly decomposition

	G11 = sqrt(UUl);
	G12 = Muv / G11;
	G13 = Mu / G11;
	G22 = sqrt(VVl - G12 * G12);
	G23 = (Mv - G12 * G13) / G22;
	G33 = sqrt(Nl - G13 * G13 - G23 * G23);

	D1 = F1 / G11;
	D2 = (F2 - G12 * D1) / G22;
	D3 = (F3 - G13 * D1 - G23 * D2) / G33;

	dR = D3 / G33;
	dY = (D2 - G23 * dR) / G22;
	dX = (D1 - G12 * dY - G13 * dR) / G11;

	if ((abs(dR) + abs(dX) + abs(dY)) / (One + Old.r) < epsilon) goto enough;

	//       updating the parameters

	New.a = Old.a - dX;
	New.b = Old.b - dY;

	if (abs(New.a) > ParLimit || abs(New.b) > ParLimit) { code = 3; goto enough; }

	New.r = Old.r - dR;

	if (New.r <= 0.) {
		lambda *= factorUp;
		if (++inner > IterMAX) { code = 2;  goto enough; }
		goto try_again;
	}

	//       compute the root-mean-square error via function Sigma; see Utilities.cpp

	New.s = Sigma(data, New);

	//       check if improvement is gained

	if (New.s < Old.s)    //   yes, improvement
	{
		lambda *= factorDown;
		goto NextIteration;
	}
	else                       //   no improvement
	{
		if (++inner > IterMAX) { code = 2;  goto enough; }
		lambda *= factorUp;
		goto try_again;
	}

	//       exit

enough:

	Old.i = iter;    // total number of outer iterations (updating the parameters)
	Old.j = inner;   // total number of inner iterations (adjusting lambda)

	circle = Old;

	return code;
}

int SMatInv::CircleFitByLevenbergMarquardtReduced(SData& data, SCircle& circleIni, double LambdaIni, SCircle& circle) {
	/*
	<------------------ Input ------------------->  <-- Output -->

	Geometric circle fit to a given set of data points (in 2D)

	Input:  data     - the class of data (contains the given points):

	data.n   - the number of data points
	data.X[] - the array of X-coordinates
	data.Y[] - the array of Y-coordinates

	circleIni - parameters of the initial circle ("initial guess")

	circleIni.a - the X-coordinate of the center of the initial circle
	circleIni.b - the Y-coordinate of the center of the initial circle
	circleIni.r - the radius of the initial circle

	LambdaIni - the initial value of the control parameter "lambda"
	for the Levenberg-Marquardt procedure
	(common choice is a small positive number, e.g. 0.001)

	Output:
	integer function value is a code:
	0:  normal termination, the best fitting circle is
	successfully found
	1:  the number of outer iterations exceeds the limit (99)
	(indicator of a possible divergence)
	2:  the number of inner iterations exceeds the limit (99)
	(another indicator of a possible divergence)
	3:  the coordinates of the center are too large
	(a strong indicator of divergence)

	circle - parameters of the fitting circle ("best fit")

	circle.a - the X-coordinate of the center of the fitting circle
	circle.b - the Y-coordinate of the center of the fitting circle
	circle.r - the radius of the fitting circle
	circle.s - the root mean square error (the estimate of sigma)
	circle.i - the total number of outer iterations (updating the parameters)
	circle.j - the total number of inner iterations (adjusting lambda)

	Algorithm:  Levenberg-Marquardt running over the "reduced" parameter space (a,b)
	(the radius r is always set to its optimal value)

	See a detailed description in Section 4.6 of the book by Nikolai Chernov:
	"Circular and linear regression: Fitting circles and lines by least squares"
	Chapman & Hall/CRC, Monographs on Statistics and Applied Probability, volume 117, 2010.

	Nikolai Chernov,  February 2014
	*/
	int code, i, pivot, iter, inner, IterMAX = 99;

	double factorUp = 10., factorDown = 0.04, lambda, ParLimit = 1.e+6;
	double dx, dy, ri, u, v;
	double Mu, Mv, Muu, Mvv, Muv, Mr, A11, A12, A22, F1, F2, dX, dY;
	double epsilon = 3.e-8;
	double G11, G12, G22, D1, D2;

	SCircle Old, New;

	data.means();   // Compute x- and y-means (via a function in class "data")

					//       starting with the given initial circle (initial guess)

	New = circleIni;

	//     compute the root-mean-square error via function SigmaReduced; see Utilities.cpp

	New.s = SigmaReduced(data, New);

	//     initializing lambda, iteration counters, and the exit code

	lambda = LambdaIni;
	iter = inner = code = 0;

NextIteration:

	Old = New;
	if (++iter > IterMAX) { code = 1;  goto enough; }

	//     computing moments

	Mu = Mv = Muu = Mvv = Muv = Mr = 0.;

	for (i = 0; i < data.n; i++) {
		dx = data.X[i] - Old.a;
		dy = data.Y[i] - Old.b;
		ri = sqrt(dx * dx + dy * dy);
		u = dx / ri;
		v = dy / ri;
		Mu += u;
		Mv += v;
		Muu += u * u;
		Mvv += v * v;
		Muv += u * v;
		Mr += ri;
	}
	Mu /= data.n;
	Mv /= data.n;
	Muu /= data.n;
	Mvv /= data.n;
	Muv /= data.n;
	Mr /= data.n;

	//    computing matrices

	F1 = Old.a + Mu * Mr - data.meanX;
	F2 = Old.b + Mv * Mr - data.meanY;

try_again:

	A11 = (Muu - Mu * Mu) + lambda;
	A22 = (Mvv - Mv * Mv) + lambda;
	A12 = Muv - Mu * Mv;

	if (A11 < epsilon || A22 < epsilon || A11 * A22 - A12 * A12 < epsilon) {
		lambda *= factorUp;
		goto try_again;
	}

	//      Cholesky decomposition with pivoting

	pivot = 0;
	if (A11 < A22) {
		swap(A11, A22);
		swap(F1, F2);
		pivot = 1;
	}

	G11 = sqrt(A11);
	G12 = A12 / G11;
	G22 = sqrt(A22 - G12 * G12);

	D1 = F1 / G11;
	D2 = (F2 - G12 * D1) / G22;

	//    updating the parameters

	dY = D2 / G22;
	dX = (D1 - G12 * dY) / G11;

	if ((abs(dX) + abs(dY)) / (One + abs(Old.a) + abs(Old.b)) < epsilon) goto enough;

	if (pivot != 0) swap(dX, dY);

	New.a = Old.a - dX;
	New.b = Old.b - dY;

	if (abs(New.a) > ParLimit || abs(New.b) > ParLimit) { code = 3; goto enough; }

	//    compute the root-mean-square error via function SigmaReduced; see Utilities.cpp

	New.s = SigmaReduced(data, New);

	//      check if improvement is gained

	if (New.s < Old.s)    //    yes, improvement
	{
		lambda *= factorDown;
		goto NextIteration;
	}
	else                  //  no improvement
	{
		if (++inner > IterMAX) { code = 2;  goto enough; }
		lambda *= factorUp;
		goto try_again;
	}

	//       exit

enough:

	//    compute the optimal radius via a function in Utilities.cpp

	Old.r = OptimalRadius(data, Old);
	Old.i = iter;
	Old.j = inner;

	circle = Old;

	return code;
}

int SMatInv::CircleFitByChernovLesort(SData& data, SCircle& circleIni, double LambdaIni, SCircle& circle) {
	/*
	<------------------ Input ------------------->  <-- Output -->

	Geometric circle fit to a given set of data points (in 2D)

	Input:  data     - the class of data (contains the given points):

	data.n   - the number of data points
	data.X[] - the array of X-coordinates
	data.Y[] - the array of Y-coordinates

	circleIni - parameters of the initial circle ("initial guess")

	circleIni.a - the X-coordinate of the center of the initial circle
	circleIni.b - the Y-coordinate of the center of the initial circle
	circleIni.r - the radius of the initial circle

	LambdaIni - the initial value of the control parameter "lambda"
	for the Levenberg-Marquardt procedure
	(common choice is a small positive number, e.g. 0.001)

	Output:
	integer function value is a code:
	0:  normal termination, the best fitting circle is
	successfully found
	1:  the number of outer iterations exceeds the limit (99)
	(indicator of a possible divergence)
	2:  the number of inner iterations exceeds the limit (99)
	(another indicator of a possible divergence)
	3:  the coordinates of the center are too large
	(a strong indicator of divergence)

	circle - parameters of the fitting circle ("best fit")

	circle.a - the X-coordinate of the center of the fitting circle
	circle.b - the Y-coordinate of the center of the fitting circle
	circle.r - the radius of the fitting circle
	circle.s - the root mean square error (the estimate of sigma)
	circle.i - the total number of outer iterations (updating the parameters)
	circle.j - the total number of inner iterations (adjusting lambda)

	Algorithm by Nikolai Chernov and Claire Lesort

	See a detailed description in the journal paper:

	N. Chernov and C. Lesort, "Least squares fitting of circles"
	in J. Math. Imag. Vision, volume 23, (2005), pages 239-251.

	the algorithm is designed to converge from any initial guess,
	but it is complicated and generally very slow

	Nikolai Chernov,  February 2014
	*/
	int code, i, iter, inner, IterMAX = 99;

	double factorUp = 10., factorDown = 0.04, lambda;
	double Aold, Fold, Told, Anew, Fnew, Tnew, DD, H, aabb;
	double Xi, Yi, Zi, Ui, Vi, Gi, CT, ST, D, ADF, SQ, DEN, FACT, DGDAi, DGDFi, DGDTi;
	double H11, H12, H13, H22, H23, H33, F1, F2, F3, dA, dF, dT;
	double epsilon = 3.e-8;
	double G11, G22, G33, G12, G13, G23, D1, D2, D3;
	double Xshift = 0., Yshift = 0., dX = One, dY = 0., aTemp, bTemp, rTemp;

	SCircle Old, New;

	//       starting with the given initial circle (initial guess)

	New = circleIni;

	//       compute the root-mean-square error via function Sigma; see Utilities.cpp

	New.s = Sigma(data, New);

	Anew = One / Two / New.r;
	aabb = New.a * New.a + New.b * New.b;
	Fnew = (aabb - New.r * New.r) * Anew;
	Tnew = acos(-New.a / sqrt(aabb));
	if (New.b > 0.) Tnew = Two * Pi - Tnew;

	if (One + Four * Anew * Fnew < epsilon) {
		Xshift += dX;
		Yshift += dY;

		New.a += dX;
		New.b += dY;
		aabb = New.a * New.a + New.b * New.b;
		Fnew = (aabb - New.r * New.r) * Anew;
		Tnew = acos(-New.a / sqrt(aabb));
		if (New.b > 0.) Tnew = Two * Pi - Tnew;
	}

	//       initializing lambda, iteration counters, and the exit code

	lambda = LambdaIni;
	iter = inner = code = 0;

NextIteration:

	Aold = Anew;
	Fold = Fnew;
	Told = Tnew;
	Old = New;

	if (++iter > IterMAX) { code = 1;  goto enough; }

	//       computing moments

shiftXY:

	DD = One + Four * Aold * Fold;
	D = sqrt(DD);
	CT = cos(Told);
	ST = sin(Told);

	H11 = H12 = H13 = H22 = H23 = H33 = F1 = F2 = F3 = 0.;

	for (i = 0; i < data.n; i++) {
		Xi = data.X[i] + Xshift;
		Yi = data.Y[i] + Yshift;
		Zi = Xi * Xi + Yi * Yi;
		Ui = Xi * CT + Yi * ST;
		Vi = -Xi * ST + Yi * CT;

		ADF = Aold * Zi + D * Ui + Fold;
		SQ = sqrt(Four * Aold * ADF + One);
		DEN = SQ + One;
		Gi = Two * ADF / DEN;
		FACT = Two / DEN * (One - Aold * Gi / SQ);
		DGDAi = FACT * (Zi + Two * Fold * Ui / D) - Gi * Gi / SQ;
		DGDFi = FACT * (Two * Aold * Ui / D + One);
		DGDTi = FACT * D * Vi;

		H11 += DGDAi * DGDAi;
		H12 += DGDAi * DGDFi;
		H13 += DGDAi * DGDTi;
		H22 += DGDFi * DGDFi;
		H23 += DGDFi * DGDTi;
		H33 += DGDTi * DGDTi;

		F1 += Gi * DGDAi;
		F2 += Gi * DGDFi;
		F3 += Gi * DGDTi;
	}
	Old.g = New.g = sqrt(F1 * F1 + F2 * F2 + F3 * F3);

try_again:

	//        Cholesky decomposition

	G11 = sqrt(H11 + lambda);
	G12 = H12 / G11;
	G13 = H13 / G11;
	G22 = sqrt(H22 + lambda - G12 * G12);
	G23 = (H23 - G12 * G13) / G22;
	G33 = sqrt(H33 + lambda - G13 * G13 - G23 * G23);

	D1 = F1 / G11;
	D2 = (F2 - G12 * D1) / G22;
	D3 = (F3 - G13 * D1 - G23 * D2) / G33;

	dT = D3 / G33;
	dF = (D2 - G23 * dT) / G22;
	dA = (D1 - G12 * dF - G13 * dT) / G11;

	//       updating the parameters

	Anew = Aold - dA;
	Fnew = Fold - dF;
	Tnew = Told - dT;

	if (One + Four * Anew * Fnew < epsilon) {
		Xshift += dX;
		Yshift += dY;

		H = sqrt(One + Four * Aold * Fold);
		aTemp = -H * cos(Told) / (Aold + Aold) + dX;
		bTemp = -H * sin(Told) / (Aold + Aold) + dY;
		rTemp = One / abs(Aold + Aold);

		Aold = One / (rTemp + rTemp);
		aabb = aTemp * aTemp + bTemp * bTemp;
		Fold = (aabb - rTemp * rTemp) * Aold;
		Told = acos(-aTemp / sqrt(aabb));
		if (bTemp > 0.) Told = Two * Pi - Told;

		lambda *= factorUp;
		inner++;
		goto shiftXY;
	}

	H = sqrt(One + Four * Anew * Fnew);
	New.a = -H * cos(Tnew) / (Anew + Anew) - Xshift;
	New.b = -H * sin(Tnew) / (Anew + Anew) - Yshift;
	New.r = One / abs(Anew + Anew);
	New.s = Sigma(data, New);

	if ((abs(New.a - Old.a) + abs(New.b - Old.b) + abs(New.r - Old.r)) / (One + Old.r) < epsilon) goto enough;

	//       check if improvement is gained

	if (New.s < Old.s)    //   yes, improvement
	{
		lambda *= factorDown;
		goto NextIteration;
	}
	else                       //   no improvement
	{
		if (++inner > IterMAX) { code = 2;  goto enough; }
		lambda *= factorUp;
		goto try_again;
	}

	//       exit

enough:

	Old.i = iter;
	Old.j = inner;

	circle = Old;

	return code;
}

void SMatInv::eigen2x2(double a, double b, double c, double& d1, double& d2, double& Vx, double& Vy) {
	/*            <------- Input ----------> <--------------- Output ----------------->

	Eigendecomposition of a symmetric 2x2 matrix
	faster and more accurate than the library function

	Input:  a,b,c - components of the matrix [a c
	c b]
	Output:  d1,d2 - eigenvalues
	Vx,Vy - unit eigenvector for d1

	The eigenvector for d2 need not be computed, it is (-Vy,Vx)

	Note:  d1 is the leading eigenvalue, i.e., |d1|>=|d2|

	Nikolai Chernov,  June 2012

	*/
	double disc, f;

	disc = pythag1(a - b, Two * c);    // discriminant

	d1 = (a + b > 0.) ? (a + b + disc) / Two : (a + b - disc) / Two;
	d2 = (a * b - c * c) / d1;

	if (abs(a - d1) > abs(b - d1)) {
		if ((f = pythag1(c, d1 - a)) == 0.) {
			Vx = One; Vy = 0.;  return;
		}
		else {
			Vx = c / f;  Vy = (d1 - a) / f;
		}
	}
	else {
		if ((f = pythag1(c, d1 - b)) == 0.) {
			Vx = One; Vy = 0.;  return;
		}
		else {
			Vx = (d1 - b) / f;  Vy = c / f;
		}
	}

	return;
}

double SMatInv::SigmaWithLargeCircleOption(SData& data, SCircle& circle) {
	/*                                <-------- Input --------->

	Compute the objective function	for the geometric circle fitting problem

	Input:  data     - the class of data (contains the given points):

	data.n   - the number of data points
	data.X[] - the array of X-coordinates
	data.Y[] - the array of Y-coordinates
	data.meanX - the mean X-coordinate
	data.meanY - the mean Y-coordinate
	(the last two must be precomputed)

	circle    - the class of circle parameters:

	circle.a - the X-coordinate of the center
	circle.b - the Y-coordinate of the center

	Output:
	the value of the objective function
	(more precisely, the square root of the average square of the distance)

	Nikolai Chernov,  January 2013
	*/
	int i, n = data.n;
	double sum = 0., dx, dy, r;
	double LargeCircle = Ten, a0, b0, del, s, c, x, y, z, p, t, g, W, Z;
	if (abs(circle.a) < LargeCircle && abs(circle.b) < LargeCircle)   // main case (not a large circle)
	{
		double* D = new double[n]; if (!D) return 0;
		for (i = 0; i < n; i++) {
			dx = data.X[i] - circle.a;
			dy = data.Y[i] - circle.b;
			D[i] = sqrt(dx * dx + dy * dy);
			sum += D[i];
		}
		r = sum / n;

		for (sum = 0., i = 0; i < n; i++)  sum += SQR(D[i] - r);
		delete[]D;
		return sum / n;
	}
	else   //  case of a large circle
	{
		a0 = circle.a - data.meanX;  b0 = circle.b - data.meanY;
		del = One / sqrt(a0 * a0 + b0 * b0);
		s = b0 * del;  c = a0 * del;

		for (W = Z = 0., i = 0; i < n; i++) {
			x = data.X[i] - data.meanX;
			y = data.Y[i] - data.meanY;
			z = x * x + y * y;
			p = x * c + y * s;
			t = del * z - Two * p;
			g = t / (One + sqrt(One + del * t));
			W += (z + p * g) / (Two + del * g);
			Z += z;
		}
		W /= n;
		Z /= n;

		return Z - W * (Two + del * del * W);
	}
}

void SMatInv::GradientHessian(SData& data, SCircle& circle, double& F1, double& F2, double& A11, double& A22, double& A12) {
	/*                    <-------- Input --------->  <----------------------- Output ----------------------->

	Compute the gradient vector and the Hessian matrix of the objective function
	for the geometric circle fitting problem

	Input:  data     - the class of data (contains the given points):

	data.n   - the number of data points
	data.X[] - the array of X-coordinates
	data.Y[] - the array of Y-coordinates
	data.meanX - the mean X-coordinate
	data.meanY - the mean Y-coordinate
	(the last two must be precomputed)

	circle    - the class of circle parameters:

	circle.a - the X-coordinate of the center
	circle.b - the Y-coordinate of the center

	Output:
	[F1 F2]  - the coordinates of the gradient vector

	A11 A12  - the components of the Hessian matrix
	A12 A22    (it is symmetric, so only three are computed)

	Nikolai Chernov,  January 2013
	*/
	int i, n = data.n;
	double LargeCircle = Ten, dx, dy, r, u, v, Mr, Mu, Mv, Muu, Mvv, Muv, Muur, Mvvr, Muvr;
	double a0, b0, del, dd, s, c, x, y, a, b, z, p, t, w, g, g1, gg1, gg2;
	double X, Y, R, U, V, T, W, AA, BB, AB, AG, BG, GG, UUR, VVR, UVR;

	if (abs(circle.a) < LargeCircle && abs(circle.b) < LargeCircle)   // main case (not a large circle)
	{
		for (Mr = Mu = Mv = Muu = Mvv = Muv = Muur = Mvvr = Muvr = 0., i = 0; i < n; i++) {
			dx = data.X[i] - circle.a;
			dy = data.Y[i] - circle.b;
			r = sqrt(dx * dx + dy * dy);
			u = dx / r;
			v = dy / r;
			Mr += r;
			Mu += u;
			Mv += v;
			Muu += u * u;
			Mvv += v * v;
			Muv += u * v;
			Muur += u * u / r;
			Mvvr += v * v / r;
			Muvr += u * v / r;
		}
		Mr /= n;
		Mu /= n;
		Mv /= n;
		Muu /= n;
		Mvv /= n;
		Muv /= n;
		Muur /= n;
		Mvvr /= n;
		Muvr /= n;

		F1 = circle.a + Mu * Mr - data.meanX;
		F2 = circle.b + Mv * Mr - data.meanY;

		A11 = One - Mu * Mu - Mr * Mvvr;
		A22 = One - Mv * Mv - Mr * Muur;
		A12 = -Mu * Mv + Mr * Muvr;
	}
	else   //  case of a large circle
	{
		a0 = circle.a - data.meanX;  b0 = circle.b - data.meanY;
		del = One / sqrt(a0 * a0 + b0 * b0);  dd = del * del;
		s = b0 * del;  c = a0 * del;

		for (X = Y = R = T = W = AA = BB = AB = AG = BG = GG = 0., i = 0; i < n; i++) {
			x = data.X[i] - data.meanX;
			y = data.Y[i] - data.meanY;
			z = x * x + y * y;
			p = x * c + y * s;
			t = Two * p - del * z;
			w = sqrt(One - del * t);
			g = -t / (One + w);
			g1 = One / (One + del * g);
			gg1 = g * g1;
			gg2 = g / (Two + del * g);
			a = (x + g * c) / w;
			b = (y + g * s) / w;
			X += x * gg1;
			Y += y * gg1;
			R += z + t * gg2;
			T += t * gg1;
			W += t * gg1 * gg2;
			AA += a * a * g1;
			BB += b * b * g1;
			AB += a * b * g1;
			AG += a * gg1;
			BG += b * gg1;
			GG += g * gg1;
		}
		X /= n;
		Y /= n;
		R /= n;
		T /= n;
		W /= n;
		AA /= n;
		BB /= n;
		AB /= n;
		AG /= n;
		BG /= n;
		GG /= n;

		U = (T - del * W) * c / Two - X + R * c / Two;
		V = (T - del * W) * s / Two - Y + R * s / Two;

		//         compute the components of the gradient vector

		F1 = del * ((dd * R * U - del * W * c + T * c) / Two - X);
		F2 = del * ((dd * R * V - del * W * s + T * s) / Two - Y);

		UUR = ((GG - R / Two) * c + Two * (AG - U)) * c + AA;
		VVR = ((GG - R / Two) * s + Two * (BG - V)) * s + BB;
		UVR = ((GG - R / Two) * c + (AG - U)) * s + (BG - V) * c + AB;

		//         compute the components of the Hessian matrix

		A11 = dd * (U * (Two * c - dd * U) - R * s * s / Two - VVR * (One + dd * R / Two));
		A22 = dd * (V * (Two * s - dd * V) - R * c * c / Two - UUR * (One + dd * R / Two));
		A12 = dd * (U * s + V * c + R * s * c / Two - dd * U * V + UVR * (One + dd * R / Two));
	}
}

BOOL SMatInv::MatrixInversion(int n, double* a) {
	double e;
	int i, j, k;
	for (k = 0; k < n; ++k) {
		e = a[k * n + k];
		a[k * n + k] = 1.0;
		if (e == 0.0) return FALSE;
		for (j = 0; j < n; ++j) {
			a[k * n + j] = a[k * n + j] / e;
		}
		for (i = 0; i < n; ++i) {
			if (i != k) {
				e = a[i * n + k];
				a[i * n + k] = 0.0;
				for (j = 0; j < n; ++j) {
					a[i * n + j] = a[i * n + j] - e * a[k * n + j];
				}
			}
		}
	}
	return TRUE;
}

int SMatInv::CircleFitByChernovHoussam(SData& data, SCircle& circleIni, double LambdaIni, SCircle& circle) {
	/*
	<------------------ Input ------------------->  <-- Output -->

	Geometric circle fit to a given set of data points (in 2D)

	Input:  data     - the class of data (contains the given points):

	data.n   - the number of data points
	data.X[] - the array of X-coordinates
	data.Y[] - the array of Y-coordinates

	circleIni - parameters of the initial circle ("initial guess")

	circleIni.a - the X-coordinate of the center of the initial circle
	circleIni.b - the Y-coordinate of the center of the initial circle
	circleIni.r - the radius of the initial circle

	LambdaIni - the initial value of the control parameter "lambda"
	for the Levenberg-Marquardt procedure
	(common choice is a small positive number, e.g. 0.001)

	Output:
	integer function value is a code:
	0:  normal termination, the best fitting circle is
	successfully found
	1:  the number of outer iterations exceeds the limit (99)
	(indicator of a possible divergence)
	2:  the number of inner iterations exceeds the limit (99)
	(another indicator of a possible divergence)
	3:  convergence to a point where the Hessian matrix
	is NOT positive definite. This indicates that
	the fitting circle may correspond to a maximum
	or to a saddle point of the objective function

	circle - parameters of the fitting circle ("best fit")

	circle.a - the X-coordinate of the center of the fitting circle
	circle.b - the Y-coordinate of the center of the fitting circle
	circle.r - the radius of the fitting circle
	circle.s - the root mean square error (the estimate of sigma)
	circle.i - the total number of outer iterations (updating the parameters)
	circle.j - the total number of inner iterations (adjusting lambda)

	Algorithm is based on the paper by H. Abdul-Rahman and N. Chernov
	"Fast and numerically stable circle fit"
	in Journal of Mathematical Imaging and Vision (2013)

	Nikolai Chernov,  January 2013
	*/
	int i, n = data.n, iter, inner, IterMAX = 200, check_line = 1, code;

	double lambda;
	double F1, F2, A11, A22, A12, dX, dY, Mxx, Myy, Mxy, Mxxy, dx, dy;
	double d1, d2, dmin = One, Vx, Vy, dL1, dL2, VLx, VLy, aL, bL, R, G1, G2, sBest, gBest, AB, progress;

	//          control parameters (have been optimized empirically):

	double ParLimit2 = 100.;
	double epsilon = 1.e+7 * REAL_EPSILON;;
	double factor1 = 32., factor2 = 32.;
	double ccc = 0.4;
	double factorUp = 10., factorDown = 0.1;

	SCircle Old, New;

	data.means();   // Compute x- and y-means (via a function in class "data")

					//    starting with the given initial guess

	New = circleIni;            //  initialize the circle
	New.s = SigmaWithLargeCircleOption(data, New);    //  compute the root-mean-square error
	GradientHessian(data, New, F1, F2, A11, A22, A12);  // compute the gradient vector and Hessian matrix
	New.Gx = F1;  New.Gy = F2;   New.g = sqrt(F1 * F1 + F2 * F2);   //  the gradient vector and its norm

	lambda = LambdaIni;         //    initialize lambda
	iter = inner = code = 0;    //    initialize iteration counters and the exit code
	sBest = gBest = progress = REAL_MAX;   //  set control variables to LARGE values
										   //if (lpr==1) std::cout << iter <<"  ("<<New.a<<","<<New.b<<")  s="<<New.s<<"  g="<< New.g<<"  L="<<lambda << endl;

NextIteration:   //  starting point of the current iteration of Newton's method

	if (iter > 0) progress = (abs(New.a - Old.a) + abs(New.b - Old.b)) / (SQR(Old.a) + SQR(Old.b) + One);
	// evaluate the progress made during the previous iteration
	Old = New;
	if (++iter > IterMAX) goto enough;   //  termination due to going over the limit

	eigen2x2(A11, A22, A12, d1, d2, Vx, Vy);  //  eigendecomposition of the Hessian matrix
	dmin = (d1 < d2) ? d1 : d2;          //  recording the smaller e-value

	AB = sqrt(SQR(Old.a) + SQR(Old.b)) + One;   //  approximation to the circle size

												//     main stopping rule: terminate iterations if
												//          the gradient vector is small enough and the progress is not substantial
	if ((Old.g < factor1 * REAL_EPSILON) && (progress < epsilon)) {
		//if (lpr==1) std::cout << "++++ gradient is small enough ++++" << endl;
		goto enough;
	}

	//     secondary stopping rule (prevents some stupid cycling)
	if ((Old.s >= sBest) && (Old.g >= gBest)) {
		//if (lpr==1) std::cout << "++++ iterations stabilize (best results repeated) ++++" << endl;
		goto enough;
	}

	if (sBest > Old.s) sBest = Old.s;  //  updating the smallest value of the objective function found so far
	if (gBest > Old.g) gBest = Old.g;  //  updating the smallest length of the gradient vector found so far

	G1 = Vx * F1 + Vy * F2;  //  rotating the gradient vector
	G2 = Vx * F2 - Vy * F1;  //  (expressing it in the eigensystem of the Hessian matrix)

try_again:   //  starting point of an "inner" iteration (adjusting lambda)

			 //           enforcing a lower bound on lambda that guarantees that
			 //           (i)  the augmented Hessian matrix is positive definite
			 //           (ii) the step is not too big (does not exceed a certain fraction of the circle size)
			 //                                         the fraction is defined by the factor "ccc")
	if (lambda < abs(G1) / AB / ccc - d1)  lambda = abs(G1) / AB / ccc - d1;
	if (lambda < abs(G2) / AB / ccc - d2)  lambda = abs(G2) / AB / ccc - d2;

	//           computing the step (dX,dY) by using the current va;ue of lambda

	dX = Old.Gx * (Vx * Vx / (d1 + lambda) + Vy * Vy / (d2 + lambda)) + Old.Gy * Vx * Vy * (One / (d1 + lambda) - One / (d2 + lambda));
	dY = Old.Gx * Vx * Vy * (One / (d1 + lambda) - One / (d2 + lambda)) + Old.Gy * (Vx * Vx / (d2 + lambda) + Vy * Vy / (d1 + lambda));

	//           updating the circle parameters

	New.a = Old.a - dX;
	New.b = Old.b - dY;

	if ((New.a == Old.a) && (New.b == Old.b))   // if no change, terminate iterations
	{
		//if (lpr==1) std::cout << "++++ iterations stabilize (no change in center) ++++" << endl;
		goto enough;
	}

	//       check if the circle is very large

	if (abs(New.a) > ParLimit2 || abs(New.b) > ParLimit2) {
		//          when the circle is very large for the first time, check if
		//          the best fitting line gives the best fit

		if (check_line)  //  initially, check_line=1, then it is set to zero
		{
			//if (lpr==1) std::cout << "  Linear zone 1st  iter=" << iter << "  (" << New.a << "," << New.b << ")" << endl;

			//                compute scatter matrix

			for (Mxx = Myy = Mxy = 0., i = 0; i < n; i++) {
				dx = data.X[i] - data.meanX;
				dy = data.Y[i] - data.meanY;
				Mxx += dx * dx;
				Myy += dy * dy;
				Mxy += dx * dy;
			}

			eigen2x2(Mxx, Myy, Mxy, dL1, dL2, VLx, VLy);  //  eigendecomposition of scatter matrix

														  //                   compute the third mixed moment (after rotation of coordinates)

			for (Mxxy = 0., i = 0; i < n; i++) {
				dx = (data.X[i] - data.meanX) * VLx + (data.Y[i] - data.meanY) * VLy;
				dy = (data.Y[i] - data.meanY) * VLx - (data.X[i] - data.meanX) * VLy;
				Mxxy += dx * dx * dy;
			}
			//              check if the line is the best fit

			//if (Mxxy == 0.) { std::cout << "  Line " << endl; cin.ignore(); }  //  need to finish this block...

			//              rough estimate of the center to be used later to recoved from the wrong valley

			R = (Mxxy > 0.) ? ParLimit2 : -ParLimit2;
			aL = -VLy * R;
			bL = VLx * R;
			check_line = 0;              //  set to zero to prevent further checks for line
		}

		if ((New.a * VLy - New.b * VLx) * R > 0.)  // check if the circle is in the wrong valley
		{
			//if (lpr==1) std::cout << "  Linear zone foul  iter=" << iter << "  (" << New.a << "," << New.b << ")" << endl;
			New.a = aL;                        //  switch to the rough circle estimate
			New.b = bL;                        //    (precomupted earlier)
			New.s = SigmaWithLargeCircleOption(data, New);           //  compute the root-mean-square error
			GradientHessian(data, New, F1, F2, A11, A22, A12);  // compute the gradient vector and Hessian matrix
			New.Gx = F1;  New.Gy = F2;   New.g = sqrt(F1 * F1 + F2 * F2);   //  the gradient vector and its norm
			lambda = LambdaIni;                //  reset lambda
			sBest = gBest = REAL_MAX;          //  reset best circle characteristics
											   //if (lpr==1) std::cout << "  Linear zone flip  iter=" << iter << "  (" << New.a << "," << New.b << ")" << endl;
			goto NextIteration;      //  restart the Newton's iteration
		}
	}

	New.s = SigmaWithLargeCircleOption(data, New);      //  compute the root-mean-square error
	GradientHessian(data, New, F1, F2, A11, A22, A12);  // compute the gradient vector and Hessian matrix
	New.Gx = F1;  New.Gy = F2;   New.g = sqrt(F1 * F1 + F2 * F2);   //  the gradient vector and its norm

																	//if (lpr==1) std::cout << setprecision(15)<<iter <<"  ("<<New.a<<","<<New.b<<"  s="<<New.s<<"  g="<< New.g<<"  L="<<lambda<<endl;

																	//                check if improvement is gained

	if (New.s < sBest * (One + factor2 * REAL_EPSILON))    //    yes, improvement
	{
		lambda *= factorDown;     //  reduce lambda
		goto NextIteration;       //  proceed to the next Newton's iteration
	}
	else                                             //  no improvement
	{
		//if (lpr==1) std::cout << "   repeat with higher lambda" << endl;
		if (++inner > IterMAX) goto enough;   //  termination due to going over the limit
		lambda = factorUp * lambda;             //  increace lambda
		goto try_again;                       //  make another inner iteration
	}

enough:                   //           exit

	Old.r = OptimalRadius(data, Old);
	Old.i = iter;
	Old.j = inner;

	circle = Old;    //  output circle class

	if (iter > IterMAX) code = 1;    //  error code set to 1
	if (inner > IterMAX) code = 2;    //  error code set to 2
	if ((dmin <= 0.) && (code == 0)) {
		//std::cout << " negative e-value=" << setprecision(20) << dmin << " iter=" << iter <<"  ("<<Old.a<<","<<Old.b<< ")" <<  endl;
		code = 3;     //  error code set to 3
	}
	return code;
}

void SMatInv::swap(double& a, double& b) {
	double t;
	t = a; a = b; b = t;
}

SCircle SMatInv::Perturb(SCircle& New, SCircle& Old, double range) {
	SCircle Perturbed;

	if (range == 0.) return New;

	Perturbed.a = New.a + (New.a - Old.a) * (range * rand() / RAND_MAX - range / Two);
	Perturbed.b = New.b + (New.b - Old.b) * (range * rand() / RAND_MAX - range / Two);
	Perturbed.r = New.r + (New.r - Old.r) * (range * rand() / RAND_MAX - range / Two);

	return Perturbed;
}

int SMatInv::TestAlgebricFit() { // this code tests algebraic circle fits
	double BenchmarkExampleDataX[6]{ 1.,2.,5.,7.,9.,3. };
	double BenchmarkExampleDataY[6]{ 7.,6.,8.,7.,5.,7. };

	SData data1(6, BenchmarkExampleDataX, BenchmarkExampleDataY);
	SCircle circle;
	std::cout.precision(7);
	/*
	Test One:  benchmark example from the journal paper

	W. Gander, G. H. Golub, and R. Strebel,
	"Least squares fitting of circles and ellipses"
	BIT, volume 34, (1994), pages 558-578

	Correct answers:

	Kasa   fit:  center (4.921166,3.835123)  radius 4.123176  sigma 0.4800269

	Pratt  fit:  center (4.615482,2.807354)  radius 4.911302  sigma 0.4610572

	Taubin fit:  center (4.613933,2.795209)  radius 4.879213  sigma 0.4572958

	Hyper  fit:  center (4.615482,2.807354)  radius 4.827575  sigma 0.4571757
	*/
	circle = CircleFitByKasa(data1);
	std::cout << "\nTest One:\n  Kasa   fit:  center ("
		<< circle.a << "," << circle.b << ")  radius "
		<< circle.r << "  sigma " << circle.s << std::endl;

	circle = CircleFitByPratt(data1);
	std::cout << "\n  Pratt  fit:  center ("
		<< circle.a << "," << circle.b << ")  radius "
		<< circle.r << "  sigma " << circle.s << std::endl;

	circle = CircleFitByTaubin(data1);
	std::cout << "\n  Taubin fit:  center ("
		<< circle.a << "," << circle.b << ")  radius "
		<< circle.r << "  sigma " << circle.s << std::endl;

	circle = CircleFitByHyper(data1);
	std::cout << "\n  Hyper  fit:  center ("
		<< circle.a << "," << circle.b << ")  radius "
		<< circle.r << "  sigma " << circle.s << std::endl;

	//            Test Two:  a randomly generated data set

	SData data2(10);    //   specify the number of data points

					   //          use the c++ random number generator to simulate data coordinates

	srand((unsigned)time(NULL));  //  seed the random generator
	SimulateRandom(data2, 1.0);       //  this function is in Utilities.cpp

	circle = CircleFitByKasa(data2);
	std::cout << "\nTest Two:\n  Kasa   fit:  center ("
		<< circle.a << "," << circle.b << ")  radius "
		<< circle.r << "  sigma " << circle.s << std::endl;

	circle = CircleFitByPratt(data2);
	std::cout << "\n  Pratt  fit:  center ("
		<< circle.a << "," << circle.b << ")  radius "
		<< circle.r << "  sigma " << circle.s << std::endl;

	circle = CircleFitByTaubin(data2);
	std::cout << "\n  Taubin fit:  center ("
		<< circle.a << "," << circle.b << ")  radius "
		<< circle.r << "  sigma " << circle.s << std::endl;

	circle = CircleFitByHyper(data2);
	std::cout << "\n  Hyper  fit:  center ("
		<< circle.a << "," << circle.b << ")  radius "
		<< circle.r << "  sigma " << circle.s << std::endl;
	/*
	Test Thee:  benchmark example from the journal paper

	W. Pratt, "Direct least-squares fitting of algebraic surfaces"
	Computer Graphics, volume 21, (1987), pages 145-152.

	It demonstrates that Kasa fit may grossly underestimate the circle size

	Correct answers:

	Kasa   fit:  center (0.0083059,-0.724455)  radius 1.042973  sigma 0.2227621

	Pratt  fit:  center (0.4908357,-22.15212)  radius 22.18006  sigma 0.05416545

	Taubin fit:  center (0.4909211,-22.15598)  radius 22.18378  sigma 0.05416513

	Hyper  fit:  center (0.4908357,-22.15212)  radius 22.17979  sigma 0.05416513
	*/

	double BenchmarkExample2DataX[4]{ -1.,-0.3,0.3,1. };
	double BenchmarkExample2DataY[4]{ 0.,-0.06,0.1,0. };

	SData data3(4, BenchmarkExample2DataX, BenchmarkExample2DataY);

	circle = CircleFitByKasa(data3);
	std::cout << "\nTest Three:\n  Kasa   fit:  center ("
		<< circle.a << "," << circle.b << ")  radius "
		<< circle.r << "  sigma " << circle.s << std::endl;

	circle = CircleFitByPratt(data3);
	std::cout << "\n  Pratt  fit:  center ("
		<< circle.a << "," << circle.b << ")  radius "
		<< circle.r << "  sigma " << circle.s << std::endl;

	circle = CircleFitByTaubin(data3);
	std::cout << "\n  Taubin fit:  center ("
		<< circle.a << "," << circle.b << ")  radius "
		<< circle.r << "  sigma " << circle.s << std::endl;

	circle = CircleFitByHyper(data3);
	std::cout << "\n  Hyper  fit:  center ("
		<< circle.a << "," << circle.b << ")  radius "
		<< circle.r << "  sigma " << circle.s << std::endl;
	return 0;
}

int SMatInv::TestGeometricFit()
//             this code tests geometric circle fits
{
	int code;

	double BenchmarkExampleDataX[6]{ 1.,2.,5.,7.,9.,3. };
	double BenchmarkExampleDataY[6]{ 7.,6.,8.,7.,5.,7. };
	double LambdaIni = 0.01;

	SData data1(6, BenchmarkExampleDataX, BenchmarkExampleDataY);
	SCircle circle, circleIni;
	std::cout.precision(7);
	/*
	Test One:  benchmark example from the journal paper

	W. Gander, G. H. Golub, and R. Strebel,
	"Least squares fitting of circles and ellipses"
	BIT, volume 34, (1994), pages 558-578

	Correct answers:

	Taubin fit should give
	center (4.613933,2.795209)  radius 4.879213  sigma 0.4572958

	All the four geometric fits should give
	center (4.739782,2.983533)  radius 4.714226  sigma 0.4523271
	and converge after 5-10 iterations
	*/
	circleIni = CircleFitByTaubin(data1);
	std::cout << "\nTest One:\n  Initial circle by Taubin fit:\n  center ("
		<< circleIni.a << "," << circleIni.b << ")  radius "
		<< circleIni.r << "  sigma " << circleIni.s << std::endl;

	code = CircleFitByLevenbergMarquardtFull(data1, circleIni, LambdaIni, circle);
	if ((code == 1) || (code == 2)) std::cout << "\n Geometric circle by Levenberg-Marquardt (full) did not converge. Iterations maxed out.\n";
	if (code == 3) std::cout << "\n Geometric circle by Levenberg-Marquardt (full) did not converge. Fitting circle too big.\n";
	if (code == 0) std::cout << "\n Geometric circle by Levenberg-Marquardt (full):\n  center ("
		<< circle.a << "," << circle.b << ")  radius "
		<< circle.r << "  sigma " << circle.s << "  iterations: " << circle.i << std::endl;

	code = CircleFitByLevenbergMarquardtReduced(data1, circleIni, LambdaIni, circle);
	if ((code == 1) || (code == 2)) std::cout << "\n Geometric circle by Levenberg-Marquardt (reduced) did not converge. Iterations maxed out.\n";
	if (code == 3) std::cout << "\n Geometric circle by Levenberg-Marquardt (reduced) did not converge. Fitting circle too big.\n";
	if (code == 0) std::cout << "\n Geometric circle by Levenberg-Marquardt (reduced):\n  center ("
		<< circle.a << "," << circle.b << ")  radius "
		<< circle.r << "  sigma " << circle.s << "  iterations: " << circle.i << std::endl;

	code = CircleFitByChernovLesort(data1, circleIni, LambdaIni, circle);
	if ((code == 1) || (code == 2)) std::cout << "\n Geometric circle by Chernov-Lesort did not converge. Iterations maxed out.\n";
	if (code == 3) std::cout << "\n Geometric circle by Chernov-Lesort did not converge. Fitting circle too big.\n";
	if (code == 0) std::cout << "\n Geometric circle by Chernov-Lesort:\n  center ("
		<< circle.a << "," << circle.b << ")  radius "
		<< circle.r << "  sigma " << circle.s << "  iterations: " << circle.i << std::endl;

	code = CircleFitByChernovHoussam(data1, circleIni, LambdaIni, circle);
	if ((code == 1) || (code == 2)) std::cout << "\n Geometric circle by Chernov-Houssam did not converge. Iterations maxed out.\n";
	if (code == 3) std::cout << "\n Geometric circle by Chernov-Houssam did not converge. Fitting circle too big.\n";
	if (code == 0) std::cout << "\n Geometric circle by Chernov-Houssam:\n  center ("
		<< circle.a << "," << circle.b << ")  radius "
		<< circle.r << "  sigma " << circle.s << "  iterations: " << circle.i << std::endl;

	//            Test Two:  a randomly generated data set

	SData data2(10);    //   specify the number of data points

					   //          use the c++ random number generator to simulate data coordinates

	srand((unsigned)time(NULL));  //  seed the random generator
	SimulateRandom(data2, 1.0);       //  this function is in Utilities.cpp

	circleIni = CircleFitByTaubin(data2);
	std::cout << "\nTest Two:\n  Initial circle by Taubin fit:\n  center ("
		<< circleIni.a << "," << circleIni.b << ")  radius "
		<< circleIni.r << "  sigma " << circleIni.s << std::endl;

	code = CircleFitByLevenbergMarquardtFull(data2, circleIni, LambdaIni, circle);
	if ((code == 1) || (code == 2)) std::cout << "\n Geometric circle by Levenberg-Marquardt (full) did not converge. Iterations maxed out.\n";
	if (code == 3) std::cout << "\n Geometric circle by Levenberg-Marquardt (full) did not converge. Fitting circle too big.\n";
	if (code == 0) std::cout << "\n Geometric circle by Levenberg-Marquardt (full):\n  center ("
		<< circle.a << "," << circle.b << ")  radius "
		<< circle.r << "  sigma " << circle.s << "  iterations: " << circle.i << std::endl;

	code = CircleFitByLevenbergMarquardtReduced(data2, circleIni, LambdaIni, circle);
	if ((code == 1) || (code == 2)) std::cout << "\n Geometric circle by Levenberg-Marquardt (reduced) did not converge. Iterations maxed out.\n";
	if (code == 3) std::cout << "\n Geometric circle by Levenberg-Marquardt (reduced) did not converge. Fitting circle too big.\n";
	if (code == 0) std::cout << "\n Geometric circle by Levenberg-Marquardt (reduced):\n  center ("
		<< circle.a << "," << circle.b << ")  radius "
		<< circle.r << "  sigma " << circle.s << "  iterations: " << circle.i << std::endl;

	code = CircleFitByChernovLesort(data2, circleIni, LambdaIni, circle);
	if ((code == 1) || (code == 2)) std::cout << "\n Geometric circle by Chernov-Lesort did not converge. Iterations maxed out.\n";
	if (code == 3) std::cout << "\n Geometric circle by Chernov-Lesort did not converge. Fitting circle too big.\n";
	if (code == 0) std::cout << "\n Geometric circle by Chernov-Lesort:\n  center ("
		<< circle.a << "," << circle.b << ")  radius "
		<< circle.r << "  sigma " << circle.s << "  iterations: " << circle.i << std::endl;

	code = CircleFitByChernovHoussam(data2, circleIni, LambdaIni, circle);
	if ((code == 1) || (code == 2)) std::cout << "\n Geometric circle by Chernov-Houssam did not converge. Iterations maxed out.\n";
	if (code == 3) std::cout << "\n Geometric circle by Chernov-Houssam did not converge. Fitting circle too big.\n";
	if (code == 0) std::cout << "\n Geometric circle by Chernov-Houssam:\n  center ("
		<< circle.a << "," << circle.b << ")  radius "
		<< circle.r << "  sigma " << circle.s << "  iterations: " << circle.i << std::endl;
	/*
	Test Three:  a special sample of five points

	This is a modified "W-example" from Section 5.13 of the book by Nikolai Chernov:
	"Circular and linear regression: Fitting circles and lines by least squares"
	Chapman & Hall/CRC, Monographs on Statistics and Applied Probability, volume 117, 2010.

	In this example, the algebraic fit by Taubin mistakenly places
	the initial circle on the "wrong side" of the sample
	(in fact, most algebraic fits do the same in this example).

	As a result, both versions of the standard Levenberg-Marquardt algorithm
	move the fitting circle in the wrong direction and diverge.

	Chernov-Lesort and Chernov-Houssam methods, designed to converge
	from any initial circle, flip the fitting circle to the "right side"
	of the sample and find the best fitting circle

	Correct answers:

	Taubin fit should give
	center (0.0,1237.1)  radius 1237.  sigma  1.09612

	The best fitting circle is
	center (0.0,-3283.3)  radius 3283.3

	sigma computed by a standard, numerically unstable formula is 1.09579
	sigma computed by a special,  numerically stable   formula is 1.20075
	*/

	double epsilon = 0.001;
	SData data3(5);
	data3.X[0] = 0.;  data3.Y[0] = 2.;
	data3.X[1] = 1.; data3.X[2] = -data3.X[1];
	data3.Y[1] = data3.Y[2] = -(1. + epsilon);
	data3.X[3] = 10.;  data3.X[4] = -data3.X[3];
	data3.Y[3] = data3.Y[4] = epsilon;

	circleIni = CircleFitByTaubin(data3);
	std::cout << "\nTest Three:\n  Initial circle by Taubin fit:\n  center ("
		<< circleIni.a << "," << circleIni.b << ")  radius "
		<< circleIni.r << "  sigma " << circleIni.s << std::endl;

	code = CircleFitByLevenbergMarquardtFull(data3, circleIni, LambdaIni, circle);
	if ((code == 1) || (code == 2)) std::cout << "\n Geometric circle by Levenberg-Marquardt (full) did not converge. Iterations maxed out.\n";
	if (code == 3) std::cout << "\n Geometric circle by Levenberg-Marquardt (full) did not converge. Fitting circle too big.\n";
	if (code == 0) std::cout << "\n Geometric circle by Levenberg-Marquardt (full):\n  center ("
		<< circle.a << "," << circle.b << ")  radius "
		<< circle.r << "  sigma " << circle.s << "  iterations: " << circle.i << std::endl;

	code = CircleFitByLevenbergMarquardtReduced(data3, circleIni, LambdaIni, circle);
	if ((code == 1) || (code == 2)) std::cout << "\n Geometric circle by Levenberg-Marquardt (reduced) did not converge. Iterations maxed out.\n";
	if (code == 3) std::cout << "\n Geometric circle by Levenberg-Marquardt (reduced) did not converge. Fitting circle too big.\n";
	if (code == 0) std::cout << "\n Geometric circle by Levenberg-Marquardt (reduced):\n  center ("
		<< circle.a << "," << circle.b << ")  radius "
		<< circle.r << "  sigma " << circle.s << "  iterations: " << circle.i << std::endl;

	code = CircleFitByChernovLesort(data3, circleIni, LambdaIni, circle);
	if ((code == 1) || (code == 2)) std::cout << "\n Geometric circle by Chernov-Lesort did not converge. Iterations maxed out.\n";
	if (code == 3) std::cout << "\n Geometric circle by Chernov-Lesort did not converge. Fitting circle too big.\n";
	if (code == 0) std::cout << "\n Geometric circle by Chernov-Lesort:\n  center ("
		<< circle.a << "," << circle.b << ")  radius "
		<< circle.r << "  sigma " << circle.s << "  iterations: " << circle.i << std::endl;

	code = CircleFitByChernovHoussam(data3, circleIni, LambdaIni, circle);
	if ((code == 1) || (code == 2)) std::cout << "\n Geometric circle by Chernov-Houssam did not converge. Iterations maxed out.\n";
	if (code == 3) std::cout << "\n Geometric circle by Chernov-Houssam did not converge. Fitting circle too big.\n";
	if (code == 0) std::cout << "\n Geometric circle by Chernov-Houssam:\n  center ("
		<< circle.a << "," << circle.b << ")  radius "
		<< circle.r << "  sigma " << circle.s << "  iterations: " << circle.i << std::endl;
	return 0;
}

void SData::Dealloc() {
	if (X) delete[]X; X = nullptr;
	if (Y) delete[]Y; Y = nullptr;
}

bool SData::Alloc(int nn) {
	Dealloc();
	X = new double[nn]; Y = new double[nn];
	if (!X || !Y) {
		Dealloc(); return false;
	}
	n = nn;
	memset(X, 0, nn * sizeof(double));
	memset(Y, 0, nn * sizeof(double));
	return true;
}

void SData::means() {// Routine that computes the x- and y- sample means (the coordinates of the centeroid)
	if (!n) return;
	meanX = 0; meanY = 0;
	double* ax = X, * ay = Y;
	for (int i = 0; i < n; i++) {
		meanX += *(ax++); meanY += *(ay++);
	}
	meanX /= double(n); meanY /= double(n);
}

void SData::center() {// Routine that centers the data set (shifts the coordinates to the centeroid)
	double sX = 0., sY = 0.;
	int i;

	for (i = 0; i < n; i++) {
		sX += X[i];
		sY += Y[i];
	}
	sX /= n;
	sY /= n;

	for (i = 0; i < n; i++) {
		X[i] -= sX;
		Y[i] -= sY;
	}
	meanX = 0.;
	meanY = 0.;
}

void SData::scale() {// Routine that scales the coordinates (makes them of order one)
	double sXX = 0., sYY = 0., scaling;
	int i;

	for (i = 0; i < n; i++) {
		sXX += X[i] * X[i];
		sYY += Y[i] * Y[i];
	}
	scaling = sqrt((sXX + sYY) / n / Two);

	for (i = 0; i < n; i++) {
		X[i] /= scaling;
		Y[i] /= scaling;
	}
}

SData::SData() {
}

SData::SData(int n) {
	Alloc(n);
}

SData::SData(int N, double dataX[], double dataY[]) {
	if (!Alloc(N)) return;
	n = N;
	X = new double[n];
	Y = new double[n];

	for (int i = 0; i < n; i++) {
		X[i] = dataX[i];
		Y[i] = dataY[i];
	}
}

SData::~SData() {
	Dealloc();
}

SCircle::SCircle() {
}

SCircle::SCircle(double aa, double bb, double rr) {
	a = aa; b = bb; r = rr;
}

BOOL SMatInv::LSQFit(int n, int* xx, int* yy, float& a, float& b) {
	int i;
	int* x, * y;
	double d0, d1, d2;
	double sx, sy, sxy, ssx;

	x = xx;
	y = yy;

	sx = 0;
	sy = 0;
	sxy = 0;
	ssx = 0;
	long nn = n;
	for (i = 0; i < n; i++) {
		sx += (*x);
		sy += (*y);
		sxy += double(*x) * (*y);
		ssx += double(*x) * (*x);
		x++; y++;
	}
	d0 = nn * ssx - sx * sx;
	d1 = nn * sxy - sx * sy;
	d2 = sy * ssx - sx * sxy;
	if (d0) {
		a = float(d1 / d0);
		b = float(d2 / d0);
	}
	else a = b = 0;
	return TRUE;
}

/*
void CScan::DetectorCalcLsSqFit(CVecDCalc& data)
{// unit of gradient is AB count per step [3/8/2017 FSM]
// Calculate gradient, intercept and error indicators RMS
	double d0, d1, d2;
	double sx, sy, sxy, sxx, syy;
	double sdx2, sdy2, sdxdy;

	sx = 0;
	sy = 0;
	sxy = 0;
	sxx = 0;
	data.gradient = 0;
	data.xconstant = data.yconstant = 0;
	size_t nn = data.data.size();
	for (size_t i = 0; i < nn; i++)
	{
		CVec4* p = &data.data[i];
		float x = p->x;
		float w = p->w;
		sx += x;
		sy += w;
		sxy += double(x) * w;
		sxx += double(x) * x;
	}
	d0 = nn * sxx - sx * sx;
	d1 = nn * sxy - sx * sy;
	d2 = sy * sxx - sx * sxy;
	if (d0)
	{
		data.gradient = d1 / d0;
		data.yconstant = d2 / d0;
		data.xconstant = -data.yconstant / data.gradient; // y=mx+c where y=0, x = -c/m [3/9/2017 128]
	}
	data.xave = sx / double(nn);
	data.yave = sy / double(nn);
	// recalculate [3/11/2017 128]
	syy = 0;
	sdx2 = 0;
	sdy2 = 0;
	sdxdy = 0;
	for (size_t i = 0; i < nn; i++)
	{
		CVec4* p = &data.data[i];
		d2 = p->w - (p->x * data.gradient + data.yconstant);
		syy += d2 * d2;
		d0 = (p->x - data.xave); d1 = (p->w - data.yave);
		sdxdy += d0 * d1;
		sdx2 += d0 * d0;
		sdy2 += d1 * d1;
	}
	// RMS sq = ave sq * stdev sq
	data.rsq = sdxdy / sqrt(sdx2 * sdy2);
	data.rsq *= data.rsq;
	data.rmsErr = sqrt(syy / double(nn));
}

*/

/*
#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>

void polyRegression(const std::vector<int>& x, const std::vector<int>& y) {
	int n = x.size();
	std::vector<int> r(n);
	std::iota(r.begin(), r.end(), 0);
	double xm = std::accumulate(x.begin(), x.end(), 0.0) / x.size();
	double ym = std::accumulate(y.begin(), y.end(), 0.0) / y.size();
	double x2m = std::transform_reduce(r.begin(), r.end(), 0.0, std::plus<double>{}, [](double a) {return a * a; }) / r.size();
	double x3m = std::transform_reduce(r.begin(), r.end(), 0.0, std::plus<double>{}, [](double a) {return a * a * a; }) / r.size();
	double x4m = std::transform_reduce(r.begin(), r.end(), 0.0, std::plus<double>{}, [](double a) {return a * a * a * a; }) / r.size();

	double xym = std::transform_reduce(x.begin(), x.end(), y.begin(), 0.0, std::plus<double>{}, std::multiplies<double>{});
	xym /= fmin(x.size(), y.size());

	double x2ym = std::transform_reduce(x.begin(), x.end(), y.begin(), 0.0, std::plus<double>{}, [](double a, double b) { return a * a * b; });
	x2ym /= fmin(x.size(), y.size());

	double sxx = x2m - xm * xm;
	double sxy = xym - xm * ym;
	double sxx2 = x3m - xm * x2m;
	double sx2x2 = x4m - x2m * x2m;
	double sx2y = x2ym - x2m * ym;

	double b = (sxy * sx2x2 - sx2y * sxx2) / (sxx * sx2x2 - sxx2 * sxx2);
	double c = (sx2y * sxx - sxy * sxx2) / (sxx * sx2x2 - sxx2 * sxx2);
	double a = ym - b * xm - c * x2m;

	auto abc = [a, b, c](int xx) {
		return a + b * xx + c * xx*xx;
	};

	std::cout << "y = " << a << " + " << b << "x + " << c << "x^2" << std::endl;
	std::cout << " Input  Approximation" << std::endl;
	std::cout << " x   y     y1" << std::endl;

	auto xit = x.cbegin();
	auto xend = x.cend();
	auto yit = y.cbegin();
	auto yend = y.cend();
	while (xit != xend && yit != yend) {
		printf("%2d %3d  %5.1f\n", *xit, *yit, abc(*xit));
		xit = std::next(xit);
		yit = std::next(yit);
	}
}

int main() {
	using namespace std;

	vector<int> x(11);
	iota(x.begin(), x.end(), 0);

	vector<int> y{ 1, 6, 17, 34, 57, 86, 121, 162, 209, 262, 321 };

	polyRegression(x, y);

	return 0;
}
*/