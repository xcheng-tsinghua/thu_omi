#ifndef _CLKHTSP_H
#define _CLKHTSP_H

#ifdef __cplusplus
extern "C" {
#endif
#include "SRC\GainType.h"
int CLKHSolve(char *fname);
GainType SolveCTSP(int *Tour);
GainType SolveTSP(int Dimension, char *ParFileName, char *TourFileName, int *Tour, GainType Optimum, GainType Deduction);
void CLKHReadProblem(void);
void CLKHWriteTour(char *FileName, int *Tour, GainType Cost);


#ifdef __cplusplus
};
#endif

#endif // !_CLKHTSP_H

