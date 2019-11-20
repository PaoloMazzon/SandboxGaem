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
	jamAddBehaviourToMap(bMap, "BasicEnemyBehaviour", onBEnemyCreate, onBEnemyDestroy, onBEnemyFrame, onBEnemyDraw);
	jamAssetLoadINI(assetHandler, "assets/game.ini", bMap);
	gameWorld = jamGetWorldFromHandler(assetHandler, "GameWorld");
	backgroundTex = jamGetTextureFromHandler(assetHandler, "BackgroundTexture");
	printf("%f\n", jamGetEntityFromHandler(assetHandler, "EnemyEntity")->hitboxOffsetX);
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
	jamResetRenderer(481 * 2, 321 * 2, false);
	jamIntegerMaximizeScreenBuffer();
	runMenu();
	jamRendererQuit();
	return 0;
}
