#include <SandConstants.h>
#include <math.h>
#include <stdlib.h>
#include <Character.h>

extern JamAssetHandler* gGameData;

void onSkellyManCreate(JamWorld* world, JamEntity* self) {
	onCharacterCreate(world, self);
	sbCharData(self, Info, name) = "Skellyman";
	sbCharData(self, Info, passiveDialogue) = "By day, by night, we instill fright\n"
										      "Little known, we run not from light\n"
											  "But for commoners at hand\n"
											  "We are a kind little band\n"
											  "That are never too far out of sight";
	sbCharData(self, State, hp) = 100;
	sbCharData(self, Stats, maxHP) = 100;
	sbCharData(self, Stats, thorns) = 15;
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
	if (sbCharData(self, State, pauseTime) > 0) {
		sbCharTick(self, pauseTime);
	} else if (sbCharData(self, State, pauseTime) <= 0 && sbCharData(self, State, moveTime) <= 0) {
		sbCharData(self, State, moveTime) = SKELLYMAN_WALK_INTERVAL;
		sbCharData(self, State, movement) = rand() > RAND_MAX / 2 ? 1 : -1;
	}
	if (sbCharData(self, State, moveTime) > 0) {
		sbCharTick(self, moveTime);
		if (sbCharData(self, State, moveTime) <= 0) {
			sbCharData(self, State, pauseTime) = SKELLYMAN_PAUSE_INTERVAL;
			sbCharData(self, State, movement) = 0;
		}
	}

	sbProcCharacterPhysics(
			world,
			self,
			false,
			!sbCharData(self, State, fadeOut) ? sbCharData(self, State, movement) : 0,
			!sbCharData(self, State, fadeOut),
			SKELLYMAN_ACCELERATION,
			0,
			!sbCharData(self, State, fadeOut) ? SKELLYMAN_MAX_SPEED : FAST_SPEED,
			NULL,
			false
	);

	sbProcCharacterAnimations(world, self, sSkellyWalk, sSkellyStand, sSkellyStand, !sbCharData(self, State, fadeOut));

	if (sbProcessCharacterDeath(world, self)) {
		sbProcCharacterCollisions(world, self, &horizontal, NULL);
		if (horizontal && sbCharData(self, State, movement) != 0)
			sbCharData(self, State, movement) = sbCharData(self, State, movement) == 1 ? -1 : 1;
	}

	sbProcCharacterMovement(world, self);
}

void onSkellyManDraw(JamWorld* world, JamEntity* self) {
	jamDrawEntity(self);
}
