#include <JamEngine.h>
#pragma once

/// \brief Data specific to characters in the game
typedef struct {
	// What the character is currently up to
	struct {
		double moveTime;  // Amount of time remaining in this walking bit
		double pauseTime; // Amount of time remaining in this pause
		double movement;  // Which direction to move in
		bool rolling;     // Weather or not this character is rolling
		double fadeOut;   // Death fade out timer
	} State;

	// Information about this character
	struct {
		double thorns;       // Damage this enemy deals to the player on contact
		double rollDamage;   // Damage dealt while rolling
		double rollSpeed;    // Speed of this character while rolling
		double rollDuration; // Duration of the roll in delta
		double rollCooldown; // Cooldown time between rolls in delta
		double hp;           // Current health of the character
		double maxHP;        // Max health of the character
		double airRes;       // Resistance to roll damage while mid-air
	} Stats;
} CharacterData;

//////////////////// Common Data ////////////////////

/// \brief Checks weather or not an entity is rolling
bool sbGetCharacterRolling(JamEntity* ent);

/// \brief Gets an entity's thorns
double sbGetCharacterThorns(JamEntity* ent);

/// \brief Sets an entity's thorns
void sbSetCharacterThorns(JamEntity* ent, double thorns);

/// \brief Gets an entity's thorns
double sbGetCharacterRollDamage(JamEntity* ent);

/// \brief Sets an entity's thorns
void sbSetCharacterRollDamage(JamEntity* ent, double thorns);

/// \brief Gets an entity's rollSpeed
double sbGetCharacterRollSpeed(JamEntity* ent);

/// \brief Sets an entity's rollSpeed
void sbSetCharacterRollSpeed(JamEntity* ent, double rollSpeed);

/// \brief Gets an entity's rollDuration
double sbGetCharacterRollDuration(JamEntity* ent);

/// \brief Sets an entity's rollDuration
void sbSetCharacterRollDuration(JamEntity* ent, double rollDuration);

/// \brief Gets an entity's rollCooldown
double sbGetCharacterRollCooldown(JamEntity* ent);

/// \brief Sets an entity's rollCooldown
void sbSetCharacterRollCooldown(JamEntity* ent, double rollCooldown);

/// \brief Gets an entity's hp
double sbGetCharacterHp(JamEntity* ent);

/// \brief Sets an entity's hp
void sbSetCharacterHp(JamEntity* ent, double hp);

/// \brief Gets an entity's maxHP
double sbGetCharacterMaxHP(JamEntity* ent);

/// \brief Sets an entity's maxHP
void sbSetCharacterMaxHP(JamEntity* ent, double maxHP);

/// \brief Gets an entity's airRes
double sbGetCharacterAirRes(JamEntity* ent);

/// \brief Sets an entity's airRes
void sbSetCharacterAirRes(JamEntity* ent, double airRes);

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
