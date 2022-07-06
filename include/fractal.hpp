#ifndef FRACTAL_HPP_
#define FRACTAL_HPP_

#include <memory>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shader.hpp"


class Fractal {
 public:
  Fractal();
  ~Fractal();

  void Render();
  void HandleInput();
  bool ShouldClose() const;
  void CursorPosCallback(double, double);
  void WindowSizeCallback(int, int);
  void ScrollCallback(double, double);
  void KeyCallback(int, int, int, int);
  void SetMaxIterations(int);

 private:
  void CreateWindow();
  void CreateFractalRect();
  void SetWindowWidth(int);
  void SetWindowHeight(int);
  void UpdateCursorPos();
  double fractal_width() const;
  double fractal_height() const;
  glm::dvec2 fractal_center() const;
  glm::dvec2 window_size() const;
  double aspect_ratio() const;
  int max_iter() const;
  glm::dvec2 PixelToWorld(glm::dvec2) const;

  GLFWwindow* window_;
  unsigned int fractal_vao_;
  std::unique_ptr<Shader> shader_;
  double zoom_momentum_ = 0.0;
  glm::dvec2 scroll_momentum_{0.0, 0.0};
  double time_;
  glm::dvec2 cursor_world_pos_;
  glm::dvec2 cursor_pixel_pos_;
};

static void cursor_pos_callback(GLFWwindow*, double, double);
void window_size_callback(GLFWwindow*, int, int);
void scroll_callback(GLFWwindow*, double, double);
void key_callback(GLFWwindow*, int, int, int, int);


#endif
