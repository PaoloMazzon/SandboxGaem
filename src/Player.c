#include "Player.h"

// Returns -1 for negative numbers, 1 for positive, and 0 for zero
static inline double sign(double x) {
	if (x == 0) return 0;
	return (x < 0 ? -1 : 1);
}

////////////////////////////////////////////////////////////
void onPlayerCreate(JamWorld* world, JamEntity* self) {

}
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
void onPlayerDestroy(JamWorld* world, JamEntity* self) {

}
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
void onPlayerFrame(JamWorld* world, JamEntity* self) {
	register double speedSign;
	
	// Movement
	self->hSpeed = (-jamInputCheckKey(JAM_KB_LEFT) + jamInputCheckKey(JAM_KB_RIGHT)) * 3;
	self->vSpeed = (-jamInputCheckKey(JAM_KB_UP) + jamInputCheckKey(JAM_KB_DOWN)) * 3;
	
	//self->vSpeed += 0.5; // Gravity
	//if (jamInputCheckKey(JAM_KB_UP) && jamCheckMapCollFast(world->worldMaps[0], self->x, self->y + 1, 16, 32))
	//self->vSpeed = -7;

	// Collisions
	if (jamCheckMapCollision(world->worldMaps[0], self->x + self->hSpeed, self->y, 16, 24)) {
		speedSign = sign(self->hSpeed);
		while (jamCheckMapCollision(world->worldMaps[0], self->x + speedSign, self->y, 16, 24))
			self->x += speedSign;
		self->hSpeed = 0;
	}
	if (jamCheckMapCollision(world->worldMaps[0], self->x, self->y + self->vSpeed, 16, 24)) {
		speedSign = sign(self->vSpeed);
		while (jamCheckMapCollision(world->worldMaps[0], self->x, self->y + speedSign, 16, 24))
			self->y += speedSign;
		self->vSpeed = 0;
	}
	self->x += self->hSpeed;
	self->y += self->vSpeed;
}
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
void onPlayerDraw(JamWorld* world, JamEntity* self) {
	jamDrawEntity(self);
}
////////////////////////////////////////////////////////////
