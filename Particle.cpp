#include "Particle.h"

Particle::Particle(Vec3 pos) : pos(pos), old_pos(pos),acceleration(Vec3(0,0,0)), mass(1), movable(true), accumulated_normal(Vec3(0,0,0)), is_set(false) {}

Particle::Particle() : is_set(false) {}

void Particle::addForce(Vec3 f)
{
    acceleration += f/mass;
}

void Particle::timeStep()
{
    if(movable)
    {
        Vec3 temp = pos;
        pos = pos + (pos-old_pos)*(1.0-DAMPING) + acceleration*TIME_STEPSIZE2;
        old_pos = temp;
        acceleration = Vec3(0,0,0); // acceleration is reset since it HAS been translated into a change in position (and implicitely into velocity)
    }
}

Vec3& Particle::getPos()
{
    return pos;
}

void Particle::offsetPos(const Vec3 v)
{
    if(movable)
        pos += v;
}

void Particle::makeUnmovable()
{
    movable = false;
}

void Particle::addToNormal(Vec3 normal)
{
    accumulated_normal += normal.normalized();
}

Vec3& Particle::getNormal()
{
    return accumulated_normal;
}

void Particle::resetNormal()
{
    accumulated_normal = Vec3(0,0,0);
}

void Particle::setPos(Vec3 pos)
{
    this->pos = pos;
}

bool Particle::isBehind(Vec3 move_speed, Vec3 check_pos)
{
    //take difference between the new and old positions of the particle
    Vec3 diff = (pos + move_speed) - pos;

    //now if the dot of this and the vector from the old position to the check pos is negative it means we are behind
    Vec3 check_diff = check_pos - pos;

    if(check_diff.dot(diff) < 0.0f)
        return true;

    return false;
}

