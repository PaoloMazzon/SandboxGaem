#include <stdio.h>
#include <JamEngine.h>
#include <Character.h>
#include <math.h>
#include <SandConstants.h>
#include <string.h>
#include <Message.h>
#include <stdlib.h>
#include <Levels.h>

// The global asset handler for the game
JamAssetHandler* gGameData;

// The controls handler
JamControlMap* gControlMap;

// Weather or not debug is enabled
bool gDebug;

void runGame() {
	// Data we're allocating and need to free
	JamBehaviourMap* bMap = jamBehaviourMapCreate();
	gGameData = jamAssetHandlerCreate(1000);
	jamBehaviourMapAdd(bMap, "PlayerBehaviour", onPlayerCreate, onPlayerDestroy, onPlayerFrame, onPlayerDraw);
	jamBehaviourMapAdd(bMap, "SkellyManBehaviour", onSkellyManCreate, onSkellyManDestroy, onSkellyManFrame, onSkellyManDraw);
	jamAssetHandlerLoadINI(gGameData, "assets/game.ini", bMap);

	// Load levels
	jamWorldHandlerAdd("assets/tmx/StartingWorld.tmx.tmx", NULL, onStartingWorldCreate, NULL, NULL);
	jamWorldHandlerAdd("assets/tmx/Overworld1.tmx", NULL, NULL, onOverworld1Frame, NULL);

	// Run the game
	jamWorldHandlerRun(gGameData);

	jamAssetHandlerFree(gGameData);
	jamBehaviourMapFree(bMap);
}

void runMenu() {
	runGame();
}

int main(int argc, const char* argv[]) {
	if (argc > 1 && strcmp(argv[1], "--debug") == 0)
		gDebug = true;

	// Setup jamengine
	jamRendererInit(&argc, (char**)argv, "Gaem", GAME_WIDTH, GAME_HEIGHT, 60);
	jamRendererReset(GAME_WIDTH * DEFAULT_GAME_SCALE, GAME_HEIGHT * DEFAULT_GAME_SCALE, false);
	jamRendererIntegerScale();
	jamRendererSetVSync(true);
	jamRendererSetIcon("assets/icon.png");
	
	// Setup the controls
	gControlMap = jamControlMapCreate();
	jamControlMapAddInput(gControlMap, "move", JAM_KB_RIGHT, 0, JAM_KEYBOARD_INPUT, JAM_INPUT_ACTIVE, 1);
	jamControlMapAddInput(gControlMap, "move", JAM_KB_LEFT, 0, JAM_KEYBOARD_INPUT, JAM_INPUT_ACTIVE, -1);
	jamControlMapAddInput(gControlMap, "move", JAM_AXIS_LEFTX, 0, JAM_GAMEPAD_INPUT, JAM_INPUT_ACTIVE, 1);
	jamControlMapAddInput(gControlMap, "move", JAM_BUTTON_DPAD_LEFT, 0, JAM_GAMEPAD_INPUT, JAM_INPUT_ACTIVE, -1);
	jamControlMapAddInput(gControlMap, "move", JAM_BUTTON_DPAD_RIGHT, 0, JAM_GAMEPAD_INPUT, JAM_INPUT_ACTIVE, 1);
	jamControlMapAddInput(gControlMap, "jump", JAM_KB_UP, 0, JAM_KEYBOARD_INPUT, JAM_INPUT_PRESSED, 1);
	jamControlMapAddInput(gControlMap, "jump", JAM_AXIS_LEFTY, 0, JAM_GAMEPAD_INPUT, JAM_INPUT_PRESSED, 1);
	jamControlMapAddInput(gControlMap, "jump", JAM_BUTTON_A, 0, JAM_GAMEPAD_INPUT, JAM_INPUT_PRESSED, 1);
	jamControlMapAddInput(gControlMap, "interact", JAM_KB_C, 0, JAM_KEYBOARD_INPUT, JAM_INPUT_PRESSED, 1);
	jamControlMapAddInput(gControlMap, "interact", JAM_BUTTON_A, 0, JAM_GAMEPAD_INPUT, JAM_INPUT_PRESSED, 1);
	jamControlMapAddInput(gControlMap, "run", JAM_KB_Z, 0, JAM_KEYBOARD_INPUT, JAM_INPUT_ACTIVE, 1);
	jamControlMapAddInput(gControlMap, "run", JAM_BUTTON_X, 0, JAM_GAMEPAD_INPUT, JAM_INPUT_ACTIVE, 1);
	jamControlMapAddInput(gControlMap, "roll", JAM_KB_X, 0, JAM_KEYBOARD_INPUT, JAM_INPUT_PRESSED, 1);
	jamControlMapAddInput(gControlMap, "roll", JAM_BUTTON_B, 0, JAM_GAMEPAD_INPUT, JAM_INPUT_PRESSED, 1);
	jamControlMapAddInput(gControlMap, "message", JAM_BUTTON_A, 0, JAM_GAMEPAD_INPUT, JAM_INPUT_PRESSED, 1);
	jamControlMapAddInput(gControlMap, "message", JAM_KB_Z, 0, JAM_KEYBOARD_INPUT, JAM_INPUT_PRESSED, 1);
	jamControlMapAddInput(gControlMap, "message_scroll", JAM_KB_DOWN, 0, JAM_KEYBOARD_INPUT, JAM_INPUT_ACTIVE, 1);
	jamControlMapAddInput(gControlMap, "message_scroll", JAM_KB_UP, 0, JAM_KEYBOARD_INPUT, JAM_INPUT_ACTIVE, -1);
	jamControlMapAddInput(gControlMap, "message_scroll", JAM_AXIS_LEFTY, 0, JAM_GAMEPAD_INPUT, JAM_INPUT_ACTIVE, 1);
	jamControlMapAddInput(gControlMap, "message_scroll", JAM_BUTTON_DPAD_UP, 0, JAM_GAMEPAD_INPUT, JAM_INPUT_ACTIVE, -1);
	jamControlMapAddInput(gControlMap, "message_scroll", JAM_BUTTON_DPAD_DOWN, 0, JAM_GAMEPAD_INPUT, JAM_INPUT_ACTIVE, 1);

	// Run the game
	runMenu();

	// Free assets
	jamControlMapFree(gControlMap);
	jamRendererQuit();
	return 0;
}
