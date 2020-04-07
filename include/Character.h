#include <JamEngine.h>
#pragma once

/// \brief Data specific to characters in the game
typedef struct {
	// What the character is currently up to
	struct {
		double moveTime;     // Amount of time remaining in this walking bit
		double pauseTime;    // Amount of time remaining in this pause
		double movement;     // Which direction to move in
		bool rolling;        // Weather or not this character is rolling
		double rollCooldown; // Current cooldown on the roll
		double rollDuration; // Current duration of the roll
		double hp;           // Current hp
		double fadeOut;      // Death fade out timer
	} State;

	// Information about the rpg bits
	struct {
		double thorns;       // Damage this enemy deals to the player on contact
		double rollDamage;   // Damage dealt while rolling
		double rollSpeed;    // Speed of this character while rolling
		double rollDuration; // Duration of the roll in delta
		double rollCooldown; // Cooldown time between rolls in delta
		double maxHP;        // Max health of the character
		double airRes;       // Resistance to roll damage while mid-air
		double level;        // This character's level
	} Stats;

	// Information about this character
	struct {
		const char* passiveDialogue; // Dialogue triggered if the player talks to this character
		const char* name;            // Name of this character
	} Info;
} CharacterData;

//////////////////// Common Data ////////////////////

/// \brief Gets the specific character variable of an entity
#define sbCharData(ent, type, attr) (((CharacterData*)ent->data)->type.attr)

/// \brief Reduces an attribute from State by the delta
#define sbCharTick(ent, attr) (((CharacterData*)ent->data)->State.attr) -= jamRendererGetDelta()

/// \brief Processes an entity's death and returns weather or not to perform collisions
bool sbProcessCharacterDeath(JamWorld *world, JamEntity *enemy);

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
							double acceleration, double jumpSpeed, double maxVelocity, bool *jumped, bool roll);

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
