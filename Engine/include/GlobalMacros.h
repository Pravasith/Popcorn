#pragma once

#define ENGINE_NAMESPACE_BEGIN namespace Popcorn {
#define ENGINE_NAMESPACE_END }

#define CTX_NAMESPACE_BEGIN namespace Context {
#define CTX_NAMESPACE_END }

#define GFX_NAMESPACE_BEGIN namespace Gfx {
#define GFX_NAMESPACE_END }

#define KEY_NAMESPACE_BEGIN namespace Key {
#define KEY_NAMESPACE_END }

#define BUFFER_DEFS_NAMESPACE_BEGIN namespace BufferDefs {
#define BUFFER_DEFS_NAMESPACE_END }

#define UNIFORM_DEFS_NAMESPACE_BEGIN namespace UniformDefs {
#define UNIFORM_DEFS_NAMESPACE_END }

// COMPILE ONLY
#define ENUM_TO_STRING(enum_name, option_name)                                 \
  const char *Get##enum_name##Name() const override { return #option_name; }
