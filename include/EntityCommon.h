/// \file EntityCommon.h
/// \author plo

#pragma once
#include <JamEngine.h>

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
void sbProcessPhysics(JamWorld* world, JamEntity* self, bool jump, double movement, bool friction, double acceleration, double jumpSpeed, double maxVelocity, bool *jumped);

/// \brief Processes an entity's animations
/// \param world World the entity is in
/// \param self Entity to be processed
/// \param walking Sprite it uses when walking
/// \param standing Sprite it uses when standing
/// \param jumping Sprite it uses while mid-air
/// \param stopRot Weather or not to set rotation to 0 while on the ground
void sbProcessAnimations(JamWorld* world, JamEntity* self, JamSprite* walking, JamSprite* standing, JamSprite* jumping, bool stopRot);

/// \brief Processes an entity's collisions with the game world
/// \param world World the entity is in
/// \param self Entity to process
/// \param horizontal Optional that is set to true if a horizontal collision happens
/// \param vertical Optional that is set to true if a vertical collision happens
///
/// You can leave horizontal and vertical as NULL if you don't need to know when
/// collisions happen.
void sbProcessCollisions(JamWorld* world, JamEntity* self, bool *horizontal, bool *vertical);

/// \brief Moves an entity the amount they are expected to
/// \param world World the entity is in
/// \param self Entity that is to be processed
void sbProcessMovement(JamWorld* world, JamEntity* self);