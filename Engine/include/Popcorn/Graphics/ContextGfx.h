#pragma once

#include "Animation/CurveFactory.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class ContextGfx {
  // Class owned by Application

public:
  inline static ContextGfx *Get() {
    if (s_instance) {
      return s_instance;
    };

    s_instance = new ContextGfx();

    // Create singletons
    s_instance->GraphicsInit();

    return s_instance;
  };

  static void Destroy() {
    if (s_instance) {
      // Destroy singletons
      s_instance->GraphicsCleanUp();

      delete s_instance;
      s_instance = nullptr;
    } else {
      PC_WARN("Trying to destroy a non-existant instance of ContextGfx")
    };
  };

  const CurveFactory *AppCurves = s_curveFactory;

private:
  void GraphicsInit();
  void GraphicsCleanUp();

private:
  // DELETE THE COPY CONSTRUCTOR AND COPY ASSIGNMENT OPERATOR
  ContextGfx(const ContextGfx &) = delete;
  ContextGfx &operator=(const ContextGfx &) = delete;

  // DELETE THE MOVE CONSTRUCTOR AND MOVE ASSIGNMENT OPERATOR
  ContextGfx(ContextGfx &&) = delete;
  ContextGfx &operator=(ContextGfx &&) = delete;

private:
  ContextGfx() {
    s_curveFactory = CurveFactory::Get();

    PC_PRINT("CREATED", TagType::Constr, "ContextGfx.h")
  };
  ~ContextGfx() { PC_PRINT("DESTROYED", TagType::Destr, "ContextGfx.h") };

private:
  static ContextGfx *s_instance;

  static CurveFactory *s_curveFactory;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
