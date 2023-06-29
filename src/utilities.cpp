#include "utilities.h"
#include <iostream>
#include <fstream>
#include <sstream>

GLuint CreateShader(GLenum eShaderType, const std::string& strShaderFile) {
  GLuint shader = glCreateShader(eShaderType);
  const char* strFileData = strShaderFile.c_str();
  glShaderSource(shader, 1, &strFileData, nullptr);

  glCompileShader(shader);

  GLint status;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  if (status == GL_FALSE) {
    GLint infoLogLength;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

    GLchar* strInfoLog = new GLchar[infoLogLength + 1];
    glGetShaderInfoLog(shader, infoLogLength, nullptr, strInfoLog);

    const char* strShaderType = nullptr;
    switch (eShaderType) {
      case GL_VERTEX_SHADER:
        strShaderType = "vertex";
        break;
      case GL_GEOMETRY_SHADER:
        strShaderType = "geometry";
        break;
      case GL_FRAGMENT_SHADER:
        strShaderType = "fragment";
        break;
    }

    std::cerr << "Compile failure in " << strShaderType << " shader:\n"
              << strInfoLog << std::endl;
    delete[] strInfoLog;
    throw ShaderCreationException();
  }

  return shader;
}

GLuint CreateProgram(const std::vector<GLuint>& shaderList) {
  GLuint program = glCreateProgram();

  for (size_t iLoop = 0; iLoop < shaderList.size(); iLoop++) {
    glAttachShader(program, shaderList[iLoop]);
  }

  glLinkProgram(program);

  GLint status;
  glGetProgramiv(program, GL_LINK_STATUS, &status);
  if (status == GL_FALSE) {
    GLint infoLogLength;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

    GLchar* strInfoLog = new GLchar[infoLogLength + 1];
    glGetProgramInfoLog(program, infoLogLength, nullptr, strInfoLog);
    std::cerr << "Linker failure: " << strInfoLog << std::endl;
    delete[] strInfoLog;
  }

  for (size_t iLoop = 0; iLoop < shaderList.size(); iLoop++) {
    glDetachShader(program, shaderList[iLoop]);
  }

  if (status == GL_FALSE) {
    throw ProgramCreationException();
  }

  return program;
}

GLuint LoadShader(GLenum eShaderType, const std::string& FileName) {
  std::ifstream shaderFile(FileName.c_str());
  if (!shaderFile) {
    std::cerr << "File not found: " << FileName << std::endl;
    throw FileNotFoundException();
  }
  std::stringstream shaderData;
  shaderData << shaderFile.rdbuf();
  shaderFile.close();

  return CreateShader(eShaderType, shaderData.str());
}
