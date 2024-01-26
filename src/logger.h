#ifndef LOGGER_H
#define LOGGER_H

#include <spdlog/spdlog.h>

// we are using the default logger (stdout, multi-threaded, colored)

// https://fmt.dev/latest/syntax.html

#define LOG_TRACE(...) spdlog::trace(__VA_ARGS__)
#define LOG_INFO(...) spdlog::info(__VA_ARGS__)
#define LOG_WARN(...) spdlog::warn(__VA_ARGS__)
#define LOG_ERROR(...) spdlog::error(__VA_ARGS__)

#endif  // LOGGER_h
