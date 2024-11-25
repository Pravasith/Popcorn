#pragma once

#include "Global_Macros.h"
#include "Popcorn/Core/Base.h"
#include <vector>

ENGINE_NAMESPACE_BEGIN
class Shader {
public:
  Shader() { PC_PRINT("CREATED", TagType::Constr, "SHADER"); }
  virtual ~Shader() { PC_PRINT("DESTROYED", TagType::Destr, "SHADER"); }

  [[nodiscard]] virtual std::vector<char> LoadFile(const std::string &) = 0;
};

class ShaderLib {
  // SOMETHING FOR THE FUTURE
};

ENGINE_NAMESPACE_END
