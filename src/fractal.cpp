#include "fractal.hpp"

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


Fractal::Fractal() {
  CreateWindow();
  CreateFractalRect();
  LoadTextures();
  LoadShaders();
  shader_ = shaders_["mandelbrot"].get();

  time_ = glfwGetTime();
}


Fractal::~Fractal() {
  glfwTerminate();
}


void Fractal::Render() {
  double dt = glfwGetTime() - time_;
  time_ = glfwGetTime();

  // Zoom
  zoom_momentum_ *= glm::exp(-10*dt);
  shader_->Use();
  shader_->SetUniform(
      "fractal_height",
      fractal_height()*glm::exp(-zoom_momentum_*dt));
  shader_->SetUniform("fractal_width", fractal_height()*aspect_ratio());
  const auto dir = PixelToWorld(cursor_pos()) - fractal_center();
  shader_->SetUniform(
      "fractal_center",
      fractal_center() + zoom_momentum_*dt*dir);

  // Scroll
  scroll_momentum_ *= glm::exp(-5*dt);
  shader_->SetUniform(
      "fractal_center", fractal_center() + dt*scroll_momentum_);

  // Draw
  glBindVertexArray(fractal_vao_);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  glfwSwapBuffers(window_);
}


void Fractal::HandleInput() {
  glfwPollEvents();
}


bool Fractal::ShouldClose() const {
  return glfwWindowShouldClose(window_) || should_close_;
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

  glfwSetWindowUserPointer(window_, this);
  glfwSetCursorPosCallback(window_, &cursor_pos_callback);
  glfwSetMouseButtonCallback(window_, &mouse_button_callback);
  glfwSetWindowSizeCallback(window_, &window_size_callback);
  glfwSetScrollCallback(window_, &scroll_callback);
  glfwSetKeyCallback(window_, &key_callback);
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
}


void Fractal::LoadTextures() {
  int width, height, nchannels;
  unsigned char* data;

  data = stbi_load(
      "textures/pal0.png", &width, &height, &nchannels, 0);
  unsigned int textures[2];
  glGenTextures(2, textures);
  glActiveTexture(GL_TEXTURE0 + 0);
  glBindTexture(GL_TEXTURE_1D, textures[0]);
  glTexImage1D(
      GL_TEXTURE_1D, 0, GL_RGB, width, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_1D);
  stbi_image_free(data);

  data = stbi_load(
      "textures/pal1.png", &width, &height, &nchannels, 0);
  glActiveTexture(GL_TEXTURE0 + 1);
  glBindTexture(GL_TEXTURE_1D, textures[1]);
  glTexImage1D(
      GL_TEXTURE_1D, 0, GL_RGB, width, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_1D);
  stbi_image_free(data);
}


void Fractal::LoadShaders() {
  std::vector<std::string> shader_names = {"mandelbrot", "newton"};

  for (const auto& name : shader_names) {
    const auto frag_path = "shaders/" + name + ".frag";
    shaders_[name] = std::make_unique<Shader>(
        "shaders/default.vert", frag_path.c_str());

    shaders_[name]->Use();
    shaders_[name]->SetUniform("window_width", window_size().x);
    shaders_[name]->SetUniform("window_height", window_size().y);
    shaders_[name]->SetUniform("fractal_center", {0.0, 0.0});
    shaders_[name]->SetUniform("fractal_width", 2.0*aspect_ratio());
    shaders_[name]->SetUniform("fractal_height", 2.0);
    shaders_[name]->SetUniform("max_iter", 50);
    shaders_[name]->SetUniform("pal0", 0);
    shaders_[name]->SetUniform("pal1", 1);
  }
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


glm::dvec2 Fractal::cursor_pos() const {
  double xpos, ypos;
  glfwGetCursorPos(window_, &xpos, &ypos);
  return {xpos, ypos};
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
  glm::dvec2 c = fractal_center();
  return {c.x + (px-0.5)*fractal_width(),
          c.y + (0.5-py)*fractal_height()};
}


glm::dvec2 Fractal::PixelToWorldDelta(glm::dvec2 p) const {
  int window_width, window_height;
  glfwGetWindowSize(window_, &window_width, &window_height);
  double px = p.x/window_width;
  double py = p.y/window_height;
  return {px*fractal_width(), -py*fractal_height()};
}


void Fractal::CursorPosCallback(double x, double y) {
  int mouse_left = glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_LEFT);

  glm::dvec2 new_pixel_pos{x, y};
  glm::dvec2 world_delta = PixelToWorldDelta(cursor_pixel_pos_ - new_pixel_pos);

  if (mouse_left == GLFW_PRESS) {
    shader_->SetUniform("fractal_center", fractal_center()+world_delta);
    scroll_momentum_ = {0, 0};
    mouse_pressed_ = true;
  }
  else if (mouse_pressed_ == true) {
    scroll_momentum_ = 100.0*world_delta;
    mouse_pressed_ = false;
  }

  cursor_pixel_pos_ = {x, y};
  cursor_world_pos_ = PixelToWorld({x, y});
}


void Fractal::MouseButtonCallback(int button, int action, int mods) {
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    scroll_momentum_ = {0, 0};
  }
}


void Fractal::WindowSizeCallback(int width, int height) {
  for (auto const& [name, shader] : shaders_) {
    shader->Use();
    shader->SetUniform("window_width", width);
    shader->SetUniform("window_height", height);
    shader->SetUniform("fractal_width", fractal_height()*aspect_ratio());
  }
  glViewport(0, 0, width, height);
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
      case GLFW_KEY_ESCAPE:
        should_close_ = true;
        break;
      case GLFW_KEY_1:
        shader_ = shaders_["mandelbrot"].get();
        scroll_momentum_ = {0, 0};
        zoom_momentum_ = 0;
        break;
      case GLFW_KEY_2:
        shader_ = shaders_["newton"].get();
        scroll_momentum_ = {0, 0};
        zoom_momentum_ = 0;
        break;
      case GLFW_KEY_J:
        shader_->SetUniform("max_iter", max_iter()+10);
        break;
      case GLFW_KEY_K:
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


void mouse_button_callback(GLFWwindow* w, int button, int action, int mods) {
  Fractal* fractal = static_cast<Fractal*>(glfwGetWindowUserPointer(w));
  fractal->MouseButtonCallback(button, action, mods);
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
