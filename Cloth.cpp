#include "Cloth.h"


Particle * Cloth::getParticle(int x, int y)
{
    return &particles[y*num_particles_width + x];
}

void Cloth::makeConstraint(Particle *p1, Particle *p2)
{
    constraints.push_back(Constraint(p1,p2));
}

Vec3 Cloth::calcTriangleNormal(Particle *p1, Particle *p2, Particle *p3)
{
    Vec3 pos1 = p1->getPos();
    Vec3 pos2 = p2->getPos();
    Vec3 pos3 = p3->getPos();

    Vec3 v1 = pos2-pos1;
    Vec3 v2 = pos3-pos1;

    return v1.cross(v2);
}

void Cloth::addWindForcesForTriangle(Particle *p1, Particle *p2, Particle *p3, const Vec3 direction)
{
    Vec3 normal = calcTriangleNormal(p1,p2,p3);
    Vec3 d = normal.normalized();
    Vec3 force = normal*(d.dot(direction));
    p1->addForce(force);
    p2->addForce(force);
    p3->addForce(force);
}

void Cloth::drawTriangle(Particle *p1, Particle *p2, Particle *p3, const Vec3 color)
{
	glTranslatef(pos.f[0], pos.f[1], pos.f[3]);
    glColor3fv( (GLfloat*) &color );

    Vec3 p1n = p1->getNormal().normalized();
    Vec3 p2n = p2->getNormal().normalized();
    Vec3 p3n = p3->getNormal().normalized();

    glNormal3fv((GLfloat *) &(p1n));
    glVertex3fv((GLfloat *) &(p1->getPos() ));

    glNormal3fv((GLfloat *) &(p2n));
    glVertex3fv((GLfloat *) &(p2->getPos() ));

    glNormal3fv((GLfloat *) &(p3n));
    glVertex3fv((GLfloat *) &(p3->getPos() ));
}

Cloth::Cloth(float width, float height, int num_particles_width, int num_particles_height, Vec3 pos) : num_particles_width(num_particles_width), num_particles_height(num_particles_height), pos(pos)
{
    particles.resize(num_particles_width*num_particles_height); //I am essentially using this vector as an array with room for num_particles_width*num_particles_height particles

    // creating particles in a grid of particles from (0,0,0) to (width,-height,0)
    for(int x=0; x<num_particles_width; x++)
    {
        for(int y=0; y<num_particles_height; y++)
        {
			Vec3 pos = Vec3(width * (x/(float)num_particles_width) + this ->pos.f[0],
							-height * (y / (float)num_particles_height) + this->pos.f[1],
                            this ->pos.f[2]);
            particles[y*num_particles_width+x]= Particle(pos); // insert particle in column x at y'th row

        }
    }

    // Connecting immediate neighbor particles with constraints (distance 1 and sqrt(2) in the grid)
    for(int x=0; x<num_particles_width; x++)
    {
        for(int y=0; y<num_particles_height; y++)
        {
            if (x<num_particles_width-1) makeConstraint(getParticle(x,y),getParticle(x+1,y));
            if (y<num_particles_height-1) makeConstraint(getParticle(x,y),getParticle(x,y+1));
            if (x<num_particles_width-1 && y<num_particles_height-1) makeConstraint(getParticle(x,y),getParticle(x+1,y+1));
            if (x<num_particles_width-1 && y<num_particles_height-1) makeConstraint(getParticle(x+1,y),getParticle(x,y+1));
        }
    }


    // Connecting secondary neighbors with constraints (distance 2 and sqrt(4) in the grid)
    for(int x=0; x<num_particles_width; x++)
    {
        for(int y=0; y<num_particles_height; y++)
        {
            if (x<num_particles_width-2) makeConstraint(getParticle(x,y),getParticle(x+2,y));
            if (y<num_particles_height-2) makeConstraint(getParticle(x,y),getParticle(x,y+2));
            if (x<num_particles_width-2 && y<num_particles_height-2) makeConstraint(getParticle(x,y),getParticle(x+2,y+2));
            if (x<num_particles_width-2 && y<num_particles_height-2) makeConstraint(getParticle(x+2,y),getParticle(x,y+2));			}
    }


    // making the upper left most three and right most three particles unmovable
        getParticle(num_particles_width/2,num_particles_height/2)->makeUnmovable();
}

void Cloth::drawShaded()
{
    // reset normals (which where written to last frame)
    std::vector<Particle>::iterator particle;
    for(particle = particles.begin(); particle != particles.end(); particle++)
    {
        (*particle).resetNormal();
    }

    //create smooth per particle normals by adding up all the (hard) triangle normals that each particle is part of
    for(int x = 0; x<num_particles_width-1; x++)
    {
        for(int y=0; y<num_particles_height-1; y++)
        {
            Vec3 normal = calcTriangleNormal(getParticle(x+1,y),getParticle(x,y),getParticle(x,y+1));
            getParticle(x+1,y)->addToNormal(normal);
            getParticle(x,y)->addToNormal(normal);
            getParticle(x,y+1)->addToNormal(normal);

            normal = calcTriangleNormal(getParticle(x+1,y+1),getParticle(x+1,y),getParticle(x,y+1));
            getParticle(x+1,y+1)->addToNormal(normal);
            getParticle(x+1,y)->addToNormal(normal);
            getParticle(x,y+1)->addToNormal(normal);
        }
    }

    glBegin(GL_TRIANGLES);
    for(int x = 0; x<num_particles_width-1; x++)
    {
        for(int y=0; y<num_particles_height-1; y++)
        {
            drawTriangle(getParticle(x+1,y),getParticle(x,y),getParticle(x,y+1),color);
            drawTriangle(getParticle(x+1,y+1),getParticle(x+1,y),getParticle(x,y+1),color);
        }
    }
    glEnd();
}

void Cloth::timeStep()
{
    std::vector<Constraint>::iterator constraint;
    for(int i=0; i<CONSTRAINT_ITERATIONS; i++) // iterate over all constraints several times
    {
        for(constraint = constraints.begin(); constraint != constraints.end(); constraint++ )
        {
            (*constraint).satisfyConstraint(); // satisfy constraint.
        }
    }

    std::vector<Particle>::iterator particle;
    for(particle = particles.begin(); particle != particles.end(); particle++)
    {
        (*particle).timeStep(); // calculate the position of each particle at the next time step.
    }
}

void Cloth::addForce(const Vec3 direction)
{
    std::vector<Particle>::iterator particle;
    for(particle = particles.begin(); particle != particles.end(); particle++)
    {
        (*particle).addForce(direction); // add the forces to each particle
    }

}

void Cloth::windForce(const Vec3 direction)
{
    for(int x = 0; x<num_particles_width-1; x++)
    {
        for(int y=0; y<num_particles_height-1; y++)
        {
            addWindForcesForTriangle(getParticle(x+1,y),getParticle(x,y),getParticle(x,y+1),direction);
            addWindForcesForTriangle(getParticle(x+1,y+1),getParticle(x+1,y),getParticle(x,y+1),direction);
        }
    }
}

void Cloth::ballCollision(const Vec3 center, const float radius)
{
    std::vector<Particle>::iterator particle;
    for(particle = particles.begin(); particle != particles.end(); particle++)
    {
        Vec3 v = (*particle).getPos()-center;
        float l = v.length();
        if ( v.length() < radius) // if the particle is inside the ball
        {
            (*particle).offsetPos(v.normalized()*(radius-l)); // project the particle to the surface of the ball
            (*particle).is_set = true;
        }
    }
}

void Cloth::move(Vec3 move_speed, Vec3 ball_pos)
{
    for(int i = 0; i < particles.size(); i++)
    {
        if(!particles[i].is_movable())
            particles[i].setPos(particles[i].getPos() + move_speed);

    }
}