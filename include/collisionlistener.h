#pragma once

/** User Pointer for bullet3 collisionobjects
 */
class CollisionListener
{
public:
    virtual void playerOnTop() {};
};

class GoalListener : public CollisionListener
{
public:
    bool win = false;
    virtual void playerOnTop() {
        win = true;
    };
};
