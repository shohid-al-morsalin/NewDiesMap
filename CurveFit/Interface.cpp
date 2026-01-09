#include "stdio.h"
#include <math.h>
#include <stdio.h>
#include "Interface.h"
#include "lmmin.h"
#include "lm_eval.h"
unsigned NumParam = 0;

extern "C" double my_fit_function(double t, double* pVal) {
	double RetVal = 0;
	for (int Index = 0; Index < (int)NumParam; Index++) {
		RetVal += pVal[Index] * ::pow(t, Index);
	}
	return RetVal;
}

double my_fit_function2(double t, double* p) {
	return (p[0] * t + (1 - p[0] + p[1] + p[2]) * t * t) /
		(1 + p[1] * t + p[2] * t * t);
}

extern "C" double my_fit_function3(double t, double* pVal) {
	double RetVal = 1;
	for (unsigned Index = 0; Index < ::NumParam; Index++) {
		RetVal *= (pVal[Index] - t);
	}
	return RetVal;
}

extern "C" int CurveFit(double* XData, double* YData, double* Res, unsigned nData, unsigned nOrder) {
	//::NumParam = nOrder;
	::NumParam = nOrder;
	/*    double *pVal = new double[nOrder];
		for(unsigned Index= 0;Index <nOrder;Index++)
		{
			pVal[Index] =1.;
		}*/

		// auxiliary settings:

	lm_control_type control;
	lm_data_type data;

	lm_initialize_control(&control);

	data.user_func = my_fit_function;
	data.user_t = XData;
	data.user_y = YData;

	// perform the fit:

//     printf
// 	("modify or replace lm_print_default for less verbous fitting\n");

	lm_minimize(nData, nOrder, Res, lm_evaluate_default, lm_print_default,
		&data, &control);

	// print results:

//     printf("status: %s after %d evaluations\n",
// 	   lm_shortmsg[control.info], control.nfev);
//	delete [] pVal;
	return ((control.info > 0) && (control.info < 4));
}