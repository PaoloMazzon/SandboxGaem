#include "Player.h"
#include <malloc.h>
#include <math.h>
#include <Tweening.h>
#include <SandConstants.h>

// How many frames (in delta time) should the flicker last
#define FLICKER_FRAMES 90

// Maximum velocity of the player
#define MAXIMUM_PLAYER_WALK_VELOCITY 3
#define MAXIMUM_PLAYER_RUN_VELOCITY 5

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
	static bool sInvincible = false;
	if (sPlayerWalkSprite == NULL && sPlayerStandSprite == NULL && sPlayerJumpSprite == NULL) {
		sPlayerWalkSprite = jamAssetHandlerGetSprite(gGameData, "PlayerMovingSprite");
		sPlayerStandSprite = jamAssetHandlerGetSprite(gGameData, "PlayerStandingSprite");
		sPlayerJumpSprite = jamAssetHandlerGetSprite(gGameData, "PlayerJumpingSprite");
	}

	// Other variables
	register double speedSign;
	JamEntity* collEnemy = jamWorldEntityCollision(world, self, self->x, self->y);
	collEnemy = (collEnemy != NULL && collEnemy->type == TYPE_ENEMY) ? collEnemy : NULL;
	
	// Throw the player away from the enemy if colliding
	if (collEnemy != NULL && gFlicker <= 0 && !sInvincible) {
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
	if (jamEntityTileMapCollision(self, world->worldMaps[0], self->x, self->y + 1)) {
	    double left = -jamInputCheckKey(JAM_KB_LEFT);
		double right = jamInputCheckKey(JAM_KB_RIGHT);
		self->hSpeed += (left + right) * 0.25;
		if (left + right == 0) {
			double old = sign(self->hSpeed);
			self->hSpeed -= old * 0.25 * jamRendererGetDelta();
			if (old != sign(self->hSpeed))
				self->hSpeed = 0;
		}

	} else {
		// Slower movement mid-air
		double left = -jamInputCheckKey(JAM_KB_LEFT);
		double right = jamInputCheckKey(JAM_KB_RIGHT);
		self->hSpeed += (left + right) * 0.15 * jamRendererGetDelta();
	}

	// Restrict velocity
	double maxSpeed = jamInputCheckKey(JAM_KB_Z) ? MAXIMUM_PLAYER_RUN_VELOCITY : MAXIMUM_PLAYER_WALK_VELOCITY;
 	if (fabs(self->hSpeed) > maxSpeed)
		self->hSpeed = sign(self->hSpeed) * maxSpeed;

	// Jump
	self->vSpeed += 0.5 * jamRendererGetDelta(); // Gravity
	if (jamInputCheckKey(JAM_KB_UP) && jamEntityTileMapCollision(self, world->worldMaps[0], self->x, self->y + 1)) {
		self->vSpeed = -9;
	}

	// Change sprites and direction facing before hspeed potentially gets zeroed during collisions
	if (!jamEntityTileMapCollision(self, world->worldMaps[0], self->x, self->y + 1))
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
	if (jamEntityTileMapCollision(self, world->worldMaps[0], self->x + self->hSpeed, self->y)) {
		jamEntitySnapX(self, world->worldMaps[0], (int)sign(self->hSpeed));
		self->hSpeed = 0;
	}
	if (jamEntityTileMapCollision(self, world->worldMaps[0], self->x, self->y + self->vSpeed)) {
		jamEntitySnapY(self, world->worldMaps[0], (int)sign(self->vSpeed));
		self->vSpeed = 0;
	}

	// Update player position
	self->x += self->hSpeed;
	self->y += self->vSpeed;

	// Tween the camera position towards the player
	jamRendererMoveCamera(
			((self->x - GAME_WIDTH / 2) - jamRendererGetCameraX()) * 0.25,
			(((self->y - GAME_HEIGHT / 2) - 50) - jamRendererGetCameraY()) * 0.25
	);

	//////////////////////// Debug ////////////////////////
	if (jamInputCheckKeyPressed(JAM_KB_F7))
		sInvincible = !sInvincible;
}
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
void onPlayerDraw(JamWorld* world, JamEntity* self) {
	// Flicker the sprite when damaged/draw player
	static bool flicker = false;
	static uint64 timer = 0;
	static JamTexture* sHealthBarTex = NULL;

	// Initialize statics
	if (sHealthBarTex == NULL)
		sHealthBarTex = jamAssetHandlerGetTexture(gGameData, "PlayerHealthBarTexture");

	/////////////////// DEBUG ///////////////////
	if (jamEntityTileMapCollision(self, world->worldMaps[0], self->x, self->y)) {
		jamDrawRectangleFilled(
			(int)round(self->x + self->hitboxOffsetX),
			(int)round(self->y + self->hitboxOffsetY),
			(int)round(self->hitbox->width),
			(int)round(self->hitbox->height)
		);
	}

	if (gFlicker > 0)
		gFlicker -= jamRendererGetDelta();
	
	if (++timer % 8 == 0)
		flicker = !flicker;

	if (gFlicker > 0 && flicker)
		jamDrawEntity(self);
	else if (gFlicker <= 0)
		jamDrawEntity(self);

	// Draw the player health
	jamDrawRectangleFilled((int)round(jamRendererGetCameraX()) + 16, (int)round(jamRendererGetCameraY()) + 16, 100, 16);
	jamDrawTexturePart(sHealthBarTex, (int)round(jamRendererGetCameraX()) + 16, (int)round(jamRendererGetCameraY()) + 16, 0, 0, gPlayerHP, 16);
}
////////////////////////////////////////////////////////////
