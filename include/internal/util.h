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

#define vec_for_each( index, vec ) for( size_t index = 0 ; index < vec.size() ; ++index )

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

static glm::vec3 __nullvec = glm::vec3(0);
static glm::vec3 __rotvec = glm::vec3(2*M_PI, 0, 0);
static glm::vec3 __scalevec = glm::vec3(1);
glm::mat4 makeModel( const glm::vec3& origin = __nullvec,
                     const glm::vec3& rotation = __rotvec,
                     const glm::vec3& scale = __scalevec );

#endif //INTERNAL_UTIL_H
