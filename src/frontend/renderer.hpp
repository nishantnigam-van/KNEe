#ifndef RENDERER_H
#define RENDERER_H

#include <memory>
#include <queue>

#include "event.hpp"
#include "model.hpp"

class Renderer {
  public:
    Renderer(const Model &m);
    ~Renderer();

    std::queue<Event>& event_queue();
    void render();

  private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

#endif
