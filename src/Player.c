#include "Player.h"
#include <malloc.h>

/////////////// Player Globals ///////////////
extern JamAssetHandler* gGameData;
static JamSprite* gPlayerWalkSprite;
static JamSprite* gPlayerStandSprite;
static JamSprite* gPlayerJumpSprite;

// Returns -1 for negative numbers, 1 for positive, and 0 for zero
static inline double sign(double x) {
	if (x == 0) return 0;
	return (x < 0 ? -1 : 1);
}

////////////////////////////////////////////////////////////
void onPlayerCreate(JamWorld* world, JamEntity* self) {
	// The players data struct
	self->data = (PlayerData*)malloc(sizeof(PlayerData));
	if (self->data == NULL)
		jSetError(ERROR_ALLOC_FAILED, "Failed to allocate player data");
    
	// Grab player sprites from the handler
	gPlayerWalkSprite = jamGetSpriteFromHandler(gGameData, "PlayerMovingSprite");
	gPlayerStandSprite = jamGetSpriteFromHandler(gGameData, "PlayerStandingSprite");
	gPlayerJumpSprite = jamGetSpriteFromHandler(gGameData, "PlayerJumpingSprite");
}
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
void onPlayerDestroy(JamWorld* world, JamEntity* self) {
	free(self->data);
}
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
void onPlayerFrame(JamWorld* world, JamEntity* self) {
	register double speedSign;
	
	// Movement
	self->hSpeed = (-jamInputCheckKey(JAM_KB_LEFT) + jamInputCheckKey(JAM_KB_RIGHT)) * 3;
	self->vSpeed += 0.5; // Gravity
	if (jamInputCheckKey(JAM_KB_UP) && jamCheckEntityTileMapCollision(self, world->worldMaps[0], self->x, self->y + 1))
		self->vSpeed = -9;

	// Change sprites and direction facing before hspeed potentially gets zeroed during collisions
	if (!jamCheckEntityTileMapCollision(self, world->worldMaps[0], self->x, self->y + 1))
		self->sprite = gPlayerJumpSprite;
	else if (self->hSpeed != 0)
		self->sprite = gPlayerWalkSprite;
	else
		self->sprite = gPlayerStandSprite;
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
	
	self->x += self->hSpeed;
	self->y += self->vSpeed;
}
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
void onPlayerDraw(JamWorld* world, JamEntity* self) {
	static bool flicker = false;
	static uint64 timer = 0;
	bool collision = jamEntityListCollision(self->x, self->y, self, world->entityTypes[et_NPC]);

	if (++timer % 10 == 0)
		flicker = !flicker;

	if (collision && flicker)
		jamDrawEntity(self);
	else if (!collision)
		jamDrawEntity(self);
}
////////////////////////////////////////////////////////////
