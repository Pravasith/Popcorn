#pragma once

#include "Global_Macros.h"
#include "Popcorn/Core/Base.h"

ENGINE_NAMESPACE_BEGIN
class Shader {
public:
  Shader() { PC_PRINT("CREATED", TagType::Constr, "SHADER"); }
  virtual ~Shader() { PC_PRINT("DESTROYED", TagType::Destr, "SHADER"); }

  virtual void LoadFile() = 0;
  // FOR VULKAN SHADERS
  virtual bool Compile();
};

class ShaderLib {
  // SOMETHING FOR THE FUTURE
};

ENGINE_NAMESPACE_END
