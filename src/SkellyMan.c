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
	if (sbGetEnemyPauseTime(self) > 0) {
		sbTickEnemyPauseTime(self);
	} else if (sbGetEnemyPauseTime(self) <= 0 && sbGetEnemyMoveTime(self) <= 0) {
		sbSetEnemyMoveTime(self, SKELLYMAN_WALK_INTERVAL);
		sbSetEnemyMovement(self, rand() > RAND_MAX / 2 ? 1 : -1);
	}
	if (sbGetEnemyMoveTime(self) > 0) {
		sbTickEnemyMoveTime(self);
		if (sbGetEnemyMoveTime(self) <= 0) {
			sbSetEnemyPauseTime(self, SKELLYMAN_PAUSE_INTERVAL);
			sbSetEnemyMovement(self, 0);
		}
	}

	sbProcessPhysics(
			world,
			self,
			false,
			!sbGetEnemyDead(self) ? sbGetEnemyMovement(self) : 0,
			!sbGetEnemyDead(self),
			SKELLYMAN_ACCELERATION,
			0,
			!sbGetEnemyDead(self) ? SKELLYMAN_MAX_SPEED : FAST_SPEED,
			NULL
	);

	sbProcessAnimations(world, self, sSkellyWalk, sSkellyStand, sSkellyStand, !sbGetEnemyDead(self));

	if (sbProcessEnemyDeath(world, self)) {
		sbProcessCollisions(world, self, &horizontal, NULL);
		if (horizontal && sbGetEnemyMovement(self) != 0)
			sbSetEnemyMovement(self, sbGetEnemyMovement(self) == 1 ? -1 : 1);
	}

	sbProcessMovement(world, self);
}

void onSkellyManDraw(JamWorld* world, JamEntity* self) {
	jamDrawEntity(self);
}
