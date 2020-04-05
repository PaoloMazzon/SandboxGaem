#include <SandConstants.h>
#include <math.h>
#include <stdlib.h>
#include <Character.h>

extern JamAssetHandler* gGameData;

void onSkellyManCreate(JamWorld* world, JamEntity* self) {
	onCharacterCreate(world, self);
}

void onSkellyManDestroy(JamWorld* world, JamEntity* self) {
	onCharacterDestroy(world, self);
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
	if (sbGetCharacterPauseTime(self) > 0) {
		sbTickCharacterPauseTime(self);
	} else if (sbGetCharacterPauseTime(self) <= 0 && sbGetCharacterMoveTime(self) <= 0) {
		sbSetCharacterMoveTime(self, SKELLYMAN_WALK_INTERVAL);
		sbSetCharacterMovement(self, rand() > RAND_MAX / 2 ? 1 : -1);
	}
	if (sbGetCharacterMoveTime(self) > 0) {
		sbTickCharacterMoveTime(self);
		if (sbGetCharacterMoveTime(self) <= 0) {
			sbSetCharacterPauseTime(self, SKELLYMAN_PAUSE_INTERVAL);
			sbSetCharacterMovement(self, 0);
		}
	}

	sbProcCharacterPhysics(
			world,
			self,
			false,
			!sbGetCharacterDead(self) ? sbGetCharacterMovement(self) : 0,
			!sbGetCharacterDead(self),
			SKELLYMAN_ACCELERATION,
			0,
			!sbGetCharacterDead(self) ? SKELLYMAN_MAX_SPEED : FAST_SPEED,
			NULL
	);

	sbProcCharacterAnimations(world, self, sSkellyWalk, sSkellyStand, sSkellyStand, !sbGetCharacterDead(self));

	if (sbProcessCharacterDeath(world, self)) {
		sbProcCharacterCollisions(world, self, &horizontal, NULL);
		if (horizontal && sbGetCharacterMovement(self) != 0)
			sbSetCharacterMovement(self, sbGetCharacterMovement(self) == 1 ? -1 : 1);
	}

	sbProcCharacterMovement(world, self);
}

void onSkellyManDraw(JamWorld* world, JamEntity* self) {
	jamDrawEntity(self);
}
