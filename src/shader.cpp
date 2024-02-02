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
  for (const GLuint x : _compiled_shaders) {
    glDeleteShader(x);
  }

  if (_program != 0) {
    glDeleteProgram(_program);
    _program = 0;
  }

  LOG_TRACE("~Shader()");
}

void Shader::addShader(const GLenum type, const std::filesystem::path& path) {
  ShaderSource res;
  switch (type) {
    case GL_VERTEX_SHADER:
    case GL_FRAGMENT_SHADER:
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

  _shaders.push_back(res);
}

GLuint Shader::compileShader(const GLenum type, const std::string& src) {
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

void Shader::init() {
  // Vertex and fragment shaders are successfully compiled.
  // Now time to link them together into a program.
  // Get a program object.
  _program = glCreateProgram();

  for (const ShaderSource& x : _shaders) {
    GLuint currentShader = compileShader(x.type, x.source);
    _compiled_shaders.push_back(currentShader);

    // Attach our shaders to our program
    glAttachShader(_program, currentShader);
  }

  // Link our program
  glLinkProgram(_program);

  // Note the different functions here: glGetProgram* instead of glGetShader*.
  GLint isLinked = 0;
  glGetProgramiv(_program, GL_LINK_STATUS, &isLinked);
  if (isLinked == GL_FALSE) {
    GLint maxLength = 0;
    glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &maxLength);

    GLchar* infoLog = new GLchar[maxLength];
    glGetProgramInfoLog(_program, maxLength, &maxLength, infoLog);
    LOG_ERROR("Shader linkage fail: {}", infoLog);

    delete[] infoLog;

    // We don't need the program anymore, it doesn't work
    glDeleteProgram(_program);
  }

  // Always detach shaders after a successful link.
  for (const GLuint x : _compiled_shaders) {
    glDetachShader(_program, x);
  }

  // These are not needed after we have a program
  for (const GLuint x : _compiled_shaders) {
    glDeleteShader(x);
  }

  if (isLinked == GL_FALSE) {
    throw ProgramCreationException();
  }
}

void Shader::enable() const {
  glUseProgram(_program);
}

void Shader::disable() const {
  glUseProgram(0);
}

GLint Shader::getUniformLocation(const std::string& uniform_name) const {
  GLint Location = glGetUniformLocation(_program, uniform_name.c_str());

  if (Location == -1) {
    LOG_WARN("Unable to get uniform location {}", uniform_name);
    return INVALID_UNIFORM_LOCATION;
  }

  return Location;
}

void Shader::setUniformMat4(const std::string& uniform_name,
                            const glm::mat4& matrix) const {
  GLint uniform_location = getUniformLocation(uniform_name);
  if (uniform_location != INVALID_UNIFORM_LOCATION) {
    glUniformMatrix4fv(uniform_location, 1, GL_FALSE, glm::value_ptr(matrix));
  } else {
    LOG_WARN("Error setting {} uniform", uniform_name);
  }
}

void Shader::setUniformFloat(const std::string& uniform_name,
                             const float value) const {
  GLint uniform_location = getUniformLocation(uniform_name);
  if (uniform_location != INVALID_UNIFORM_LOCATION) {
    glUniform1f(uniform_location, value);
  } else {
    LOG_WARN("Error setting {} uniform", uniform_name);
  }
}

void Shader::setUniformVec3(const std::string& uniform_name,
                            const glm::vec3& vec) const {
  GLint uniform_location = getUniformLocation(uniform_name);
  if (uniform_location != INVALID_UNIFORM_LOCATION) {
    glUniform3fv(uniform_location, 1, glm::value_ptr(vec));
  } else {
    LOG_WARN("Error setting {} uniform", uniform_name);
  }
}

void Shader::setUnifromSampler(const std::string& uniform_name,
                               const int id) const {
  GLint uniform_location = getUniformLocation(uniform_name);
  if (uniform_location != INVALID_UNIFORM_LOCATION) {
    glUniform1i(uniform_location, id);
  } else {
    LOG_WARN("Error setting {} uniform", uniform_name);
  }
}
