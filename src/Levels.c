//
// Created by plo on 4/9/20.
//
#include <Levels.h>
#include <math.h>
#include <SandConstants.h>
#include <Message.h>

extern JamAssetHandler* gGameData;
extern JamControlMap* gControlMap;
extern bool gDebug;

void onGenericWorldCreate(JamWorld* self, JamAssetHandler* handler) {

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
	JamTexture* background1Tex, *background2Tex, *background3Tex, *skyTex;
	JamFont* debugFont;

	// Load assets
	background1Tex = jamAssetHandlerGetTexture(gGameData, "BackLayer1Texture");
	background2Tex = jamAssetHandlerGetTexture(gGameData, "BackLayer2Texture");
	background3Tex = jamAssetHandlerGetTexture(gGameData, "BackLayer3Texture");
	skyTex = jamAssetHandlerGetTexture(gGameData, "SkyTexture");
	debugFont = jamAssetHandlerGetFont(gGameData, "DebugFont");
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