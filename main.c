#include <stdio.h>
#include <JamEngine.h>
#include <EntityBehaviour.h>
#include <math.h>
#include <SandConstants.h>

// The global asset handler for the game
JamAssetHandler* gGameData;

void runGame() {
	bool stopRunning = false;

	// Data we're allocating and need to free
	JamBehaviourMap* bMap = jamBehaviourMapCreate();
	gGameData = jamAssetHandlerCreate(1000);
	
	// Data handled by the asset handler (not our problem)
	JamWorld* gameWorld;
	JamTexture* background1Tex, *background2Tex, *skyTex;
	JamFont* debugFont;
	
	// Load assets
	jamBehaviourMapAdd(bMap, "PlayerBehaviour", onPlayerCreate, onPlayerDestroy, onPlayerFrame, onPlayerDraw);
	jamBehaviourMapAdd(bMap, "BasicEnemyBehaviour", onBEnemyCreate, onBEnemyDestroy, onBEnemyFrame, onBEnemyDraw);
	jamAssetHandlerLoadINI(gGameData, "assets/game.ini", bMap);
	gameWorld = jamAssetHandlerGetWorld(gGameData, "GameWorld");
	background1Tex = jamAssetHandlerGetTexture(gGameData, "BackLayer1Texture");
	background2Tex = jamAssetHandlerGetTexture(gGameData, "BackLayer2Texture");
	skyTex = jamAssetHandlerGetTexture(gGameData, "SkyTexture");
	debugFont = jamAssetHandlerGetFont(gGameData, "DebugFont");

	// Game data
	double camX;
	double camY;
    
	while (jamRendererProcEvents() && !stopRunning && !jGetError()) {
		// Draw the background
		jamDrawTexture(skyTex, (int)round(jamRendererGetCameraX()), (int)round(jamRendererGetCameraY()));
		camX = jamRendererGetCameraX() * 0.80;
		camY = jamRendererGetCameraY() + 30;
		while (camX < jamRendererGetCameraX() - GAME_WIDTH)
			camX += GAME_WIDTH;
		jamDrawTexture(background2Tex, (int)round(camX), (int)round(camY));
		jamDrawTexture(background2Tex, (int)round(camX + GAME_WIDTH), (int)round(camY));
		camX = jamRendererGetCameraX() * 0.65;
		while (camX < jamRendererGetCameraX() - GAME_WIDTH)
			camX += GAME_WIDTH;
		jamDrawTexture(background1Tex, (int)round(camX), (int)round(camY));
		jamDrawTexture(background1Tex, (int)round(camX + GAME_WIDTH), (int)round(camY));

		// Draw tiles
		jamDrawTileMap(gameWorld->worldMaps[WORLD_BACKGROUND_LAYER], 0, 0, 0, 0, 0, 0);
		jamDrawTileMap(gameWorld->worldMaps[WORLD_FOREGROUND_LAYER], 0, 0, 0, 0, 0, 0);
		jamDrawTileMap(gameWorld->worldMaps[WORLD_WALL_LAYER], 0, 0, 0, 0, 0, 0);

		// Make escape exit the program
		if (jamInputCheckKey(JAM_KB_ESCAPE))
			stopRunning = true;
		
		// Process the game frame
		jamWorldProcFrame(gameWorld);
		jamDrawTileMap(gameWorld->worldMaps[WORLD_FOREFRONT_LAYER], 0, 0, 0, 0, 0, 0);

		// Debug
		jamFontRender(
				debugFont,
				(int)round(jamRendererGetCameraX()) + GAME_WIDTH - jamFontWidth(debugFont, "FPS: 60"),
				(int)round(jamRendererGetCameraY()) - 3,
				"FPS: %f",
				round(jamRendererGetFramerate())
		);

		jamRendererProcEndFrame();
	}

	jamAssetHandlerFree(gGameData);
	jamBehaviourMapFree(bMap);
}

void runMenu() {
	runGame();
}

int main(int argc, const char* argv[]) {
	jamRendererInit(&argc, (char**)argv, "Gaem", GAME_WIDTH, GAME_HEIGHT, 60);
	jamRendererReset(GAME_WIDTH * DEFAULT_GAME_SCALE, GAME_HEIGHT * DEFAULT_GAME_SCALE, false);
	jamRendererIntegerScale();
	jamRendererSetIcon("assets/icon.png");
	runMenu();
	jamRendererQuit();
	return 0;
}
