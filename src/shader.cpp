#include "shader.hpp"

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>


Shader::Shader(const char* vertex_path, const char* fragment_path) {
  std::string vertex_code;
  std::string fragment_code;
  std::ifstream vshader_file;
  std::ifstream fshader_file;
  vshader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  fshader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

  try {
    vshader_file.open(vertex_path);
    fshader_file.open(fragment_path);
    std::stringstream vshader_stream;
    std::stringstream fshader_stream;
    vshader_stream << vshader_file.rdbuf();
    fshader_stream << fshader_file.rdbuf();
    vshader_file.close();
    fshader_file.close();
    vertex_code = vshader_stream.str();
    fragment_code = fshader_stream.str();
  }
  catch (std::ifstream::failure e) {
    std::cout << "ERROR:SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
  }

  const char* vshader_code = vertex_code.c_str();
  const char* fshader_code = fragment_code.c_str();

  unsigned int vertex, fragment;
  int success;
  char info_log[512];

  vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, &vshader_code, NULL);
  glCompileShader(vertex);
  glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertex, 512, NULL, info_log);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
              << info_log << std::endl;
  }

  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &fshader_code, NULL);
  glCompileShader(fragment);
  glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragment, 512, NULL, info_log);
    std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
              << info_log << std::endl;
  }

  id = glCreateProgram();
  glAttachShader(id, vertex);
  glAttachShader(id, fragment);
  glLinkProgram(id);

  glGetProgramiv(id, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(id, 512, NULL, info_log);
    std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
              << info_log << std::endl;
  }

  glDeleteShader(vertex);
  glDeleteShader(fragment);
}


void Shader::Use() const {
  glUseProgram(id);
}


void Shader::SetUniform(std::string name, double v) const {
  glUniform1d(glGetUniformLocation(id, name.c_str()), v); 
}


void Shader::SetUniform(std::string name, glm::dvec2 v) const {
  glUniform2d(glGetUniformLocation(id, name.c_str()), v.x, v.y); 
}


void Shader::SetUniform(std::string name, int v) const {
  glUniform1i(glGetUniformLocation(id, name.c_str()), v); 
}
