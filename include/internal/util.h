#ifndef INTERNAL_UTIL_H
#define INTERNAL_UTIL_H

#include <stdarg.h>

#define VA_ARGS(...) ,##__VA_ARGS__
#define errorExit( format, ... ) __errorExit( __FILE__, __FUNCTION__, __LINE__, format VA_ARGS(__VA_ARGS__))

void __errorExit( const char * file, const char * func, unsigned int line, const char * format, ... );

#endif //INTERNAL_UTIL_H
