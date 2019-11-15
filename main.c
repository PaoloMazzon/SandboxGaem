#include <stdio.h>
#include <JamEngine.h>
#include <EntityBehaviour.h>

void runGame() {
	bool stopRunning;
	JamBehaviourMap* bMap = jamCreateBehaviourMap();
	JamAssetHandler* assetHandler = jamCreateAssetHandler();
	JamWorld* gameWorld;
	JamTexture* backgroundTex;
	
	// Load assets
	jamAddBehaviourToMap(bMap, "PlayerBehaviour", onPlayerCreate, onPlayerDestroy, onPlayerFrame, onPlayerDraw);
	jamAssetLoadINI(assetHandler, "assets/game.ini", bMap);
	gameWorld = jamGetWorldFromHandler(assetHandler, "GameWorld");
	backgroundTex = jamGetTextureFromHandler(assetHandler, "BackgroundTexture");

	while (jamProcEvents() && !stopRunning && !jGetError()) {
	    jamDrawTexture(backgroundTex, 0, 0);
		jamDrawTileMap(gameWorld->worldMaps[0], 0, 0, 0, 0, 0, 0);
		jamWorldProcFrame(gameWorld);
		jamProcEndFrame();
	}
}

void runMenu() {
	runGame();
}

int main(int argc, const char* argv[]) {
	jamInitRenderer(&argc, (char**)argv, "Gaem", 480, 320, 60);
	runMenu();
	jamRendererQuit();
	return 0;
}
