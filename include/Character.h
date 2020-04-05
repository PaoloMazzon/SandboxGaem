#include <JamEngine.h>
#pragma once

typedef struct {
	double moveTime;  ///< Amount of time remaining in this walking bit
	double pauseTime; ///< Amount of time remaining in this pause
	double movement;  ///< Which direction to move in
	double fadeOut;   ///< Death fade out timer
	double power;     ///< Damage this enemy deals to the player
} CharacterData;

//////////////////// Common Data ////////////////////

/// \brief Processes an enemy's death animation should he be dead, returning weather or not to process collisions
bool sbProcessCharacterDeath(JamWorld *world, JamEntity *enemy);

/// \brief Gets the moveTime from an entity
double sbGetCharacterMoveTime(JamEntity *enemy);

/// \brief Sets the moveTime of an entity
void sbSetCharacterMoveTime(JamEntity *enemy, double moveTime);

/// \brief Subtracts the delta from MoveTime
void sbTickCharacterMoveTime(JamEntity *enemy);

/// \brief Gets the pauseTime from an entity
double sbGetCharacterPauseTime(JamEntity *enemy);

/// \brief Sets the pauseTime of an entity
void sbSetCharacterPauseTime(JamEntity *enemy, double pauseTime);

/// \brief Subtracts the delta from PauseTime
void sbTickCharacterPauseTime(JamEntity *enemy);

/// \brief Gets the movement from an entity
double sbGetCharacterMovement(JamEntity *enemy);

/// \brief Sets the movement of an entity
void sbSetCharacterMovement(JamEntity *enemy, double movement);

/// \brief Gets the fadeOut from an entity
double sbGetCharacterFadeOut(JamEntity *enemy);

/// \brief Checks if an enemy is dead (fade out has begun)
bool sbGetCharacterDead(JamEntity *enemy);

/// \brief Sets the fadeOut of an entity
void sbSetCharacterFadeOut(JamEntity *enemy, double fadeOut);

/// \brief Subtracts the delta from fade out
void sbTickCharacterFadeOut(JamEntity *enemy);

/// \brief Gets the power from an entity
double sbGetCharacterPower(JamEntity *enemy);

/// \brief Sets the power of an entity
void sbSetCharacterPower(JamEntity *enemy, double power);

/// \brief Processes an entity's general physics but collisions are done with sbProcessCollisions
/// \param world World the entity is in
/// \param self Entity that is to be processed
/// \param jump Weather or not the entity will attempt to jump
/// \param movement Direction the entity wishes to move in (-1 for left, 1 for right)
/// \param friction Weather or not to apply friction
/// \param acceleration Acceleration of the entity
/// \param jumpSpeed Vertical lift when jumping
/// \param maxVelocity The maximum horizontal velocity of this entity
/// \param jumped Optional value set to weather or not the entity successfully jumped
void sbProcCharacterPhysics(JamWorld *world, JamEntity *self, bool jump, double movement, bool friction,
							double acceleration, double jumpSpeed, double maxVelocity, bool *jumped);

/// \brief Processes an entity's animations
/// \param world World the entity is in
/// \param self Entity to be processed
/// \param walking Sprite it uses when walking
/// \param standing Sprite it uses when standing
/// \param jumping Sprite it uses while mid-air
/// \param stopRot Weather or not to set rotation to 0 while on the ground
void sbProcCharacterAnimations(JamWorld *world, JamEntity *self, JamSprite *walking, JamSprite *standing,
							   JamSprite *jumping, bool stopRot);

/// \brief Processes an entity's collisions with the game world
/// \param world World the entity is in
/// \param self Entity to process
/// \param horizontal Optional that is set to true if a horizontal collision happens
/// \param vertical Optional that is set to true if a vertical collision happens
///
/// You can leave horizontal and vertical as NULL if you don't need to know when
/// collisions happen.
void sbProcCharacterCollisions(JamWorld *world, JamEntity *self, bool *horizontal, bool *vertical);

/// \brief Moves an entity the amount they are expected to
/// \param world World the entity is in
/// \param self Entity that is to be processed
void sbProcCharacterMovement(JamWorld *world, JamEntity *self);

//////////////////// Characters ////////////////////

// Common - every character needs to either have these as their behaviour or
// call them in their behaviour functions (ideally as the first thing called
// in the create function and the last thing called in the destroy function).
void onCharacterCreate(JamWorld* world, JamEntity* self);
void onCharacterDestroy(JamWorld* world, JamEntity* self);

// Player
void onPlayerCreate(JamWorld* world, JamEntity* self);
void onPlayerDestroy(JamWorld* world, JamEntity* self);
void onPlayerFrame(JamWorld* world, JamEntity* self);
void onPlayerDraw(JamWorld* world, JamEntity* self);

// Skellymen
void onSkellyManCreate(JamWorld* world, JamEntity* self);
void onSkellyManDestroy(JamWorld* world, JamEntity* self);
void onSkellyManFrame(JamWorld* world, JamEntity* self);
void onSkellyManDraw(JamWorld* world, JamEntity* self);
