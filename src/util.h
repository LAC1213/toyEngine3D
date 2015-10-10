#ifndef UTIL_H
#define UTIL_H

#include <stdarg.h>

#define VA_ARGS(...) ,##__VA_ARGS__
#define errorExit( format, ... ) __errorExit( __FILE__, __FUNCTION__, __LINE__, format VA_ARGS(__VA_ARGS__))

#define max(a,b) \
       ({ __typeof__ (a) _a = (a); \
              __typeof__ (b) _b = (b); \
            _a > _b ? _a : _b;  })

void __errorExit( const char * file, const char * func, unsigned int line, const char * format, ... );

#endif //UTIL_H
