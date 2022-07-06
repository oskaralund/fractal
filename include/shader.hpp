#ifndef SHADER_HPP_
#define SHADER_HPP_


#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>


class Shader {
 public:
  unsigned int id;

  Shader(const char* vertex_path, const char* fragment_path);
  void Use() const;
  void SetUniform(std::string, double) const;
  void SetUniform(std::string, glm::dvec2) const;
  void SetUniform(std::string, int) const;
};


#endif
