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
  ~Shader() { PC_PRINT("DESTROYED", TagType::Destr, "SHADER"); }

  [[nodiscard]] inline static Buffer ReadSpvFile(const std::string &filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
      throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t)file.tellg();

    PC_WARN("BUFFER: File reading of size: " << fileSize)

    Buffer spv;
    spv.Resize(fileSize);

    file.seekg(0);
    file.read((char *)spv.GetData(), fileSize);
    file.close();

    return spv;
  };

  // [[nodiscard]] static Buffer LoadByteCode(ShaderFiles shaderName) {
  //   return ReadSpvFile(PC_SHADER_SOURCE_MAP[shaderName]);
  // };

  template <typename T> inline static void Print(Buffer &buffer) {
    Buffer::Print<T>(buffer);
  };
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
