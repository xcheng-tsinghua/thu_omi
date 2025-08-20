#include "LKH.h"
#include "unistd.h"
#include "../CLKHSolver.h"


/*
 * This file contains the main function of the CLKH program.
 */
int CLKHSolve(char * paraFileName)
{
    GainType Cost;
    int *Tour;
    char TourFileNameBk[255];

    /* Read the specification of the problem */
    ParameterFileName = paraFileName;

    ReadParameters();
    CLKHReadProblem();
    strcpy(TourFileNameBk, OutputTourFileName);
    assert(Tour = (int *) malloc((DimensionSaved + 1) * sizeof(int)));
    Cost = SolveCTSP(Tour); //TourÊÇÂ·¾¶ÐòÁÐ
    TraceLevel = 1;
    OutputTourFileName = TourFileNameBk;
    if (OutputTourFileName)
        CLKHWriteTour(OutputTourFileName, Tour, Cost);
    printff("Value = " GainFormat "\n", Cost);
    return 0;
}

