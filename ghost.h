#ifndef COMP_477_A1_PROBLEM_1_GHOST_H
#define COMP_477_A1_PROBLEM_1_GHOST_H

#include "Vec3.h"
#include "Cloth.h"
#include "gameboard.h"
#include "pac.h"
#include <GL/glew.h>
#include "glut.h"
#define TIME_STEPSIZE2 0.5*0.5 // how large time step each particle takes each frame

enum MOVE_DIRECTION {UP, DOWN, LEFT, RIGHT};

enum AI_TYPE {NONE, TRACKER};

class Ghost 
{
private:
    Vec3 ball_pos;
    float ball_radius;
    Cloth cloth;
	float speed;
    Vec3 move_direction;
	Vec3 originalClothColor;
    Vec3 color;
	Vec3 ghost_pos;
	int tile_x;
	int tile_y;
	float acc_move;
	MOVE_DIRECTION dir;
	bool isWall(int x, int y);
	bool isIntersection(int x, int y);
	Pac* pacman;
	bool ai_move();
	AI_TYPE ai_type;


public:
    Ghost(float scale_factor, Vec3 color, int tile_x, int tile_y);
    void display();
    void move();
    inline void setMoveSpeed(float speed){this->speed = speed;}
	void setMoveDirection(MOVE_DIRECTION dir);
    inline void setColor(Vec3 color){this->color = color;}
	inline void setGhostColor(Vec3 color) { cloth.setColor(color); }
	inline void setOriginalGhostColor() { cloth.setColor(originalClothColor); }
	inline Vec3 getPosition() { return ball_pos; }
	inline void setPacman(Pac* p) { this->pacman = p; }
	inline void setAIType(AI_TYPE t) { ai_type = t; }
	inline int getTileX() { return this->tile_x; }
	inline int getTileY() { return this->tile_y; }
	Vec3 getBoardPosition();
	void collision(Pac* pac);
};

#endif
