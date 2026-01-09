#pragma once

#include "Interpolater.h"
#include <cassert>
#include <cmath>

class InverseDist {
public:
	template<class ForwardIterator>
	double GetInterpolatedZ(double xpos, double ypos,
		ForwardIterator first, ForwardIterator last
	) const throw(InterpolaterException) {
		// Check if Data is valid
		assert(first != last);
		double weight = 0;
		double totalWeight = 0;
		double nodeValue = 0;
		double dfRatio = 0.01;

		while (first != last) {
			float f = (*first).z;
			if ((*first).z <= 0) {
				++first;
				continue;
			}
			float dx = (xpos - (*first).x);
			float dy = (ypos - (*first).y);
			weight = exp(-sqrt(dx * dx + dy * dy) * dfRatio);
			totalWeight += weight;
			nodeValue += (*first).z * weight;
			++first;
		}
		return nodeValue / totalWeight;
	}
};
