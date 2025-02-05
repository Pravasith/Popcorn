#pragma once

#include "Helpers.h"
#include <sys/types.h>

ENGINE_NAMESPACE_BEGIN

#ifdef PC_DEBUG
#define PC_PRINT(msg, tag, className)                                          \
  do {                                                                         \
    std::ostringstream oss;                                                    \
    oss << msg;                                                                \
    PC_Print(oss.str(), tag, className);                                       \
  } while (0);

#define PC_WARN(msg)                                                           \
  do {                                                                         \
    std::ostringstream oss;                                                    \
    oss << "\033[33mWARNING!! " << msg << "\033[0m";                           \
    PC_Print(oss.str(), TagType::Print, "");                                   \
  } while (0);

#define PC_ERROR(msg, errType)                                                 \
  do {                                                                         \
    std::ostringstream oss;                                                    \
    oss << "\033[31mERROR!! " << msg << "\033[0m";                             \
    PC_Print(oss.str(), TagType::Print, errType);                              \
  } while (0);

#define PC_VK_NULL_CHECK(vulkan_obj)                                           \
  if (vulkan_obj == VK_NULL_HANDLE) {                                          \
    PC_WARN(#vulkan_obj << " isn't null in " << PC_FUNC_NAME)                  \
  };
#else
#define PC_PRINT(x, y, z)
#define PC_WARN(msg)
#define PC_ERROR(msg, errType)
#define PC_VK_NULL_CHECK(vulkan_obj, classname)
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
