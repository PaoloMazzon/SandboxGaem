/// \file Levels.h
/// \author plo

#pragma once
#include <JamEngine.h>

#ifdef __cplusplus
extern "C" {
#endif

/// \brief Saves data about a door and world to a file
void sbSaveWorldData(const char* name, uint32 doorType);

/// \brief Loads data from the file about where to start the game
void sbLoadWorldData(const char** name, uint32* doorType);

/// \brief Common creation function for worlds that finds where the player should spawn and places him
void onGenericWorldCreate(JamWorld* self, JamAssetHandler* handler);

/// \brief Creation function for the starting world
void onStartingWorldCreate(JamWorld* self, JamAssetHandler* handler);

/// \brief Frame function for starting world
void onStartingWorldFrame(JamWorld* self, JamAssetHandler* handler);

/// \brief Proc function for Overworld1.tmx
void onOverworld1Frame(JamWorld* self, JamAssetHandler* handler);

#ifdef __cplusplus
}
#endif