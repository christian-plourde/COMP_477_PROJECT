#pragma once
#include "creature.h"
#include "gameboard.h"
#include "TrailParticle.h"

class Pac : public Creature {

public:
	double z;
	Pac(int x, int y);
	~Pac();

	// abilities:
	bool Consume();

	// visual:
	void Draw();

	// movement:
	void Turn(int v);
	void Pad();
	bool WallCheck();
	void PadAndMove(int a);

	bool ConsumeMap(int x, int y);

	// info:
	void onTileChange() override;
	void onTileCenter() override;

	// next tile prediction:
	int getNextTileX(int dist);
	int getNextTileY(int dist);

	int getTileX() { return (int)this->x; }
	int getTileY() { return GameBoard::DIM_Y - (int)(y + 1); }

	bool isDead;
	bool invulnerable;

	bool isPoweredUp;

	float pacmanHitTimer;
	float pacmanHitCooldown;

};
