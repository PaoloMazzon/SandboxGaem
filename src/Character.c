#include <JamEngine.h>
#include <Character.h>
#include <malloc.h>
#include <SandConstants.h>

////////////////////////////////////////////////////////////////////
bool sbProcessCharacterDeath(JamWorld *world, JamEntity *enemy) {
	if (sbGetCharacterDead(enemy)) {
		enemy->rot += ENEMY_DEATH_ROT_SPEED;
		enemy->vSpeed += COMICAL_GRAVITY_ACCELERATION - GRAVITY_ACCELERATION;
		enemy->alpha = (uint8) (sbGetCharacterFadeOut(enemy) / ENEMY_FADE_OUT_TIME * 255);
		sbTickCharacterFadeOut(enemy);
		if (sbGetCharacterFadeOut(enemy) <= 0) {
			enemy->destroy = true;
		}
		return false;
	} else {
		return true;
	}
}
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
double sbGetCharacterMoveTime(JamEntity *enemy) {
	return ((CharacterData*)enemy->data)->moveTime;
}
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
void sbSetCharacterMoveTime(JamEntity *enemy, double moveTime) {
	((CharacterData*)enemy->data)->moveTime = moveTime;
}
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
void sbTickCharacterMoveTime(JamEntity *enemy) {
	((CharacterData*)enemy->data)->moveTime -= jamRendererGetDelta();
}
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
double sbGetCharacterPauseTime(JamEntity *enemy) {
	return ((CharacterData*)enemy->data)->pauseTime;
}
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
void sbSetCharacterPauseTime(JamEntity *enemy, double pauseTime) {
	((CharacterData*)enemy->data)->pauseTime = pauseTime;
}
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
void sbTickCharacterPauseTime(JamEntity *enemy) {
	((CharacterData*)enemy->data)->pauseTime -= jamRendererGetDelta();
}
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
double sbGetCharacterMovement(JamEntity *enemy) {
	return ((CharacterData*)enemy->data)->movement;
}
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
void sbSetCharacterMovement(JamEntity *enemy, double movement) {
	((CharacterData*)enemy->data)->movement = movement;
}
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
double sbGetCharacterFadeOut(JamEntity *enemy) {
	return ((CharacterData*)enemy->data)->fadeOut;
}
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
bool sbGetCharacterDead(JamEntity *enemy) {
	return ((CharacterData*)enemy->data)->fadeOut > 0;
}
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
void sbSetCharacterFadeOut(JamEntity *enemy, double fadeOut) {
	((CharacterData*)enemy->data)->fadeOut = fadeOut;
}
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
void sbTickCharacterFadeOut(JamEntity *enemy) {
	((CharacterData*)enemy->data)->fadeOut -= jamRendererGetDelta();
}
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
double sbGetCharacterPower(JamEntity *enemy) {
	return ((CharacterData*)enemy->data)->power;
}
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
void sbSetCharacterPower(JamEntity *enemy, double power) {
	((CharacterData*)enemy->data)->power = power;
}
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
void sbProcCharacterPhysics(JamWorld *world, JamEntity *self, bool jump, double movement, bool friction,
							double acceleration, double jumpSpeed, double maxVelocity, bool *jumped) {
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
void sbProcCharacterAnimations(JamWorld *world, JamEntity *self, JamSprite *walking, JamSprite *standing,
							   JamSprite *jumping, bool stopRot) {
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
void sbProcCharacterCollisions(JamWorld *world, JamEntity *self, bool *horizontal, bool *vertical) {
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
void sbProcCharacterMovement(JamWorld *world, JamEntity *self) {
	self->x += self->hSpeed;
	self->y += self->vSpeed;
}
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
void onCharacterCreate(JamWorld* world, JamEntity* self) {
	self->data = calloc(1, sizeof(CharacterData));
	if (!self->data)
		jSetError(ERROR_ALLOC_FAILED, "Failed to create character data for type %i", self->type);
}
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
void onCharacterDestroy(JamWorld* world, JamEntity* self) {
	free(self->data);
}
////////////////////////////////////////////////////////////////////////