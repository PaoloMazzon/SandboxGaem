#include <EntityBehaviour.h>
#include <SandConstants.h>
#include <math.h>
#include <stdlib.h>
#include <Enemies.h>

extern JamAssetHandler* gGameData;

void onSkellyManCreate(JamWorld* world, JamEntity* self) {
	self->data = sbCreateEnemyData(10);
}

void onSkellyManDestroy(JamWorld* world, JamEntity* self) {
	free(self->data);
}

void onSkellyManFrame(JamWorld* world, JamEntity* self) {
	static JamSprite* sSkellyStand;
	static JamSprite* sSkellyWalk;

	// Load animations
	if (sSkellyStand == NULL)
		sSkellyStand = jamAssetHandlerGetSprite(gGameData, "SkellyManStandingSprite");
	if (sSkellyWalk == NULL)
		sSkellyWalk = jamAssetHandlerGetSprite(gGameData, "SkellyManRunningSprite");

	// Determine where this feller going
	if (((EnemyData*)self->data)->pauseTime > 0) {
		((EnemyData*)self->data)->pauseTime -= jamRendererGetDelta();
	} else if (((EnemyData*)self->data)->pauseTime <= 0 && ((EnemyData*)self->data)->moveTime <= 0) {
		((EnemyData*)self->data)->moveTime = SKELLYMAN_WALK_INTERVAL;
		((EnemyData*)self->data)->movement = rand() > RAND_MAX / 2 ? 1 : -1;
	}
	if (((EnemyData*)self->data)->moveTime > 0) {
		((EnemyData*)self->data)->moveTime -= jamRendererGetDelta();
		if (((EnemyData*)self->data)->moveTime <= 0) {
			((EnemyData*)self->data)->pauseTime = SKELLYMAN_PAUSE_INTERVAL;
			((EnemyData*)self->data)->movement = 0;
		}
	}

	// Fade out (death) trumps AI
	if (((EnemyData*)self->data)->fadeOut != 0)
		((EnemyData*)self->data)->movement = 0;

	// Gravity
	self->vSpeed += GRAVITY_ACCELERATION;
	if (jamEntityTileMapCollision(self, world->worldMaps[WORLD_WALL_LAYER], self->x, self->y + 1)) {
		self->hSpeed += ((EnemyData*)self->data)->movement * SKELLYMAN_ACCELERATION;
		if (((EnemyData*)self->data)->movement == 0) {
			double old = sign(self->hSpeed);
			if (((EnemyData*)self->data)->fadeOut == 0)
				self->hSpeed -= old * COEFFICIENT_OF_FRICTION * jamRendererGetDelta();
			if (old != sign(self->hSpeed))
				self->hSpeed = 0;
		}

	} else {
		// Slower movement mid-air
		self->hSpeed += ((EnemyData*)self->data)->movement * GRAVITY_ACCELERATION * jamRendererGetDelta();
	}

	if (((EnemyData*)self->data)->fadeOut == 0) {
		if (fabs(self->hSpeed) > SKELLYMAN_MAX_SPEED)
			self->hSpeed = sign(self->hSpeed) * SKELLYMAN_MAX_SPEED;

		// Change sprites and direction facing before hspeed potentially gets zeroed during collisions
		if (self->hSpeed != 0) {
			self->sprite = sSkellyWalk;
			self->rot = 0;
		} else {
			self->sprite = sSkellyStand;
			self->rot = 0;
		}
		if (self->hSpeed > 0)
			self->scaleX = 1;
		else if (self->hSpeed < 0)
			self->scaleX = -1;

		// Horizontally, we can go climb 1 block at a time without jumping if there is not a block above
		// us and we are currently on the ground (to prevent ledge grabbing)
		if (jamEntityTileMapCollision(self, world->worldMaps[WORLD_WALL_LAYER], self->x + self->hSpeed, self->y) &&
			!jamEntityTileMapCollision(self, world->worldMaps[WORLD_WALL_LAYER], self->x + self->hSpeed,
									   self->y - 10) &&
			jamEntityTileMapCollision(self, world->worldMaps[WORLD_WALL_LAYER], self->x, self->y + 1)) {
			self->y -= BLOCK_SIZE + 1;
		}

		// Collisions
		if (jamEntityTileMapCollision(self, world->worldMaps[WORLD_WALL_LAYER], self->x + self->hSpeed, self->y)) {
			jamEntitySnapX(self, world->worldMaps[WORLD_WALL_LAYER], (int) sign(self->hSpeed));
			self->hSpeed = 0;

			if (((EnemyData *) self->data)->movement != 0)
				((EnemyData *) self->data)->movement = ((EnemyData *) self->data)->movement == 1 ? -1 : 1;
		}
		if (jamEntityTileMapCollision(self, world->worldMaps[WORLD_WALL_LAYER], self->x, self->y + self->vSpeed)) {
			jamEntitySnapY(self, world->worldMaps[WORLD_WALL_LAYER], (int) sign(self->vSpeed));
			self->vSpeed = 0;
		}
	} else {
		self->rot += ENEMY_DEATH_ROT_SPEED;
		self->alpha = (uint8)(((EnemyData*)self->data)->fadeOut / ENEMY_FADE_OUT_TIME * 255);
		((EnemyData *) self->data)->fadeOut -= jamRendererGetDelta();
		if (((EnemyData *) self->data)->fadeOut <= 0) {
			jamWorldRemoveEntity(world, self->id);
		}
	}

	self->x += self->hSpeed;
	self->y += self->vSpeed;
}

void onSkellyManDraw(JamWorld* world, JamEntity* self) {
	jamDrawEntity(self);
}
