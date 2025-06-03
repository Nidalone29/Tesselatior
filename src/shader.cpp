#include "shader.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <glm/gtc/type_ptr.hpp>

#include "utilities.h"  // FileNotFoundException
#include "logger.h"

#define INVALID_UNIFORM_LOCATION 0xffffffff

Shader::Shader() {
  LOG_TRACE("Shader()");
}

Shader::~Shader() {
  for (const GLuint x : compiled_shaders_) {
    glDeleteShader(x);
  }

  if (program_ != 0) {
    glDeleteProgram(program_);
    program_ = 0;
  }

  LOG_TRACE("~Shader()");
}

void Shader::AddShaderFile(const GLenum type,
                           const std::filesystem::path& path) {
  ShaderSource res;
  switch (type) {
    case GL_VERTEX_SHADER:
    case GL_FRAGMENT_SHADER:
    case GL_TESS_CONTROL_SHADER:
    case GL_TESS_EVALUATION_SHADER:
      res.type = type;
      break;
    default:
      LOG_ERROR("Wrong/Unsupported shader type");
      LOG_ERROR("\"{} not loaded\"", path.string());
      return;
      break;
  }

  std::ifstream shaderFile(path.c_str());
  if (!shaderFile) {
    LOG_ERROR("File not found: {}", path.string());
    throw FileNotFoundException();
  }

  std::stringstream shaderData;
  shaderData << shaderFile.rdbuf();
  shaderFile.close();
  res.source = shaderData.str();

  shaders_.push_back(res);
}

GLuint Shader::CompileShader(const GLenum type, const std::string& src) {
  GLuint shader = glCreateShader(type);
  const GLchar* source = src.c_str();
  glShaderSource(shader, 1, &source, nullptr);
  glCompileShader(shader);

  GLint isCompiled = 0;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);

  if (isCompiled == GL_FALSE) {
    // Generate error message
    GLint maxLength = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
    // The maxLength includes the NULL character
    GLchar* infoLog = new GLchar[maxLength];
    glGetShaderInfoLog(shader, maxLength, &maxLength, infoLog);
    LOG_ERROR("Shader compilation fail: {}", infoLog);

    delete[] infoLog;
    throw ShaderCompilationException();
  }

  return shader;
}

void Shader::Init() {
  // Vertex and fragment shaders are successfully compiled.
  // Now time to link them together into a program.
  // Get a program object.
  program_ = glCreateProgram();

  for (const ShaderSource& x : shaders_) {
    GLuint currentShader = CompileShader(x.type, x.source);
    compiled_shaders_.push_back(currentShader);

    // Attach our shaders to our program
    glAttachShader(program_, currentShader);
  }

  // Link our program
  glLinkProgram(program_);

  // Note the different functions here: glGetProgram* instead of glGetShader*.
  GLint isLinked = 0;
  glGetProgramiv(program_, GL_LINK_STATUS, &isLinked);
  if (isLinked == GL_FALSE) {
    GLint maxLength = 0;
    glGetProgramiv(program_, GL_INFO_LOG_LENGTH, &maxLength);

    GLchar* infoLog = new GLchar[maxLength];
    glGetProgramInfoLog(program_, maxLength, &maxLength, infoLog);
    LOG_ERROR("Shader linkage fail: {}", infoLog);

    delete[] infoLog;

    // We don't need the program anymore, it doesn't work
    glDeleteProgram(program_);
  }

  // Always detach shaders after a successful link.
  for (const GLuint x : compiled_shaders_) {
    glDetachShader(program_, x);
  }

  // These are not needed after we have a program
  for (const GLuint x : compiled_shaders_) {
    glDeleteShader(x);
  }

  if (isLinked == GL_FALSE) {
    throw ProgramCreationException();
  }
}

// TODO maybe improvable with OpenGL 4.1
// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glProgramUniform.xhtml
void Shader::Enable() const {
  glUseProgram(program_);
}

void Shader::Disable() const {
  glUseProgram(0);
}

GLuint Shader::program_id() const {
  return program_;
}

GLint Shader::GetUniformLocation(const std::string& uniform_name) const {
  GLint location = glGetUniformLocation(program_, uniform_name.c_str());

  if (location == -1) {
    LOG_WARN("Unable to get uniform location {}", uniform_name);
    return INVALID_UNIFORM_LOCATION;
  }

  return location;
}

void Shader::SetUniformMat4(const std::string& uniform_name,
                            const glm::mat4& matrix) const {
  GLint uniform_location = GetUniformLocation(uniform_name);
  if (uniform_location != INVALID_UNIFORM_LOCATION) {
    glUniformMatrix4fv(uniform_location, 1, GL_FALSE, glm::value_ptr(matrix));
  } else {
    LOG_WARN("Error setting {} uniform", uniform_name);
  }
}

void Shader::SetUniformFloat(const std::string& uniform_name,
                             const float value) const {
  GLint uniform_location = GetUniformLocation(uniform_name);
  if (uniform_location != INVALID_UNIFORM_LOCATION) {
    glUniform1f(uniform_location, value);
  } else {
    LOG_WARN("Error setting {} uniform", uniform_name);
  }
}

void Shader::SetUniformVec3(const std::string& uniform_name,
                            const glm::vec3& vec) const {
  GLint uniform_location = GetUniformLocation(uniform_name);
  if (uniform_location != INVALID_UNIFORM_LOCATION) {
    glUniform3fv(uniform_location, 1, glm::value_ptr(vec));
  } else {
    LOG_WARN("Error setting {} uniform", uniform_name);
  }
}

void Shader::SetUniformVec4(const std::string& uniform_name,
                            const glm::vec4& vec) const {
  GLint uniform_location = GetUniformLocation(uniform_name);
  if (uniform_location != INVALID_UNIFORM_LOCATION) {
    glUniform4fv(uniform_location, 1, glm::value_ptr(vec));
  } else {
    LOG_WARN("Error setting {} uniform", uniform_name);
  }
}

void Shader::SetUnifromSampler(const std::string& uniform_name,
                               const TEXTURE_TYPE id) const {
  GLint uniform_location = GetUniformLocation(uniform_name);
  if (uniform_location != INVALID_UNIFORM_LOCATION) {
    glUniform1i(uniform_location, to_underlying(id));
  } else {
    LOG_WARN("Error setting {} uniform", uniform_name);
  }
}

ShaderManager& ShaderManager::Instance() {
  static ShaderManager instance_;
  return instance_;
}

Shader* ShaderManager::GetShader(const std::string& shader_id) {
  if (shaders_.find(shader_id) == shaders_.end()) {
    LOG_WARN("shader {} does not exists", shader_id);
    return nullptr;
  }
  return shaders_.at(shader_id);
}

ShaderManager::~ShaderManager() {
  for (auto [k, v] : shaders_) {
    delete v;
  }
}

void ShaderManager::AddShaders(
    const std::initializer_list<std::pair<std::string, Shader*>> in_shaders) {
  for (const auto& [shader_id, shader_ptr] : in_shaders) {
    if (shaders_.find(shader_id) == shaders_.end()) {
      // LOG OVERWRITING SHADER
    }
    shaders_.insert({shader_id, shader_ptr});
  }
}