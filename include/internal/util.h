#ifndef INTERNAL_UTIL_H
#define INTERNAL_UTIL_H

#include <stdarg.h>
#include <iostream>
#include <string>

#include <btBulletDynamicsCommon.h>
#include <glm/glm.hpp>

#define VA_ARGS(...) ,##__VA_ARGS__
#define errorExit( format, ... ) __errorExit( __FILE__, __FUNCTION__, __LINE__, format VA_ARGS(__VA_ARGS__))

void __errorExit( const char * file, const char * func, unsigned int line, const char * format, ... );

enum Log
{
    log_warn,
    log_info,
    log_alert,
    log_endl
};

std::ostream& operator<<( std::ostream& os, Log l );
std::ostream& operator<<( std::ostream& os, glm::vec2 v2 );
std::ostream& operator<<( std::ostream& os, glm::vec3 v3 );
std::ostream& operator<<( std::ostream& os, glm::vec4 v4 );

glm::vec3 bt2glm( const btVector3& vec );
btVector3 glm2bt( const glm::vec3& vec );

glm::mat4 makeModel( const glm::vec3& origin, const glm::vec3& rotation, const glm::vec3& scale );

#endif //INTERNAL_UTIL_H
