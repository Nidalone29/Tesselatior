#include "shader.h"

#define INVALID_UNIFORM_LOCATION 0xffffffff

#include <iostream>
#include <fstream>
#include <sstream>

#include "utilities.h"  // FileNotFoundException
#include "logger.h"

Shader::Shader() {
  LOG_TRACE("Shader()");
}

Shader::~Shader() {
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
      // TODO: error handling wrong shader type
      LOG_ERROR("Wrong/Unsupported shader type");
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

  // DEBUG call to say that a shader has been loaded
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
    GLint maxLength = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

    // The maxLength includes the NULL character
    std::vector<GLchar> infoLog(maxLength);
    glGetShaderInfoLog(shader, maxLength, &maxLength, infoLog.data());

    // We don't need the shader anymore.
    // TODO delete all the shader
    glDeleteShader(shader);

    LOG_ERROR("Shader compilation fail: {}", infoLog.data());

    throw ShaderCreationException();
  }

  return shader;
}

void Shader::init() {
  // Vertex and fragment shaders are successfully compiled.
  // Now time to link them together into a program.
  // Get a program object.
  _program = glCreateProgram();

  std::vector<GLuint> compiled_shaders;

  for (const ShaderSource& x : _shaders) {
    GLuint currentShader = compileShader(x.type, x.source);
    compiled_shaders.push_back(currentShader);

    // Attach our shaders to our program
    glAttachShader(_program, currentShader);
  }

  // Link our program
  glLinkProgram(_program);

  // Note the different functions here: glGetProgram* instead of glGetShader*.
  GLint isLinked = 0;
  glGetProgramiv(_program, GL_LINK_STATUS, (int*)&isLinked);
  if (isLinked == GL_FALSE) {
    GLint maxLength = 0;
    glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &maxLength);

    // The maxLength includes the NULL character
    std::vector<GLchar> infoLog(maxLength);
    glGetProgramInfoLog(_program, maxLength, &maxLength, infoLog.data());

    LOG_ERROR("Shader compilation fail: {}", infoLog.data());

    // We don't need the program anymore.
    glDeleteProgram(_program);

    // Don't leak shaders either.
    for (const GLuint x : compiled_shaders) {
      glDeleteShader(x);
    }

    // In this simple program, we'll just leave
    return;
  }

  // Always detach shaders after a successful link.
  for (const GLuint x : compiled_shaders) {
    glDetachShader(_program, x);
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

void Shader::setUniformMat4(const std::string uniform_name,
                            const glm::mat4& matrix) const {
  GLint uniform_location = getUniformLocation(uniform_name);
  if (uniform_location != INVALID_UNIFORM_LOCATION) {
    glUniformMatrix4fv(uniform_location, 1, GL_FALSE,
                       const_cast<float*>(&matrix[0][0]));
  } else {
    LOG_WARN("Error setting {} uniform", uniform_name);
  }
}

void Shader::setUniformFloat(const std::string uniform_name,
                             const float value) const {
  GLint uniform_location = getUniformLocation(uniform_name);
  if (uniform_location != INVALID_UNIFORM_LOCATION) {
    glUniform1f(uniform_location, value);
  } else {
    LOG_WARN("Error setting {} uniform", uniform_name);
  }
}

void Shader::setUniformVec3(const std::string uniform_name,
                            const glm::vec3& vec) const {
  GLint uniform_location = getUniformLocation(uniform_name);
  if (uniform_location != INVALID_UNIFORM_LOCATION) {
    glUniform3fv(uniform_location, 1, const_cast<float*>(&vec[0]));
  } else {
    LOG_WARN("Error setting {} uniform", uniform_name);
  }
}

void Shader::setUnifromSampler(const std::string uniform_name,
                               const int id) const {
  GLint uniform_location = getUniformLocation(uniform_name);
  if (uniform_location != INVALID_UNIFORM_LOCATION) {
    glUniform1i(uniform_location, id);
  } else {
    LOG_WARN("Error setting {} uniform", uniform_name);
  }
}
