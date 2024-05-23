#pragma once

#define ENGINE_NAMESPACE_BEGIN namespace Popcorn {
#define ENGINE_NAMESPACE_END }

// Compile only
#define ENUM_T0_STRING(enum_name, option_name)                                 \
  const char *Get##enum_name##Name() const { return #option_name; }
