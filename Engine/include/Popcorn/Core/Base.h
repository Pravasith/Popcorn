#pragma once

#include "GlobalMacros.h"
#include <cstddef>

#ifdef PC_DEBUG
#include <iostream>
#include <sstream>
#include <string>
#endif

#include <sys/types.h>

ENGINE_NAMESPACE_BEGIN
constexpr std::size_t shift_l(std::size_t n) { return 1 << n; }

enum TagType { Constr, Destr, Print };

#ifdef PC_DEBUG
extern int PC_print_lvl;
static void PC_Print(const std::string &msg, const TagType tag,
                     const std::string &className) {

  if (PC_print_lvl < 0) {
    std::cout << "MISSING CONSTRUCTOR MESSAGE" << '\n';
    return;
  };

  std::stringstream ss;
  auto lvl = PC_print_lvl < 10 ? " " + std::to_string(PC_print_lvl)
                               : std::to_string(PC_print_lvl);

  ss << lvl << " ";

  for (int i = 0; i < PC_print_lvl; ++i) {
    ss << "| ";
  }

  ss << "| " << className << ": " << msg << '\n';
  std::cout << ss.str();

  if (tag == TagType::Constr) {
    PC_print_lvl++;
  } else if (tag == TagType::Destr)
    PC_print_lvl--;
};

#define PC_PRINT(msg, tag, className)                                          \
  do {                                                                         \
    std::ostringstream oss;                                                    \
    oss << msg;                                                                \
    PC_Print(oss.str(), tag, className);                                       \
  } while (0);
#define PC_WARN(msg)                                                           \
  do {                                                                         \
    std::ostringstream oss;                                                    \
    oss << "WARNING !! " << msg;                                               \
    PC_Print(oss.str(), TagType::Print, "");                                   \
  } while (0);
#define PC_ERROR(msg, errType)                                                 \
  do {                                                                         \
    std::ostringstream oss;                                                    \
    oss << "ERROR !! " << msg;                                                 \
    PC_Print(oss.str(), TagType::Print, errType);                              \
  } while (0);
#else
#define PC_PRINT(x, y, z)
#define PC_WARN(msg)
#define PC_ERROR(msg, errType)
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
