#include "Player.h"
#include <malloc.h>
#include <math.h>
#include <Tweening.h>
#include <SandConstants.h>

// How many frames (in delta time) should the flicker last
#define FLICKER_FRAMES 90

// Maximum velocity of the player
#define MAXIMUM_PLAYER_VELOCITY 3

/////////////// Player Globals ///////////////
extern JamAssetHandler* gGameData;
static double gFlicker; // This is to flicker when the player gets hit for x frames
static int gPlayerHP = 100;
static int gMaxPlayerHP = 100;

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
	if (collEnemy != NULL && gFlicker <= 0) {
		self->hSpeed = sign(self->x - collEnemy->x) * 5;
		self->vSpeed = -5;
		gFlicker = FLICKER_FRAMES;
		gPlayerHP -= 10;
	}

	/* Movement is frictional and somewhat realistic
	 *  1. Friction is only applied while on the ground, movement mid-air is limited
	 *  2. Pressing left/right controls acceleration, not velocity
	 *  3. Friction is only applied if you're on the ground and not pressing left/right
	 *  4. Movement speed is capped at MAXIMUM_PLAYER_VELOCITY
	 *  5. Gravity is applied at all times
	 *  6. Jumping simply shoots the player's velocity upwards
	 *  7. Movement is handled first, then animations, then collisions
	 */
	if (jamCheckEntityTileMapCollision(self, world->worldMaps[0], self->x, self->y + 1)) {
	    double left = -jamInputCheckKey(JAM_KB_LEFT);
		double right = jamInputCheckKey(JAM_KB_RIGHT);
		self->hSpeed += (left + right) * 0.25;
		if (left + right == 0) {
			double old = sign(self->hSpeed);
			self->hSpeed -= old * 0.25;
			if (old != sign(self->hSpeed))
				self->hSpeed = 0;
		}

	} else {
		// Slower movement mid-air
		double left = -jamInputCheckKey(JAM_KB_LEFT);
		double right = jamInputCheckKey(JAM_KB_RIGHT);
		self->hSpeed += (left + right) * 0.15;
	}

	// Restrict velocity
	if (fabs(self->hSpeed) > MAXIMUM_PLAYER_VELOCITY)
		self->hSpeed = sign(self->hSpeed) * MAXIMUM_PLAYER_VELOCITY;

	// Jump
	self->vSpeed += 0.5; // Gravity
	if (jamInputCheckKey(JAM_KB_UP) && jamCheckEntityTileMapCollision(self, world->worldMaps[0], self->x, self->y + 1)) {
		self->vSpeed = -9;
	}

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
	
	// Update player position
	self->x += self->hSpeed * jamRendererGetDelta();
	self->y += self->vSpeed * jamRendererGetDelta();

	// Tween the camera position towards the player
	jamRendererMoveCamera(
			((self->x - GAME_WIDTH / 2) - jamRendererGetCameraX()) * 0.25,
			((self->y - GAME_HEIGHT / 2) - jamRendererGetCameraY()) * 0.25
	);
}
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
void onPlayerDraw(JamWorld* world, JamEntity* self) {
	// Flicker the sprite when damaged/draw player
	static bool flicker = false;
	static uint64 timer = 0;

	if (gFlicker > 0)
		gFlicker -= jamRendererGetDelta();
	
	if (++timer % 8 == 0)
		flicker = !flicker;

	if (gFlicker > 0 && flicker)
		jamDrawEntity(self);
	else if (gFlicker <= 0)
		jamDrawEntity(self);

	// Draw the player health
	jamDrawSetColour(0, 0, 0, 255);
	jamDrawRectangleFilled((int)jamRendererGetCameraX() + 16, (int)jamRendererGetCameraY() + 16, 100, 16);
	jamDrawSetColour(255, 0, 0, 255);
	jamDrawRectangleFilled((int)jamRendererGetCameraX() + 17, (int)jamRendererGetCameraY() + 17, 98 * gPlayerHP / gMaxPlayerHP, 14);
}
////////////////////////////////////////////////////////////
