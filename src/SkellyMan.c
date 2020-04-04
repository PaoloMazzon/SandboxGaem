#include <EntityBehaviour.h>
#include <SandConstants.h>
#include <math.h>
#include <stdlib.h>
#include <Enemies.h>
#include <EntityCommon.h>

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
	bool horizontal;

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

	sbProcessPhysics(
			world,
			self,
			false,
			((EnemyData*)self->data)->fadeOut == 0 ? ((EnemyData*)self->data)->movement : 0,
			!((EnemyData*)self->data)->fadeOut,
			SKELLYMAN_ACCELERATION,
			0,
			((EnemyData*)self->data)->fadeOut == 0 ? SKELLYMAN_MAX_SPEED : FAST_SPEED,
			NULL
	);

	// Animations
	sbProcessAnimations(world, self, sSkellyWalk, sSkellyStand, sSkellyStand, ((EnemyData*)self->data)->fadeOut == 0);

	// Either death or collisions
	if (((EnemyData *) self->data)->fadeOut != 0) {
		self->rot += ENEMY_DEATH_ROT_SPEED;
		self->vSpeed += COMICAL_GRAVITY_ACCELERATION - GRAVITY_ACCELERATION;
		self->alpha = (uint8) (((EnemyData *) self->data)->fadeOut / ENEMY_FADE_OUT_TIME * 255);
		((EnemyData *) self->data)->fadeOut -= jamRendererGetDelta();
		if (((EnemyData *) self->data)->fadeOut <= 0) {
			jamWorldRemoveEntity(world, self->id);
		}
	} else {
		sbProcessCollisions(world, self, &horizontal, NULL);
		if (horizontal && ((EnemyData *) self->data)->movement != 0)
			((EnemyData *) self->data)->movement = ((EnemyData *) self->data)->movement == 1 ? -1 : 1;
	}

	sbProcessMovement(world, self);
}

void onSkellyManDraw(JamWorld* world, JamEntity* self) {
	jamDrawEntity(self);
}
