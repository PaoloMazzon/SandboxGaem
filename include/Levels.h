/// \file Levels.h
/// \author plo

#pragma once
#include <JamEngine.h>

#ifdef __cplusplus
extern "C" {
#endif

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