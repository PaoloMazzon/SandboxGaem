//
// Created by plo on 4/4/20.
//

#include <Message.h>
#include <malloc.h>
#include <math.h>
#include <JamEngine.h>
#include <SandConstants.h>
#include <Sprite.h>

typedef struct {
	const char* message; // Content of the message
	const char* name;    // Name of the speaker
	int id;              // ID of the entity speaking it or NULL
} _SBMessageContent;

typedef struct {
	_SBMessageContent queue[DISPLAY_QUEUE_SIZE]; // Queue of messages
	int currentMessage;                          // Current message (wraps around)
	int queueEnd;                                // Last message in the queue
} _SBMessageQueue;

static _SBMessageQueue gMessageQueue;
static JamTexture* gMessageTexture; // The message texture we will grab from the handler
static JamFont* gGameFont; // Font we will grab from the handler
static JamTexture* gDialogueBubble; // Another texture from the handler
extern JamAssetHandler* gGameData;
extern JamControlMap* gControlMap;

//////////////////////////////////////////////////////////////////////////////////////////////
void sbStartMesssageQueue() {
	if (gMessageTexture == NULL) {
		gMessageTexture = jamAssetHandlerGetTexture(gGameData, "TextboxTexture");
		gGameFont = jamAssetHandlerGetFont(gGameData, "GameFont");
		gDialogueBubble = jamAssetHandlerGetTexture(gGameData, "SpeechBubbleTexture");
	}

	gMessageQueue.queueEnd = 0;
	gMessageQueue.currentMessage = 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////
bool sbMessageActive() {
	return gMessageQueue.queueEnd != 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////
void sbQueueMessage(const char* name, const char* message, int id) {
	gMessageQueue.queue[gMessageQueue.queueEnd].name = name;
	gMessageQueue.queue[gMessageQueue.queueEnd].id = id;
	gMessageQueue.queue[gMessageQueue.queueEnd].message = message;
	gMessageQueue.queueEnd++;
}
//////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////
void sbDrawMessage(JamWorld* world) {
	sint32 texX, texY;
	JamEntity* speaker;
	if (gMessageQueue.queueEnd > 0) {
		texX = (sint32)round(jamRendererGetCameraX()) + (GAME_WIDTH - gMessageTexture->w) / 2;
		texY = (sint32)round(jamRendererGetCameraY()) + (GAME_HEIGHT - gMessageTexture->h) - 4;

		// The textbox
		jamDrawTexture(gMessageTexture, texX, texY);

		// Name of the speaker
		jamFontRender(
				gGameFont,
				texX + 1,
				texY + 4,
				gMessageQueue.queue[gMessageQueue.currentMessage].name
		);

		// Content of the message
		jamFontRenderExt(
				gGameFont,
				texX + 5,
				texY + 22,
				gMessageQueue.queue[gMessageQueue.currentMessage].message,
				gMessageTexture->w - 8
		);

		// Speech bubble
		if (gMessageQueue.queue[gMessageQueue.currentMessage].id != ID_NOT_ASSIGNED) {
			speaker = jamWorldFindEntity(world, gMessageQueue.queue[gMessageQueue.currentMessage].id);
			if (speaker != NULL && speaker->sprite != NULL) {
				jamDrawTexture(
						gDialogueBubble,
						(sint32)round(speaker->x - speaker->sprite->originX),
						(sint32)round(speaker->y - speaker->sprite->originY - gDialogueBubble->h)
				);
			}
		}

		// Manage queue
		if (jamControlMapCheck(gControlMap, "message")) {
			gMessageQueue.currentMessage++;
			if (gMessageQueue.queueEnd == gMessageQueue.currentMessage)
				gMessageQueue.queueEnd = 0;
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////