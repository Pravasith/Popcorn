#pragma once

#include "Global_Macros.h"
#include <cstddef>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/types.h>

ENGINE_NAMESPACE_BEGIN
constexpr std::size_t bit_shift_left(std::size_t n) { return 1 << n; }

#ifdef PC_DEBUG
static void pc_print(const std::string &msg, unsigned int lvl,
                     const std::string &tag) {
  std::stringstream ss;

  if (lvl > 20)
    lvl = 20;

  ss << ">> ";

  for (int i = 0; i < lvl; ++i) {
    ss << "  ";
  }

  ss << "---- " << tag << ": " << msg << '\n';
  std::cout << ss.str();
};

#define PC_PRINT_DEBUG(msg, lvl, tag)                                          \
  do {                                                                         \
    std::ostringstream oss;                                                    \
    oss << msg;                                                                \
    pc_print(oss.str(), lvl, tag);                                             \
  } while (0)
#else
#define PC_PRINT_DEBUG(msg, lvl, tag)
#endif

// VARIANT 1: (std::bind -- slow)
// #define PC_BIND_EVENT_FUNC(eventType, func) \
      // std::bind(&func, this, std::placeholders::_1)

// VARIANT 2: lambda with event name
#define PC_BIND_EVENT_FUNC(eventType, eventCbFunc)                             \
  [this](eventType &e) { return this->eventCbFunc(e); }

// VARIANT 3: lambda with variadic templates -- from Cherno's Hazel Engine
// #define PC_BIND_EVENT_FUNC(eventType) \
      // [this](auto &&...args) -> decltype(auto) {
//   return this->eventType(std::forward<decltype(args)>(args)...); }

ENGINE_NAMESPACE_END
