#include "Player.h"
#include <malloc.h>
#include <math.h>

// How many frames (in delta time) should the flicker last
#define FLICKER_FRAMES 10

/////////////// Player Globals ///////////////
extern JamAssetHandler* gGameData;
static double gFlicker; // This is to flicker when the player gets hit for x frames
static int gPlayerHP = 100;

////////////////////////////////////////////////////////////
void onPlayerCreate(JamWorld* world, JamEntity* self) {
	
}
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
void onPlayerDestroy(JamWorld* world, JamEntity* self) {
    
}
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
void onPlayerFrame(JamWorld* world, JamEntity* self) {
	/////// INITIALISE STATICS
	static JamSprite* sPlayerWalkSprite = NULL;
	static JamSprite* sPlayerStandSprite = NULL;
	static JamSprite* sPlayerJumpSprite = NULL;
	if (sPlayerWalkSprite == NULL && sPlayerStandSprite == NULL && sPlayerJumpSprite == NULL) {
		sPlayerWalkSprite = jamGetSpriteFromHandler(gGameData, "PlayerMovingSprite");
		sPlayerStandSprite = jamGetSpriteFromHandler(gGameData, "PlayerStandingSprite");
		sPlayerJumpSprite = jamGetSpriteFromHandler(gGameData, "PlayerJumpingSprite");
	}

	// Other variables
	register double speedSign;
	JamEntity* collEnemy = jamEntityListCollision(self->x, self->y, self, world->entityTypes[et_NPC]);
	
	// Throw the player away from the enemy if colliding
	if (collEnemy != NULL && false) {
		self->hSpeed = sign(self->x - collEnemy->x) * 5;
		self->vSpeed = -5;
		gFlicker = FLICKER_FRAMES;
		gPlayerHP -= 10;
	}
	
	// Movement/Acceleration
	if (jamCheckEntityTileMapCollision(self, world->worldMaps[0], self->x, self->y + 1)) {
	    double left = -jamInputCheckKey(JAM_KB_LEFT);
		double right = jamInputCheckKey(JAM_KB_RIGHT);
		if (fabs(self->hSpeed) < 3)
			self->hSpeed += (left + right);
		if (!left && !right && self->hSpeed != 0) {
			double old = sign(self->hSpeed);
			self->hSpeed -= -sign(self->hSpeed);
			if (old != sign(self->hSpeed))
				self->hSpeed = 0;
		}
		if (fabs(self->hSpeed) > 3)
			self->hSpeed = sign(self->hSpeed) * 3;
	}
	

	// Jump
	self->vSpeed += 0.5; // Gravity
	if (jamInputCheckKey(JAM_KB_UP) && jamCheckEntityTileMapCollision(self, world->worldMaps[0], self->x, self->y + 1))
		self->vSpeed = -9;

	// Change sprites and direction facing before hspeed potentially gets zeroed during collisions
	if (!jamCheckEntityTileMapCollision(self, world->worldMaps[0], self->x, self->y + 1))
		self->sprite = sPlayerJumpSprite;
	else if (self->hSpeed != 0)
		self->sprite = sPlayerWalkSprite;
	else
		self->sprite = sPlayerStandSprite;
	if (self->hSpeed > 0)
		self->scaleX = 1;
	else if (self->hSpeed < 0)
		self->scaleX = -1;
	
	// Collisions
	if (jamCheckEntityTileMapCollision(self, world->worldMaps[0], self->x + self->hSpeed, self->y)) {
		speedSign = sign(self->hSpeed);
		while (!jamCheckEntityTileMapCollision(self, world->worldMaps[0], self->x + speedSign, self->y))
			self->x += speedSign;
		self->hSpeed = 0;
	}
	if (jamCheckEntityTileMapCollision(self, world->worldMaps[0], self->x, self->y + self->vSpeed)) {
		speedSign = sign(self->vSpeed);
		while (!jamCheckEntityTileMapCollision(self, world->worldMaps[0], self->x, self->y + speedSign))
			self->y += speedSign;
		self->vSpeed = 0;
	}
	
	self->x += self->hSpeed * jamRendererGetDelta();
	self->y += self->vSpeed * jamRendererGetDelta();
}
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
void onPlayerDraw(JamWorld* world, JamEntity* self) {
	static bool flicker = false;
	static uint64 timer = 0;

	if (gFlicker > 0)
		gFlicker -= jamRendererGetDelta();
	
	if (++timer % 10 == 0)
		flicker = !flicker;

	if (gFlicker > 0 && flicker)
		jamDrawEntity(self);
	else if (!(gFlicker > 0))
		jamDrawEntity(self);
}
////////////////////////////////////////////////////////////
