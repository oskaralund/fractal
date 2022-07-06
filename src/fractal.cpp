#include "fractal.hpp"

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


Fractal::Fractal() {
  CreateWindow();
  CreateFractalRect();
  shader_ = std::make_unique<Shader>(
      "shaders/default.vert", "shaders/default.frag");
  shader_->SetUniform("fractal_center", {0.0, 0.0});
  shader_->SetUniform("fractal_width", 2.0*aspect_ratio());
  shader_->SetUniform("fractal_height", 2.0);
  shader_->SetUniform("max_iter", 50);
  glfwSetWindowUserPointer(window_, this);
  glfwSetCursorPosCallback(window_, &cursor_pos_callback);
  glfwSetWindowSizeCallback(window_, &window_size_callback);
  glfwSetScrollCallback(window_, &scroll_callback);
  glfwSetKeyCallback(window_, &key_callback);
  glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  time_ = glfwGetTime();
}


Fractal::~Fractal() {
  glfwTerminate();
}


void Fractal::Render() {
  double dt = glfwGetTime() - time_;
  time_ = glfwGetTime();

  zoom_momentum_ *= glm::exp(-10*dt);
  shader_->SetUniform(
      "fractal_height",
      fractal_height()*glm::exp(-zoom_momentum_*dt));
  shader_->SetUniform("fractal_width", fractal_height()*aspect_ratio());

  scroll_momentum_ *= glm::exp(-10*dt);
  shader_->SetUniform(
      "fractal_center", fractal_center() + dt*scroll_momentum_);

  shader_->Use();
  glBindVertexArray(fractal_vao_);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  glfwSwapBuffers(window_);
  UpdateCursorPos();
}


void Fractal::HandleInput() {
  glfwPollEvents();
}


bool Fractal::ShouldClose() const {
  return glfwWindowShouldClose(window_);
}


void Fractal::CreateWindow() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window_ = glfwCreateWindow(600, 600, "fractal", NULL, NULL);
  if (window_ == NULL)
  {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return;
  }

  glfwMakeContextCurrent(window_);


  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return;
  }


  glViewport(0, 0, 600, 600);
}


void Fractal::CreateFractalRect() {
  float vertices[] = {
     1.0f,  1.0f, 0.0f,
     1.0f, -1.0f, 0.0f,
    -1.0f, -1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f
  };
  unsigned int indices[] = {
    0, 1, 3,
    1, 2, 3
  };

  unsigned int EBO;
  glGenBuffers(1, &EBO);

  unsigned int VBO;
  glGenBuffers(1, &VBO);

  glGenVertexArrays(1, &fractal_vao_);

  glBindVertexArray(fractal_vao_);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(
      GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


  // Texture
  int width, height, nchannels;
  unsigned char *data = stbi_load(
      "textures/pal.png", &width, &height, &nchannels, 0);
  unsigned int texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_1D, texture);
  glTexImage1D(
      GL_TEXTURE_1D, 0, GL_RGB, width, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_1D);
  stbi_image_free(data);
}


void Fractal::UpdateCursorPos() {
  double xpos, ypos;
  glfwGetCursorPos(window_, &xpos, &ypos);
  cursor_pixel_pos_ = {xpos, ypos};
  cursor_world_pos_ = PixelToWorld({xpos, ypos});
}


double Fractal::fractal_width() const {
  const auto loc = glGetUniformLocation(shader_->id, "fractal_width");
  double w;
  glGetUniformdv(shader_->id, loc, &w);
  return w;
}


double Fractal::fractal_height() const {
  const auto loc = glGetUniformLocation(shader_->id, "fractal_height");
  double h;
  glGetUniformdv(shader_->id, loc, &h);
  return h;
}


glm::dvec2 Fractal::fractal_center() const {
  const auto loc = glGetUniformLocation(shader_->id, "fractal_center");
  glm::dvec2 c;
  glGetUniformdv(shader_->id, loc, &c.x);
  return c;
}


glm::dvec2 Fractal::window_size() const {
  int w, h;
  glfwGetWindowSize(window_, &w, &h);
  return {w, h};
}


double Fractal::aspect_ratio() const {
  const auto size = window_size();
  return size.x/size.y;
}


int Fractal::max_iter() const {
  const auto loc = glGetUniformLocation(shader_->id, "max_iter");
  int max_iter;
  glGetUniformiv(shader_->id, loc, &max_iter);
  return max_iter;
}


glm::dvec2 Fractal::PixelToWorld(glm::dvec2 p) const {
  int window_width, window_height;
  glfwGetWindowSize(window_, &window_width, &window_height);
  double px = p.x/window_width;
  double py = p.y/window_height;
  return {fractal_center().x + (0.5-px)*fractal_width(),
          fractal_center().y + (0.5-py)*fractal_height()};
}


void Fractal::CursorPosCallback(double x, double y) {
  glm::dvec2 new_pixel_pos{x, y};
  glm::dvec2 new_world_pos = PixelToWorld(new_pixel_pos);
  glm::dvec2 world_delta = new_world_pos - cursor_world_pos_;
  scroll_momentum_ += 10.0*world_delta;
  UpdateCursorPos();
}


void Fractal::WindowSizeCallback(int width, int height) {
  shader_->SetUniform("window_width", width);
  shader_->SetUniform("window_height", height);
  glViewport(0, 0, width, height);
  shader_->SetUniform("fractal_width", fractal_height()*aspect_ratio());
}


void Fractal::ScrollCallback(double xoffset, double yoffset) {

  if (yoffset > 0) {
    zoom_momentum_ += 1;
  }
  else {
    zoom_momentum_ -= 1;
  }
}


void Fractal::KeyCallback(int key, int scancode, int action, int mods) {
  if (action == GLFW_PRESS) {
    switch (key) {
      case GLFW_KEY_A:
        shader_->SetUniform("max_iter", max_iter()+10);
        break;
      case GLFW_KEY_B:
        if (max_iter() > 10) {
          shader_->SetUniform("max_iter", max_iter()-10);
        }
        break;
    }
  }
}


static void cursor_pos_callback(GLFWwindow* w, double x, double y) {
  Fractal* fractal = static_cast<Fractal*>(glfwGetWindowUserPointer(w));
  fractal->CursorPosCallback(x, y);
}


void window_size_callback(GLFWwindow* w, int width, int height) {
  Fractal* fractal = static_cast<Fractal*>(glfwGetWindowUserPointer(w));
  fractal->WindowSizeCallback(width, height);
}


void scroll_callback(GLFWwindow* w, double xoffset, double yoffset) {
  Fractal* fractal = static_cast<Fractal*>(glfwGetWindowUserPointer(w));
  fractal->ScrollCallback(xoffset, yoffset);
}


void key_callback(GLFWwindow* w, int key, int scancode, int action, int mods) {
  Fractal* fractal = static_cast<Fractal*>(glfwGetWindowUserPointer(w));
  fractal->KeyCallback(key, scancode, action, mods);
}
