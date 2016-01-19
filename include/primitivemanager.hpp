#pragma once

#include <resourcemanager.hpp>
#include <mesh.h>

#include <internal/util.h>

enum PrimitiveType
{
    P_Cube,
    P_Sphere,
    P_Tetrahedron
};

class PrimitiveManagerT : public ResourceManager<PrimitiveType, MeshObject>
{
public:
    virtual MeshObject * loadResource ( const PrimitiveType& p )
    {
        switch ( p )
        {
        case P_Cube:
            return MeshObject::genCube();
        case P_Sphere:
            return new IcoSphere;
        case P_Tetrahedron:
            return MeshObject::genTetrahedron();
        }
        INVALID_CODE_PATH;
        return nullptr;
    }
};
