#include "stdafx.h"
#include "PtLine.h"

CPtLine::CPtLine() {
}

CPtLine::~CPtLine() {
}

// return minimum distance between line segment vw and point p, and the closest point on the line segment, q

float CPtLine::DotProduct(Vec2& v1, Vec2& v2) {
	return  v1._x * v2._x + v1._y * v2._y;
}

float CPtLine::PerpDotProduct(Vec2& v1, Vec2& v2) {
	return  v1._x * v2._y - v1._y * v2._x;
}

float CPtLine::PerpDotProduct(Vec2& v1, Vec2& v2, Vec2& p) {
	return (v1._x - p._x) * (v2._y - p._y) - (v1._y - p._y) * (v2._x - p._x);
}

float CPtLine::GetEpsilon(Vec2 v1, Vec2 v2) {
	float dx1 = v2._x - v1._x;
	float dy1 = v2._y - v1._y;
	return float(0.003 * (dx1 * dx1 + dy1 * dy1));
}

BOOL CPtLine::IsPointOnLine(Vec2 v1, Vec2 v2, Vec2 p) {
	return fabs(PerpDotProduct(v1, v2, p)) < fabs(GetEpsilon(v1, v2));
}

BOOL CPtLine::IsPointOnLineSegment(Vec2 v1, Vec2 v2, Vec2 p) {
	if (!((v1._x <= p._x && p._x <= v2._x) || (v2._x <= p._x && p._x <= v1._x))) {
		return false;
	}
	if (!((v1._y <= p._y && p._y <= v2._y) || (v2._y <= p._y && p._y <= v1._y))) {
		return false;
	}
	return IsPointOnLine(v1, v2, p);
}

BOOL CPtLine::IsProjectedPointOnLineSegment(Vec2 v1, Vec2 v2, Vec2 p) {
	// Check if the projected test point onto the line is on the line segment.
	// @return true if projected point is on line segment, false otherwise
	// get dotproduct |e1| * |e2|
	Vec2 e1(v2._x - v1._x, v2._y - v1._y);
	float recArea = DotProduct(e1, e1);
	// dot product of |e1| * |e2|
	Vec2 e2(p._x - v1._x, p._y - v1._y);
	double val = DotProduct(e1, e2);
	return (val > 0 && val < recArea);
}

CPtLine::Vec2 CPtLine::GetProjectedPointOnLineFast(Vec2 v1, Vec2 v2, Vec2 p) {
	//Get projected point P' of P on line e1. Faster version.
	//@return projected point p.
	// get dot product of e1, e2
	Vec2 e1(v2._x - v1._x, v2._y - v1._y);
	Vec2 e2(p._x - v1._x, p._y - v1._y);
	double val = DotProduct(e1, e2);
	// get squared length of e1
	double len2 = e1._x * e1._x + e1._y * e1._y;
	Vec2 v(float(v1._x + (val * e1._x) / len2), float(v1._y + (val * e1._y) / len2));
	return v;
}

float CPtLine::DistancePointToLine(const Vec2 v, const Vec2 w, const Vec2 p, Vec2* const q) {// v-w describe the line, p is a point, q (lying on the line) is the nearest point to p [6/19/2020 yuenl]
	float distSq = v.MagnitudeSquare(w); // i.e. |w-v|^2 ... avoid a sqrt
	if (distSq == 0.0) {
		// v == w case
		(*q) = v;
		return v.DistanceTo(p);
	}
	// consider the line extending the segment, parameterized as v + t (w - v)
	// we find projection of point p onto the line
	// it falls where t = [(p-v) . (w-v)] / |w-v|^2
	const float t = (p - v).DotProduct(w - v) / distSq;
	if (t < 0.0) {
		// beyond the v end of the segment
		(*q) = v;
		return v.DistanceTo(p);
	}
	else if (t > 1.0) {
		// beyond the w end of the segment
		(*q) = w;
		return w.DistanceTo(p);
	}
	// projection falls on the segment
	const Vec2 projection = v + ((w - v) * t);
	(*q) = projection;
	return p.DistanceTo(projection);
}

float CPtLine::DistancePointToLineSeg(float segX1, float segY1, float segX2, float segY2, float pX, float pY, float* qX, float* qY) {
	Vec2 q;
	float distance = DistancePointToLine(Vec2(segX1, segY1), Vec2(segX2, segY2), Vec2(pX, pY), &q);
	(*qX) = q._x;
	(*qY) = q._y;
	return distance;
}

void CPtLine::TestDistanceFromLineSegmentToPoint(float segmentX1, float segmentY1, float segmentX2, float segmentY2, float pX, float pY) {
	float qX;
	float qY;
	float d = DistancePointToLineSeg(segmentX1, segmentY1, segmentX2, segmentY2, pX, pY, &qX, &qY);
	printf("line segment = ( ( %f, %f ), ( %f, %f ) ), p = ( %f, %f ), distance = %f, q = ( %f, %f )\n",
		segmentX1, segmentY1, segmentX2, segmentY2, pX, pY, d, qX, qY);
}

void CPtLine::TestDistanceFromLineSegmentToPoint() {
	TestDistanceFromLineSegmentToPoint(0, 0, 1, 1, 1, 0);
	TestDistanceFromLineSegmentToPoint(0, 0, 20, 10, 5, 4);
	TestDistanceFromLineSegmentToPoint(0, 0, 20, 10, 30, 15);
	TestDistanceFromLineSegmentToPoint(0, 0, 20, 10, -30, 15);
	TestDistanceFromLineSegmentToPoint(0, 0, 10, 0, 5, 1);
	TestDistanceFromLineSegmentToPoint(0, 0, 0, 10, 1, 5);
}