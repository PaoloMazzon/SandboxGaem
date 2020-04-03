// Constants used in gaem

#pragma once

// Display constants
#define GAME_WIDTH 240
#define GAME_HEIGHT 160
#define DEFAULT_GAME_SCALE 1
#define WORLD_BACKGROUND_LAYER 0
#define WORLD_FOREGROUND_LAYER 1
#define WORLD_WALL_LAYER 2
#define WORLD_FOREFRONT_LAYER 3
#define JUMP_ROT_SPEED 15

// Entity types
#define TYPE_PLAYER 1
#define TYPE_ENEMY 2

// How many frames (in delta time) should the flicker last
#define FLICKER_FRAMES 90

// Player physics, anytime velocity is mentioned it implies initial velocity
#define MAXIMUM_PLAYER_WALK_VELOCITY 2.5
#define MAXIMUM_PLAYER_RUN_VELOCITY 3.5
#define COEFFICIENT_OF_FRICTION 0.35
#define PLAYER_ACCELERATION 0.20
#define GRAVITY_ACCELERATION 0.10
#define KNOCKBACK_VELOCITY 5
#define JUMP_VELOCITY (-5.5)