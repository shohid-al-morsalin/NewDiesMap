#pragma once

#include <vector>
#include <cassert>
// using namespace std;

#include "BaseException.h"
#include "BaseData.h"

class InterpolaterException : public BaseException {
public:
	/**	Constructor.
	*	@param		message - This should be a useful message for error tracking.
	*	@exceptions	None
	*/
	InterpolaterException(string message) throw() : BaseException(message) {
	}
	/**	Constructor.
	*	@param		message - This should be a useful message for error tracking.
	*	@param		location - Additional message. You can use LOCATION macro here.
	*	@exceptions	None
	*/
	InterpolaterException(string message, string location) throw() : BaseException(message, location) {
	}
};

template<class T>
class Interpolater : public T {
public:
	template<class ForwardIterator>
	double GetInterpolatedZ(double xpos, double ypos,
		ForwardIterator first, ForwardIterator last
	) const throw(InterpolaterException) {
		return T::GetInterpolatedZ(xpos, ypos, first, last);
	}
};

inline bool PointCompare(const Point3D& pt1, const Point3D& pt2) throw() {
	if (pt1.y > pt2.y)
		return true;
	else if (pt1.y == pt2.y) {
		if (pt1.x < pt2.x)
			return true;
		else
			return false;
	}
	else
		return false;
}

template<class InterpolateAlogrithm, class ResultContainer, class ForwardIterator>
void Smooth3DData(const Interpolater<InterpolateAlogrithm>* pInter, ResultContainer& result, ForwardIterator first,
	ForwardIterator last, double xmin = -100, double xmax = 100, double xstep = 1, double ymin = -100,
	double ymax = 100, double ystep = 1, double radius = 200, double exponent = 2) throw() {
	// Check if Data is valid
	assert(first != last);
	assert(xmax > xmin);
	assert(ymax > ymin);

	// If user gave non-empty result vector, clear it.
	result.clear();

	int counter = 0;
	//Step through grid where y is each row (i) and x is each column (j)
	for (double ypos = ymin; ypos <= ymax; ypos += ystep) { //for each row
		double distance = 0;
		double weight = 0;
		double totalWeight = 0;
		double nodeValue = 0;
		for (double xpos = xmin; xpos <= xmax; xpos += xstep) { //for each column
// Maybe resizing before-hand is more efficient....
			result.resize(result.size() + 1);
			Point3D pt(xpos, ypos, pInter->GetInterpolatedZ(xpos, ypos, first, last, exponent));
			result.push_back(pt);
			counter++;
		}//endfor each point
	}//endfor each polygon

	sort(result.begin(), result.end(), PointCompare);
}
