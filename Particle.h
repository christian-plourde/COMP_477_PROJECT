#ifndef COMP_477_PARTICLE_H
#define COMP_477_PARTICLE_H

#include "Vec3.h"

/* Some physics constants */
#define DAMPING 0.01 // how much to damp the cloth simulation each frame
#define TIME_STEPSIZE2 0.5*0.5 // how large time step each particle takes each frame
#define CONSTRAINT_ITERATIONS 15 // how many iterations of constraint satisfaction each frame (more is rigid, less is soft)



/* The particle class represents a particle of mass that can move around in 3D space*/
class Particle
{
private:
    bool movable; // can the particle move or not ? used to pin parts of the cloth
    float mass; // the mass of the particle (is always 1 in this example)
    Vec3 pos; // the current position of the particle in 3D space
    Vec3 old_pos; // the position of the particle in the previous time step, used as part of the verlet numerical integration scheme
    Vec3 acceleration; // a vector representing the current acceleration of the particle
    Vec3 accumulated_normal; // an accumulated normal (i.e. non normalized), used for OpenGL soft shading



public:
    Particle(Vec3 pos);
    Particle();
    void addForce(Vec3 f);
    bool is_set;

    /* This is one of the important methods, where the time is progressed a single step size (TIME_STEPSIZE)
       The method is called by Cloth.time_step()
       Given the equation "force = mass * acceleration" the next position is found through verlet integration*/
    void timeStep();

    Vec3& getPos();

    void offsetPos(const Vec3 v);

    void makeUnmovable();

    void addToNormal(Vec3 normal);

    Vec3& getNormal();

    void resetNormal();

    void setPos(Vec3 pos);

    bool isBehind(Vec3 move_speed, Vec3 check_pos);

    inline bool is_movable(){return movable;}

};

#endif
