#include "Ghost.h"
#include "Particle.h"
#include "TrailParticle.h"
#include "Time.h"
#include "SoundManager.h"
#include <iostream>
#include <ctime>
#include <cstdlib>

#define TILE_SIZE_LEFT 0.98
#define TILE_SIZE_RIGHT 0.98
#define TILE_SIZE_UP 0.98
#define TILE_SIZE_DOWN 0.98
#define INTERSECTION_DIR_CHANGE_PROB 40
#define TRACKER_RANGE_STOP 3

Ghost::Ghost(float scale_factor, Vec3 color, int tile_x, int tile_y) : cloth(scale_factor*12, scale_factor*12,55,45, Vec3(tile_x - 15.8, 6.8 - tile_y, -14.5)),
ball_radius(scale_factor*2),
ball_pos(4.5 * scale_factor,-1.87 * scale_factor ,-4 * scale_factor),
move_direction(0,0,0),
pacman(nullptr)
{
	ball_pos += Vec3(tile_x - 15.8, 6.8 - tile_y, -14.5);
	ball_pos.f[0] = ball_pos.f[0] + 1 * scale_factor;
	ball_pos.f[1] = ball_pos.f[1] - 3.8 * scale_factor;
	ball_pos.f[2] = ball_pos.f[2] + 0.6 * scale_factor;
	ghost_pos = ball_pos;
    this->color = color;
    cloth.setColor(color);
	originalClothColor = color;
	acc_move = 0;
	speed = 0;
	ai_type = NONE;
	dir = DOWN;
	this->tile_x = tile_x;
	this->tile_y = tile_y;
	srand(time(0));
}

void Ghost::display()
{
    move();
    cloth.addForce(Vec3(0,0,-2.0*speed)*TIME_STEPSIZE2); // add gravity each frame, pointing down
    cloth.windForce(-move_direction*speed*60);
    cloth.timeStep(); // calculate the particle positions of the next frame
    cloth.ballCollision(ball_pos,ball_radius); // resolve collision with the ball
    cloth.drawShaded(); // finally draw the cloth with smooth shading
    glPushMatrix(); // to draw the ball we use glutSolidSphere, and need to draw the sphere at the position of the ball
    glTranslatef(ball_pos.f[0],ball_pos.f[1],ball_pos.f[2]); // hence the translation of the sphere onto the ball position
  //  glColor3f(color.f[0],color.f[1],color.f[2]);
    //glColor3f(color.f[0],color.f[1],color.f[2]);
    glutSolidSphere(ball_radius,50,50); // draw the ball, but with a slightly lower radius, otherwise we could get ugly visual artifacts of cloth penetrating the ball slightly
    glPopMatrix();
}

bool Ghost::ai_move()
{
	//this will try to move the ghost based on its ai
	if (ai_type == NONE)
		return false;

	if (ai_type == TRACKER)
	{
		//if the ai is tracker type, the ghost tries to go right to pacman
		//first we need to check pacmans position with respect to our ghost
		int pac_x = pacman->tileX;
		int pac_y = GameBoard::DIM_Y - pacman->tileY - 1;

		int dist_x = abs(tile_x - pac_x);
		int dist_y = abs(tile_y - pac_y);
		
		//now we try to move the ghost to pacman
		if (tile_x > pac_x && dist_x >= TRACKER_RANGE_STOP && isIntersection(tile_x, tile_y))
		{
			//if pacman is to our left, we will try to move left
			if (!isWall(tile_x - 1, tile_y))
			{
				setMoveDirection(LEFT);
				return true; //indicate that we already changed direction
			}

			else
				return false;
		}

		if (tile_x < pac_x && dist_x >= TRACKER_RANGE_STOP && isIntersection(tile_x, tile_y))
		{
			//if pacman is to our right, we will try to move right
			{
				if (!isWall(tile_x + 1, tile_y))
				{
					setMoveDirection(RIGHT);
					return true;
				}

				else
					return false;
			}
		}

		if (tile_y > pac_y && dist_y >= TRACKER_RANGE_STOP && isIntersection(tile_x, tile_y))
		{
			//if pacman is above us
			{
				if (!isWall(tile_x, tile_y - 1))
				{
					setMoveDirection(UP);
					return true;
				}

				else
					return false;
			}
		}

		if (tile_y < pac_y && dist_y >= TRACKER_RANGE_STOP && isIntersection(tile_x, tile_y))
		{
			//if pacman is above us
			{
				if (!isWall(tile_x, tile_y + 1))
				{
					setMoveDirection(DOWN);
					return true;
				}

				else
					return false;
			}
		}
		
		return false;

	}
}

void Ghost::move()
{
	
	//we need to move both the ball and the cloth
	ball_pos += move_direction * speed;
	cloth.move(move_direction * speed, ball_pos);

	acc_move += speed;
	bool dir_changed = false;

	//if the move based on ai type is impossible then we keep going with normal movement
	if ((dir == DOWN && acc_move >= TILE_SIZE_DOWN)
		|| (dir == UP && acc_move >= TILE_SIZE_UP)
		|| (dir == LEFT && acc_move >= TILE_SIZE_LEFT)
		|| (dir == RIGHT && acc_move >= TILE_SIZE_RIGHT))
	{
		acc_move = 0;

		if (dir == UP && !dir_changed)
		{
			tile_y--;
			if (isWall(tile_x, tile_y - 1))
			{
				//we need to check if there is a wall to the left.
				//if yes we go right otherwise we go left
				if (isWall(tile_x - 1, tile_y))
				{
					setMoveDirection(RIGHT);
				}

				else
				{
					setMoveDirection(LEFT);
				}

				dir_changed = true;
			}
			
		}

		if (dir == DOWN && !dir_changed)
		{
			tile_y++;
			if (isWall(tile_x, tile_y + 1))
			{
				if (isWall(tile_x - 1, tile_y))
				{
					setMoveDirection(RIGHT);
				}

				else
				{
					setMoveDirection(LEFT);
				}

				dir_changed = true;
			}
			
		}

		if (dir == RIGHT && !dir_changed)
		{
			tile_x++;
			if (isWall(tile_x + 1, tile_y))
			{
				//std::cout << "wall to right" << std::endl;
				//we need to check if there is a wall above.
				//if yes we go down otherwise we go up
				if (isWall(tile_x, tile_y - 1))
				{
					setMoveDirection(DOWN);
				}

				else
				{
					setMoveDirection(UP);
				}

				dir_changed = true;
			}
			
		}

		if (dir == LEFT && !dir_changed)
		{
			tile_x--;
			if (isWall(tile_x - 1, tile_y))
			{
				if (isWall(tile_x, tile_y - 1))
				{
					setMoveDirection(DOWN);
				}

				else
				{
					setMoveDirection(UP);
				}

				dir_changed = true;
			}
			
		}

		//std::cout << "GHOST AT TILE: (" << tile_x << ", " << tile_y << ")" << std::endl;
		if (ai_move())
			return;

		if (isIntersection(tile_x, tile_y) && !dir_changed)
		{
			//when pacman reaches an intersection, he has a chance to change course
			int r = (rand() % 100) + 1;

			if (r < INTERSECTION_DIR_CHANGE_PROB)
			{
				if (dir == UP || dir == DOWN)
				{
					if (!isWall(tile_x - 1, tile_y) && r > 50)
					{
						setMoveDirection(LEFT);
					}

					else if (!isWall(tile_x + 1, tile_y))
					{
						setMoveDirection(RIGHT);
					}
				}

				else if (dir == LEFT || dir == RIGHT)
				{
					if (!isWall(tile_x, tile_y + 1) && r > 50)
					{
						setMoveDirection(DOWN);
					}

					else if (!isWall(tile_x, tile_y - 1))
					{
						setMoveDirection(UP);
					}
				}

			}
		}
	}
}

void Ghost::setMoveDirection(MOVE_DIRECTION dir)
{
	if (dir == UP)
	{
		move_direction = Vec3(0, 1, 0);
		this->dir = UP;
	}
		

	if (dir == DOWN)
	{
		move_direction = Vec3(0, -1, 0);
		this->dir = DOWN;
	}
		

	if (dir == LEFT)
	{
		move_direction = Vec3(-1, 0, 0);
		this->dir = LEFT;
	}
		

	if (dir == RIGHT)
	{
		move_direction = Vec3(1, 0, 0);
		this->dir = RIGHT;
	}
		
}

bool Ghost::isWall(int x, int y)
{
	return (GameBoard::initial_map[y][x] == 1);
}

bool Ghost::isIntersection(int x, int y)
{
	if((dir == RIGHT || dir == LEFT) && (!isWall(x, y+1) || !isWall(x, y-1)))
		return true;

	if ((dir == UP || dir == DOWN) && (!isWall(x - 1, y) || !isWall(x + 1, y)))
		return true;

	return false;
}

Vec3 Ghost::getBoardPosition()
{
	Vec3 vec;

	if (dir == UP)
	{
		vec.f[0] = (float)tile_x;
		vec.f[1] = (float)tile_y - acc_move / TILE_SIZE_UP;
	}

	if (dir == DOWN)
	{
		vec.f[0] = (float)tile_x;
		vec.f[1] = (float)tile_y + acc_move / TILE_SIZE_DOWN;
	}

	if (dir == LEFT)
	{
		vec.f[0] = (float)tile_x - acc_move / TILE_SIZE_LEFT;
		vec.f[1] = (float)tile_y;
	}

	if (dir == RIGHT)
	{
		vec.f[0] = (float)tile_x + acc_move / TILE_SIZE_RIGHT;
		vec.f[1] = (float)tile_y;
	}

	return vec;

}

void Ghost::collision(Pac* pac)
{
	if (pac->getTileX() == getTileX() && pac->getTileY() == getTileY())
	{
		if (pac->isPoweredUp)
		{

		}
		else if (!pac->invulnerable)
		{
			pac->invulnerable = true;
			pac->lives -= 1;
			SoundManager::Instance()->playSound(SoundEffects::Pacman_Hit);
			if (pac->lives == 0)
			{
				pac->isDead = true;
				TrailParticle::setEnergizerParticles(true);
				pac->lives = -1;
			}
		}
	}

}
