#include <internal/util.h>
#include <glm/gtc/matrix_transform.hpp>

void __errorExit ( const char * file, const char * func, unsigned int line, const char * format, ... )
{
    va_list args;
    fprintf ( stderr, "\x1b[1;31m[%s:%u] Aborting in %s(): ", file, line, func );
    va_start ( args, format );
    vfprintf ( stderr, format, args );
    va_end ( args );
    perror ( "\x1b[0;39m\n\terrno " );

    exit ( EXIT_FAILURE );
}

std::ostream& operator<< ( std::ostream& os, Log l )
{
    switch ( l )
    {
    case log_warn:
        return os << "\x1b[33m[Warning] ";
    case log_info:
        return os << "\x1b[32m[Info] ";
    case log_alert:
        return os << "\x1b[31m[Alert] ";
    case log_endl:
        return os << "\x1b[0m" << std::endl;
    }
    return os;
}

std::ostream& operator<< ( std::ostream& os, glm::vec2 v2 )
{
    return os << "( " << v2.x << ", " << v2.y << " )";
}

std::ostream& operator<< ( std::ostream& os, glm::vec3 v3 )
{
    return os << "( " << v3.x << ", " << v3.y << ", " << v3.z << " )";
}

std::ostream& operator<< ( std::ostream& os, glm::vec4 v4 )
{
    return os << "( " << v4.x << ", " << v4.y << ", " << v4.z << ", " << v4.w << " )";
}

glm::vec3 bt2glm ( const btVector3& vec )
{
    return glm::vec3 ( vec.getX(), vec.getY(), vec.getZ() );
}

btVector3 glm2bt ( const glm::vec3& vec )
{
    return btVector3 ( vec.x, vec.y, vec.z );
}

/** \brief construct model-space to world-space transform matrix.
 */
glm::mat4 makeModel ( const glm::vec3& origin, const glm::vec3& rotation, const glm::vec3& scale )
{
    glm::mat4 s = glm::scale ( glm::mat4 ( 1 ), scale );
    glm::mat4 r = glm::rotate ( glm::mat4 ( 1 ), glm::length ( rotation ), glm::normalize ( rotation ) );
    glm::mat4 t = glm::translate ( glm::mat4 ( 1 ), origin );
    if ( glm::dot ( rotation, rotation ) <= 0.01 )
    {
        return t * s;
    }
    else
    {
        return t * r * s;
    }
}
