
#pragma once

#include "Global_Macros.h"
#include "Popcorn/Core/Base.h"
#include "Shader.h"

ENGINE_NAMESPACE_BEGIN
class ShaderVk : public Shader {
  ShaderVk() { PC_PRINT("CREATED", TagType::Constr, "SHADER-VK"); }
  virtual ~ShaderVk() { PC_PRINT("DESTROYED", TagType::Destr, "SHADER-VK"); }

  virtual void LoadFile() override;
  virtual bool Compile() override;
};

ENGINE_NAMESPACE_END
