#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include "Popcorn/Core/Buffer.h"
#include <fstream>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN
class Shader {
public:
  Shader() { PC_PRINT("CREATED", TagType::Constr, "SHADER"); }
  virtual ~Shader() { PC_PRINT("DESTROYED", TagType::Destr, "SHADER"); }

  [[nodiscard]] inline Buffer ReadSpvFile(const std::string &filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
      throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t)file.tellg();

    Buffer spv;
    spv.Resize(fileSize);

    file.seekg(0);
    file.read((char *)spv.GetData(), fileSize);

    file.close();

    return spv;
  };

  template <typename T> inline static void Print(Buffer &buffer) {
    Buffer::Print<T>(buffer);
  };
};

class ShaderLib {
  // SOMETHING FOR THE FUTURE
};
GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
