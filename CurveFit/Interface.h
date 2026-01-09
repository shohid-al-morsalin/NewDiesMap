#ifndef CURVEFIT_INTERFACE_INCLUDED
#define CURVEFIT_INTERFACE_INCLUDED

#ifdef __cplusplus
extern "C"
{
#endif

	int CurveFit(double* XData, double* YData, double* Res, unsigned nData, unsigned nOrder);

#ifdef __cplusplus
}
#endif

#endif /* CURVEFIT_INTERFACE_INCLUDED */
