#include <JamEngine.h>
#include <Character.h>
#include <malloc.h>
#include <SandConstants.h>

////////////////////////////////////////////////////////////////////
EnemyData* sbCreateEnemyData(double power) {
	EnemyData* yes = calloc(1, sizeof(EnemyData));
	if (yes)
		yes->power = power;
	else
		jSetError(ERROR_ALLOC_FAILED, "Failed to create enemy data");
	return yes;
}
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
bool sbProcessEnemyDeath(JamWorld* world, JamEntity* enemy) {
	if (sbGetEnemyDead(enemy)) {
		enemy->rot += ENEMY_DEATH_ROT_SPEED;
		enemy->vSpeed += COMICAL_GRAVITY_ACCELERATION - GRAVITY_ACCELERATION;
		enemy->alpha = (uint8) (sbGetEnemyFadeOut(enemy) / ENEMY_FADE_OUT_TIME * 255);
		sbSetEnemyFadeOut(enemy, sbGetEnemyFadeOut(enemy) - jamRendererGetDelta());
		if (sbGetEnemyFadeOut(enemy) <= 0) {
			enemy->destroy = true;
		}
		return false;
	} else {
		return true;
	}
}
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
double sbGetEnemyMoveTime(JamEntity* enemy) {
	return ((EnemyData*)enemy->data)->moveTime;
}
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
void sbSetEnemyMoveTime(JamEntity* enemy, double moveTime) {
	((EnemyData*)enemy->data)->moveTime = moveTime;
}
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
void sbTickEnemyMoveTime(JamEntity* enemy) {
	((EnemyData*)enemy->data)->moveTime -= jamRendererGetDelta();
}
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
double sbGetEnemyPauseTime(JamEntity* enemy) {
	return ((EnemyData*)enemy->data)->pauseTime;
}
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
void sbSetEnemyPauseTime(JamEntity* enemy, double pauseTime) {
	((EnemyData*)enemy->data)->pauseTime = pauseTime;
}
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
void sbTickEnemyPauseTime(JamEntity* enemy) {
	((EnemyData*)enemy->data)->pauseTime -= jamRendererGetDelta();
}
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
double sbGetEnemyMovement(JamEntity* enemy) {
	return ((EnemyData*)enemy->data)->movement;
}
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
void sbSetEnemyMovement(JamEntity* enemy, double movement) {
	((EnemyData*)enemy->data)->movement = movement;
}
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
double sbGetEnemyFadeOut(JamEntity* enemy) {
	return ((EnemyData*)enemy->data)->fadeOut;
}
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
bool sbGetEnemyDead(JamEntity* enemy) {
	return ((EnemyData*)enemy->data)->fadeOut > 0;
}
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
void sbSetEnemyFadeOut(JamEntity* enemy, double fadeOut) {
	((EnemyData*)enemy->data)->fadeOut = fadeOut;
}
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
void sbTickEnemyFadeOut(JamEntity* enemy) {
	((EnemyData*)enemy->data)->fadeOut -= jamRendererGetDelta();
}
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
double sbGetEnemyPower(JamEntity* enemy) {
	return ((EnemyData*)enemy->data)->power;
}
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
void sbSetEnemyPower(JamEntity* enemy, double power) {
	((EnemyData*)enemy->data)->power = power;
}
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
void sbProcessPhysics(JamWorld* world, JamEntity* self, bool jump, double movement, bool friction, double acceleration, double jumpSpeed, double maxVelocity, bool *jumped) {
	bool onGround = jamEntityTileMapCollision(self, world->worldMaps[WORLD_WALL_LAYER], self->x, self->y + 1);

	// Gravity
	self->vSpeed += GRAVITY_ACCELERATION * jamRendererGetDelta();
	if (onGround) {
		self->hSpeed += movement * acceleration;
		if (movement == 0) {
			double old = sign(self->hSpeed);
			if (friction)
				self->hSpeed -= old * COEFFICIENT_OF_FRICTION * jamRendererGetDelta();
			if (old != sign(self->hSpeed))
				self->hSpeed = 0;
		}

	} else {
		// Slower movement mid-air
		self->hSpeed += movement * AIR_VELOCITY_CONTROL * jamRendererGetDelta();
	}

	// Jump if possible
	if (onGround && jump) {
		self->vSpeed += jumpSpeed;
		if (jumped) *jumped = true;
	}

	// Clamp velocity between +/- max velocity
	self->hSpeed = clamp(self->hSpeed, -maxVelocity, maxVelocity);
}
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
void sbProcessAnimations(JamWorld* world, JamEntity* self, JamSprite* walking, JamSprite* standing, JamSprite* jumping, bool stopRot) {
	// Animation control
	if (jamEntityTileMapCollision(self, world->worldMaps[WORLD_WALL_LAYER], self->x, self->y + 1)) {
		if (self->hSpeed != 0) {
			jamEntitySetSprite(self, walking);
			self->rot = stopRot ? 0 : self->rot;
		} else {
			jamEntitySetSprite(self, standing);
			self->rot = stopRot ? 0 : self->rot;
		}
	} else {
		jamEntitySetSprite(self, jumping);
	}
	if (self->hSpeed > 0)
		self->scaleX = 1;
	else if (self->hSpeed < 0)
		self->scaleX = -1;
}
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
void sbProcessCollisions(JamWorld* world, JamEntity* self, bool *horizontal, bool *vertical) {
	// Horizontally, we can go climb 1 block at a time without jumping if there is not a block above
	// us and we are currently on the ground (to prevent ledge grabbing)
	if (jamEntityTileMapCollision(self, world->worldMaps[WORLD_WALL_LAYER], self->x + self->hSpeed, self->y) &&
		!jamEntityTileMapCollision(self, world->worldMaps[WORLD_WALL_LAYER], self->x + self->hSpeed, self->y - 10) &&
		jamEntityTileMapCollision(self, world->worldMaps[WORLD_WALL_LAYER], self->x, self->y + 1))
		self->y -= BLOCK_SIZE + 1;

	// Collisions
	if (jamEntityTileMapCollision(self, world->worldMaps[WORLD_WALL_LAYER], self->x + self->hSpeed, self->y)) {
		jamEntitySnapX(self, world->worldMaps[WORLD_WALL_LAYER], (int) sign(self->hSpeed));
		self->hSpeed = 0;
		if (horizontal) *horizontal = true;
	}
	if (jamEntityTileMapCollision(self, world->worldMaps[WORLD_WALL_LAYER], self->x, self->y + self->vSpeed)) {
		jamEntitySnapY(self, world->worldMaps[WORLD_WALL_LAYER], (int) sign(self->vSpeed));
		self->vSpeed = 0;
		if (vertical) *vertical = true;
	}
}
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
void sbProcessMovement(JamWorld* world, JamEntity* self) {
	self->x += self->hSpeed;
	self->y += self->vSpeed;
}
////////////////////////////////////////////////////////////////////////