#include "LKH.h"
#include <stdarg.h>

/* 
 * The eprintf function prints an error message and exits.
 */

void eprintf(const char *fmt, ...)
{
    va_list args;
    char q;

    if (LastLine && *LastLine)
        fprintf(stderr, "\n%s\n", LastLine);
    fprintf(stderr, "\n*** Error ***\n");
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");
    printf("����q����:\n");
    scanf("%c", &q);
    exit(EXIT_FAILURE);
}
