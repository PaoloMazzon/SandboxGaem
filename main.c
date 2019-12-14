#include <stdio.h>
#include <JamEngine.h>
#include <EntityBehaviour.h>
#include <SandConstants.h>

// The global asset handler for the game
JamAssetHandler* gGameData;

void runGame() {
	bool stopRunning = false;

	// Data we're allocating and need to free
	JamBehaviourMap* bMap = jamCreateBehaviourMap();
	gGameData = jamCreateAssetHandler(1000);
	
	// Data handled by the asset handler (not our problem)
	JamWorld* gameWorld;
	JamTexture* background1Tex, *background2Tex;
	
	// Load assets
	jamAddBehaviourToMap(bMap, "PlayerBehaviour", onPlayerCreate, onPlayerDestroy, onPlayerFrame, onPlayerDraw);
	jamAddBehaviourToMap(bMap, "BasicEnemyBehaviour", onBEnemyCreate, onBEnemyDestroy, onBEnemyFrame, onBEnemyDraw);
	jamAssetLoadINI(gGameData, "assets/game.ini", bMap);
	gameWorld = jamGetWorldFromHandler(gGameData, "GameWorld");
	background1Tex = jamGetTextureFromHandler(gGameData, "BackLayer1Texture");
	background2Tex = jamGetTextureFromHandler(gGameData, "BackLayer2Texture");

	// Game data
	double camX;
	double camY;

	// Autotile the level
	if (gameWorld != NULL)
		jamAutoTileMap(gameWorld->worldMaps[0], jamGetSpriteFromHandler(gGameData, "DirtTilesetSprite"));
    
	while (jamProcEvents() && !stopRunning && !jGetError()) {
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
		jamProcEndFrame();
	}

	jamFreeAssetHandler(gGameData);
	jamFreeBehaviourMap(bMap);
}

void runMenu() {
	runGame();
}

int main(int argc, const char* argv[]) {
	jamInitRenderer(&argc, (char**)argv, "Gaem", 480, 320, 53);
	jamResetRenderer(GAME_WIDTH * 2, GAME_HEIGHT * 2, false);
	jamIntegerMaximizeScreenBuffer();
	runMenu();
	jamRendererQuit();
	return 0;
}
