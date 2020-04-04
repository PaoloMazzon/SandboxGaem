//
// Created by plo on 4/4/20.
//

#include <EntityCommon.h>
#include <math.h>
#include <SandConstants.h>

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
			self->sprite = walking;
			self->rot = stopRot ? 0 : self->rot;
		} else {
			self->sprite = standing;
			self->rot = stopRot ? 0 : self->rot;
		}
	} else {
		self->sprite = jumping;
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