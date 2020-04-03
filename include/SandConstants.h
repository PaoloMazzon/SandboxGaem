// Constants used in gaem

#pragma once

// Display constants
#define GAME_WIDTH 240
#define GAME_HEIGHT 160
#define DEFAULT_GAME_SCALE 3

// Entity types
#define TYPE_PLAYER 1
#define TYPE_ENEMY 2

// How many frames (in delta time) should the flicker last
#define FLICKER_FRAMES 90

// Player physics, anytime velocity is mentioned it implies initial velocity
#define MAXIMUM_PLAYER_WALK_VELOCITY 3
#define MAXIMUM_PLAYER_RUN_VELOCITY 5
#define COEFFICIENT_OF_FRICTION 0.35
#define PLAYER_ACCELERATION 0.20
#define GRAVITY_ACCELERATION 0.10
#define KNOCKBACK_VELOCITY 5
#define JUMP_VELOCITY (-6)