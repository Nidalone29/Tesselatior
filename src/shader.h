#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <vector>
#include <filesystem>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "texture.h"

// this actually rappresents a shader program (that can have multiple shaders
// files maybe)
class Shader {
 public:
  Shader();
  ~Shader();

  /**
   * @brief loads a shader given the source file
   *
   * @param type Vertex or Fragment shader
   * @param path path to the source code of the shader
   */
  void AddShader(const GLenum type, const std::filesystem::path& path);

  /**
   * @brief compiling and linking all the added shaders to a program
   *
   */
  void Init();

  /**
   * @brief enabling the program
   *
   */
  void Enable() const;
  void Disable() const;

  GLuint program_id() const;

  void SetUniformMat4(const std::string& uniform_name,
                      const glm::mat4& matrix) const;
  void SetUniformFloat(const std::string& uniform_name,
                       const float value) const;
  void SetUniformVec3(const std::string& uniform_name,
                      const glm::vec3& vec) const;
  void SetUniformVec4(const std::string& uniform_name,
                      const glm::vec4& vec) const;
  void SetUnifromSampler(const std::string& uniform_name,
                         const TEXTURE_TYPE id) const;

 private:
  static GLuint CompileShader(const GLenum type, const std::string& src);
  GLint GetUniformLocation(const std::string& uniform_name) const;

  struct ShaderSource {
    GLenum type;
    std::string source;
  };

  std::vector<ShaderSource> shaders_;
  std::vector<GLuint> compiled_shaders_;

  GLuint program_;
};

#endif  // SHADER_H
