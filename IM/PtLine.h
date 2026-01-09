#pragma once

class CPtLine {// maths between a line and a point [2/1/2020 yuenl]
public:
	CPtLine();
	~CPtLine();

	class Vec2 {
	public:
		float _x = 0, _y = 0;
		Vec2() { _x = 0; _y = 0; }
		Vec2(const float x, const float y) { _x = x; _y = y; }
		Vec2 operator+(const Vec2& v) const { return Vec2(this->_x + v._x, this->_y + v._y); }
		Vec2 operator-(const Vec2& v) const { return Vec2(this->_x - v._x, this->_y - v._y); }
		Vec2 operator*(const float f) const { return Vec2(this->_x * f, this->_y * f); }
		float MagnitudeSquare(const Vec2 p) const {
			const float dX = p._x - this->_x; const float dY = p._y - this->_y;
			return dX * dX + dY * dY;
		}
		float DistanceTo(const Vec2 p) const { return float(sqrt(this->MagnitudeSquare(p))); }
		float DotProduct(const Vec2 p) const { return this->_x * p._x + this->_y * p._y; }
		float PerpDotProduct(const Vec2 p) const { return this->_x * p._y - this->_y * p._x; }
	};

	float DotProduct(Vec2& v1, Vec2& v2);
	float PerpDotProduct(Vec2& v1, Vec2& v2);
	float PerpDotProduct(Vec2& v1, Vec2& v2, Vec2& p);
	float GetEpsilon(Vec2 v1, Vec2 v2);
	BOOL IsPointOnLine(Vec2 v1, Vec2 v2, Vec2 p);
	BOOL IsPointOnLineSegment(Vec2 v1, Vec2 v2, Vec2 p);
	BOOL IsProjectedPointOnLineSegment(Vec2 v1, Vec2 v2, Vec2 p);
	Vec2 GetProjectedPointOnLineFast(Vec2 v1, Vec2 v2, Vec2 p);
	// return minimum distance between line segment vw and point p, and the closest point on the line segment, q
	float DistancePointToLine(const Vec2 v, const Vec2 w, const Vec2 p, Vec2* const q);
	float DistancePointToLineSeg(float segX1, float segY1, float segX2, float segY2, float pX, float pY, float* qX, float* qY);

	void TestDistanceFromLineSegmentToPoint(float segmentX1, float segmentY1, float segmentX2, float segmentY2, float pX, float pY);
	void TestDistanceFromLineSegmentToPoint();
};
