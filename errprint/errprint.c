#include <stdio.h>
#include <stdarg.h>

#include "errprint.h"

void errprint (const char *p_color, const char *p_msg, ...)
{
    va_list args;
    va_start(args, p_msg);

    fprintf(stderr, "%s", p_color); // set color

    vfprintf(stderr, p_msg, args);  // print args

    fprintf(stderr, RESET);        // reset color
    fprintf(stderr, "\n");         // end in newline

    va_end(args);
}