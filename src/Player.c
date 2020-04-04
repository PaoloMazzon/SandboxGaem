#include "Player.h"
#include <malloc.h>
#include <math.h>
#include <EntityCommon.h>
#include <SandConstants.h>
#include <JamEngine.h>
#include <TileMap.h>
#include <Enemies.h>

/////////////// Player Globals ///////////////
extern JamAssetHandler* gGameData;
static double gFlicker; // This is to flicker when the player gets hit for x frames
static int gPlayerHP = 100;
static int gMaxPlayerHP = 100;
static JamControlMap* gPlayerControls;

////////////////////////////////////////////////////////////
void onPlayerCreate(JamWorld* world, JamEntity* self) {
	gPlayerControls = jamControlMapCreate();
	jamControlMapAddInput(gPlayerControls, "move", JAM_KB_RIGHT, 0, JAM_KEYBOARD_INPUT, JAM_INPUT_ACTIVE, 1);
	jamControlMapAddInput(gPlayerControls, "move", JAM_KB_LEFT, 0, JAM_KEYBOARD_INPUT, JAM_INPUT_ACTIVE, -1);
	jamControlMapAddInput(gPlayerControls, "move", JAM_AXIS_LEFTX, 0, JAM_GAMEPAD_INPUT, JAM_INPUT_ACTIVE, 1);
	jamControlMapAddInput(gPlayerControls, "move", JAM_BUTTON_DPAD_LEFT, 0, JAM_GAMEPAD_INPUT, JAM_INPUT_ACTIVE, -1);
	jamControlMapAddInput(gPlayerControls, "move", JAM_BUTTON_DPAD_RIGHT, 0, JAM_GAMEPAD_INPUT, JAM_INPUT_ACTIVE, 1);
	jamControlMapAddInput(gPlayerControls, "jump", JAM_KB_UP, 0, JAM_KEYBOARD_INPUT, JAM_INPUT_PRESSED, 1);
	jamControlMapAddInput(gPlayerControls, "jump", JAM_AXIS_LEFTY, 0, JAM_GAMEPAD_INPUT, JAM_INPUT_PRESSED, 1);
	jamControlMapAddInput(gPlayerControls, "jump", JAM_BUTTON_A, 0, JAM_GAMEPAD_INPUT, JAM_INPUT_PRESSED, 1);
	jamControlMapAddInput(gPlayerControls, "run", JAM_KB_Z, 0, JAM_KEYBOARD_INPUT, JAM_INPUT_ACTIVE, 1);
	jamControlMapAddInput(gPlayerControls, "run", JAM_BUTTON_X, 0, JAM_GAMEPAD_INPUT, JAM_INPUT_ACTIVE, 1);
	jamControlMapAddInput(gPlayerControls, "roll", JAM_KB_X, 0, JAM_KEYBOARD_INPUT, JAM_INPUT_PRESSED, 1);
	jamControlMapAddInput(gPlayerControls, "roll", JAM_BUTTON_B, 0, JAM_GAMEPAD_INPUT, JAM_INPUT_PRESSED, 1);
}
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
void onPlayerDestroy(JamWorld* world, JamEntity* self) {
    jamControlMapFree(gPlayerControls);
}
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
void onPlayerFrame(JamWorld* world, JamEntity* self) {
	/////// INITIALISE STATICS
	static JamSprite* sPlayerWalkSprite = NULL;
	static JamSprite* sPlayerStandSprite = NULL;
	static JamSprite* sPlayerJumpSprite = NULL;
	static bool sPlayerJumped = false;
	static bool sPlayerRolling = false;
	static bool sInvincible = false;
	static double sRollCooldown = 0;
	static double sRollDuration = 0;
	bool hor, vert;
	bool standing = jamEntityTileMapCollision(self, world->worldMaps[WORLD_WALL_LAYER], self->x, self->y + 1);
	if (sPlayerWalkSprite == NULL && sPlayerStandSprite == NULL && sPlayerJumpSprite == NULL) {
		sPlayerWalkSprite = jamAssetHandlerGetSprite(gGameData, "PlayerMovingSprite");
		sPlayerStandSprite = jamAssetHandlerGetSprite(gGameData, "PlayerStandingSprite");
		sPlayerJumpSprite = jamAssetHandlerGetSprite(gGameData, "PlayerJumpingSprite");
	}

	///////////////// Enemy collisions /////////////////
	JamEntity* collEnemy = jamWorldEntityCollision(world, self, self->x, self->y);
	collEnemy = (collEnemy != NULL && collEnemy->type == TYPE_ENEMY) ? collEnemy : NULL;
	
	// Throw the player away from the enemy if colliding or if rolling throw the enemy out
	if (collEnemy != NULL && gFlicker <= 0 && !sPlayerRolling && !sInvincible
			&& ((EnemyData*)collEnemy->data)->fadeOut == 0) {
		self->hSpeed = sign(self->x - collEnemy->x) * KNOCKBACK_VELOCITY;
		self->vSpeed = -KNOCKBACK_VELOCITY;
		gFlicker = FLICKER_FRAMES;
		gPlayerHP -= ((EnemyData*)collEnemy->data)->power;
	} else if (collEnemy != NULL && sPlayerRolling) {
		((EnemyData*)collEnemy->data)->fadeOut = ENEMY_FADE_OUT_TIME;
		collEnemy->hSpeed = ENEMY_KNOCKBACK_VELOCITY * self->scaleX;
		collEnemy->vSpeed = -ENEMY_KNOCKBACK_VELOCITY;
	}

	// Rolling
	if (standing && !sPlayerRolling && sRollCooldown <= 0 && sRollDuration <= 0 && jamControlMapCheck(gPlayerControls, "roll")) {
		sPlayerRolling = true;
		self->hSpeed = ROLL_SPEED * self->scaleX;
		sRollCooldown = ROLL_COOLDOWN;
		sRollDuration = ROLL_DURATION;
	}
	sRollCooldown -= jamRendererGetDelta();

	// Physics
	sbProcessPhysics(
			world,
			self,
			jamControlMapCheck(gPlayerControls, "jump") != 0,
			jamControlMapCheck(gPlayerControls, "move"),
			true,
			PLAYER_ACCELERATION,
			PLAYER_JUMP_VELOCITY,
			(jamControlMapCheck(gPlayerControls, "run") ? MAXIMUM_PLAYER_RUN_VELOCITY
														: MAXIMUM_PLAYER_WALK_VELOCITY),
			&sPlayerJumped
	);

	sbProcessAnimations(
			world,
			self,
			sPlayerWalkSprite,
			sPlayerStandSprite,
			sPlayerJumpSprite,
			!sPlayerJumped && !sPlayerRolling
	);

	// Spinning effects
	if (sPlayerJumped)
		self->rot += jamRendererGetDelta() * self->scaleX * JUMP_ROT_SPEED;
	if (sPlayerRolling) {
		self->rot += jamRendererGetDelta() * self->scaleX * ROLL_ROT_SPEED;
		sRollDuration -= jamRendererGetDelta();
		self->hSpeed = ROLL_SPEED * self->scaleX;
		if (sRollDuration <= 0)
			sPlayerRolling = false;
	}

	// Collisions
	sbProcessCollisions(world, self, &hor, &vert);
	if (hor) {
		sPlayerRolling = false;
		sRollCooldown = 0;
		sRollDuration = 0;
	}
	if (vert)
		sPlayerJumped = false;

	sbProcessMovement(world, self);

	// Player can't horizontally walk out of the game world
	self->x = clamp(self->x, 0, world->worldMaps[0]->width * world->worldMaps[0]->cellWidth);

	// Tween the camera position towards the player
	double xx = jamRendererGetCameraX() + (((self->x - GAME_WIDTH / 2) - jamRendererGetCameraX()) * 0.25);
	double yy = jamRendererGetCameraY() + ((((self->y - GAME_HEIGHT / 2) - 25) - jamRendererGetCameraY()) * 0.25);

	// The camera is clamped to only show the game world
	jamRendererSetCameraPos(
			clamp(xx, 0, world->worldMaps[0]->width * world->worldMaps[0]->cellWidth - GAME_WIDTH),
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
	jamFontRender(sGameFont, (int)round(jamRendererGetCameraX()) + 9, (int)round(jamRendererGetCameraY()) + 10, "HP: %f%%", round((double)gPlayerHP));
}
////////////////////////////////////////////////////////////
