#ifndef FRACTAL_HPP_
#define FRACTAL_HPP_

#include <memory>
#include <vector>
#include <map>

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
  void MouseButtonCallback(int, int, int);
  void WindowSizeCallback(int, int);
  void ScrollCallback(double, double);
  void KeyCallback(int, int, int, int);

 private:
  void CreateWindow();
  void CreateFractalRect();
  void LoadTextures();
  void LoadShaders();
  double fractal_width() const;
  double fractal_height() const;
  glm::dvec2 fractal_center() const;
  glm::dvec2 window_size() const;
  glm::dvec2 cursor_pos() const;
  double aspect_ratio() const;
  int max_iter() const;
  glm::dvec2 PixelToWorld(glm::dvec2) const;
  glm::dvec2 PixelToWorldDelta(glm::dvec2) const;

  GLFWwindow* window_;
  unsigned int fractal_vao_;
  Shader* shader_;
  std::map<std::string, std::unique_ptr<Shader>> shaders_;
  double zoom_momentum_ = 0.0;
  glm::dvec2 scroll_momentum_{0.0, 0.0};
  bool mouse_pressed_ = false;
  bool zoom_key_held_ = false;
  double time_;
  glm::dvec2 cursor_world_pos_;
  glm::dvec2 cursor_pixel_pos_;
  bool should_close_ = false;
};

static void cursor_pos_callback(GLFWwindow*, double, double);
void mouse_button_callback(GLFWwindow* w, int, int, int);
void window_size_callback(GLFWwindow*, int, int);
void scroll_callback(GLFWwindow*, double, double);
void key_callback(GLFWwindow*, int, int, int, int);


#endif
