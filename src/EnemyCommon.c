//
// Created by plo on 4/4/20.
//
#include <Enemies.h>
#include <malloc.h>

EnemyData* sbCreateEnemyData(double power) {
	EnemyData* yes = calloc(1, sizeof(EnemyData));
	if (yes)
		yes->power = power;
	else
		jSetError(ERROR_ALLOC_FAILED, "Failed to create enemy data");
	return yes;
}