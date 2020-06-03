//
// Created by plo on 4/4/20.
//

#include <Message.h>
#include <malloc.h>
#include <math.h>
#include <JamEngine.h>
#include <string.h>
#include <SandConstants.h>
#include <Sprite.h>
#include <memory.h>

typedef struct {
	JamTexture* content; // Content of the message in a texture
	const char* name;    // Name of the speaker
	int id;              // ID of the entity speaking it or NULL
	int height;          // Height in pixels of the actual content
} _SBMessageContent;

typedef struct {
	_SBMessageContent queue[DISPLAY_QUEUE_SIZE]; // Queue of messages
	int currentMessage;                          // Current message (wraps around)
	int queueEnd;                                // Last message in the queue
	int scroll;                                  // How far down in pixels we are in the content texture
} _SBMessageQueue;

static _SBMessageQueue gMessageQueue;
static JamTexture* gMessageTexture; // The message texture we will grab from the handler
static JamFont* gGameFont; // Font we will grab from the handler
static JamTexture* gDialogueBubble; // Another texture from the handler
extern JamAssetHandler* gGameData;
extern JamControlMap* gControlMap;

// Returns the number of characters in the current word *
static int _widthOfNextWord(char* string) {
	int pos = 0;
	int size = 0;
	uint32 c = jamStringNextUnicode(string, &pos);

	while (c != 0 && c != 32 && c != '\n') {
		size += FONT_CHARACTER_WIDTH;
		c = jamStringNextUnicode(string, &pos);
	}

	return size;
}

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
	if (name != NULL && message != NULL) {
		int i = 0;
		int x = 0;
		int y = 0;
		int h = 0;
		int pos = 0;
		uint32 c = jamStringNextUnicode(message, &pos);
		int len = (int) strlen(message);

		gMessageQueue.queue[gMessageQueue.queueEnd].name = name;
		gMessageQueue.queue[gMessageQueue.queueEnd].id = id;

		// I have arbitrarily decided that the texture is 180 pixels tall (up to 20 lines of text)
		gMessageQueue.queue[gMessageQueue.queueEnd].content = jamTextureCreate(gMessageTexture->w - 8, 180);

		jamRendererSetTarget(gMessageQueue.queue[gMessageQueue.queueEnd].content);
		while (c != 0) {
			if (c == '\n' || _widthOfNextWord((char *) (message + i)) + x > gMessageTexture->w - 8) {
				y += FONT_CHARACTER_HEIGHT + 1;
				x = 0;
			}
			jamFontRenderCharacter(gGameFont, x, y, c, 255, 255, 255);
			x += c != '\n' ? FONT_CHARACTER_WIDTH : 0;
			c = jamStringNextUnicode(message, &pos);
			i++;
		}
		jamRendererSetTarget(NULL);

		gMessageQueue.queue[gMessageQueue.queueEnd].height = y + FONT_CHARACTER_HEIGHT;

		gMessageQueue.queueEnd++;
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////
void sbDrawMessage(JamWorld* world) {
	sint32 texX, texY;
	JamEntity* speaker;
	if (gMessageQueue.queueEnd > 0) {
		texX = (sint32)round(jamRendererGetCameraX()) + (GAME_WIDTH - gMessageTexture->w) / 2;
		texY = (sint32)round(jamRendererGetCameraY()) + (GAME_HEIGHT - gMessageTexture->h) - 2;

		// The textbox
		jamDrawTexture(gMessageTexture, texX, texY);

		// Name of the speaker
		jamFontRenderColour(
				gGameFont,
				texX + 1,
				texY + 2,
				gMessageQueue.queue[gMessageQueue.currentMessage].name,
				0x14, 0x07, 0x52
		);

		// Content of the message
		jamDrawTexturePart(
				gMessageQueue.queue[gMessageQueue.currentMessage].content,
				texX + 5,
				texY + 14,
				0,
				gMessageQueue.scroll,
				gMessageQueue.queue[gMessageQueue.currentMessage].content->w,
				9 * 5
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
		gMessageQueue.scroll += jamControlMapCheck(gControlMap, "message_scroll");
		gMessageQueue.scroll = (int)clamp(gMessageQueue.scroll, 0, gMessageQueue.queue[gMessageQueue.currentMessage].height - 9 * 5 + 1);
		if (jamControlMapCheck(gControlMap, "message")) {
			jamTextureFree(gMessageQueue.queue->content);
			gMessageQueue.queue->content = NULL;
			gMessageQueue.currentMessage++;
			gMessageQueue.scroll = 0;
			if (gMessageQueue.queueEnd == gMessageQueue.currentMessage) {
				gMessageQueue.queueEnd = 0;
				gMessageQueue.currentMessage = 0;
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////