/// \file ${HEADER_FILENAME}
/// \author plo
/// \brief 

#pragma once
#include <JamEngine.h>

/// \brief Queues a message to the message
/// \param name Name of the speaker
/// \param message Message content
/// \param id ID of the entity speaking or NULL
void sbQueueMessage(const char* name, const char* message, int id);

/// \brief Draws the current message as well as updates it
void sbDrawMessage(JamWorld* world);