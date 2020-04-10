#include <malloc.h>
#include <math.h>
#include <SandConstants.h>
#include <JamEngine.h>
#include <TileMap.h>
#include <Character.h>
#include <Message.h>
#include "Levels.h"
#include <Buffer.h>

/////////////// Player Globals ///////////////
extern JamAssetHandler* gGameData;
extern JamControlMap* gControlMap;
static double gFlicker; // This is to flicker when the player gets hit for x frames

////////////////////////////////////////////////////////////
void onPlayerCreate(JamWorld* world, JamEntity* self) {
	JamBuffer* buffer;
	onCharacterCreate(world, self);

	// So the camera doesn't jump to the player
	jamRendererSetCameraPos(
			self->x - GAME_WIDTH / 2.0,
			self->y - GAME_HEIGHT / 2.0
	);

	// Load player stats or set default ones
	buffer = jamBufferLoad(PLAYER_SAVE_FILE);

	if (buffer != NULL && buffer->size != 0) {
		jamBufferReadByteX(buffer, self->data, sizeof(CharacterData));
	} else {
		sbCharData(self, Stats, thorns) = 0;
		sbCharData(self, Stats, rollDamage) = 100;
		sbCharData(self, Stats, rollSpeed) = ROLL_SPEED;
		sbCharData(self, Stats, rollDuration) = ROLL_DURATION;
		sbCharData(self, Stats, rollCooldown) = ROLL_COOLDOWN;
		sbCharData(self, Stats, maxHP) = 100;
		sbCharData(self, Stats, airRes) = 0;
		sbCharData(self, Stats, maxHP) = 100;
		sbCharData(self, State, hp) = 100;
		sbCharData(self, Stats, level) = 1;
	}
	jamBufferFree(buffer);
}
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
void onPlayerDestroy(JamWorld* world, JamEntity* self) {
	// Save player data
	JamBuffer* buffer = jamBufferCreate(sizeof(CharacterData));
	jamBufferAddByteX(buffer, self->data, sizeof(CharacterData));
	jamBufferSave(buffer, PLAYER_SAVE_FILE);
	jamBufferFree(buffer);

    onCharacterDestroy(world, self);
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
	bool vert;
	if (sPlayerWalkSprite == NULL && sPlayerStandSprite == NULL && sPlayerJumpSprite == NULL) {
		sPlayerWalkSprite = jamAssetHandlerGetSprite(gGameData, "PlayerMovingSprite");
		sPlayerStandSprite = jamAssetHandlerGetSprite(gGameData, "PlayerStandingSprite");
		sPlayerJumpSprite = jamAssetHandlerGetSprite(gGameData, "PlayerJumpingSprite");
	}

	//////////////////////////// Entity Collisions ////////////////////////////
	JamEntity* collision = jamWorldEntityCollision(world, self, self->x, self->y);

	while (collision != NULL) {
		// Enemy collisions
		if (IS_TYPE_ENEMY(collision->type)) {
			// Throw the player away from the enemy if colliding or if rolling throw the enemy out
			if (gFlicker <= 0 && !sbCharData(self, State, rolling) && !sInvincible
				&& sbCharData(collision, State, fadeOut) == 0) {
				gFlicker = FLICKER_FRAMES;
				self->hSpeed -= sign(self->hSpeed) * KNOCKBACK_VELOCITY;
				self->vSpeed -= 5;
				if (sbCharData(collision, State, rolling))
					sbCharData(self, State, hp) -= sbCharData(collision, Stats, rollDamage);
				else
					sbCharData(self, State, hp) -= sbCharData(collision, Stats, thorns);
			} else if (sbCharData(self, State, rolling)) {
				sbCharData(collision, State, hp) -= sbCharData(self, Stats, rollDamage);
				collision->hSpeed = ENEMY_KNOCKBACK_VELOCITY * self->scaleX;
				collision->vSpeed = -ENEMY_KNOCKBACK_VELOCITY;
			}
		} else if (IS_TYPE_NPC(collision->type)) {
			if (jamControlMapCheck(gControlMap, "interact") && !sbMessageActive())
				sbQueueMessage(sbCharData(collision, Info, name), sbCharData(collision, Info, passiveDialogue), collision->id);
		} else if (IS_TYPE_DOOR(collision->type) && collision->properties != NULL) {
			sbSaveWorldData(jamTMXDataGetProperty(collision->properties, "world")->stringVal, collision->type);
			jamWorldHandlerSwitch(jamTMXDataGetProperty(collision->properties, "world")->stringVal);
		}

		collision = jamWorldEntityCollision(world, self, self->x, self->y);
	}

	// Physics
	sbProcCharacterPhysics(
			world,
			self,
			jamControlMapCheck(gControlMap, "jump") != 0,
			jamControlMapCheck(gControlMap, "move"),
			true,
			PLAYER_ACCELERATION,
			PLAYER_JUMP_VELOCITY,
			(jamControlMapCheck(gControlMap, "run") ? MAXIMUM_PLAYER_RUN_VELOCITY
													: MAXIMUM_PLAYER_WALK_VELOCITY),
			&sPlayerJumped,
			jamControlMapCheck(gControlMap, "roll") != 0
	);

	sbProcCharacterAnimations(
			world,
			self,
			sPlayerWalkSprite,
			sPlayerStandSprite,
			sPlayerJumpSprite,
			!sPlayerJumped
	);

	// Spinning effects
	if (sPlayerJumped)
		self->rot += jamRendererGetDelta() * self->scaleX * JUMP_ROT_SPEED;

	// Collisions
	if (sbProcessCharacterDeath(world, self)) {
		sbProcCharacterCollisions(world, self, NULL, &vert);
		if (vert)
			sPlayerJumped = false;
	}

	sbProcCharacterMovement(world, self);

	// Player can't horizontally walk out of the game world
	self->x = clamp(self->x, 0, world->worldMaps[0]->width * world->worldMaps[0]->cellWidth);

	// Tween the camera position towards the player
	double xx = jamRendererGetCameraX() + (((self->x - GAME_WIDTH / 2.0) - jamRendererGetCameraX()) * 0.25);
	double yy;

	if (sbMessageActive())
		yy = jamRendererGetCameraY() + ((((self->y + 32 - GAME_HEIGHT / 2.0) - 25) - jamRendererGetCameraY()) * 0.25);
	else
		yy = jamRendererGetCameraY() + ((((self->y - GAME_HEIGHT / 2.0) - 25) - jamRendererGetCameraY()) * 0.25);

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
	jamDrawTexturePart(sHealthBarTex, (int)round(jamRendererGetCameraX()) + 8, (int)round(jamRendererGetCameraY()) + 8, 0, 0, (int)((sbCharData(self, State, hp) / sbCharData(self, Stats, maxHP) * size)), 12);
	jamFontRender(sGameFont, (int)round(jamRendererGetCameraX()) + 9, (int)round(jamRendererGetCameraY()) + 10, "Level %f", round(sbCharData(self, Stats, level)));
}
////////////////////////////////////////////////////////////
