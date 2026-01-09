#pragma once

template<typename T>
struct TPoint2D {
	TPoint2D() : x(0), y(0) {
	}
	TPoint2D(T xx, T yy) : x(xx), y(yy) {
	}
	T x;
	T y;
};

typedef TPoint2D<double> Point2D;

template<typename T>
struct TPoint3D {
	TPoint3D() : x(0), y(0), z(0), d(1) {
	}
	TPoint3D(T xx, T yy, T zz) : x(xx), y(yy), z(zz), d(1) {
	}
	T x;
	T y;
	T z;
	char d;
};

typedef TPoint3D<float> Point3D;

inline bool Point3DCompareZ(const Point3D& pt1, const Point3D& pt2) {
	return pt1.z < pt2.z;
}

inline bool Point3DCompareX(const Point3D& pt1, const Point3D& pt2) {
	return pt1.x < pt2.x;
}
inline bool Point3DCompareY(const Point3D& pt1, const Point3D& pt2) {
	return pt1.y < pt2.y;
}
