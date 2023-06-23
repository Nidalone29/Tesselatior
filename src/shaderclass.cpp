#include "shaderclass.h"
#include <iostream>

ShaderClass::ShaderClass() : _program(0) {}

ShaderClass::~ShaderClass() {
  Shaders::const_iterator s, se;

  for (s = _shaders.begin(), se = _shaders.end(); s != se; ++s) {
    glDeleteShader(*s);
  }

  if (_program != 0) {
    glDeleteProgram(_program);
    _program = 0;
  }
}

void ShaderClass::enable() { glUseProgram(_program); }

bool ShaderClass::init() {
  if (!load_shaders()) return false;

  try {
    _program = CreateProgram(_shaders);
  } catch (...) {
    return false;
  }

  Shaders::const_iterator s, se;

  for (s = _shaders.begin(), se = _shaders.end(); s != se; ++s) {
    glDeleteShader(*s);
  }
  _shaders.clear();

  return load_done();
}

bool ShaderClass::add_shader(GLenum ShaderType, const std::string& FileName) {
  GLuint id;
  try {
    id = LoadShader(ShaderType, FileName.c_str());
    _shaders.push_back(id);
    return true;
  } catch (...) {
    return false;
  }
}

GLint ShaderClass::get_uniform_location(const std::string& UniformName) const {
  GLint Location = glGetUniformLocation(_program, UniformName.c_str());

  if (Location == -1) {
    std::cerr << "Warning! Unable to get the location of uniform '"
              << UniformName << "'" << std::endl;
    return INVALID_UNIFORM_LOCATION;
  }

  return Location;
}
