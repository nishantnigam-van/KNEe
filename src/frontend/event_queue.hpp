#ifndef EVENT_QUEUE_H
#define EVENT_QUEUE_H

#include <memory>
#include <queue>

#include "event.hpp"

using EventQueue = std::shared_ptr<std::queue<Event>>;

#endif
