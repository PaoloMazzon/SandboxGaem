#include <JamEngine.h>
#include <Character.h>
#include <malloc.h>
#include <SandConstants.h>
#include <Message.h>

// Getters and setters at the bottom

////////////////////////////////////////////////////////////////////
bool sbProcessCharacterDeath(JamWorld *world, JamEntity *enemy) {
	if (sbCharData(enemy, State, hp) <= 0 && sbCharData(enemy, State, fadeOut) == 0) {
		sbCharData(enemy, State, fadeOut) = FADE_OUT_TIME;
		enemy->vSpeed -= 1.5;
	}

	if (sbCharData(enemy, State, fadeOut) != 0) {
		enemy->rot += ENEMY_DEATH_ROT_SPEED;
		enemy->vSpeed += COMICAL_GRAVITY_ACCELERATION - GRAVITY_ACCELERATION;
		enemy->alpha = (uint8) (sbCharData(enemy, State, fadeOut) / FADE_OUT_TIME * 255);
		sbCharTick(enemy, fadeOut);
		if (sbCharData(enemy, State, fadeOut) <= 0) {
			enemy->destroy = true;
		}
		return false;
	} else {
		return true;
	}
}
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
void sbProcCharacterPhysics(JamWorld *world, JamEntity *self, bool jump, double movement, bool friction,
							double acceleration, double jumpSpeed, double maxVelocity, bool *jumped, bool roll) {
	// Don't accept controls while dialogue is active
	jump = sbMessageActive() ? false : jump;
	movement = sbMessageActive() ? 0 : movement;

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

	// Rolling
	if (onGround && !sbCharData(self, State, rolling) && sbCharData(self, State, rollCooldown) <= 0 &&
		sbCharData(self, State, rollDuration) <= 0 && (!sbMessageActive() ? roll : false)) {
		sbCharData(self, State, rolling) = true;
		self->hSpeed = sbCharData(self, Stats, rollSpeed) * self->scaleX;
		sbCharData(self, State, rollCooldown) = sbCharData(self, Stats, rollCooldown);
		sbCharData(self, State, rollDuration) = sbCharData(self, Stats, rollDuration);
	}
	if (!sbCharData(self, State, rolling)) sbCharTick(self, rollCooldown);

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
			self->rot = stopRot && !sbCharData(self, State, rolling) ? 0 : self->rot;
		} else {
			jamEntitySetSprite(self, standing);
			self->rot = stopRot && !sbCharData(self, State, rolling) ? 0 : self->rot;
		}
	} else {
		jamEntitySetSprite(self, jumping);
	}
	if (self->hSpeed > 0)
		self->scaleX = 1;
	else if (self->hSpeed < 0)
		self->scaleX = -1;

	// Rolling effects
	if (sbCharData(self, State, rolling)) {
		self->rot += 360 / sbCharData(self, Stats, rollDuration) * self->scaleX;
		sbCharTick(self, rollDuration);
		self->hSpeed = ROLL_SPEED * self->scaleX;
		if (sbCharData(self, State, rollDuration) <= 0)
			sbCharData(self, State, rolling) = false;
	}
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
