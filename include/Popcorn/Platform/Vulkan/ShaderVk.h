
#pragma once

#include "Global_Macros.h"
#include "Popcorn/Core/Base.h"
#include "Shader.h"

ENGINE_NAMESPACE_BEGIN
class ShaderVk : public Shader {
public:
  ShaderVk() { PC_PRINT("CREATED", TagType::Constr, "SHADER-VK"); }
  virtual ~ShaderVk() { PC_PRINT("DESTROYED", TagType::Destr, "SHADER-VK"); }

  [[nodiscard]] virtual std::vector<char>
  LoadFile(const std::string &) override;
};

ENGINE_NAMESPACE_END
