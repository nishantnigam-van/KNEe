#ifndef RENDERER_H
#define RENDERER_H

#include <memory>

#include "model.hpp"
#include "event_queue.hpp"

class Renderer {
  public:
    Renderer(const Model &m, EventQueue event_queue);
    ~Renderer();

    auto render() -> void;

  private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

#endif
