#include <EntityBehaviour.h>

void onBEnemyCreate(JamWorld* world, JamEntity* self) {
	self->hSpeed = 3;
}

void onBEnemyDestroy(JamWorld* world, JamEntity* self) {
	
}

void onBEnemyFrame(JamWorld* world, JamEntity* self) {
	double lookAhead = self->sprite->frames[0]->w * sign(self->hSpeed);
    if (!jamCheckEntityTileMapCollision(self, world->worldMaps[0], self->x + lookAhead, self->y + 1)
		|| jamCheckEntityTileMapCollision(self, world->worldMaps[0], self->x + self->hSpeed, self->y)) {
		self->hSpeed = -self->hSpeed;
	}

	self->x += self->hSpeed;
}

void onBEnemyDraw(JamWorld* world, JamEntity* self) {
	jamDrawEntity(self);
}
