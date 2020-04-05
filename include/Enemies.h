#include <JamEngine.h>
#pragma once

typedef struct {
	double moveTime;  ///< Amount of time remaining in this walking bit
	double pauseTime; ///< Amount of time remaining in this pause
	double movement;  ///< Which direction to move in
	double fadeOut;   ///< Death fade out timer
	double power;     ///< Damage this enemy deals to the player
} EnemyData;

//////////////////// Common Data ////////////////////

/// \brief Creates enemy data
EnemyData* sbCreateEnemyData(double power);

/// \brief Processes an enemy's death animation should he be dead, returning weather or not to process collisions
bool sbProcessEnemyDeath(JamWorld* world, JamEntity* enemy);

/// \brief Gets the moveTime from an entity
double sbGetEnemyMoveTime(JamEntity* enemy);

/// \brief Sets the moveTime of an entity
void sbSetEnemyMoveTime(JamEntity* enemy, double moveTime);

/// \brief Subtracts the delta from MoveTime
void sbTickEnemyMoveTime(JamEntity* enemy);

/// \brief Gets the pauseTime from an entity
double sbGetEnemyPauseTime(JamEntity* enemy);

/// \brief Sets the pauseTime of an entity
void sbSetEnemyPauseTime(JamEntity* enemy, double pauseTime);

/// \brief Subtracts the delta from PauseTime
void sbTickEnemyPauseTime(JamEntity* enemy);

/// \brief Gets the movement from an entity
double sbGetEnemyMovement(JamEntity* enemy);

/// \brief Sets the movement of an entity
void sbSetEnemyMovement(JamEntity* enemy, double movement);

/// \brief Gets the fadeOut from an entity
double sbGetEnemyFadeOut(JamEntity* enemy);

/// \brief Checks if an enemy is dead (fade out has begun)
bool sbGetEnemyDead(JamEntity* enemy);

/// \brief Sets the fadeOut of an entity
void sbSetEnemyFadeOut(JamEntity* enemy, double fadeOut);

/// \brief Subtracts the delta from fade out
void sbTickEnemyFadeOut(JamEntity* enemy);

/// \brief Gets the power from an entity
double sbGetEnemyPower(JamEntity* enemy);

/// \brief Sets the power of an entity
void sbSetEnemyPower(JamEntity* enemy, double power);

//////////////////// Enemies ////////////////////

// Skellymen
void onSkellyManCreate(JamWorld* world, JamEntity* self);
void onSkellyManDestroy(JamWorld* world, JamEntity* self);
void onSkellyManFrame(JamWorld* world, JamEntity* self);
void onSkellyManDraw(JamWorld* world, JamEntity* self);
