#pragma once

#include "event.hpp"
#include "model.hpp"

void create_default(Model &model);
void handle_action(Model &model, const Event &event);
