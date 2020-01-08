#include <stdio.h>
#include <JamEngine.h>
#include <EntityBehaviour.h>
#include <SandConstants.h>

#define DEFAULT_GAME_SCALE 1

// The global asset handler for the game
JamAssetHandler* gGameData;

void runGame() {
	bool stopRunning = false;

	// Data we're allocating and need to free
	JamBehaviourMap* bMap = jamBehaviourMapCreate();
	gGameData = jamAssetHandlerCreate(1000);
	
	// Data handled by the asset handler (not our problem)
	JamWorld* gameWorld;
	JamTexture* background1Tex, *background2Tex;
	
	// Load assets
	jamBehaviourMapAdd(bMap, "PlayerBehaviour", onPlayerCreate, onPlayerDestroy, onPlayerFrame, onPlayerDraw);
	jamBehaviourMapAdd(bMap, "BasicEnemyBehaviour", onBEnemyCreate, onBEnemyDestroy, onBEnemyFrame, onBEnemyDraw);
	jamAssetHandlerLoadINI(gGameData, "assets/game.ini", bMap);
	gameWorld = jamAssetHandlerGetWorld(gGameData, "GameWorld");
	background1Tex = jamAssetHandlerGetTexture(gGameData, "BackLayer1Texture");
	background2Tex = jamAssetHandlerGetTexture(gGameData, "BackLayer2Texture");

	// Game data
	double camX;
	double camY;

	// Autotile the level
	if (gameWorld != NULL)
		jamTileMapAuto(gameWorld->worldMaps[0], jamAssetHandlerGetSprite(gGameData, "DirtTilesetSprite"));
    
	while (jamRendererProcEvents() && !stopRunning && !jGetError()) {
		// Double parallax background - 3 renders/background to cover all possible places
		camX = jamRendererGetCameraX();
		camY = jamRendererGetCameraY();
		jamDrawFillColour(0, 150, 255, 255);
		jamDrawTexture(background2Tex, (((int)camX / (GAME_WIDTH * 10)) - 1) * GAME_WIDTH + (int)(camX * 0.9), (int)camY);
		jamDrawTexture(background2Tex, ((int)camX / (GAME_WIDTH * 10)) * GAME_WIDTH + (int)(camX * 0.9), (int)camY);
		jamDrawTexture(background2Tex, (((int)camX / (GAME_WIDTH * 10)) + 1) * GAME_WIDTH + (int)(camX * 0.9), (int)camY);
		jamDrawTexture(background1Tex, (((int)camX / (GAME_WIDTH * 4)) - 1) * GAME_WIDTH + (int)(camX * 0.75), (int)camY + 7);
		jamDrawTexture(background1Tex, ((int)camX / (GAME_WIDTH * 4)) * GAME_WIDTH + (int)(camX * 0.75), (int)camY + 7);
		jamDrawTexture(background1Tex, (((int)camX / (GAME_WIDTH * 4)) + 1) * GAME_WIDTH + (int)(camX * 0.75), (int)camY + 7);

		// And the world tiles
		jamDrawTileMap(gameWorld->worldMaps[0], 0, 0, 0, 0, 0, 0);

		// Make escape exit the program
		if (jamInputCheckKey(JAM_KB_ESCAPE))
			stopRunning = true;
		
		// Process the game frame
		jamWorldProcFrame(gameWorld);
		jamRendererProcEndFrame();
	}

	jamAssetHandlerFree(gGameData);
	jamBehaviourMapFree(bMap);
}

void runMenu() {
	runGame();
}

int main(int argc, const char* argv[]) {
	jamRendererInit(&argc, (char**)argv, "Gaem", 480, 320, 60);
	jamRendererReset(GAME_WIDTH * DEFAULT_GAME_SCALE, GAME_HEIGHT * DEFAULT_GAME_SCALE, false);
	jamRendererIntegerScale();
	jamRendererSetIcon("assets/icon.png");
	runMenu();
	jamRendererQuit();
	return 0;
}
