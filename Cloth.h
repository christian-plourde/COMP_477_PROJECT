#ifndef COMP_477_CLOTH_H
#define COMP_477_CLOTH_H

#include <vector>
#include "Particle.h"
#include "Constraint.h"
#include <GL/glew.h>
#include <iostream>

class Cloth
{
private:

    int num_particles_width; // number of particles in "width" direction
    int num_particles_height; // number of particles in "height" direction
    // total number of particles is num_particles_width*num_particles_height

    std::vector<Particle> particles; // all particles that are part of this cloth
    std::vector<Constraint> constraints; // all constraints between particles as part of this cloth

    Particle* getParticle(int x, int y);
    void makeConstraint(Particle *p1, Particle *p2);


    /* A private method used by drawShaded() and addWindForcesForTriangle() to retrieve the
    normal vector of the triangle defined by the position of the particles p1, p2, and p3.
    The magnitude of the normal vector is equal to the area of the parallelogram defined by p1, p2 and p3
    */
    Vec3 calcTriangleNormal(Particle *p1,Particle *p2,Particle *p3);


    /* A private method used by windForce() to calcualte the wind force for a single triangle
    defined by p1,p2,p3*/
    void addWindForcesForTriangle(Particle *p1,Particle *p2,Particle *p3, const Vec3 direction);

    /* A private method used by drawShaded(), that draws a single triangle p1,p2,p3 with a color*/
    void drawTriangle(Particle *p1, Particle *p2, Particle *p3, const Vec3 color);

    Vec3 color;
	

public:

    /* This is a important constructor for the entire system of particles and constraints*/
    Cloth(float width, float height, int num_particles_width, int num_particles_height, Vec3 pos);

    /* drawing the cloth as a smooth shaded (and colored according to column) OpenGL triangular mesh
    Called from the display() method
    The cloth is seen as consisting of triangles for four particles in the grid as follows:

    (x,y)   *--* (x+1,y)
            | /|
            |/ |
    (x,y+1) *--* (x+1,y+1)

    */
    void drawShaded();

    /* this is an important methods where the time is progressed one time step for the entire cloth.
    This includes calling satisfyConstraint() for every constraint, and calling timeStep() for all particles
    */
    void timeStep();

    /* used to add gravity (or any other arbitrary vector) to all particles*/
    void addForce(const Vec3 direction);

    /* used to add wind forces to all particles, is added for each triangle since the final force is proportional to the triangle area as seen from the wind direction*/
    void windForce(const Vec3 direction);

    /* used to detect and resolve the collision of the cloth with the ball.
    This is based on a very simples scheme where the position of each particle is simply compared to the sphere and corrected.
    This also means that the sphere can "slip through" if the ball is small enough compared to the distance in the grid bewteen particles
    */
    void ballCollision(const Vec3 center,const float radius );

    void move(Vec3 move_speed, Vec3 ball_pos);

    inline void setColor(Vec3 c){this->color = c;}
	Vec3 pos;
};


#endif
