#include "LKH.h"
#include "../CLKHSolver.h"

void CLKHWriteTour(char *FileName, int *Tour, GainType Cost)
{
    FILE *TourFile;
    int i, j, n, Forwards;

    time_t Now;

    if (FileName == 0)
        return;

    Now = time(&Now);
    assert(TourFile = fopen(FileName, "w"));
    fprintf(TourFile, "NAME : %s." GainFormat ".tour\n", Name, Cost);
    fprintf(TourFile, "COMMENT : Length = " GainFormat "\n", Cost);
    fprintf(TourFile, "COMMENT : Found by CLKH [Keld Helsgaun] %s",
            ctime(&Now));
    fprintf(TourFile, "TYPE : TOUR\n");
    n = ProblemType != ATSP ? Dimension : Dimension / 2;
    fprintf(TourFile, "DIMENSION : %d\n", n);
    fprintf(TourFile, "TOUR_SECTION\n");
    
    i = 1;
    for (j = 1; j <= n; j++)
        if (Tour[j] < Tour[i])
            i = j;
    Forwards = ProblemType == ATSP ||
        Tour[i < n ? i + 1 : 1] < Tour[i > 1 ? i - 1 : Dimension];
    for (j = 1; j <= n; j++) {
        fprintf(TourFile, "%d\n", Tour[i]);
        if (Forwards) {
            if (++i > n)
                i = 1;
        } else if (--i < 1)
            i = n;
    }
    fprintf(TourFile, "-1\nEOF\n");
    if (TourFile != NULL) fclose(TourFile);
    if (TraceLevel >= 1) printff("done\n");
}
