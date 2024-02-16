#ifndef UTILITIES_H
#define UTILITIES_H

#include <stdexcept>
#include <type_traits>

// this exists because std::to_underlying is C++23 only
// https://en.cppreference.com/w/cpp/utility/to_underlying
template <class T>
[[nodiscard]] constexpr typename std::underlying_type<T>::type to_underlying(
    T val) noexcept {
  return static_cast<typename std::underlying_type<T>::type>(val);
}

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
