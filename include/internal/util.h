#ifndef INTERNAL_UTIL_H
#define INTERNAL_UTIL_H

#include <stdarg.h>
#include <iostream>
#include <string>
#include <sstream>

#include <btBulletDynamicsCommon.h>
#include <glm/glm.hpp>

#include <yaml-cpp/yaml.h>

#define VA_ARGS(...) ,##__VA_ARGS__
#define errorExit( format, ... ) __errorExit( __FILE__, __FUNCTION__, __LINE__, format VA_ARGS(__VA_ARGS__))

void __errorExit ( const char * file, const char * func, unsigned int line, const char * format, ... );

#define vec_for_each( index, vec ) for( size_t index = 0 ; index < (vec).size() ; ++index )
#define list_for_each( it, xs ) for( auto it = xs.begin() ; it != xs.end() ; ++it )

#define INVALID_CODE_PATH std::cerr << log_alert << "INVALID_CODE_PATH" << log_endl; abort();

extern std::stringstream log_stream;
#define LOG log_stream

enum Log {
    log_warn,
    log_info,
    log_alert,
    log_endl
};

std::ostream& operator<< ( std::ostream& os, Log l );
std::ostream& operator<< ( std::ostream& os, glm::vec2 v2 );
std::ostream& operator<< ( std::ostream& os, glm::vec3 v3 );
std::ostream& operator<< ( std::ostream& os, glm::vec4 v4 );

inline float randomFloat()
{
    return ( ( float ) rand() ) /RAND_MAX;
}

inline float lerp ( float a, float b, float f )
{
    return a + f * ( b - a );
}

glm::vec3 bt2glm ( const btVector3& vec );
btVector3 glm2bt ( const glm::vec3& vec );

static glm::vec3 __nullvec = glm::vec3 ( 0 );
static glm::vec3 __rotvec = glm::vec3 ( 0 );
static glm::vec3 __scalevec = glm::vec3 ( 1 );
glm::mat4 makeModel ( const glm::vec3& origin = __nullvec,
                      const glm::vec3& rotation = __rotvec,
                      const glm::vec3& scale = __scalevec );

struct Transform {
    const glm::vec3 &getOrigin() const {
        return origin;
    }

    void setOrigin ( const glm::vec3 &origin ) {
        Transform::origin = origin;
        mat = makeModel ( origin, rot, scale );
    }

    const glm::vec3 &getRot() const {
        return rot;
    }

    void setRot ( const glm::vec3 &rot ) {
        Transform::rot = rot;
        mat = makeModel ( origin, rot, scale );
    }

    const glm::vec3 &getScale() const {
        return scale;
    }

    void setScale ( const glm::vec3 &scale ) {
        Transform::scale = scale;
        mat = makeModel ( origin, rot, scale );
    }

    glm::mat4 mat = glm::mat4 ( 1 );

private:
    glm::vec3 origin;
    glm::vec3 rot;
    glm::vec3 scale = glm::vec3 ( 1 );
};

namespace YAML
{

template<>
struct convert<glm::vec3> {
    static Node encode ( const glm::vec3& rhs ) {
        Node node;
        node.push_back ( rhs.x );
        node.push_back ( rhs.y );
        node.push_back ( rhs.z );
        return node;
    }

    static bool decode ( const Node& node, glm::vec3& rhs ) {
        if ( !node.IsSequence() || node.size() != 3 ) {
            return false;
        }

        rhs.x = node[0].as<float>();
        rhs.y = node[1].as<float>();
        rhs.z = node[2].as<float>();
        return true;
    }
};

template<>
struct convert<glm::vec4> {
    static Node encode ( const glm::vec4& rhs ) {
        Node node;
        node.push_back ( rhs.x );
        node.push_back ( rhs.y );
        node.push_back ( rhs.z );
        node.push_back ( rhs.w );
        return node;
    }

    static bool decode ( const Node& node, glm::vec4& rhs ) {
        if ( !node.IsSequence() || node.size() != 4 ) {
            return false;
        }

        rhs.x = node[0].as<float>();
        rhs.y = node[1].as<float>();
        rhs.z = node[2].as<float>();
        rhs.w = node[3].as<float>();
        return true;
    }
};
}

#endif //INTERNAL_UTIL_H
