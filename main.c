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
	JamTexture* backgroundTex;
	
	// Load assets
	jamAddBehaviourToMap(bMap, "PlayerBehaviour", onPlayerCreate, onPlayerDestroy, onPlayerFrame, onPlayerDraw);
	jamAddBehaviourToMap(bMap, "BasicEnemyBehaviour", onBEnemyCreate, onBEnemyDestroy, onBEnemyFrame, onBEnemyDraw);
	jamAssetLoadINI(gGameData, "assets/game.ini", bMap);
	gameWorld = jamGetWorldFromHandler(gGameData, "GameWorld");
	backgroundTex = jamGetTextureFromHandler(gGameData, "BackgroundTexture");

	// Autotile the level
	if (gameWorld != NULL)
		jamAutoTileMap(gameWorld->worldMaps[0], jamGetSpriteFromHandler(gGameData, "DirtTilesetSprite"));
    
	while (jamProcEvents() && !stopRunning && !jGetError()) {
		// Draw the background and world
	    jamDrawTexture(backgroundTex, (int)jamRendererGetCameraX(), (int)jamRendererGetCameraY());
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
	jamInitRenderer(&argc, (char**)argv, "Gaem", 480, 320, 60);
	jamResetRenderer(GAME_WIDTH * 2, GAME_HEIGHT * 2, false);
	jamIntegerMaximizeScreenBuffer();
	runMenu();
	jamRendererQuit();
	return 0;
}
