#pragma once

#include "Global_Macros.h"

ENGINE_NAMESPACE_BEGIN
// SINGLETON
class Renderer {
public:
  static void Create();
  static void Destroy();

private:
  Renderer();
  virtual ~Renderer();

  static Renderer *s_instance;
};

ENGINE_NAMESPACE_END
