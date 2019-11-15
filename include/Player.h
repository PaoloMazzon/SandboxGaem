#include <JamEngine.h>

typedef struct {
	int hp;
} PlayerData;

void onPlayerCreate(JamWorld* world, JamEntity* self);
void onPlayerDestroy(JamWorld* world, JamEntity* self);
void onPlayerFrame(JamWorld* world, JamEntity* self);
void onPlayerDraw(JamWorld* world, JamEntity* self);
