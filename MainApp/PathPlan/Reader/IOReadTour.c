#include "io.h"

static const char Delimiters[] = " :=\n\t\r\f\v\xef\xbb\xbf";
/*读取结果的Tour文件*/
int IOReadTour(char* fname) {
	int rval = 1;
	int i;
	char *Line, *Keyword;

	if ((Tour = fopen(fname, "r")) == NULL)
		IOeprintf("无法打开文件%s",fname);

	tour = (int*)malloc(sizeof(int)*IONumPoint);

	while (Line = IOReadLine(Tour)) {
		if ((Keyword = strtok(Line, Delimiters)) == NULL)
			continue;
		for (i = 0; i < (int)strlen(Keyword); i++)
			Keyword[i] = (char)toupper(Keyword[i]);
        if (!strcmp(Keyword, "TOUR_SECTION")) {
            for (i = 1; i <= IONumPoint; i++) {
                IOfscanint(Tour, &tour[i - 1]);
            }
        }
        else if (!strcmp(Keyword, "EOF")) {
            rval = 0; goto CLEANUP;
        }
        else continue;
	}
CLEANUP:
	if (Tour != (FILE*) NULL) {
		fclose(Tour); 
	}
    if (IOLastLine != NULL) TSP_Free(IOLastLine);
	return rval;
}
