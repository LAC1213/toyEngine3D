#include <util.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <math.h>
#include <memory.h>

void __errorExit( const char * file, const char * func, unsigned int line, const char * format, ... )
{
    va_list args;                          
    fprintf(stderr, "\x1b[1;31m[%s:%u] Aborting in %s(): ", file, line, func);
    fprintf(stderr, format, args);
    perror("\x1b[0;39m\n\terrno ");
     
    exit(EXIT_FAILURE);
}

