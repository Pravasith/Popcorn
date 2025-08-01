#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include "Popcorn/Core/Buffer.h"
#include "Renderer.h"
#include "Sources.h"
#include <fstream>
#include <map>
#include <unordered_map>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

enum ShaderStages {
  None = 0,
  //
  // Graphics types
  VertexBit = 1,
  TesselationBit = shift_l(1),
  GeometryBit = shift_l(2),
  FragmentBit = shift_l(3),
  //
  // Compute types
  ComputeBit = shift_l(4)
  //
  // Ray tracing types
  // TODO: Fill it out when time arrives
};

class Shader {
public:
  Shader() { PC_PRINT("CREATED", TagType::Constr, "Shader"); }
  ~Shader() { PC_PRINT("DESTROYED", TagType::Destr, "Shader"); }

  [[nodiscard]] inline static Buffer ReadSpvFile(const std::string &filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
      throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t)file.tellg();

    PC_WARN("Shader: Reading SPV File of size: " << fileSize)

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

class ShaderLibrary {
public:
  [[nodiscard]] inline static ShaderLibrary *Get() {
    if (s_instance) {
      return s_instance;
    };

    s_instance = new ShaderLibrary();
    return s_instance;
  };

  static void Destroy() {
    if (s_instance) {
      delete s_instance;
      s_instance = nullptr;
    } else {
      PC_WARN("Trying to destroy a non-existant instance of ShaderLibrary")
    };
  };

public:
  template <RendererType R> void LoadShaders();
  template <RendererType R> void UnloadShaders();

  template <RendererType R, ShaderFiles T> Buffer &GetShader() {
    switch (R) {
    case RendererType::Vulkan: {
      auto it = s_shaders.find(T);
      if (it != s_shaders.end()) {
        return it->second;
      } else {
        throw std::runtime_error("Shader not found!");
      }
      break;
    }
    case RendererType::OpenGL:
      break;
    case RendererType::None:
      break;
    }
  }

private:
  ShaderLibrary() { PC_PRINT("CREATED", TagType::Constr, "ShaderLibrary.h") };
  ~ShaderLibrary() { PC_PRINT("DESTROYED", TagType::Destr, "ShaderLibrary.h") };

  // DELETE THE COPY CONSTRUCTOR AND COPY ASSIGNMENT OPERATOR
  ShaderLibrary(const ShaderLibrary &) = delete;
  ShaderLibrary &operator=(const ShaderLibrary &) = delete;

  // DELETE THE MOVE CONSTRUCTOR AND MOVE ASSIGNMENT OPERATOR
  ShaderLibrary(ShaderLibrary &&) = delete;
  ShaderLibrary &operator=(ShaderLibrary &&) = delete;

private:
  bool m_shadersLoaded = false;
  static ShaderLibrary *s_instance;
  static std::map<ShaderFiles, Buffer> s_shaders;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
