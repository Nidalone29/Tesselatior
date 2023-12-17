#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <vector>
#include <filesystem>

#include <GL/glew.h>
#include <glm/glm.hpp>

// this actually rappresents a shader program (that can have multiple shaders
// files maybe)
class Shader {
 public:
  Shader();

  /**
   * @brief loads a shader given the source file
   *
   * @param type Vertex or Fragment shader
   * @param path path to the source code of the shader
   */
  void addShader(const GLenum type, const std::filesystem::path& path);

  /**
   * @brief compiling and linking all the added shaders to a program
   *
   */
  void init();

  /**
   * @brief enabling the program
   *
   */
  void enable() const;
  void disable() const;

  // TODO deal with uniforms, and change the old code in app
  // and after this i should have "finished"
  void setUniformMat4(const std::string uniform_name,
                      const glm::mat4& matrix) const;
  void setUniformFloat(const std::string uniform_name, const float value) const;
  void setUniformVec3(const std::string uniform_name,
                      const glm::vec3& vec) const;
  void setUnifromSampler(const std::string uniform_name, int id) const;

 private:
  static GLuint compileShader(const GLenum type, const std::string& src);
  GLint getUniformLocation(const std::string& uniform_name) const;

  struct ShaderSource {
    GLenum type;
    std::string source;
  };

  std::vector<ShaderSource> _shaders;

  GLuint _program;

  // bool enabled; successfully enabled? for uniforms?
};

#endif  // SHADER_H