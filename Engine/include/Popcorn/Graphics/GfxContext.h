#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include "Shader.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class GfxContext {
public:
  [[nodiscard]] inline static GfxContext *Get() {
    if (s_instance) {
      return s_instance;
    };

    s_instance = new GfxContext();
    return s_instance;
  };

  static void Destroy() {
    if (s_instance) {
      delete s_instance;
      s_instance = nullptr;
    } else {
      PC_WARN("Trying to destroy a non-existant instance of GfxContext")
    };
  };

  static ShaderLibrary *Shaders() {
    if (!s_shaderLibrary) {
      throw std::runtime_error("ShaderLibrary is null");
    }
    return s_shaderLibrary;
  };

private:
  GfxContext() { PC_PRINT("CREATED", TagType::Constr, "GfxContext.h") };
  ~GfxContext() { PC_PRINT("DESTROYED", TagType::Destr, "GfxContext.h") };

  // DELETE THE COPY CONSTRUCTOR AND COPY ASSIGNMENT OPERATOR
  GfxContext(const GfxContext &) = delete;
  GfxContext &operator=(const GfxContext &) = delete;

  // DELETE THE MOVE CONSTRUCTOR AND MOVE ASSIGNMENT OPERATOR
  GfxContext(GfxContext &&) = delete;
  GfxContext &operator=(GfxContext &&) = delete;

private:
  static GfxContext *s_instance;
  static ShaderLibrary *s_shaderLibrary;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
