#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include "event.hpp"
#include "model.hpp"

void create_default(Model &model);
void handle_action(Model &model, const Event &event);

#endif
