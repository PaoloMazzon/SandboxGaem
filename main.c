#include <stdio.h>
#include <JamEngine.h>
#include <EntityBehaviour.h>
#include <math.h>
#include <SandConstants.h>
#include <string.h>

// The global asset handler for the game
JamAssetHandler* gGameData;

void runGame(bool debug) {
	bool stopRunning = false;

	// Data we're allocating and need to free
	JamBehaviourMap* bMap = jamBehaviourMapCreate();
	gGameData = jamAssetHandlerCreate(1000);
	
	// Data handled by the asset handler (not our problem)
	JamWorld* gameWorld;
	JamTexture* background1Tex, *background2Tex, *background3Tex, *skyTex;
	JamFont* debugFont;
	
	// Load assets
	jamBehaviourMapAdd(bMap, "PlayerBehaviour", onPlayerCreate, onPlayerDestroy, onPlayerFrame, onPlayerDraw);
	jamBehaviourMapAdd(bMap, "SkellyManBehaviour", onSkellyManCreate, onSkellyManDestroy, onSkellyManFrame, onSkellyManDraw);
	jamAssetHandlerLoadINI(gGameData, "assets/game.ini", bMap);
	gameWorld = jamAssetHandlerGetWorld(gGameData, "GameWorld");
	background1Tex = jamAssetHandlerGetTexture(gGameData, "BackLayer1Texture");
	background2Tex = jamAssetHandlerGetTexture(gGameData, "BackLayer2Texture");
	background3Tex = jamAssetHandlerGetTexture(gGameData, "BackLayer3Texture");
	skyTex = jamAssetHandlerGetTexture(gGameData, "SkyTexture");
	debugFont = jamAssetHandlerGetFont(gGameData, "DebugFont");

	// Game data
	double camX;
	double camY;
    
	while (jamRendererProcEvents() && !stopRunning && !jGetError()) {
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
		camX = jamRendererGetCameraX() * 0.60;
		while (camX < jamRendererGetCameraX() - GAME_WIDTH)
			camX += GAME_WIDTH;
		jamDrawTexture(background1Tex, (int)floor(camX), (int)round(camY));
		jamDrawTexture(background1Tex, (int)floor(camX + GAME_WIDTH), (int)round(camY));

		// Draw tiles
		jamDrawTileMap(gameWorld->worldMaps[WORLD_BACKGROUND_LAYER], 0, 0, 0, 0, 0, 0);
		jamDrawTileMap(gameWorld->worldMaps[WORLD_FOREGROUND_LAYER], 0, 0, 0, 0, 0, 0);
		jamDrawTileMap(gameWorld->worldMaps[WORLD_WALL_LAYER], 0, 0, 0, 0, 0, 0);
		
		// Process the game frame
		jamWorldProcFrame(gameWorld);
		jamDrawTileMap(gameWorld->worldMaps[WORLD_FOREFRONT_LAYER], 0, 0, 0, 0, 0, 0);

		// Debug
		if (debug) {
			jamFontRender(
					debugFont,
					(int) round(jamRendererGetCameraX()) + GAME_WIDTH - jamFontWidth(debugFont, "FPS: 60"),
					(int) round(jamRendererGetCameraY()) - 3,
					"FPS: %f",
					round(jamRendererGetFramerate())
			);
		}

		jamRendererProcEndFrame();
	}

	jamAssetHandlerFree(gGameData);
	jamBehaviourMapFree(bMap);
}

void runMenu(bool debug) {
	runGame(debug);
}

int main(int argc, const char* argv[]) {
	bool debug = false;

	if (argc > 1 && strcmp(argv[1], "--debug") == 0)
		debug = true;

	jamRendererInit(&argc, (char**)argv, "Gaem", GAME_WIDTH, GAME_HEIGHT, 60);
	jamRendererReset(GAME_WIDTH * DEFAULT_GAME_SCALE, GAME_HEIGHT * DEFAULT_GAME_SCALE, false);
	jamRendererIntegerScale();
	jamRendererSetIcon("assets/icon.png");
	runMenu(debug);
	jamRendererQuit();
	return 0;
}
