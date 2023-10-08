#ifndef UTILITIES_H
#define UTILITIES_H

#include <GL/glew.h>
#include <stdexcept>

/**
 * Semplici strutture che raprresnetano delle eccezioni
 */
struct FileNotFoundException : public std::exception {};
struct ShaderCreationException : public std::exception {};
struct ProgramCreationException : public std::exception {};

#endif  // UTILITIES_H