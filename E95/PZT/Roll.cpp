#include "stdafx.h"
#include <math.h>
#include <stdio.h>
#include <memory.h>
#include "Roll.h"

//#define MATRIXDIM 15

CRoll::CRoll() {
	x1 = NULL;
	y1 = NULL;
	z1 = NULL;
	r = NULL;
	phi = NULL;
	c1 = NULL;
	s1 = NULL;
	c2 = NULL;
	s2 = NULL;
	c3 = NULL;
	s3 = NULL;
	c4 = NULL;
	s4 = NULL;
	dr1_20 = NULL;
	dr1_40 = NULL;
	dr1_11 = NULL;
	dr1_31 = NULL;
	dr1_22 = NULL;
	dr1_42 = NULL;
	dr1_33 = NULL;
	dr1_44 = NULL;
	dr2_20 = NULL;
	dr2_40 = NULL;
	dr2_11 = NULL;
	dr2_31 = NULL;
	dr2_22 = NULL;
	dr2_42 = NULL;
	dr2_33 = NULL;
	dr2_44 = NULL;

	zernike20 = NULL;
	zernike40 = NULL;
	zernike11 = NULL;
	zernike31 = NULL;
	zernike22 = NULL;
	zernike42 = NULL;
	zernike33 = NULL;
	zernike44 = NULL;

	D1Wr = NULL;
	D1Wphi = NULL;
	D2Wr = NULL;
	D2Wphi = NULL;
	D2Wrphi = NULL;
	AA = NULL;
	BB = NULL;
	RH = NULL;
}

CRoll::~CRoll() {
	if (RH) {
		delete[] RH;
		RH = NULL;
	}
	if (BB) {
		delete[] BB;
		BB = NULL;
	}
	if (AA) {
		delete[] AA;
		AA = NULL;
	}
	if (D2Wrphi) {
		delete[] D2Wrphi;
		D2Wrphi = NULL;
	}
	if (D2Wphi) {
		delete[] D2Wphi;
		D2Wphi = NULL;
	}
	if (D2Wr) {
		delete[] D2Wr;
		D2Wr = NULL;
	}
	if (D1Wphi) {
		delete[] D1Wphi;
		D1Wphi = NULL;
	}
	if (D1Wr) {
		delete[] D1Wr;
		D1Wr = NULL;
	}

	if (zernike44) {
		delete[] zernike44;
		zernike44 = NULL;
	}
	if (zernike33) {
		delete[] zernike33;
		zernike33 = NULL;
	}
	if (zernike42) {
		delete[] zernike42;
		zernike42 = NULL;
	}
	if (zernike22) {
		delete[] zernike22;
		zernike22 = NULL;
	}
	if (zernike31) {
		delete[] zernike31;
		zernike31 = NULL;
	}
	if (zernike11) {
		delete[] zernike11;
		zernike11 = NULL;
	}
	if (zernike40) {
		delete[] zernike40;
		zernike40 = NULL;
	}
	if (zernike20) {
		delete[] zernike20;
		zernike20 = NULL;
	}

	if (dr2_44) {
		delete[] dr2_44;
		dr2_44 = NULL;
	}
	if (dr2_33) {
		delete[] dr2_33;
		dr2_33 = NULL;
	}
	if (dr2_42) {
		delete[] dr2_42;
		dr2_42 = NULL;
	}
	if (dr2_22) {
		delete[] dr2_22;
		dr2_22 = NULL;
	}
	if (dr2_31) {
		delete[] dr2_31;
		dr2_31 = NULL;
	}
	if (dr2_11) {
		delete[] dr2_11;
		dr2_11 = NULL;
	}
	if (dr2_40) {
		delete[] dr2_40;
		dr2_40 = NULL;
	}
	if (dr2_20) {
		delete[] dr2_20;
		dr2_20 = NULL;
	}
	if (dr1_44) {
		delete[] dr1_44;
		dr1_44 = NULL;
	}
	if (dr1_33) {
		delete[] dr1_33;
		dr1_33 = NULL;
	}
	if (dr1_42) {
		delete[] dr1_42;
		dr1_42 = NULL;
	}
	if (dr1_22) {
		delete[] dr1_22;
		dr1_22 = NULL;
	}
	if (dr1_31) {
		delete[] dr1_31;
		dr1_31 = NULL;
	}
	if (dr1_11) {
		delete[] dr1_11;
		dr1_11 = NULL;
	}
	if (dr1_40) {
		delete[] dr1_40;
		dr1_40 = NULL;
	}
	if (dr1_20) {
		delete[] dr1_20;
		dr1_20 = NULL;
	}
	if (s4) {
		delete[] s4;
		s4 = NULL;
	}
	if (c4) {
		delete[] c4;
		c4 = NULL;
	}
	if (s3) {
		delete[] s3;
		s3 = NULL;
	}
	if (c3) {
		delete[] c3;
		c3 = NULL;
	}
	if (s2) {
		delete[] s2;
		s2 = NULL;
	}
	if (c2) {
		delete[] c2;
		c2 = NULL;
	}
	if (s1) {
		delete[] s1;
		s1 = NULL;
	}
	if (c1) {
		delete[] c1;
		c1 = NULL;
	}
	if (c1) {
		delete[] c1;
		c1 = NULL;
	}
	if (phi) {
		delete[] phi;
		phi = NULL;
	}
	if (r) {
		delete[] r;
		r = NULL;
	}
	if (z1) {
		delete[] z1;
		z1 = NULL;
	}
	if (y1) {
		delete[] y1;
		y1 = NULL;
	}
	if (x1) {
		delete[] x1;
		x1 = NULL;
	}
}

void CRoll::Initialize(int NPoint, StressPoint* p, double fDiam, double fPoisson) {
	if (x1) {
		delete[] x1;
		x1 = NULL;
	}
	x1 = new double[NPoint];
	if (y1) {
		delete[] y1;
		y1 = NULL;
	}
	y1 = new double[NPoint];
	if (z1) {
		delete[] z1;
		z1 = NULL;
	}
	z1 = new double[NPoint];
	if (r) {
		delete[] r;
		r = NULL;
	}
	r = new double[NPoint];
	if (phi) {
		delete[] phi;
		phi = NULL;
	}
	phi = new double[NPoint];
	if (c1) {
		delete[] c1;
		c1 = NULL;
	}
	c1 = new double[NPoint];
	if (s1) {
		delete[] s1;
		s1 = NULL;
	}
	s1 = new double[NPoint];
	if (c2) {
		delete[] c2;
		c2 = NULL;
	}
	c2 = new double[NPoint];
	if (s2) {
		delete[] s2;
		s2 = NULL;
	}
	s2 = new double[NPoint];
	if (c3) {
		delete[] c3;
		c3 = NULL;
	}
	c3 = new double[NPoint];
	if (s3) {
		delete[] s3;
		s3 = NULL;
	}
	s3 = new double[NPoint];
	if (c4) {
		delete[] c4;
		c4 = NULL;
	}
	c4 = new double[NPoint];
	if (s4) {
		delete[] s4;
		s4 = NULL;
	}
	s4 = new double[NPoint];

	if (dr1_20) {
		delete[] dr1_20;
		dr1_20 = NULL;
	}
	dr1_20 = new double[NPoint];
	if (dr1_40) {
		delete[] dr1_40;
		dr1_40 = NULL;
	}
	dr1_40 = new double[NPoint];
	if (dr1_11) {
		delete[] dr1_11;
		dr1_11 = NULL;
	}
	dr1_11 = new double[NPoint];
	if (dr1_31) {
		delete[] dr1_31;
		dr1_31 = NULL;
	}
	dr1_31 = new double[NPoint];
	if (dr1_22) {
		delete[] dr1_22;
		dr1_22 = NULL;
	}
	dr1_22 = new double[NPoint];
	if (dr1_42) {
		delete[] dr1_42;
		dr1_42 = NULL;
	}
	dr1_42 = new double[NPoint];
	if (dr1_33) {
		delete[] dr1_33;
		dr1_33 = NULL;
	}
	dr1_33 = new double[NPoint];
	if (dr1_44) {
		delete[] dr1_44;
		dr1_44 = NULL;
	}
	dr1_44 = new double[NPoint];

	if (dr2_20) {
		delete[] dr2_20;
		dr2_20 = NULL;
	}
	dr2_20 = new double[NPoint];
	if (dr2_40) {
		delete[] dr2_40;
		dr2_40 = NULL;
	}
	dr2_40 = new double[NPoint];
	if (dr2_11) {
		delete[] dr2_11;
		dr2_11 = NULL;
	}
	dr2_11 = new double[NPoint];
	if (dr2_31) {
		delete[] dr2_31;
		dr2_31 = NULL;
	}
	dr2_31 = new double[NPoint];
	if (dr2_22) {
		delete[] dr2_22;
		dr2_22 = NULL;
	}
	dr2_22 = new double[NPoint];
	if (dr2_42) {
		delete[] dr2_42;
		dr2_42 = NULL;
	}
	dr2_42 = new double[NPoint];
	if (dr2_33) {
		delete[] dr2_33;
		dr2_33 = NULL;
	}
	dr2_33 = new double[NPoint];
	if (dr2_44) {
		delete[] dr2_44;
		dr2_44 = NULL;
	}
	dr2_44 = new double[NPoint];

	if (zernike20) {
		delete[] zernike20;
		zernike20 = NULL;
	}
	zernike20 = new double[NPoint];
	if (zernike40) {
		delete[] zernike40;
		zernike40 = NULL;
	}
	zernike40 = new double[NPoint];
	if (zernike11) {
		delete[] zernike11;
		zernike11 = NULL;
	}
	zernike11 = new double[NPoint];
	if (zernike31) {
		delete[] zernike31;
		zernike31 = NULL;
	}
	zernike31 = new double[NPoint];
	if (zernike22) {
		delete[] zernike22;
		zernike22 = NULL;
	}
	zernike22 = new double[NPoint];
	if (zernike42) {
		delete[] zernike42;
		zernike42 = NULL;
	}
	zernike42 = new double[NPoint];
	if (zernike33) {
		delete[] zernike33;
		zernike33 = NULL;
	}
	zernike33 = new double[NPoint];
	if (zernike44) {
		delete[] zernike44;
		zernike44 = NULL;
	}
	zernike44 = new double[NPoint];

	if (D1Wr) {
		delete[] D1Wr;
		D1Wr = NULL;
	}
	D1Wr = new double[NPoint];
	if (D1Wphi) {
		delete[] D1Wphi;
		D1Wphi = NULL;
	}
	D1Wphi = new double[NPoint];
	if (D2Wr) {
		delete[] D2Wr;
		D2Wr = NULL;
	}
	D2Wr = new double[NPoint];
	if (D2Wphi) {
		delete[] D2Wphi;
		D2Wphi = NULL;
	}
	D2Wphi = new double[NPoint];
	if (D2Wrphi) {
		delete[] D2Wrphi;
		D2Wrphi = NULL;
	}
	D2Wrphi = new double[NPoint];

	if (AA) {
		delete[] AA;
		AA = NULL;
	}

	AA = new double[15 * 15];
	memset(AA, 0, sizeof(double) * 15 * 15);
	if (BB) {
		delete[] BB;
		BB = NULL;
	}
	BB = new double[15 * 15];
	memset(BB, 0, sizeof(double) * 15 * 15);
	if (RH) {
		delete[] RH;
		RH = NULL;
	}
	RH = new double[15];
	memset(RH, 0, sizeof(double) * 15);

	double R = fDiam / 2;

	int i;

	//normalize 'x' and 'y'
	for (i = 0; i < NPoint; i++) {
		x1[i] = p[i].x / R;
		y1[i] = p[i].y / R;
	}

	//convert to polar coordinates
	for (i = 0; i < NPoint; i++) {
		r[i] = sqrt(x1[i] * x1[i] + y1[i] * y1[i]);
		phi[i] = ArcTangent(x1[i], y1[i]);
	}

	//fill sin and cos arrays
	for (i = 0; i < NPoint; i++) {
		c1[i] = cos(phi[i]);
		s1[i] = sin(phi[i]);
		c2[i] = cos(2 * phi[i]);
		s2[i] = sin(2 * phi[i]);
		c3[i] = cos(3 * phi[i]);
		s3[i] = sin(3 * phi[i]);
		c4[i] = cos(4 * phi[i]);
		s4[i] = sin(4 * phi[i]);
	}

	//calculate dr functions
	for (i = 0; i < NPoint; i++) {
		double rr = r[i] * r[i];

		dr1_20[i] = dr1(2, 0, r[i]);//4;
		dr1_40[i] = dr1(4, 0, r[i]);//24*rr - 12;
		dr1_11[i] = dr1(1, 1, r[i]);//1/r[i];
		dr1_31[i] = dr1(3, 1, r[i]);//6*r[i] - 2/r[i];
		dr1_22[i] = dr1(2, 2, r[i]);//2;
		dr1_42[i] = dr1(4, 2, r[i]);//16*rr-6;
		dr1_33[i] = dr1(3, 3, r[i]);//3*r[i];
		dr1_44[i] = dr1(4, 4, r[i]);//4*rr;

		dr2_20[i] = dr2(2, 0, r[i]);
		dr2_40[i] = dr2(4, 0, r[i]);
		dr2_11[i] = dr2(1, 1, r[i]);
		dr2_31[i] = dr2(3, 1, r[i]);
		dr2_22[i] = dr2(2, 2, r[i]);
		dr2_42[i] = dr2(4, 2, r[i]);
		dr2_33[i] = dr2(3, 3, r[i]);
		dr2_44[i] = dr2(4, 4, r[i]);
	}

	//calculate Zernike polinomials
	for (i = 0; i < NPoint; i++) {
		double rr = r[i] * r[i];
		zernike20[i] = zernike(2, 0, r[i]);//2*rr-1;
		zernike40[i] = zernike(4, 0, r[i]);//6*rr*rr-6*rr+1;
		zernike11[i] = zernike(1, 1, r[i]);//r[i];
		zernike31[i] = zernike(3, 1, r[i]);//r[i]*(3*rr-2);
		zernike22[i] = zernike(2, 2, r[i]);//rr;
		zernike42[i] = zernike(4, 2, r[i]);//4*rr*rr-3*rr;
		zernike33[i] = zernike(3, 3, r[i]);//r[i]*rr;
		zernike44[i] = zernike(4, 4, r[i]);//rr*rr;
	}

	//Matrix Row 1
	for (i = 0; i < NPoint; i++) {
		AA[0 * 15 + 0] += 1.0;
		AA[0 * 15 + 1] += zernike20[i];
		AA[0 * 15 + 2] += zernike40[i];
		AA[0 * 15 + 3] += zernike11[i] * c1[i];
		AA[0 * 15 + 4] += zernike11[i] * s1[i];
		AA[0 * 15 + 5] += zernike31[i] * c1[i];
		AA[0 * 15 + 6] += zernike31[i] * s1[i];
		AA[0 * 15 + 7] += zernike22[i] * c2[i];
		AA[0 * 15 + 8] += zernike22[i] * s2[i];
		AA[0 * 15 + 9] += zernike42[i] * c2[i];
		AA[0 * 15 + 10] += zernike42[i] * s2[i];
		AA[0 * 15 + 11] += zernike33[i] * c3[i];
		AA[0 * 15 + 12] += zernike33[i] * s3[i];
		AA[0 * 15 + 13] += zernike44[i] * c4[i];
		AA[0 * 15 + 14] += zernike44[i] * s4[i];
	}

	for (i = 0; i < 15; i++) {
		AA[0 + i] = AA[0 + i] / NPoint;
	}

	//Matrix Row 2
	for (i = 0; i < NPoint; i++) {
		AA[1 * 15 + 0] += zernike20[i];
		AA[1 * 15 + 1] += zernike20[i] * zernike20[i];
		AA[1 * 15 + 2] += zernike40[i] * zernike20[i];
		AA[1 * 15 + 3] += zernike11[i] * c1[i] * zernike20[i];
		AA[1 * 15 + 4] += zernike11[i] * s1[i] * zernike20[i];
		AA[1 * 15 + 5] += zernike31[i] * c1[i] * zernike20[i];
		AA[1 * 15 + 6] += zernike31[i] * s1[i] * zernike20[i];
		AA[1 * 15 + 7] += zernike22[i] * c2[i] * zernike20[i];
		AA[1 * 15 + 8] += zernike22[i] * s2[i] * zernike20[i];
		AA[1 * 15 + 9] += zernike42[i] * c2[i] * zernike20[i];
		AA[1 * 15 + 10] += zernike42[i] * s2[i] * zernike20[i];
		AA[1 * 15 + 11] += zernike33[i] * c3[i] * zernike20[i];
		AA[1 * 15 + 12] += zernike33[i] * s3[i] * zernike20[i];
		AA[1 * 15 + 13] += zernike44[i] * c4[i] * zernike20[i];
		AA[1 * 15 + 14] += zernike44[i] * s4[i] * zernike20[i];
	}

	//Matrix Row 3
	for (i = 0; i < NPoint; i++) {
		AA[2 * 15 + 0] += zernike40[i];
		AA[2 * 15 + 1] += zernike20[i] * zernike40[i];
		AA[2 * 15 + 2] += zernike40[i] * zernike40[i];
		AA[2 * 15 + 3] += zernike11[i] * c1[i] * zernike40[i];
		AA[2 * 15 + 4] += zernike11[i] * s1[i] * zernike40[i];
		AA[2 * 15 + 5] += zernike31[i] * c1[i] * zernike40[i];
		AA[2 * 15 + 6] += zernike31[i] * s1[i] * zernike40[i];
		AA[2 * 15 + 7] += zernike22[i] * c2[i] * zernike40[i];
		AA[2 * 15 + 8] += zernike22[i] * s2[i] * zernike40[i];
		AA[2 * 15 + 9] += zernike42[i] * c2[i] * zernike40[i];
		AA[2 * 15 + 10] += zernike42[i] * s2[i] * zernike40[i];
		AA[2 * 15 + 11] += zernike33[i] * c3[i] * zernike40[i];
		AA[2 * 15 + 12] += zernike33[i] * s3[i] * zernike40[i];
		AA[2 * 15 + 13] += zernike44[i] * c4[i] * zernike40[i];
		AA[2 * 15 + 14] += zernike44[i] * s4[i] * zernike40[i];
	}

	//Matrix Row 4
	for (i = 0; i < NPoint; i++) {
		AA[3 * 15 + 0] += zernike11[i] * c1[i];
		AA[3 * 15 + 1] += zernike20[i] * zernike11[i] * c1[i];
		AA[3 * 15 + 2] += zernike40[i] * zernike11[i] * c1[i];
		AA[3 * 15 + 3] += zernike11[i] * c1[i] * zernike11[i] * c1[i];
		AA[3 * 15 + 4] += zernike11[i] * s1[i] * zernike11[i] * c1[i];
		AA[3 * 15 + 5] += zernike31[i] * c1[i] * zernike11[i] * c1[i];
		AA[3 * 15 + 6] += zernike31[i] * s1[i] * zernike11[i] * c1[i];
		AA[3 * 15 + 7] += zernike22[i] * c2[i] * zernike11[i] * c1[i];
		AA[3 * 15 + 8] += zernike22[i] * s2[i] * zernike11[i] * c1[i];
		AA[3 * 15 + 9] += zernike42[i] * c2[i] * zernike11[i] * c1[i];
		AA[3 * 15 + 10] += zernike42[i] * s2[i] * zernike11[i] * c1[i];
		AA[3 * 15 + 11] += zernike33[i] * c3[i] * zernike11[i] * c1[i];
		AA[3 * 15 + 12] += zernike33[i] * s3[i] * zernike11[i] * c1[i];
		AA[3 * 15 + 13] += zernike44[i] * c4[i] * zernike11[i] * c1[i];
		AA[3 * 15 + 14] += zernike44[i] * s4[i] * zernike11[i] * c1[i];
	}

	//Matrix Row 5
	for (i = 0; i < NPoint; i++) {
		AA[4 * 15 + 0] += zernike11[i] * s1[i];
		AA[4 * 15 + 1] += zernike20[i] * zernike11[i] * s1[i];
		AA[4 * 15 + 2] += zernike40[i] * zernike11[i] * s1[i];
		AA[4 * 15 + 3] += zernike11[i] * c1[i] * zernike11[i] * s1[i];
		AA[4 * 15 + 4] += zernike11[i] * s1[i] * zernike11[i] * s1[i];
		AA[4 * 15 + 5] += zernike31[i] * c1[i] * zernike11[i] * s1[i];
		AA[4 * 15 + 6] += zernike31[i] * s1[i] * zernike11[i] * s1[i];
		AA[4 * 15 + 7] += zernike22[i] * c2[i] * zernike11[i] * s1[i];
		AA[4 * 15 + 8] += zernike22[i] * s2[i] * zernike11[i] * s1[i];
		AA[4 * 15 + 9] += zernike42[i] * c2[i] * zernike11[i] * s1[i];
		AA[4 * 15 + 10] += zernike42[i] * s2[i] * zernike11[i] * s1[i];
		AA[4 * 15 + 11] += zernike33[i] * c3[i] * zernike11[i] * s1[i];
		AA[4 * 15 + 12] += zernike33[i] * s3[i] * zernike11[i] * s1[i];
		AA[4 * 15 + 13] += zernike44[i] * c4[i] * zernike11[i] * s1[i];
		AA[4 * 15 + 14] += zernike44[i] * s4[i] * zernike11[i] * s1[i];
	}

	//Matrix Row 6
	for (i = 0; i < NPoint; i++) {
		AA[5 * 15 + 0] += zernike31[i] * c1[i];
		AA[5 * 15 + 1] += zernike20[i] * zernike31[i] * c1[i];
		AA[5 * 15 + 2] += zernike40[i] * zernike31[i] * c1[i];
		AA[5 * 15 + 3] += zernike11[i] * c1[i] * zernike31[i] * c1[i];
		AA[5 * 15 + 4] += zernike11[i] * s1[i] * zernike31[i] * c1[i];
		AA[5 * 15 + 5] += zernike31[i] * c1[i] * zernike31[i] * c1[i];
		AA[5 * 15 + 6] += zernike31[i] * s1[i] * zernike31[i] * c1[i];
		AA[5 * 15 + 7] += zernike22[i] * c2[i] * zernike31[i] * c1[i];
		AA[5 * 15 + 8] += zernike22[i] * s2[i] * zernike31[i] * c1[i];
		AA[5 * 15 + 9] += zernike42[i] * c2[i] * zernike31[i] * c1[i];
		AA[5 * 15 + 10] += zernike42[i] * s2[i] * zernike31[i] * c1[i];
		AA[5 * 15 + 11] += zernike33[i] * c3[i] * zernike31[i] * c1[i];
		AA[5 * 15 + 12] += zernike33[i] * s3[i] * zernike31[i] * c1[i];
		AA[5 * 15 + 13] += zernike44[i] * c4[i] * zernike31[i] * c1[i];
		AA[5 * 15 + 14] += zernike44[i] * s4[i] * zernike31[i] * c1[i];
	}

	//Matrix Row 7
	for (i = 0; i < NPoint; i++) {
		AA[6 * 15 + 0] += zernike31[i] * s1[i];
		AA[6 * 15 + 1] += zernike20[i] * zernike31[i] * s1[i];
		AA[6 * 15 + 2] += zernike40[i] * zernike31[i] * s1[i];
		AA[6 * 15 + 3] += zernike11[i] * c1[i] * zernike31[i] * s1[i];
		AA[6 * 15 + 4] += zernike11[i] * s1[i] * zernike31[i] * s1[i];
		AA[6 * 15 + 5] += zernike31[i] * c1[i] * zernike31[i] * s1[i];
		AA[6 * 15 + 6] += zernike31[i] * s1[i] * zernike31[i] * s1[i];
		AA[6 * 15 + 7] += zernike22[i] * c2[i] * zernike31[i] * s1[i];
		AA[6 * 15 + 8] += zernike22[i] * s2[i] * zernike31[i] * s1[i];
		AA[6 * 15 + 9] += zernike42[i] * c2[i] * zernike31[i] * s1[i];
		AA[6 * 15 + 10] += zernike42[i] * s2[i] * zernike31[i] * s1[i];
		AA[6 * 15 + 11] += zernike33[i] * c3[i] * zernike31[i] * s1[i];
		AA[6 * 15 + 12] += zernike33[i] * s3[i] * zernike31[i] * s1[i];
		AA[6 * 15 + 13] += zernike44[i] * c4[i] * zernike31[i] * s1[i];
		AA[6 * 15 + 14] += zernike44[i] * s4[i] * zernike31[i] * s1[i];
	}

	//Matrix Row 8
	for (i = 0; i < NPoint; i++) {
		AA[7 * 15 + 0] += zernike22[i] * c2[i];
		AA[7 * 15 + 1] += zernike20[i] * zernike22[i] * c2[i];
		AA[7 * 15 + 2] += zernike40[i] * zernike22[i] * c2[i];
		AA[7 * 15 + 3] += zernike11[i] * c1[i] * zernike22[i] * c2[i];
		AA[7 * 15 + 4] += zernike11[i] * s1[i] * zernike22[i] * c2[i];
		AA[7 * 15 + 5] += zernike31[i] * c1[i] * zernike22[i] * c2[i];
		AA[7 * 15 + 6] += zernike31[i] * s1[i] * zernike22[i] * c2[i];
		AA[7 * 15 + 7] += zernike22[i] * c2[i] * zernike22[i] * c2[i];
		AA[7 * 15 + 8] += zernike22[i] * s2[i] * zernike22[i] * c2[i];
		AA[7 * 15 + 9] += zernike42[i] * c2[i] * zernike22[i] * c2[i];
		AA[7 * 15 + 10] += zernike42[i] * s2[i] * zernike22[i] * c2[i];
		AA[7 * 15 + 11] += zernike33[i] * c3[i] * zernike22[i] * c2[i];
		AA[7 * 15 + 12] += zernike33[i] * s3[i] * zernike22[i] * c2[i];
		AA[7 * 15 + 13] += zernike44[i] * c4[i] * zernike22[i] * c2[i];
		AA[7 * 15 + 14] += zernike44[i] * s4[i] * zernike22[i] * c2[i];
	}

	//Matrix Row 9
	for (i = 0; i < NPoint; i++) {
		AA[8 * 15 + 0] += zernike22[i] * s2[i];
		AA[8 * 15 + 1] += zernike20[i] * zernike22[i] * s2[i];
		AA[8 * 15 + 2] += zernike40[i] * zernike22[i] * s2[i];
		AA[8 * 15 + 3] += zernike11[i] * c1[i] * zernike22[i] * s2[i];
		AA[8 * 15 + 4] += zernike11[i] * s1[i] * zernike22[i] * s2[i];
		AA[8 * 15 + 5] += zernike31[i] * c1[i] * zernike22[i] * s2[i];
		AA[8 * 15 + 6] += zernike31[i] * s1[i] * zernike22[i] * s2[i];
		AA[8 * 15 + 7] += zernike22[i] * c2[i] * zernike22[i] * s2[i];
		AA[8 * 15 + 8] += zernike22[i] * s2[i] * zernike22[i] * s2[i];
		AA[8 * 15 + 9] += zernike42[i] * c2[i] * zernike22[i] * s2[i];
		AA[8 * 15 + 10] += zernike42[i] * s2[i] * zernike22[i] * s2[i];
		AA[8 * 15 + 11] += zernike33[i] * c3[i] * zernike22[i] * s2[i];
		AA[8 * 15 + 12] += zernike33[i] * s3[i] * zernike22[i] * s2[i];
		AA[8 * 15 + 13] += zernike44[i] * c4[i] * zernike22[i] * s2[i];
		AA[8 * 15 + 14] += zernike44[i] * s4[i] * zernike22[i] * s2[i];
	}

	//Matrix Row 10
	for (i = 0; i < NPoint; i++) {
		AA[9 * 15 + 0] += zernike42[i] * c2[i];
		AA[9 * 15 + 1] += zernike20[i] * zernike42[i] * c2[i];
		AA[9 * 15 + 2] += zernike40[i] * zernike42[i] * c2[i];
		AA[9 * 15 + 3] += zernike11[i] * c1[i] * zernike42[i] * c2[i];
		AA[9 * 15 + 4] += zernike11[i] * s1[i] * zernike42[i] * c2[i];
		AA[9 * 15 + 5] += zernike31[i] * c1[i] * zernike42[i] * c2[i];
		AA[9 * 15 + 6] += zernike31[i] * s1[i] * zernike42[i] * c2[i];
		AA[9 * 15 + 7] += zernike22[i] * c2[i] * zernike42[i] * c2[i];
		AA[9 * 15 + 8] += zernike22[i] * s2[i] * zernike42[i] * c2[i];
		AA[9 * 15 + 9] += zernike42[i] * c2[i] * zernike42[i] * c2[i];
		AA[9 * 15 + 10] += zernike42[i] * s2[i] * zernike42[i] * c2[i];
		AA[9 * 15 + 11] += zernike33[i] * c3[i] * zernike42[i] * c2[i];
		AA[9 * 15 + 12] += zernike33[i] * s3[i] * zernike42[i] * c2[i];
		AA[9 * 15 + 13] += zernike44[i] * c4[i] * zernike42[i] * c2[i];
		AA[9 * 15 + 14] += zernike44[i] * s4[i] * zernike42[i] * c2[i];
	}

	//Matrix Row 11
	for (i = 0; i < NPoint; i++) {
		AA[10 * 15 + 0] += zernike42[i] * s2[i];
		AA[10 * 15 + 1] += zernike20[i] * zernike42[i] * s2[i];
		AA[10 * 15 + 2] += zernike40[i] * zernike42[i] * s2[i];
		AA[10 * 15 + 3] += zernike11[i] * c1[i] * zernike42[i] * s2[i];
		AA[10 * 15 + 4] += zernike11[i] * s1[i] * zernike42[i] * s2[i];
		AA[10 * 15 + 5] += zernike31[i] * c1[i] * zernike42[i] * s2[i];
		AA[10 * 15 + 6] += zernike31[i] * s1[i] * zernike42[i] * s2[i];
		AA[10 * 15 + 7] += zernike22[i] * c2[i] * zernike42[i] * s2[i];
		AA[10 * 15 + 8] += zernike22[i] * s2[i] * zernike42[i] * s2[i];
		AA[10 * 15 + 9] += zernike42[i] * c2[i] * zernike42[i] * s2[i];
		AA[10 * 15 + 10] += zernike42[i] * s2[i] * zernike42[i] * s2[i];
		AA[10 * 15 + 11] += zernike33[i] * c3[i] * zernike42[i] * s2[i];
		AA[10 * 15 + 12] += zernike33[i] * s3[i] * zernike42[i] * s2[i];
		AA[10 * 15 + 13] += zernike44[i] * c4[i] * zernike42[i] * s2[i];
		AA[10 * 15 + 14] += zernike44[i] * s4[i] * zernike42[i] * s2[i];
	}

	//Matrix Row 12
	for (i = 0; i < NPoint; i++) {
		AA[11 * 15 + 0] += zernike33[i] * c3[i];
		AA[11 * 15 + 1] += zernike20[i] * zernike33[i] * c3[i];
		AA[11 * 15 + 2] += zernike40[i] * zernike33[i] * c3[i];
		AA[11 * 15 + 3] += zernike11[i] * c1[i] * zernike33[i] * c3[i];
		AA[11 * 15 + 4] += zernike11[i] * s1[i] * zernike33[i] * c3[i];
		AA[11 * 15 + 5] += zernike31[i] * c1[i] * zernike33[i] * c3[i];
		AA[11 * 15 + 6] += zernike31[i] * s1[i] * zernike33[i] * c3[i];
		AA[11 * 15 + 7] += zernike22[i] * c2[i] * zernike33[i] * c3[i];
		AA[11 * 15 + 8] += zernike22[i] * s2[i] * zernike33[i] * c3[i];
		AA[11 * 15 + 9] += zernike42[i] * c2[i] * zernike33[i] * c3[i];
		AA[11 * 15 + 10] += zernike42[i] * s2[i] * zernike33[i] * c3[i];
		AA[11 * 15 + 11] += zernike33[i] * c3[i] * zernike33[i] * c3[i];
		AA[11 * 15 + 12] += zernike33[i] * s3[i] * zernike33[i] * c3[i];
		AA[11 * 15 + 13] += zernike44[i] * c4[i] * zernike33[i] * c3[i];
		AA[11 * 15 + 14] += zernike44[i] * s4[i] * zernike33[i] * c3[i];
	}

	//Matrix Row 13
	for (i = 0; i < NPoint; i++) {
		AA[12 * 15 + 0] += zernike33[i] * s3[i];
		AA[12 * 15 + 1] += zernike20[i] * zernike33[i] * s3[i];
		AA[12 * 15 + 2] += zernike40[i] * zernike33[i] * s3[i];
		AA[12 * 15 + 3] += zernike11[i] * c1[i] * zernike33[i] * s3[i];
		AA[12 * 15 + 4] += zernike11[i] * s1[i] * zernike33[i] * s3[i];
		AA[12 * 15 + 5] += zernike31[i] * c1[i] * zernike33[i] * s3[i];
		AA[12 * 15 + 6] += zernike31[i] * s1[i] * zernike33[i] * s3[i];
		AA[12 * 15 + 7] += zernike22[i] * c2[i] * zernike33[i] * s3[i];
		AA[12 * 15 + 8] += zernike22[i] * s2[i] * zernike33[i] * s3[i];
		AA[12 * 15 + 9] += zernike42[i] * c2[i] * zernike33[i] * s3[i];
		AA[12 * 15 + 10] += zernike42[i] * s2[i] * zernike33[i] * s3[i];
		AA[12 * 15 + 11] += zernike33[i] * c3[i] * zernike33[i] * s3[i];
		AA[12 * 15 + 12] += zernike33[i] * s3[i] * zernike33[i] * s3[i];
		AA[12 * 15 + 13] += zernike44[i] * c4[i] * zernike33[i] * s3[i];
		AA[12 * 15 + 14] += zernike44[i] * s4[i] * zernike33[i] * s3[i];
	}

	//Matrix Row 14
	for (i = 0; i < NPoint; i++) {
		AA[13 * 15 + 0] += zernike44[i] * c4[i];
		AA[13 * 15 + 1] += zernike20[i] * zernike44[i] * c4[i];
		AA[13 * 15 + 2] += zernike40[i] * zernike44[i] * c4[i];
		AA[13 * 15 + 3] += zernike11[i] * c1[i] * zernike44[i] * c4[i];
		AA[13 * 15 + 4] += zernike11[i] * s1[i] * zernike44[i] * c4[i];
		AA[13 * 15 + 5] += zernike31[i] * c1[i] * zernike44[i] * c4[i];
		AA[13 * 15 + 6] += zernike31[i] * s1[i] * zernike44[i] * c4[i];
		AA[13 * 15 + 7] += zernike22[i] * c2[i] * zernike44[i] * c4[i];
		AA[13 * 15 + 8] += zernike22[i] * s2[i] * zernike44[i] * c4[i];
		AA[13 * 15 + 9] += zernike42[i] * c2[i] * zernike44[i] * c4[i];
		AA[13 * 15 + 10] += zernike42[i] * s2[i] * zernike44[i] * c4[i];
		AA[13 * 15 + 11] += zernike33[i] * c3[i] * zernike44[i] * c4[i];
		AA[13 * 15 + 12] += zernike33[i] * s3[i] * zernike44[i] * c4[i];
		AA[13 * 15 + 13] += zernike44[i] * c4[i] * zernike44[i] * c4[i];
		AA[13 * 15 + 14] += zernike44[i] * s4[i] * zernike44[i] * c4[i];
	}

	//Matrix Row 15
	for (i = 0; i < NPoint; i++) {
		AA[14 * 15 + 0] += zernike44[i] * s4[i];
		AA[14 * 15 + 1] += zernike20[i] * zernike44[i] * s4[i];
		AA[14 * 15 + 2] += zernike40[i] * zernike44[i] * s4[i];
		AA[14 * 15 + 3] += zernike11[i] * c1[i] * zernike44[i] * s4[i];
		AA[14 * 15 + 4] += zernike11[i] * s1[i] * zernike44[i] * s4[i];
		AA[14 * 15 + 5] += zernike31[i] * c1[i] * zernike44[i] * s4[i];
		AA[14 * 15 + 6] += zernike31[i] * s1[i] * zernike44[i] * s4[i];
		AA[14 * 15 + 7] += zernike22[i] * c2[i] * zernike44[i] * s4[i];
		AA[14 * 15 + 8] += zernike22[i] * s2[i] * zernike44[i] * s4[i];
		AA[14 * 15 + 9] += zernike42[i] * c2[i] * zernike44[i] * s4[i];
		AA[14 * 15 + 10] += zernike42[i] * s2[i] * zernike44[i] * s4[i];
		AA[14 * 15 + 11] += zernike33[i] * c3[i] * zernike44[i] * s4[i];
		AA[14 * 15 + 12] += zernike33[i] * s3[i] * zernike44[i] * s4[i];
		AA[14 * 15 + 13] += zernike44[i] * c4[i] * zernike44[i] * s4[i];
		AA[14 * 15 + 14] += zernike44[i] * s4[i] * zernike44[i] * s4[i];
	}

	for (i = 0; i < 15 * 15; i++) {
		BB[i] = AA[i];
	}

	MatrixInversion(15, BB);
}

double CRoll::GetDegree(double x, int n) {
	double y;
	if (x <= 0)
		return 0;
	else {
		y = exp(n * log(x));
	}
	return y;
}

int CRoll::factorial1(int k) {
	if (k <= 0)
		return 1;
	else {
		double sum = 1;
		for (int i = 1; i <= k; i++)
			sum *= i;
		return (int(sum));
	}
}

double CRoll::ArcTangent(double x, double y) {
	double zz = 0.0;
	double pi = 4 * atan(1.0f);
	if (sqrt(x * x + y * y) <= 1.0) {
		if ((x >= 0) && (y == 0)) {
			return 0;
		}
		if ((x < 0) && (y == 0)) {
			return pi;
		}
		if ((x == 0) && (y > 0)) {
			return 0.5 * pi;
		}
		if ((x == 0) && (y < 0)) {
			return -0.5 * pi;
		}
		if ((x > 0.0) && (y > 0.0))
			zz = atan2(y, x);
		if ((x < 0.0) && (y > 0.0))
			zz = pi - atan2(y, -x);
		if ((x < 0.0) && (y < 0.0))
			zz = -pi + atan2(-y, -x);
		if ((x > 0.0) && (y < 0.0))
			zz = -atan2(-y, x);
	}
	return zz;
}

void CRoll::Swap(double a, double b) {
	double c = a;
	a = b;
	b = c;
}

void CRoll::MatrixInversion(int n, double* a) {
	double	e;
	int i, j, k;

	for (k = 0; k < n; ++k) {
		e = a[k * n + k];
		a[k * n + k] = 1.0;
		if (e == 0.0)
			return;
		for (j = 0; j < n; ++j)
			a[k * n + j] = a[k * n + j] / e;
		for (i = 0; i < n; ++i) {
			if (i != k) {
				e = a[i * n + k];
				a[i * n + k] = 0.0;
				for (j = 0; j < n; ++j)
					a[i * n + j] = a[i * n + j] - e * a[k * n + j];
			}
		}
	}
}

double CRoll::D1W_phi(int i) {
	double y = zernike11[i] * (-coeff[3] * s1[i] + coeff[4] * c1[i])
		+ zernike31[i] * (-coeff[5] * s1[i] + coeff[6] * c1[i])
		+ 2 * zernike22[i] * (-coeff[7] * s2[i] + coeff[8] * c2[i])
		+ 2 * zernike42[i] * (-coeff[9] * s2[i] + coeff[10] * c2[i])
		+ 3 * zernike33[i] * (-coeff[11] * s3[i] + coeff[12] * c3[i])
		+ 4 * zernike44[i] * (-coeff[13] * s4[i] + coeff[14] * c4[i]);
	return y;
}

double CRoll::D1W_r(int i) {
	double y = coeff[1] * dr1_20[i] + coeff[2] * dr1_40[i]
		+ dr1_11[i] * (coeff[5] * c1[i] + coeff[4] * s1[i])
		+ dr1_31[i] * (coeff[5] * c1[i] + coeff[6] * s1[i])
		+ dr1_22[i] * (coeff[7] * c2[i] + coeff[8] * s2[i])
		+ dr1_42[i] * (coeff[9] * c2[i] + coeff[10] * s2[i])
		+ dr1_33[i] * (coeff[11] * c3[i] + coeff[12] * s3[i])
		+ dr1_44[i] * (coeff[13] * c4[i] + coeff[14] * s4[i]);
	return y;
}

double CRoll::D2W_r(int i) {
	double y = coeff[1] * dr2_20[i] + coeff[2] * dr2_40[i]
		+ dr2_11[i] * (coeff[3] * c1[i] + coeff[4] * s1[i])
		+ dr2_31[i] * (coeff[5] * c1[i] + coeff[6] * s1[i])
		+ dr2_22[i] * (coeff[7] * c2[i] + coeff[8] * s2[i])
		+ dr2_42[i] * (coeff[9] * c2[i] + coeff[10] * s2[i])
		+ dr2_33[i] * (coeff[11] * c3[i] + coeff[12] * s3[i])
		+ dr2_44[i] * (coeff[13] * c4[i] + coeff[14] * s4[i]);
	return y;
}

double CRoll::D2W_phi(int i) {
	double y = -zernike11[i] * (coeff[3] * c1[i] + coeff[4] * s1[i])
		- zernike31[i] * (coeff[5] * c1[i] + coeff[6] * s1[i])
		- 4 * zernike22[i] * (coeff[7] * c2[i] + coeff[8] * s2[i])
		- 4 * zernike42[i] * (coeff[9] * c2[i] + coeff[10] * s2[i])
		- 9 * zernike33[i] * (coeff[11] * c3[i] + coeff[12] * s3[i])
		- 16 * zernike44[i] * (coeff[13] * c4[i] + coeff[14] * s4[i]);
	return y;
}

double CRoll::D2W_r_phi(int i) {
	double y = dr1_11[i] * (-coeff[3] * s1[i] + coeff[4] * c1[i])
		+ dr1_31[i] * (-coeff[5] * s1[i] + coeff[6] * c1[i])
		+ 2 * dr1_22[i] * (-coeff[7] * s2[i] + coeff[8] * c2[i])
		+ 2 * dr1_42[i] * (-coeff[9] * s2[i] + coeff[10] * c2[i])
		+ 3 * dr1_33[i] * (-coeff[11] * s3[i] + coeff[12] * c3[i])
		+ 4 * dr1_44[i] * (-coeff[13] * s4[i] + coeff[14] * c4[i]);
	return(y);
}

double CRoll::D2xx(int i) {
	if (r[i] <= 0)
		return 0;
	double z = D2Wr[i] * c1[i] * c1[i] + (D1Wr[i] * s1[i] * s1[i] - D2Wrphi[i] * s2[i]) / r[i]
		+ (D1Wphi[i] * s2[i] + D2Wphi[i] * s1[i] * s1[i]) / r[i] / r[i];
	return (z);
}

double CRoll::D2xy(int i) {
	if (r[i] <= 0)
		return 0;
	double z = 0.5 * s2[i] * D2Wr[i] + (D2Wrphi[i] * c2[i] - D1Wr[i] * 0.5 * s2[i]) / r[i]
		- (D1Wphi[i] * c2[i] + D2Wphi[i] * 0.5 * s2[i]) / r[i] / r[i];
	return (z);
}

double CRoll::D2yy(int i) {
	if (r[i] <= 0)
		return 0;
	double z = D2Wr[i] * s1[i] * s1[i] + (D2Wrphi[i] * s2[i] + D1Wr[i] * c1[i] * c1[i]) / r[i]
		+ (D2Wphi[i] * c1[i] * c1[i] - D1Wphi[i] * s2[i]) / r[i] / r[i];
	return (z);
}

double CRoll::dr1(int n, int m1, double x) {
	double y;
	int m = abs(m1);
	int nm_minus = (n - m) / 2;
	int nm_plus = (n + m) / 2;
	if ((n == 0) && (m1 == 0))
		return 0;
	if (nm_minus == 0) {
		y = n * GetDegree(x, n - 1);
	}
	else {
		if ((n - m) % 2 != 0)
			y = 0.0;
		else {
			double f0 = factorial1(n);
			double f1 = factorial1(nm_minus);
			double f2 = factorial1(nm_plus);
			double a0 = f0 / (f1 * f2);
			a0 *= n * GetDegree(x, n - 1);
			double summa = 0.0;
			for (int l = 1; l <= nm_minus; l++) {
				if (n - 2 * l - 1 >= 0) {
					double a = factorial1(l) * factorial1(nm_plus - l) * factorial1(nm_minus - l);
					double al = factorial1(n - l) / a;
					if (l % 2 == 1)
						al *= -1;
					al *= (n - 2. * l) * GetDegree(x, n - 2 * l - 1);
					summa += al;
				}
			}
			y = a0 + summa;
		}
	}
	return (y);
}

double CRoll::dr2(int n, int m1, double x) {
	double y = 0.0;
	int m = abs(m1);
	int nm_minus = (n - m) / 2;
	int nm_plus = (n + m) / 2;
	if ((n == 0) || (n == 1))
		return 0;
	if (nm_minus == 0)
		y = n * (n - 1) * GetDegree(x, n - 2);
	else {
		if ((n - m) % 2 != 0) {
			y = 0.0;
		}
		else {
			double f0 = factorial1(n);
			double f1 = factorial1(nm_minus);
			double f2 = factorial1(nm_plus);
			double a0 = f0 / (f1 * f2);
			a0 *= n * (n - 1) * GetDegree(x, n - 2);
			double summa = 0.0;
			for (int l = 1; l <= nm_minus; l++) {
				if (n - 2 * l - 2 >= 0) {
					double a = factorial1(l) * factorial1(nm_plus - l) * factorial1(nm_minus - l);
					double al = factorial1(n - l) / a;
					if (l % 2 == 1)
						al *= -1;
					al *= (n - 2 * l) * (n - 2 * l - 1) * GetDegree(x, n - 2 * l - 2);
					summa += al;
				}
			}
			y = a0 + summa;
		}
	}
	return(y);
}

double CRoll::Laplace(int i) {
	if (r[i] <= 0)
		return 0;
	/*	double z4 = D2W_r(i);
		double z2 = D1W_r(i) / r[i];
		double z3 = D2W_phi(i) / (r[i] * r[i]);
		return (z4+z2+z3);*/
	double z = D2Wr[i] + D1Wr[i]/* / r[i]*/ + D2Wphi[i]/* / r[i] / r[i]*/;
	return (z);
}

double CRoll::Sxx(double m1, int i) {
	if (m1 == 1)
		return 0;
	double z3 = D2xx(i);
	double z2 = Laplace(i);
	z2 *= m1 / (1.0 - m1);
	return (z3 + z2);
}

double CRoll::Sxy(int i) {
	return D2xy(i);
}

double CRoll::Syy(double m1, int i) {
	if (m1 == 1)
		return 0;
	double z3 = D2yy(i);
	double z2 = Laplace(i);
	z2 *= m1 / (1 - m1);
	return (z3 + z2);
}

double CRoll::zernike(int n, int m1, double x) {
	double y = 0.0;
	int m = abs(m1);
	double nm_minus = (n - m) / 2;
	double nm_plus = (n + m) / 2;
	if (n == 0 && m1 == 0)
		return 1;
	if (nm_minus == 0)
		y = GetDegree(x, n);
	else {
		if ((n - m) % 2 != 0)
			y = 0.0;
		else {
			double f0 = factorial1(n);
			double f1 = factorial1(int(nm_minus));
			double f2 = factorial1(int(nm_plus));
			double a0 = f0 / (f1 * f2);
			a0 *= GetDegree(x, n);
			double summa = 0.0;
			for (int i = 1; i <= nm_minus; i++) {
				double a = factorial1(i) * factorial1(int(nm_plus - i)) * factorial1(int(nm_minus - i));
				double al = factorial1(n - i) / a;
				if (i % 2 == 1)
					al *= -1;
				al *= GetDegree(x, n - 2 * i);
				summa += al;
			}
			y = a0 + summa;
		}
	}
	return y;
}

void CRoll::CalculateStressMatrix(int NPoint, StressPoint* p, double fDiam, double fPoisson) {
	int i/*,j*/;

	for (i = 0; i < 15; i++)
		RH[i] = 0;

	//normalize 'x' and 'y'
	for (i = 0; i < NPoint; i++) {
		z1[i] = p[i].z;
	}

	// normalize 'z'
	double fZmin = 1e10, fZmax = -1e10;
	for (i = 0; i < NPoint; i++) {
		if (p[i].z < fZmin)
			fZmin = p[i].z;
		if (p[i].z > fZmax)
			fZmax = p[i].z;
	}
	if (fZmax <= fZmin)
		return;
	for (i = 0; i < NPoint; i++) {
		z1[i] = (p[i].z - fZmin) / (fZmax - fZmin);
	}

	for (i = 0; i < NPoint; i++) {
		RH[0] += z1[i] / NPoint;
		RH[1] += z1[i] * zernike20[i];
		RH[2] += z1[i] * zernike40[i];
		RH[3] += z1[i] * zernike11[i] * c1[i];
		RH[4] += z1[i] * zernike11[i] * s1[i];
		RH[5] += z1[i] * zernike31[i] * c1[i];
		RH[6] += z1[i] * zernike31[i] * s1[i];
		RH[7] += z1[i] * zernike22[i] * c2[i];
		RH[8] += z1[i] * zernike22[i] * s2[i];
		RH[9] += z1[i] * zernike42[i] * c2[i];
		RH[10] += z1[i] * zernike42[i] * s2[i];
		RH[11] += z1[i] * zernike33[i] * c3[i];
		RH[12] += z1[i] * zernike33[i] * s3[i];
		RH[13] += z1[i] * zernike44[i] * c4[i];
		RH[14] += z1[i] * zernike44[i] * s4[i];
	}

	//	 solving the linear algebraic equations system
	//	 operator ' transforms the vector-row in the vector-column
		//R=RH';
		//coeff = AA\R;

	for (i = 0; i < 15; i++) {
		coeff[i] = 0;
		for (int j = 0; j < 15; j++) {
			coeff[i] += BB[i * 15 + j] * RH[j];
		}
	}
	//return the result

	for (i = 0; i < NPoint; i++) {
		p[i].W = coeff[0]
			+ coeff[1] * zernike20[i] + coeff[2] * zernike40[i]
			+ zernike11[i] * (coeff[3] * c1[i] + coeff[4] * s1[i])
			+ zernike31[i] * (coeff[5] * c1[i] + coeff[6] * s1[i])
			+ zernike22[i] * (coeff[7] * c2[i] + coeff[8] * s2[i])
			+ zernike42[i] * (coeff[9] * c2[i] + coeff[10] * s2[i])
			+ zernike33[i] * (coeff[11] * c3[i] + coeff[12] * s3[i])
			+ zernike44[i] * (coeff[13] * c4[i] + coeff[14] * s4[i]);
		p[i].W *= (fZmax - fZmin);
		p[i].W += fZmin;
	}

	//	 calculate multiplier M0=6*(1+m1)*H2/(E1*H1^2)
	//	 calculate multiplier MS=M0*R^2/MZ
	//	 normalized stress(dimensionless)is Sik=sigmaik*MS
	//	 take into account that R is measured in mm.

	for (i = 0; i < NPoint; i++) {
		D1Wr[i] = D1W_r(i);
		D1Wphi[i] = D1W_phi(i);
		D2Wr[i] = D2W_r(i);
		D2Wphi[i] = D2W_phi(i);
		D2Wrphi[i] = D2W_r_phi(i);
	}

	for (i = 0; i < NPoint; i++) {
		if (r[i] != 0) {
			p[i].sigmaxx = Sxx(fPoisson, i);
			p[i].sigmaxy = Sxy(i);
			p[i].sigmayy = Syy(fPoisson, i);
		}
		else {
			p[i].sigmaxx = 0;
			p[i].sigmaxy = 0;
			p[i].sigmayy = 0;
		}
	}
}

void CRoll::CalculateCubic(int NPoint, StressPoint* p, double fDiam, double fPoisson) {
	double AA[10 * 10];
	double BB[10];
	double RH[10];
	double Summ[7 * 7];

	double R = fDiam / 2;

	int i, j, n, m;
	double S1;

	memset(AA, 0, 10 * 10 * sizeof(double));
	memset(BB, 0, 10 * sizeof(double));
	memset(RH, 0, 10 * sizeof(double));
	memset(Summ, 0, 7 * 7 * sizeof(double));

	//normalize 'x' and 'y'
/*	for (i=0; i<NPoint; i++)
	{
		p[i].x /= R;
		p[i].y /= R;
	}
	//normalize 'z'
	double fZmin = 1e10, fZmax=  -1e10;
	for (i=0; i<NPoint; i++)
	{
		if (p[i].z < fZmin)
			fZmin = p[i].z;
		if (p[i].z > fZmax)
			fZmax = p[i].z;
	}
	if (fZmax <= fZmin)
		return;
	for (i=0; i<NPoint; i++)
	{
		p[i].z = (p[i].z - fZmin)/(fZmax-fZmin);
	}*/

	for (n = 0; n < 7; n++) {
		for (m = 0; m < 7; m++) {
			for (i = 0; i < NPoint; i++) {
				S1 = 1.0;
				if (n > 0) {
					for (j = 0; j < n; j++)
						S1 *= p[i].x;
				}
				if (m > 0) {
					for (j = 0; j < m; j++)
						S1 *= p[i].y;
				}
				Summ[n * 7 + m] += S1;
			}
		}
	}

	AA[0 * 10 + 0] = Summ[4 * 7 + 0];
	AA[0 * 10 + 1] = Summ[3 * 7 + 1];
	AA[0 * 10 + 2] = Summ[2 * 7 + 2];
	AA[0 * 10 + 3] = Summ[3 * 7 + 2];
	AA[0 * 10 + 4] = Summ[4 * 7 + 1];
	AA[0 * 10 + 5] = Summ[2 * 7 + 0];
	AA[0 * 10 + 6] = Summ[5 * 7 + 0];
	AA[0 * 10 + 7] = Summ[2 * 7 + 3];
	AA[0 * 10 + 8] = Summ[3 * 7 + 0];
	AA[0 * 10 + 9] = Summ[2 * 7 + 1];

	AA[1 * 10 + 0] = Summ[3 * 7 + 1];
	AA[1 * 10 + 1] = Summ[2 * 7 + 2];
	AA[1 * 10 + 2] = Summ[1 * 7 + 3];
	AA[1 * 10 + 3] = Summ[2 * 7 + 3];
	AA[1 * 10 + 4] = Summ[3 * 7 + 2];
	AA[1 * 10 + 5] = Summ[1 * 7 + 1];
	AA[1 * 10 + 6] = Summ[4 * 7 + 1];
	AA[1 * 10 + 7] = Summ[1 * 7 + 4];
	AA[1 * 10 + 8] = Summ[2 * 7 + 1];
	AA[1 * 10 + 9] = Summ[1 * 7 + 2];

	AA[2 * 10 + 0] = Summ[2 * 7 + 2];
	AA[2 * 10 + 1] = Summ[1 * 7 + 3];
	AA[2 * 10 + 2] = Summ[0 * 7 + 4];
	AA[2 * 10 + 3] = Summ[1 * 7 + 4];
	AA[2 * 10 + 4] = Summ[2 * 7 + 3];
	AA[2 * 10 + 5] = Summ[0 * 7 + 2];
	AA[2 * 10 + 6] = Summ[3 * 7 + 2];
	AA[2 * 10 + 7] = Summ[0 * 7 + 5];
	AA[2 * 10 + 8] = Summ[1 * 7 + 2];
	AA[2 * 10 + 9] = Summ[0 * 7 + 3];

	AA[3 * 10 + 0] = Summ[3 * 7 + 2];
	AA[3 * 10 + 1] = Summ[2 * 7 + 3];
	AA[3 * 10 + 2] = Summ[1 * 7 + 4];
	AA[3 * 10 + 3] = Summ[2 * 7 + 4];
	AA[3 * 10 + 4] = Summ[3 * 7 + 3];
	AA[3 * 10 + 5] = Summ[1 * 7 + 2];
	AA[3 * 10 + 6] = Summ[4 * 7 + 2];
	AA[3 * 10 + 7] = Summ[1 * 7 + 5];
	AA[3 * 10 + 8] = Summ[2 * 7 + 2];
	AA[3 * 10 + 9] = Summ[1 * 7 + 3];

	AA[4 * 10 + 0] = Summ[4 * 7 + 1];
	AA[4 * 10 + 1] = Summ[3 * 7 + 2];
	AA[4 * 10 + 2] = Summ[2 * 7 + 3];
	AA[4 * 10 + 3] = Summ[3 * 7 + 3];
	AA[4 * 10 + 4] = Summ[4 * 7 + 2];
	AA[4 * 10 + 5] = Summ[2 * 7 + 1];
	AA[4 * 10 + 6] = Summ[5 * 7 + 1];
	AA[4 * 10 + 7] = Summ[2 * 7 + 4];
	AA[4 * 10 + 8] = Summ[3 * 7 + 1];
	AA[4 * 10 + 9] = Summ[2 * 7 + 2];

	AA[5 * 10 + 0] = Summ[2 * 7 + 0];
	AA[5 * 10 + 1] = Summ[1 * 7 + 1];
	AA[5 * 10 + 2] = Summ[0 * 7 + 2];
	AA[5 * 10 + 3] = Summ[1 * 7 + 2];
	AA[5 * 10 + 4] = Summ[2 * 7 + 1];
	AA[5 * 10 + 5] = NPoint;
	AA[5 * 10 + 6] = Summ[3 * 7 + 0];
	AA[5 * 10 + 7] = Summ[0 * 7 + 3];
	AA[5 * 10 + 8] = Summ[1 * 7 + 0];
	AA[5 * 10 + 9] = Summ[0 * 7 + 1];

	AA[6 * 10 + 0] = Summ[5 * 7 + 0];
	AA[6 * 10 + 1] = Summ[4 * 7 + 1];
	AA[6 * 10 + 2] = Summ[3 * 7 + 2];
	AA[6 * 10 + 3] = Summ[4 * 7 + 2];
	AA[6 * 10 + 4] = Summ[5 * 7 + 1];
	AA[6 * 10 + 5] = Summ[3 * 7 + 0];
	AA[6 * 10 + 6] = Summ[6 * 7 + 0];
	AA[6 * 10 + 7] = Summ[3 * 7 + 3];
	AA[6 * 10 + 8] = Summ[4 * 7 + 0];
	AA[6 * 10 + 9] = Summ[3 * 7 + 1];

	AA[7 * 10 + 0] = Summ[2 * 7 + 3];
	AA[7 * 10 + 1] = Summ[1 * 7 + 4];
	AA[7 * 10 + 2] = Summ[0 * 7 + 5];
	AA[7 * 10 + 3] = Summ[1 * 7 + 5];
	AA[7 * 10 + 4] = Summ[2 * 7 + 4];
	AA[7 * 10 + 5] = Summ[0 * 7 + 3];
	AA[7 * 10 + 6] = Summ[3 * 7 + 3];
	AA[7 * 10 + 7] = Summ[0 * 7 + 6];
	AA[7 * 10 + 8] = Summ[1 * 7 + 3];
	AA[7 * 10 + 9] = Summ[0 * 7 + 4];

	AA[8 * 10 + 0] = Summ[3 * 7 + 0];
	AA[8 * 10 + 1] = Summ[2 * 7 + 1];
	AA[8 * 10 + 2] = Summ[1 * 7 + 2];
	AA[8 * 10 + 3] = Summ[2 * 7 + 2];
	AA[8 * 10 + 4] = Summ[3 * 7 + 1];
	AA[8 * 10 + 5] = Summ[1 * 7 + 0];
	AA[8 * 10 + 6] = Summ[4 * 7 + 0];
	AA[8 * 10 + 7] = Summ[1 * 7 + 3];
	AA[8 * 10 + 8] = Summ[2 * 7 + 0];
	AA[8 * 10 + 9] = Summ[1 * 7 + 1];

	AA[9 * 10 + 0] = Summ[2 * 7 + 1];
	AA[9 * 10 + 1] = Summ[1 * 7 + 2];
	AA[9 * 10 + 2] = Summ[0 * 7 + 3];
	AA[9 * 10 + 3] = Summ[1 * 7 + 3];
	AA[9 * 10 + 4] = Summ[2 * 7 + 2];
	AA[9 * 10 + 5] = Summ[0 * 7 + 1];
	AA[9 * 10 + 6] = Summ[3 * 7 + 1];
	AA[9 * 10 + 7] = Summ[0 * 7 + 4];
	AA[9 * 10 + 8] = Summ[1 * 7 + 1];
	AA[9 * 10 + 9] = Summ[0 * 7 + 2];

	memset(RH, 0, 10 * sizeof(double));

	for (i = 0; i < NPoint; i++) {
		RH[0] += (p[i].x * p[i].x * p[i].z);
		RH[1] += (p[i].x * p[i].y * p[i].z);
		RH[2] += (p[i].y * p[i].y * p[i].z);
		RH[3] += (p[i].x * p[i].y * p[i].y * p[i].z);
		RH[4] += (p[i].x * p[i].x * p[i].y * p[i].z);
		RH[5] += p[i].z;
		RH[6] += (p[i].x * p[i].x * p[i].x * p[i].z);
		RH[7] += (p[i].y * p[i].y * p[i].y * p[i].z);
		RH[8] += (p[i].x * p[i].z);
		RH[9] += (p[i].y * p[i].z);
	}

	MatrixInversion(10, AA);

	memset(BB, 0, 10 * sizeof(double));

	for (i = 0; i < 10; i++) {
		for (j = 0; j < 10; j++) {
			BB[i] += AA[i * 10 + j] * RH[j];
		}
	}

	double fi, Dxx, Dxy, Dyy, alpha, beta, cs2, sn2, cssn;

	alpha = 1 / (1.0 - fPoisson);
	beta = 1 - alpha;

	for (i = 0; i < NPoint; i++) {
		p[i].W = BB[0] * p[i].x * p[i].x + BB[1] * p[i].x * p[i].y
			+ BB[2] * p[i].y * p[i].y + BB[3] * p[i].x * p[i].y * p[i].y
			+ BB[4] * p[i].x * p[i].x * p[i].y + BB[5]
			+ BB[6] * p[i].x * p[i].x * p[i].x + BB[7] * p[i].y * p[i].y * p[i].y
			+ BB[8] * p[i].x + BB[9] * p[i].y;
		Dxx = 2.0 * BB[0] + 2.0 * BB[4] * p[i].y + 6.0 * BB[6] * p[i].x;
		Dxy = BB[1] + 2.0 * BB[3] * p[i].y + 2.0 * BB[4] * p[i].x;
		Dyy = 2.0 * BB[2] + 2.0 * BB[3] * p[i].x + 6.0 * BB[7] * p[i].y;
		p[i].sigmaxx = alpha * Dxx + beta * Dyy;
		p[i].sigmaxy = Dxy;
		p[i].sigmayy = beta * Dxx + alpha * Dyy;
		///
		fi = ArcTangent(p[i].x, p[i].y);
		cs2 = cos(fi);
		sn2 = sin(fi);
		cssn = cs2 * sn2;
		cs2 *= cs2;
		sn2 *= sn2;
		p[i].sigmarr = p[i].sigmaxx * cs2 + 2 * p[i].sigmaxy * cssn + p[i].sigmayy * sn2;
		p[i].sigmart = (p[i].sigmayy - p[i].sigmaxy) * cssn + p[i].sigmaxy * (cs2 - sn2);
		p[i].sigmatt = p[i].sigmaxx * sn2 + 2 * p[i].sigmaxy * cssn + p[i].sigmayy * cs2;
		///
	}
}