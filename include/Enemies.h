#include <JamEngine.h>
#pragma once

typedef struct {
	double moveTime;
	double pauseTime;
	double movement;
	double fadeOut;
	double power;
} EnemyData;

// Common
EnemyData* sbCreateEnemyData(double power);

// Skellymen
void onSkellyManCreate(JamWorld* world, JamEntity* self);
void onSkellyManDestroy(JamWorld* world, JamEntity* self);
void onSkellyManFrame(JamWorld* world, JamEntity* self);
void onSkellyManDraw(JamWorld* world, JamEntity* self);
