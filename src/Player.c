#include "Player.h"
#include <malloc.h>

// Returns -1 for negative numbers, 1 for positive, and 0 for zero
static inline double sign(double x) {
	if (x == 0) return 0;
	return (x < 0 ? -1 : 1);
}

////////////////////////////////////////////////////////////
void onPlayerCreate(JamWorld* world, JamEntity* self) {
	self->data = (PlayerData*)malloc(sizeof(PlayerData));

	if (self->data == NULL)
		jSetError(ERROR_ALLOC_FAILED, "Failed to allocate player data");
}
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
void onPlayerDestroy(JamWorld* world, JamEntity* self) {
	free(self->data);
}
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
void onPlayerFrame(JamWorld* world, JamEntity* self) {
	register double speedSign;
	
	// Movement
	self->hSpeed = (-jamInputCheckKey(JAM_KB_LEFT) + jamInputCheckKey(JAM_KB_RIGHT)) * 3;
	self->vSpeed += 0.5; // Gravity
	if (jamInputCheckKey(JAM_KB_UP) && jamCheckEntityTileMapCollision(self, world->worldMaps[0], self->x, self->y + 1))
		self->vSpeed = -7;

	// Collisions
	if (jamCheckEntityTileMapCollision(self, world->worldMaps[0], self->x + self->hSpeed, self->y)) {
		speedSign = sign(self->hSpeed);
		while (!jamCheckEntityTileMapCollision(self, world->worldMaps[0], self->x + speedSign, self->y))
			self->x += speedSign;
		self->hSpeed = 0;
	}
	if (jamCheckEntityTileMapCollision(self, world->worldMaps[0], self->x, self->y + self->vSpeed)) {
		speedSign = sign(self->vSpeed);
		while (!jamCheckEntityTileMapCollision(self, world->worldMaps[0], self->x, self->y + speedSign))
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
