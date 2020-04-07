// Constants used in gaem

#pragma once

// Display constants
#define GAME_WIDTH 240
#define GAME_HEIGHT 160
#define DEFAULT_GAME_SCALE 3
#define WORLD_BACKGROUND_LAYER 0
#define WORLD_FOREGROUND_LAYER 1
#define WORLD_WALL_LAYER 2
#define WORLD_FOREFRONT_LAYER 3
#define JUMP_ROT_SPEED 15
#define BLOCK_SIZE 8
#define DISPLAY_QUEUE_SIZE 20
#define NARRATOR_NAME "Narrator"
#define FONT_CHARACTER_WIDTH 8
#define FONT_CHARACTER_HEIGHT 8

// Entity type checkers
#define IS_TYPE_CHARACTER(t) ((t) == TYPE_PLAYER || (t) > 999)
#define IS_TYPE_LOGIC(t) ((t) > 0 && (t) < 1000 && (t) != TYPE_PLAYER)
#define IS_TYPE_ENEMY(t) ((t) >= 1000 && (t) < 2000)
#define IS_TYPE_NPC(t) ((t) >= 2000 && (t) < 3000)

// Entity types of individual things
#define TYPE_UNKNOWN 0
#define TYPE_PLAYER 1
#define TYPE_SKELLYMAN 2000

// How many frames (in delta time) should the flicker last
#define FLICKER_FRAMES 90

//////// Common Physics ////////
#define FAST_SPEED 999
#define KNOCKBACK_VELOCITY 5
#define GRAVITY_ACCELERATION 0.50
#define COEFFICIENT_OF_FRICTION 0.35
#define AIR_VELOCITY_CONTROL 0.20
#define COMICAL_GRAVITY_ACCELERATION 0.1

// Player physics, anytime velocity is mentioned it implies initial velocity
#define MAXIMUM_PLAYER_WALK_VELOCITY 2.5
#define MAXIMUM_PLAYER_RUN_VELOCITY 3.5
#define PLAYER_ACCELERATION 0.20
#define PLAYER_JUMP_VELOCITY (-5.5)
#define ROLL_SPEED 3
#define ROLL_COOLDOWN 20
#define ROLL_DURATION 15

////////// Enemy physics //////////
#define ENEMY_FADE_OUT_TIME (60 * 1)
#define ENEMY_DEATH_ROT_SPEED 14
#define ENEMY_KNOCKBACK_VELOCITY 2

// Skellymen
#define SKELLYMAN_MAX_SPEED 0.5
#define SKELLYMAN_ACCELERATION 0.05
#define SKELLYMAN_PAUSE_INTERVAL (60 * 3)
#define SKELLYMAN_WALK_INTERVAL (60 * 2)