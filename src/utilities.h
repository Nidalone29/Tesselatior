#ifndef UTILITIES_H
#define UTILITIES_H

#include "GL/glew.h"
#include <string>
#include <vector>
#include <stdexcept>

/**
 * Semplici strutture che raprresnetano delle eccezioni
 */
struct FileNotFoundException : public std::exception {};
struct ShaderCreationException : public std::exception {};
struct ProgramCreationException : public std::exception {};

/**
 * Mettiamo tutti gli shader in un vettore
 */
typedef std::vector<GLuint> Shaders;

/**
 * Funzione che crea uno shader dato il tipo e il codice sorgente dello
 * shader passato come file di testo.
 * @param eShaderType tipo dello shader
 * @param fileName nome del file
 * @return l'identificativo dello shader
 * @throw FileNotFoundException in caso di problema con il file
 * @throw ShaderCreationException in caso di errori
 */
GLuint LoadShader(GLenum eShaderType, const std::string& fileName);

/**
 * Funzione che crea uno shader dato il tipo e il codice sorgente dello
 * shader passato come stringa.
 * @param eShaderType tipo dello shader
 * @param strShaderFile stringa con il codice dello shader
 * @return l'identificativo dello shader
 * @throw ShaderCreationException in caso di errori
 */
GLuint CreateShader(GLenum eShaderType, const std::string& strShaderFile);

/**
 * Funzione che crea un programma a partire dalla lista degli shader
 *
 * @param shaderList vettore con la lista degli shader
 * @return l'identificativo del programma
 * @throw ProgramCreationException in caso di errori
 */
GLuint CreateProgram(const Shaders& shaderList);

#endif  // UTILITIES_H