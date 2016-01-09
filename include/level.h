#pragma once

#include <renderable.h>

#include <vector>

class Level : public Renderable
{
public:
    Level();
    virtual ~Level();
 
    virtual void reset();
    virtual bool hasWon();
};