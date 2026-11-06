#ifndef RENDERER_H
#define RENDERER_H

#include <memory>

#include "model.hpp"

class Renderer {
  public:
    Renderer(const Model &m);
    ~Renderer();

    void render();

  private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

#endif
