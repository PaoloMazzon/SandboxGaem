//
// Created by plo on 4/9/20.
//
#include <Levels.h>
#include <math.h>
#include <SandConstants.h>
#include <Message.h>
#include <memory.h>
#include <Buffer.h>
#include <malloc.h>

extern JamAssetHandler* gGameData;
extern JamControlMap* gControlMap;
extern bool gDebug;

//////////////////////// Helper functions ////////////////////////
void sbSaveWorldData(const char* name, uint32 doorType) {
	// 4 bytes for size of name (with), x bytes for name, 4 bytes for doortype
	uint32 len = (uint32)strlen(name) + 1;
	JamBuffer* buffer = jamBufferCreate(len + 8);
	jamBufferAddByte4(buffer, &len);
	jamBufferAddByteX(buffer, (void*)name, len);
	jamBufferAddByte4(buffer, &doorType);
	jamBufferSave(buffer, WORLD_SAVE_FILE);
	jamBufferFree(buffer);
}

void sbLoadWorldData(const char** name, uint32* doorType) {
	JamBuffer* buffer = jamBufferLoad(WORLD_SAVE_FILE);
	uint32 len;
	if (buffer != NULL && buffer->size != 0) {
		jamBufferReadByte4(buffer, &len);

		if (name != NULL) {
			*name = malloc(len);
			jamBufferReadByteX(buffer, (void*)*name, len);
		} else {
			buffer->pointer += len;
		}

		jamBufferReadByte4(buffer, doorType);
	} else {
		if (name != NULL) {
			*name = malloc(14);
			strcpy((void *) *name, "StartingWorld");
		}
		*doorType = 0;
	}
	jamBufferFree(buffer);
}

//////////////////////// Generic functions ////////////////////////

void onGenericWorldCreate(JamWorld* self, JamAssetHandler* handler) {
	uint32 doorType;
	JamEntity* ent;
	sbLoadWorldData(NULL, &doorType);
	ent = jamWorldFindEntity(self, jamWorldFindEntityType(self, doorType));

	if (ent != NULL)
		jamWorldAddEntity(self, jamEntityCopy(jamAssetHandlerGetEntity(handler, "PlayerEntity"), ent->x, ent->y));
}

//////////////////////// Starting world ////////////////////////
void onStartingWorldCreate(JamWorld* self, JamAssetHandler* handler) {
	sbStartMesssageQueue();
	sbQueueMessage(NARRATOR_NAME, "There once lived a mate so boring\n"
								  "Who compared to none but his flooring\n"
								  "He decided one day\n"
								  "To throw it all away\n"
								  "And set out into the wild exploring", ID_NOT_ASSIGNED);
	sbQueueMessage(NARRATOR_NAME, "While his ignorance is naught but endless\n"
								  "And he is caught in a rut that is friendless\n"
								  "His grit was unmatched\n"
								  "And his chain unlatched\n"
								  "He thought himself a legend, deathless", ID_NOT_ASSIGNED);
}

void onStartingWorldFrame(JamWorld* self, JamAssetHandler* handler) {
	// Draw tiles
	jamDrawTileMap(self->worldMaps[WORLD_BACKGROUND_LAYER], 0, 0, 0, 0, 0, 0);
	jamDrawTileMap(self->worldMaps[WORLD_FOREGROUND_LAYER], 0, 0, 0, 0, 0, 0);
	jamDrawTileMap(self->worldMaps[WORLD_WALL_LAYER], 0, 0, 0, 0, 0, 0);

	// Process the game frame
	jamWorldProcFrame(self);
	jamDrawTileMap(self->worldMaps[WORLD_FOREFRONT_LAYER], 0, 0, 0, 0, 0, 0);

	// Draw any active messages
	sbDrawMessage(self);
}

//////////////////////// Overworld 1 ////////////////////////
void onOverworld1Frame(JamWorld* self, JamAssetHandler* handler) {
	// Data handled by the asset handler (not our problem)
	static JamTexture* background1Tex, *background2Tex, *background3Tex, *skyTex;
	static JamFont* debugFont;

	// Load assets
	if (background1Tex == NULL) {
		background1Tex = jamAssetHandlerGetTexture(gGameData, "BackLayer1Texture");
		background2Tex = jamAssetHandlerGetTexture(gGameData, "BackLayer2Texture");
		background3Tex = jamAssetHandlerGetTexture(gGameData, "BackLayer3Texture");
		skyTex = jamAssetHandlerGetTexture(gGameData, "SkyTexture");
		debugFont = jamAssetHandlerGetFont(gGameData, "DebugFont");
	}

	double camX;
	double camY;
	bool fullscreen = false;

	// Sky texture
	jamDrawTexture(skyTex, (int)round(jamRendererGetCameraX()), (int)round(jamRendererGetCameraY()));

	// Back-most mountain
	camY = jamRendererGetCameraY() + 30;
	camX = (jamRendererGetCameraX() - 15) * 0.80;
	while (camX < jamRendererGetCameraX() - GAME_WIDTH)
		camX += GAME_WIDTH;
	jamDrawTexture(background3Tex, (int)floor(camX), (int)round(camY) - 20);
	jamDrawTexture(background3Tex, (int)floor(camX + GAME_WIDTH), (int)round(camY) - 20);

	// Middle mountain
	camX = jamRendererGetCameraX() * 0.70;
	while (camX < jamRendererGetCameraX() - GAME_WIDTH)
		camX += GAME_WIDTH;
	jamDrawTexture(background2Tex, (int)floor(camX), (int)round(camY));
	jamDrawTexture(background2Tex, (int)floor(camX + GAME_WIDTH), (int)round(camY));

	// Front mountain
	camX = jamRendererGetCameraX() * 0.55;
	while (camX < jamRendererGetCameraX() - GAME_WIDTH)
		camX += GAME_WIDTH;
	jamDrawTexture(background1Tex, (int)floor(camX), (int)round(camY));
	jamDrawTexture(background1Tex, (int)floor(camX + GAME_WIDTH), (int)round(camY));

	// Draw tiles
	jamDrawTileMap(self->worldMaps[WORLD_BACKGROUND_LAYER], 0, 0, 0, 0, 0, 0);
	jamDrawTileMap(self->worldMaps[WORLD_FOREGROUND_LAYER], 0, 0, 0, 0, 0, 0);
	jamDrawTileMap(self->worldMaps[WORLD_WALL_LAYER], 0, 0, 0, 0, 0, 0);

	// Process the game frame
	jamWorldProcFrame(self);
	jamDrawTileMap(self->worldMaps[WORLD_FOREFRONT_LAYER], 0, 0, 0, 0, 0, 0);

	// Draw any active messages
	sbDrawMessage(self);

	// Debug
	if (gDebug) {
		jamFontRender(
				debugFont,
				(int) round(jamRendererGetCameraX()) + GAME_WIDTH - jamFontWidth(debugFont, "FPS: 60"),
				(int) round(jamRendererGetCameraY()) - 3,
				"FPS: %f",
				round(jamRendererGetFramerate())
		);
	}

	if (jamInputCheckKeyPressed(JAM_KB_F4)) {
		fullscreen = !fullscreen;
		if (fullscreen) {
			jamRendererReset(GAME_WIDTH, GAME_HEIGHT, true);
			jamRendererIntegerScale();
		} else {
			jamRendererReset(GAME_WIDTH * DEFAULT_GAME_SCALE, GAME_HEIGHT * DEFAULT_GAME_SCALE, false);
			jamRendererConfig(GAME_WIDTH, GAME_HEIGHT, GAME_WIDTH * DEFAULT_GAME_SCALE, GAME_HEIGHT * DEFAULT_GAME_SCALE);
		}
	}
}