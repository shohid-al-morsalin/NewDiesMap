#ifndef __ROLL_H__
#define __ROLL_H__

typedef struct {
	double x;
	double y;
	double z;
	double W;
	double sigmaxx;
	double sigmaxy;
	double sigmayy;
	double sigmarr;
	double sigmart;
	double sigmatt;
} StressPoint;

class CRoll {
public:
	CRoll();
	virtual ~CRoll();

	void CalculateStressMatrix(int NPoint, StressPoint* p, double fDiam, double fPoisson);
	void Initialize(int NPoint, StressPoint* p, double fDiam, double fPoisson);
	void CalculateCubic(int NPoint, StressPoint* p, double fDiam, double fPoisson);
	double ArcTangent(double x, double y);

protected:
	double* x1;
	double* y1;
	double* z1;
	double* r;
	double* phi;
	double* c1;
	double* s1;
	double* c2;
	double* s2;
	double* c3;
	double* s3;
	double* c4;
	double* s4;

	double* AA;
	double* BB;
	double* RH;

	double* dr1_20;
	double* dr1_40;
	double* dr1_11;
	double* dr1_31;
	double* dr1_22;
	double* dr1_42;
	double* dr1_33;
	double* dr1_44;

	double* dr2_20;
	double* dr2_40;
	double* dr2_11;
	double* dr2_31;
	double* dr2_22;
	double* dr2_42;
	double* dr2_33;
	double* dr2_44;

	double* zernike20;
	double* zernike40;
	double* zernike11;
	double* zernike31;
	double* zernike22;
	double* zernike42;
	double* zernike33;
	double* zernike44;

	double* D1Wr;
	double* D1Wphi;
	double* D2Wr;
	double* D2Wphi;
	double* D2Wrphi;

	double coeff[15];
	double GetDegree(double x, int n);
	int factorial1(int k);
	void Swap(double a, double b);
	void MatrixInversion(int n, double* a);
	double D1W_phi(int i);
	double D1W_r(int i);
	double D2W_r(int i);
	double D2W_phi(int i);
	double D2W_r_phi(int i);
	double D2xx(int i);
	double D2xy(int i);
	double D2yy(int i);
	double dr1(int n, int m1, double x);
	double dr2(int n, int m1, double x);
	double Laplace(int i);
	double Sxx(double m1, int i);
	double Sxy(int i);
	double Syy(double m1, int i);
	double zernike(int n, int m1, double x);
};

#endif