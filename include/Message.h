/// \file ${HEADER_FILENAME}
/// \author plo
/// \brief 

#pragma once
#include <JamEngine.h>

/// \brief Starts a message queue, overwriting any previous ones
void sbStartMesssageQueue();

/// \brief Returns weather or not there is a message active
bool sbMessageActive();

/// \brief Queues a message to the message
/// \param name Name of the speaker
/// \param message Message content
/// \param id ID of the entity speaking or ID_NOT_ASSIGNED
void sbQueueMessage(const char* name, const char* message, int id);

/// \brief Draws the current message as well as updates it
void sbDrawMessage(JamWorld* world);