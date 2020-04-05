//
// Created by plo on 4/4/20.
//

#include <Message.h>
#include <malloc.h>
#include <JamEngine.h>
#include <SandConstants.h>

typedef struct {
	const char* message; // Content of the message
	const char* name;    // Name of the speaker
	int id;              // ID of the entity speaking it or NULL
} _SBMessageContent;

typedef struct {
	_SBMessageContent queue[DISPLAY_QUEUE_SIZE]; // Queue of messages
	int currentMessage;                          // Current message (wraps around)
	int currentCharacter;                        // Current character in the message
	int queueEnd;                                // Last message in the queue
} _SBMessageQueue;

static _SBMessageQueue gMessageQueue;

//////////////////////////////////////////////////////////////////////////////////////////////
void sbStartMesssageQueue() {
	gMessageQueue.queueEnd = 0;
	gMessageQueue.currentMessage = 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////
bool sbMessageActive() {
	return gMessageQueue.queueEnd == gMessageQueue.currentMessage;
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

}
//////////////////////////////////////////////////////////////////////////////////////////////