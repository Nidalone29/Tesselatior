#ifndef UTILITIES_H
#define UTILITIES_H

#include <stdexcept>

#include <GL/glew.h>

#include "logger.h"

/**
 * Semplici strutture che rappresentano delle eccezioni
 */
struct AppInitException : public std::exception {};

struct FileNotFoundException : public std::exception {};

struct FramebufferException : public std::exception {};

struct MeshImportException : public std::exception {};

struct ShaderCompilationException : public std::exception {};

struct ProgramCreationException : public std::exception {};

#endif  // UTILITIES_H
