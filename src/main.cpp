#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "fractal.hpp"


int main() {
  Fractal fractal;

  while (!fractal.ShouldClose()) {
    fractal.HandleInput();
    fractal.Render();
  }

  return 0;
}
