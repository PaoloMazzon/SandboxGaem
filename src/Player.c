#include "Player.h"
#include <malloc.h>
#include <math.h>
#include <Tweening.h>
#include <SandConstants.h>
#include <JamEngine.h>

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
	static bool sPlayerJumped = false;
	static bool sInvincible = false;
	if (sPlayerWalkSprite == NULL && sPlayerStandSprite == NULL && sPlayerJumpSprite == NULL) {
		sPlayerWalkSprite = jamAssetHandlerGetSprite(gGameData, "PlayerMovingSprite");
		sPlayerStandSprite = jamAssetHandlerGetSprite(gGameData, "PlayerStandingSprite");
		sPlayerJumpSprite = jamAssetHandlerGetSprite(gGameData, "PlayerJumpingSprite");
	}

	// Other variables
	JamEntity* collEnemy = jamWorldEntityCollision(world, self, self->x, self->y);
	collEnemy = (collEnemy != NULL && collEnemy->type == TYPE_ENEMY) ? collEnemy : NULL;
	
	// Throw the player away from the enemy if colliding
	if (collEnemy != NULL && gFlicker <= 0 && !sInvincible) {
		self->hSpeed = sign(self->x - collEnemy->x) * KNOCKBACK_VELOCITY;
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
	if (jamEntityTileMapCollision(self, world->worldMaps[WORLD_WALL_LAYER], self->x, self->y + 1)) {
	    double left = -jamInputCheckKey(JAM_KB_LEFT);
		double right = jamInputCheckKey(JAM_KB_RIGHT);
		self->hSpeed += (left + right) * PLAYER_ACCELERATION;
		if (left + right == 0) {
			double old = sign(self->hSpeed);
			self->hSpeed -= old * COEFFICIENT_OF_FRICTION * jamRendererGetDelta();
			if (old != sign(self->hSpeed))
				self->hSpeed = 0;
		}

	} else {
		// Slower movement mid-air
		double left = -jamInputCheckKey(JAM_KB_LEFT);
		double right = jamInputCheckKey(JAM_KB_RIGHT);
		self->hSpeed += (left + right) * GRAVITY_ACCELERATION * jamRendererGetDelta();
	}

	// Restrict velocity
	double maxSpeed = jamInputCheckKey(JAM_KB_Z) ? MAXIMUM_PLAYER_RUN_VELOCITY : MAXIMUM_PLAYER_WALK_VELOCITY;
 	if (fabs(self->hSpeed) > maxSpeed)
		self->hSpeed = sign(self->hSpeed) * maxSpeed;

	// Jump
	self->vSpeed += 0.5 * jamRendererGetDelta(); // Gravity
	if (jamInputCheckKeyPressed(JAM_KB_UP) && jamEntityTileMapCollision(self, world->worldMaps[WORLD_WALL_LAYER], self->x, self->y + 1)) {
		self->vSpeed = JUMP_VELOCITY;
		sPlayerJumped = true;
	}

	// Change sprites and direction facing before hspeed potentially gets zeroed during collisions
	if (!jamEntityTileMapCollision(self, world->worldMaps[WORLD_WALL_LAYER], self->x, self->y + 1)) {
		self->sprite = sPlayerJumpSprite;
		if (sPlayerJumped)
			self->rot += jamRendererGetDelta() * self->scaleX * JUMP_ROT_SPEED;
	} else if (self->hSpeed != 0) {
		self->sprite = sPlayerWalkSprite;
		self->rot = 0;
	} else {
		self->sprite = sPlayerStandSprite;
		self->rot = 0;
	}
	if (self->hSpeed > 0)
		self->scaleX = 1;
	else if (self->hSpeed < 0)
		self->scaleX = -1;


	// Horizontally, we can go climb 1 block at a time without jumping if there is not a block above
	// us and we are currently on the ground (to prevent ledge grabbing)
	if (jamEntityTileMapCollision(self, world->worldMaps[WORLD_WALL_LAYER], self->x + self->hSpeed, self->y) &&
			!jamEntityTileMapCollision(self, world->worldMaps[WORLD_WALL_LAYER], self->x + self->hSpeed, self->y - 10) &&
			jamEntityTileMapCollision(self, world->worldMaps[WORLD_WALL_LAYER], self->x, self->y + 1)) {
		self->y -= BLOCK_SIZE + 1;
	}

	// Collisions
	if (jamEntityTileMapCollision(self, world->worldMaps[WORLD_WALL_LAYER], self->x + self->hSpeed, self->y)) {
		jamEntitySnapX(self, world->worldMaps[WORLD_WALL_LAYER], (int)sign(self->hSpeed));
		self->hSpeed = 0;
	}
	if (jamEntityTileMapCollision(self, world->worldMaps[WORLD_WALL_LAYER], self->x, self->y + self->vSpeed)) {
		jamEntitySnapY(self, world->worldMaps[WORLD_WALL_LAYER], (int)sign(self->vSpeed));
		self->vSpeed = 0;
		sPlayerJumped = false;
	}

	// Update player position
	self->x += self->hSpeed;
	self->y += self->vSpeed;

	// Tween the camera position towards the player
	double xx = jamRendererGetCameraX() + (((self->x - GAME_WIDTH / 2) - jamRendererGetCameraX()) * 0.25);
	double yy = jamRendererGetCameraY() + ((((self->y - GAME_HEIGHT / 2) - 25) - jamRendererGetCameraY()) * 0.25);

	jamRendererSetCameraPos(
			xx < 0 ? 0 : xx,
			yy
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
	static JamFont* sGameFont = NULL;

	// Initialize statics
	if (sHealthBarTex == NULL)
		sHealthBarTex = jamAssetHandlerGetTexture(gGameData, "PlayerHealthBarTexture");
	if (sGameFont == NULL)
		sGameFont = jamAssetHandlerGetFont(gGameData, "GameFont");

	if (gFlicker > 0)
		gFlicker -= jamRendererGetDelta();
	
	if (++timer % 8 == 0)
		flicker = !flicker;

	if (gFlicker > 0 && flicker)
		jamDrawEntity(self);
	else if (gFlicker <= 0)
		jamDrawEntity(self);

	// Draw the player health
	uint32 size = 75;
	jamDrawRectangleFilled((int)round(jamRendererGetCameraX()) + 8, (int)round(jamRendererGetCameraY()) + 8, size, 12);
	jamDrawTexturePart(sHealthBarTex, (int)round(jamRendererGetCameraX()) + 8, (int)round(jamRendererGetCameraY()) + 8, 0, 0, (int)(((double)gPlayerHP / (double)gMaxPlayerHP) * size), 12);
	jamFontRender(sGameFont, (int)round(jamRendererGetCameraX()) + 9, (int)round(jamRendererGetCameraY()) + 10, "HP: %f", round((double)gPlayerHP));
}
////////////////////////////////////////////////////////////
