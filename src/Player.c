#include <malloc.h>
#include <math.h>
#include <SandConstants.h>
#include <JamEngine.h>
#include <TileMap.h>
#include <Character.h>
#include <Message.h>

/////////////// Player Globals ///////////////
extern JamAssetHandler* gGameData;
extern JamControlMap* gControlMap;
static double gFlicker; // This is to flicker when the player gets hit for x frames
static int gPlayerHP = 100;
static int gMaxPlayerHP = 100;

////////////////////////////////////////////////////////////
void onPlayerCreate(JamWorld* world, JamEntity* self) {
	onCharacterCreate(world, self);
	sbStartMesssageQueue();
	sbQueueMessage(NARRATOR_NAME, "There once lived a mate so boring\n"
								  "Who compared to none but his flooring\n"
								  "He decided one day\n"
								  "To throw it all away\n"
								  "And set out into the wild exploring", self->id);
	sbQueueMessage(NARRATOR_NAME, "While his ignorance is naught but endless\n"
								  "And he is caught in a rut that is friendless\n"
								  "His grit was unmatched\n"
								  "And his chain unlatched\n"
								  "He thought himself a legend, deathless", self->id);
}
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
void onPlayerDestroy(JamWorld* world, JamEntity* self) {
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

	//////////////////////////// Entity Collisions ////////////////////////////
	JamEntity* collision = jamWorldEntityCollision(world, self, self->x, self->y);

	while (collision != NULL) {
		// Enemy collisions
		if (IS_TYPE_ENEMY(collision->type)) {
			// Throw the player away from the enemy if colliding or if rolling throw the enemy out
			if (gFlicker <= 0 && !sPlayerRolling && !sInvincible
				&& sbGetCharacterFadeOut(collision) == 0) {
				self->hSpeed = sign(self->x - collision->x) * KNOCKBACK_VELOCITY;
				self->vSpeed = -KNOCKBACK_VELOCITY;
				gFlicker = FLICKER_FRAMES;
				gPlayerHP -= sbGetCharacterThorns(collision);
			} else if (sPlayerRolling) {
				sbSetCharacterFadeOut(collision, ENEMY_FADE_OUT_TIME);
				collision->hSpeed = ENEMY_KNOCKBACK_VELOCITY * self->scaleX;
				collision->vSpeed = -ENEMY_KNOCKBACK_VELOCITY;
			}
		} else if (IS_TYPE_NPC(collision->type)) {
			if (jamControlMapCheck(gControlMap, "interact"))
				sbQueueMessage(sbGetCharacterName(collision), sbGetCharacterPassiveDialogue(collision), collision->id);
		}

		collision = jamWorldEntityCollision(world, self, self->x, self->y);
	}

	// Rolling
	if (standing && !sPlayerRolling && sRollCooldown <= 0 && sRollDuration <= 0 &&
			(!sbMessageActive() ? jamControlMapCheck(gControlMap, "roll") : false)) {
		sPlayerRolling = true;
		self->hSpeed = ROLL_SPEED * self->scaleX;
		sRollCooldown = ROLL_COOLDOWN;
		sRollDuration = ROLL_DURATION;
	}
	sRollCooldown -= jamRendererGetDelta();

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
			&sPlayerJumped
	);

	sbProcCharacterAnimations(
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
	sbProcCharacterCollisions(world, self, &hor, &vert);
	if (hor) {
		sPlayerRolling = false;
		sRollCooldown = 0;
		sRollDuration = 0;
	}
	if (vert)
		sPlayerJumped = false;

	sbProcCharacterMovement(world, self);

	// Player can't horizontally walk out of the game world
	self->x = clamp(self->x, 0, world->worldMaps[0]->width * world->worldMaps[0]->cellWidth);

	// Tween the camera position towards the player
	double xx = jamRendererGetCameraX() + (((self->x - GAME_WIDTH / 2) - jamRendererGetCameraX()) * 0.25);
	double yy;

	if (sbMessageActive())
		yy = jamRendererGetCameraY() + ((((self->y + 32 - GAME_HEIGHT / 2) - 25) - jamRendererGetCameraY()) * 0.25);
	else
		yy = jamRendererGetCameraY() + ((((self->y - GAME_HEIGHT / 2) - 25) - jamRendererGetCameraY()) * 0.25);

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
