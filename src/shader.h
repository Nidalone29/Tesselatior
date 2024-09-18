#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <vector>
#include <filesystem>
#include <unordered_map>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "logger.h"
#include "texture.h"

// this actually represents a shader program (that can have multiple shaders
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
  void AddShaderFile(const GLenum type, const std::filesystem::path& path);

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

// singleton class that handles shaders
// owns all the shaders it has
class ShaderManager {
 public:
  ShaderManager(const ShaderManager&) = delete;
  ShaderManager& operator=(const ShaderManager&) = delete;
  ShaderManager(ShaderManager&&) = delete;
  ShaderManager& operator=(ShaderManager&&) = delete;
  ~ShaderManager();

  static ShaderManager& Instance();

  Shader* GetShader(const std::string& shader_id);

  void AddShaders(
      std::initializer_list<std::pair<std::string, Shader*>> in_shaders);

 private:
  ShaderManager() = default;
  // ShaderID to Shader
  std::unordered_map<std::string, Shader*> shaders_;
};

#endif  // SHADER_H
